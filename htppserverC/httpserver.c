#include <pthread.h>

#ifdef _WIN32

#include <Winsock2.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <time.h>

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

void err(int code, const char *msg)
{
    printf("ERROR %d: %s\n", code, msg);
    exit(code);
}
void errx(int code, const char *msg, const char *data)
{
    printf("ERROR %d: ", code);
    printf(msg, data);
    printf("\n");
    exit(code);
}
void warn(const char *msg)
{
    printf("WARNNING: %s\n", msg);
}

#else

// Basic standard input output.
#include <stdio.h>
// stat structure is here.
#include <sys/stat.h>
// for isalnum()
#include <ctype.h>
// For err() function that prints formatted error messages.
#include <err.h>
// fcntl is for Linux based opetating systems.
#include <fcntl.h>

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#endif

struct queue_node_s
{
    int data;
    struct queue_node_s *next_node;
};

struct queue_int_s
{
    struct queue_node_s *head;
    struct queue_node_s *tail;
};

struct queue_int_s *create_queue()
{
    struct queue_int_s *newQueue = malloc(sizeof(struct queue_int_s));

    if (newQueue != NULL)
    {
        newQueue->head = NULL;
        newQueue->tail = NULL;
    }

    return newQueue;
}

/* enqueue(...)
 *
 *	Adds the new integer to the end of the queue.
 *
 *	Returns 1 if enqueuing is successful. Returns 0 otherwise.
 */
int enqueue(struct queue_int_s *queue, int data)
{
    struct queue_node_s *newNode;
    if (queue->head == NULL)
    {
        /* If the queue is empty */
        queue->head = malloc(sizeof(struct queue_node_s));
        if (queue->head == NULL)
            return 0;
        queue->head->data = data;
        queue->head->next_node = NULL;
        queue->tail = queue->head;
    }
    else
    {
        /* Create a new node */
        newNode = malloc(sizeof(struct queue_node_s));
        newNode->data = data;
        newNode->next_node = NULL;

        /* Attach new node to the end of the queue */
        queue->tail->next_node = newNode;

        /* Update the new tail */
        queue->tail = newNode;
    }
    return 1;
}

/* dequeue(...)
 *	Removes the head of the queue. The value of the head is stored
 *		in the second parameter of the function.
 *
 *	Returns 0 if user attempts to dequeue from an empty queue.
 *	Returns 1 otherwise.
 */
int dequeue(struct queue_int_s *queue, int *data)
{
    if (is_empty_queue(queue))
        return 0;

    struct queue_node_s *nextHead = queue->head->next_node;

    *data = queue->head->data;

    /* Deallocate memeory of the head */
    struct queue_node_s *oldHead = queue->head;
    queue->head = nextHead;
    free(oldHead);

    if (queue->head == NULL)
        /* The queue now is empty */
        queue->tail = NULL;
    return 1;
}

/* delete_queue(...)
 *
 *	Deallocate all the memory of the queue structure.
 */
void delete_queue(struct queue_int_s **queuePointer)
{
    if (queuePointer == NULL)
        return;
    struct queue_node_s *curNode = (*queuePointer)->head;
    struct queue_node_s *temp;

    while (curNode != NULL)
    {
        temp = curNode->next_node;
        free(curNode);
        curNode = temp;
    }
    free(*queuePointer);
    *queuePointer = NULL;
}

/* is_empty_queue(...)
 *
 *	Returns 1 if the queue is empty. Returns 0 otherwise.
 */
int is_empty_queue(struct queue_int_s *queue)
{
    return (queue->head == NULL) ? 1 : 0;
}

/**
   Converts a string to an 16 bits unsigned integer.
   Returns 0 if the string is malformed or out of the range.
 */
uint16_t strtouint16(char number[])
{
    char *last;
    long num = strtol(number, &last, 10);
    if (num <= 0 || num > UINT16_MAX || *last != '\0')
    {
        return 0;
    }
    return num;
}

/**
   Creates a socket for listening for connections.
   Closes the program and prints an error message on error.
 */
int create_listen_socket(uint16_t port)
{
    struct sockaddr_in addr;
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        err(EXIT_FAILURE, "socket error");
    }

    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htons(INADDR_ANY);
    addr.sin_port = htons(port);
    if (bind(listenfd, (struct sockaddr *)&addr, sizeof addr) < 0)
    {
        err(EXIT_FAILURE, "bind error");
    }

    if (listen(listenfd, 500) < 0)
    {
        err(EXIT_FAILURE, "listen error");
    }

    return listenfd;
}

#define BUFLEN (1024 * 1024)
char recvbuf[BUFLEN] = "";
char sendbuf[BUFLEN] = "";
char hostName[17] = "";
char logfile_name[20] = "";
typedef enum
{
    false,
    true
} boolean;
boolean isLogging = false;

void send_buf(int connfd, const char *buf, int len)
{
    send(connfd, buf, len, 0);
}

void send_bin(int connfd, const char *buf, int len)
{
    printf("binary: length: %d\n", len);
    send_buf(connfd, buf, len);
}

void send_string(int connfd, const char *buf)
{
    printf("send_string %s\n", buf);
    send_buf(connfd, buf, strlen(buf));
}

int send_result(int connfd, int code, const char *text)
{
    char responsetext[64] = "";
    strcpy(responsetext, text);
    sprintf(sendbuf, "HTTP/1.1 %d %s\r\nContent-Length: %ld\r\n\r\n%s\n", code, responsetext, strlen(responsetext) + 1, responsetext);
    send_string(connfd, sendbuf);

#ifdef _WIN32
    closesocket(connfd);
#else
    close(connfd);
#endif

    printf("=== connection closed. ===\n");

    return 0;
}

int get_file_size(const char *filename)
{
    struct stat st;
    int fh = open(filename, O_RDONLY);
    if (fh == -1)
    {
        if (errno == EACCES)
            return -2;
        return -1;
    }

    stat(filename, &st);
    close(fh);

    return st.st_size;
}

int get_file(const char *filename, char *buf, int len)
{
    int fh = open(filename, O_RDONLY);
    if (fh == -1)
    {
        if (errno == EACCES)
            return -2;
        return -1;
    }

    len = read(fh, buf, len);
    if (len == -1)
        len = -2;
    if (len >= 0)
        buf[len] = 0;

    close(fh);

    return len;
}

int put_file(const char *filename, const char *buf, int len)
{
    int fh = open(filename, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    if (fh == -1)
    {
        if (errno == EACCES)
            return -2;
        return -1;
    }

    len = write(fh, buf, len);
    if (len == -1)
        len = -2;

    close(fh);

    return len;
}

int get_header(const char *p, int buflen, const char *name, char *value)
{
    int i;
    char header[17] = "";
    char variable[17] = "";
    for (i = 0; i < buflen && i < 16;)
    {
        if (p[i] == ':')
        {
            strncpy(header, p, i);
            header[i] = 0;
            i += 2;
            p += i;
            buflen -= i;
            for (i = 0; i < buflen && i < 16; i++)
            {
                if (p[i] == '\r')
                {
                    strncpy(variable, p, i);
                    variable[i] = 0;
                    i += 2;
                    p += i;
                    buflen -= i;
                    break;
                }
            }
            if (strcmp(header, name) == 0)
            {
                strcpy(value, variable);
                return 1;
            }
            i = 0;
        }
        else
        {
            i++;
        }
    }
    return 0;
}

int check_host(const char *p, int buflen)
{
    char variable[17] = "";
    if (!get_header(p, buflen, "Host", variable))
        return 0;
    strcpy(hostName, variable);
    printf("host: %s\n", variable);
    for (p = variable; *p; p++)
    {
        if (isspace(*p))
            return 0;
    }
    return 1;
}

int isnumber(const char *p)
{
    for (; *p; p++)
    {
        if (!isdigit(*p))
            return 0;
    }
    return 1;
}

int getLineOfLog()
{
    int ch = 0;
    int lines = 0;
    FILE *log_file;

    log_file = fopen(logfile_name, "r");

    if (log_file == NULL)
        return 0;

    while (!feof(log_file))
    {
        ch = fgetc(log_file);
        if (ch == '\n')
        {
            lines++;
        }
    }

    return lines;
}

int getErrOfLog()
{

    FILE *file = fopen(logfile_name, "r");

    if (!file)
    {
        printf("\n Unable to open : %s ", logfile_name);
        return -1;
    }

    char line[1000];
    int errCount = 0;

    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, "FAIL"))
            errCount++;
    }
    return errCount;
}

void recordLog(char *request, char *filename, char *host, int filelen, char *data, int errCode)
{
    FILE *log_file;
    char log[100] = "";
    int logSize;

    log_file = fopen(logfile_name, "a");
    if (strcmp(request, "HEAD") == 0 && errCode == 0 && filename != NULL && filelen > 0 && host != NULL)
        sprintf(log, "%s\t%s\t%s\t%d\n", request, filename, host, filelen);
    else if (strcmp(request, "GET") == 0 && errCode == 0 && filename != NULL && filelen > 0 && host != NULL && data != NULL)
        sprintf(log, "%s\t%s\t%s\t%d\t%s", request, filename, host, filelen, data);
    else if (strcmp(request, "PUT") == 0 && errCode == 0 && filename != NULL && filelen > 0 && host != NULL && data != NULL)
        sprintf(log, "%s\t%s\t%s\t%d\t%s", request, filename, host, filelen, data);
    else if (errCode != 0)
    {

        sprintf(log, "%s\t%s\t%s\t%d\n", "FAIL", request, "HTTP/1.1", errCode);
    }
    logSize = strlen(log);
    fwrite(log, logSize, 1, log_file);
    fclose(log_file);
}

struct queue_int_s *clientQueue;
pthread_mutex_t queue_mutex;
pthread_cond_t queue_has_client;

void *handle_connection(void *ptr)
{
    char command[17] = "";
    char protocol[17] = "";
    // char header[17] = "";
    char variable[17] = "";
    char filename[20] = "";
    int contentlen = 0, clientSocket;
    int filelen = 0;
    int buflen = BUFLEN;
    int i = 0;
    char *p = recvbuf;

    printf("\n\n=== new connection!!! ===\n");

    int id = *((int *)ptr);

    while (1)
    {
        clientSocket = -1;
        pthread_mutex_lock(&queue_mutex);
        {
            if (is_empty_queue(clientQueue))
            {
                pthread_cond_wait(&queue_has_client, &queue_mutex);
            }
            else
                /* Take 1 client out of the queue */
                dequeue(clientQueue, &clientSocket);
        }
        pthread_mutex_unlock(&queue_mutex);

        if (clientSocket >= 0)
        {

            recvbuf[buflen] = 0;
            buflen = recv(clientSocket, recvbuf, sizeof(recvbuf), 0); // receive all bytes from socket
            if (buflen == 0)
            {
                send_result(clientSocket, 501, "Internal Server Error");
                // recordLog("FAIL", NULL, NULL, 0, NULL, 500);
            }
            printf("\n%d bytes received.\n\n", buflen);
            printf("%s", recvbuf);

            // get command
            for (i = 0; i < buflen && i < 16; i++)
            {
                if (p[i] == ' ')
                {
                    strncpy(command, p, i);
                    command[i] = 0;
                    i++;
                    p += i;
                    buflen -= i;
                    break;
                }
            }
            printf("command: %s\n", command);

            if (strcmp(command, "HEAD") == 0)
            { // HEAD command process
                if (p[0] != '/')
                {
                    send_result(clientSocket, 400, "Bad Request");
                    if (isLogging == true)
                        recordLog("HEAD", NULL, NULL, 0, NULL, 400);
                }

                // get filename
                p++;
                buflen--;
                for (i = 0; i < buflen && i < 19; i++)
                {
                    if (p[i] == ' ')
                    {
                        strncpy(filename, p, i);
                        filename[i] = 0;
                        i++;
                        p += i;
                        buflen -= i;
                        break;
                    }
                    if (!isalnum(p[i]) && p[i] != '_' && p[i] != '.')
                        break;
                }
                if (filename[0] == 0)
                {
                    send_result(clientSocket, 400, "Bad Request");
                    if (isLogging == true)
                        recordLog("HEAD", NULL, NULL, 0, NULL, 400);
                }
                printf("filename: %s\n", filename);

                // get protocol
                for (i = 0; i < buflen && i < 16; i++)
                {
                    if (p[i] == '\r')
                    {
                        strncpy(protocol, p, i);
                        protocol[i] = 0;
                        i += 2;
                        p += i;
                        buflen -= i;
                        break;
                    }
                }
                if (strcmp(protocol, "HTTP/1.1") != 0)
                {
                    send_result(clientSocket, 400, "Bad Request");
                    if (isLogging == true)
                        recordLog("HEAD", NULL, NULL, 0, NULL, 400);
                }
                printf("protocol: %s\n", protocol);

                if (!check_host(p, buflen))
                {
                    send_result(clientSocket, 400, "Bad Request");
                    if (isLogging == true)
                        recordLog("HEAD", NULL, NULL, 0, NULL, 400);
                }
                // get file size and send response
                filelen = get_file_size(filename);
                if (filelen == -1)
                {
                    send_result(clientSocket, 400, "File Not Found");
                    if (isLogging == true)
                        recordLog("HEAD", NULL, NULL, 0, NULL, 400);
                }
                if (filelen == -2)
                {
                    send_result(clientSocket, 403, "Forbidden");
                    if (isLogging == true)
                        recordLog("HEAD", NULL, NULL, 0, NULL, 403);
                }

                sprintf(sendbuf, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", filelen);
                send_string(clientSocket, sendbuf);

                if (isLogging == true)
                {
                    recordLog("HEAD", filename, hostName, filelen, NULL, 0);
                }
            }

            else if (strcmp(command, "GET") == 0)
            { // GET command process
                if (p[0] != '/')
                {
                    send_result(clientSocket, 400, "Bad Request");
                    if (isLogging == true)
                        recordLog("GET", NULL, NULL, 0, NULL, 400);
                }

                // get filename
                p++;
                buflen--;
                for (i = 0; i < buflen && i < 19; i++)
                {
                    if (p[i] == ' ')
                    {
                        strncpy(filename, p, i);
                        filename[i] = 0;
                        i++;
                        p += i;
                        buflen -= i;
                        break;
                    }
                    if (!isalnum(p[i]) && p[i] != '_' && p[i] != '.')
                        break;
                }
                if (filename[0] == 0)
                {
                    if (isLogging == true)
                        recordLog("GET", NULL, NULL, 0, NULL, 400);

                    send_result(clientSocket, 400, "Bad Request");
                    // if (isLogging == true)
                    //	recordLog("GET", NULL, NULL, 0, NULL, 400);
                }

                if (!strcmp(filename, "healthcheck"))
                {
                    int lines = getLineOfLog();
                    int errCount = getErrOfLog();
                    sprintf(sendbuf, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n%d\n%d\n", 50, errCount, lines);
                    send_string(clientSocket, sendbuf);
                    send_bin(clientSocket, recvbuf, 50);
                }
                else
                {
                    printf("filename: %s\n", filename);
                    // get protocol
                    for (i = 0; i < buflen && i < 16; i++)
                    {
                        if (p[i] == '\r')
                        {
                            strncpy(protocol, p, i);
                            protocol[i] = 0;
                            i += 2;
                            p += i;
                            buflen -= i;
                            break;
                        }
                    }
                    if (strcmp(protocol, "HTTP/1.1") != 0)
                    {
                        send_result(clientSocket, 400, "Bad Request");
                        if (isLogging == true)
                            recordLog("GET", NULL, NULL, 0, NULL, 400);
                    }
                    printf("protocol: %s\n", protocol);

                    if (!check_host(p, buflen))
                    {
                        send_result(clientSocket, 400, "Bad Request");
                        if (isLogging == true)
                            recordLog("GET", NULL, NULL, 0, NULL, 400);
                    }

                    // get file content and send response
                    filelen = get_file(filename, recvbuf, contentlen);
                    if (filelen == -1)
                    {
                        send_result(clientSocket, 404, "File Not Found");
                        if (isLogging == true)
                            recordLog("GET", NULL, NULL, 0, NULL, 404);
                    }

                    if (filelen == -2)
                    {
                        send_result(clientSocket, 403, "Forbidden");
                        if (isLogging == true)
                            recordLog("GET", NULL, NULL, 0, NULL, 403);
                    }

                    sprintf(sendbuf, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", filelen);
                    send_string(clientSocket, sendbuf);
                    send_bin(clientSocket, recvbuf, filelen);
                    if (isLogging == true)
                    {
                        recordLog("GET", filename, hostName, filelen, recvbuf, 0);
                    }
                }
            }
            else if (strcmp(command, "PUT") == 0)
            { // PUT command process
                if (p[0] != '/')
                {
                    send_result(clientSocket, 400, "Bad Request");
                    if (isLogging == true)
                        recordLog("PUT", NULL, NULL, 0, NULL, 400);
                }

                // get filename
                p++;
                buflen--;
                for (i = 0; i < buflen && i < 19; i++)
                {
                    if (p[i] == ' ')
                    {
                        strncpy(filename, p, i);
                        filename[i] = 0;
                        i++;
                        p += i;
                        buflen -= i;
                        break;
                    }
                    if (!isalnum(p[i]) && p[i] != '_' && p[i] != '.')
                        break;
                }
                if (filename[0] == 0)
                {
                    send_result(clientSocket, 400, "Bad Request");
                    if (isLogging == true)
                        recordLog("PUT", NULL, NULL, 0, NULL, 400);
                }
                printf("filename: %s\n", filename);

                // get protocol
                for (i = 0; i < buflen && i < 16; i++)
                {
                    if (p[i] == '\r')
                    {
                        strncpy(protocol, p, i);
                        protocol[i] = 0;
                        i += 2;
                        p += i;
                        buflen -= i;
                        break;
                    }
                }
                if (strcmp(protocol, "HTTP/1.1") != 0)
                {
                    send_result(clientSocket, 400, "Bad Request");
                    if (isLogging == true)
                        recordLog("PUT", NULL, NULL, 0, NULL, 400);
                }
                printf("protocol: %s\n", protocol);

                if (!check_host(p, buflen))
                {
                    send_result(clientSocket, 400, "Bad Request");
                    if (isLogging == true)
                        recordLog("PUT", NULL, NULL, 0, NULL, 400);
                }

                // get content length
                if (get_header(p, buflen, "Content-Length", variable))
                {
                    if (isnumber(variable))
                        contentlen = atoi(variable);
                    else
                        contentlen = 0;
                }

                if (contentlen == 0)
                {
                    send_result(clientSocket, 400, "Bad Request");
                    if (isLogging == true)
                        recordLog("PUT", NULL, NULL, 0, NULL, 400);
                }

                // receive content data
                for (i = 0; i < contentlen;)
                {
                    buflen = recv(clientSocket, recvbuf + i, contentlen - i, 0);
                    if (buflen == 0)
                    {
                        send_result(clientSocket, 500, "Internal Server Error");
                        if (isLogging == true)
                            recordLog("PUT", NULL, NULL, 0, NULL, 500);
                    }
                    i += buflen;
                }

                recvbuf[contentlen] = 0;

                // get file content and send response
                filelen = put_file(filename, recvbuf, contentlen);

                if (filelen == -1)
                {
                    send_result(clientSocket, 500, "Internal Server Error");
                    if (isLogging == true)
                        recordLog("PUT", NULL, NULL, 0, NULL, 500);
                }
                if (filelen == -2)
                {
                    send_result(clientSocket, 403, "Forbidden");
                    if (isLogging == true)
                        recordLog("PUT", NULL, NULL, 0, NULL, 403);
                }
                // send(clientSocket, "201 Created", 40, 0);
                send_result(clientSocket, 201, "Created");

                if (isLogging == true)
                {
                    recordLog("PUT", filename, hostName, filelen, recvbuf, 0);
                }
            }
            else
            { // not process other commands
                send_result(clientSocket, 501, "Not Implemented");
                if (isLogging == true)
                    recordLog("PUT", NULL, NULL, 0, NULL, 501);
            }
            close(clientSocket);
        }
    }

#ifdef _WIN32
    // closesocket(connfd);
#else
    close(clientSocket);

#endif

    printf("=== connection closed. ===\n");
    pthread_exit(NULL);
    return 0;
}

static const int DEFAULT_WORKER = 5;

void create_worker(pthread_t *workerArr, int *threadId_arr, int numWorker)
{
    int threadIndex;
    int returnVal;
    char message[100];

    for (threadIndex = 0; threadIndex < numWorker; threadIndex++)
    {
        threadId_arr[threadIndex] = threadIndex;

        returnVal = pthread_create(&workerArr[threadIndex], NULL, handle_connection, &threadId_arr[threadIndex]);

        if (returnVal)
        {
            printf("pthread_create() fails with error code %d", returnVal);
        }
    }
}

int main(int argc, char *argv[])
{
    int listenfd, flagSignal, fd, connfd, numThread, i, notOverload;
    int isSelectedPort = 0;

    pthread_t workerArr[DEFAULT_WORKER];
    int threadId_arr[DEFAULT_WORKER];

    // pthread_t threads[100]; //100 posible threads, just to be safe
    int thread_count = 0;
    int port = 0;

#ifdef _WIN32

    WSADATA wsaData = {0};
    int iResult = 0;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        errx(EXIT_FAILURE, "WSAStartup failed: %d", iResult);
        return 1;
    }

#endif
    numThread = DEFAULT_WORKER;
    if (isdigit(*argv[1]))
    {
        port = strtouint16(argv[1]);
        isSelectedPort = 1;
    }
    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-N") == 0)
        {
            numThread = atoi(argv[i + 1]);
            if (isSelectedPort == 0 && isdigit(*argv[i + 2]))
                port = strtouint16(argv[i + 2]);
        }
        if (strcmp(argv[i], "-l") == 0)
        {
            strcpy(logfile_name, argv[i + 1]);
            isLogging = true;
        }
    }
    pthread_t *threads;
    threads = malloc(numThread * sizeof(pthread_t));

    if (port == 0)
    {
        errx(EXIT_FAILURE, "invalid port number: %s", port);
    }

    listenfd = create_listen_socket(port);

    clientQueue = create_queue();
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&queue_has_client, NULL);

    /********  create worker thread  ***********/
    create_worker(workerArr, threadId_arr, numThread);

    while (1)
    {
        connfd = accept(listenfd, NULL, NULL);
        if (connfd < 0)
        {
            perror("Accept Error");
            return (errno);
        }
        pthread_mutex_lock(&queue_mutex);
        {
            flagSignal = 0;
            if (is_empty_queue(clientQueue))
                /* There must be workers waiting, so send signal */
                flagSignal = 1;
            notOverload = enqueue(clientQueue, connfd);
            if (flagSignal)
                pthread_cond_broadcast(&queue_has_client);
        }
        pthread_mutex_unlock(&queue_mutex);
    }

    // fclose(log_file);
    return EXIT_SUCCESS;
}
