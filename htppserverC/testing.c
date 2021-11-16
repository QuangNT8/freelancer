#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
/*

This only tests for GET and PUT

Make sure to be in same directory for both terminals

HOW TO RUN testScript:
1.) Terminal 1, run "make" and then "./httpserver 8080"
2.) Terminal 2, run "gcc testing.c" and then "./a.out"

*/

int main()
{
    int t1, t2, t3, t4;

    system("echo Creating test files. Please wait for them to be created...");
    system("rm -f x*.txt");
    system("rm -f y*.txt");
    system("head /dev/urandom -c 1000 > y1.txt");
    system("head /dev/urandom -c 5000 > y2.txt");
    system("head /dev/urandom -c 20500 > y3.txt");
    system("head /dev/urandom -c 40500 > y4.txt");
    system("head /dev/urandom -c 12345678 > y5.txt");
    system("head /dev/urandom -c 100200300 > y6.txt");
    system("head /dev/urandom -c 111222333 > y7.txt");
    system("touch x1.txt");
    system("touch x2.txt");
    system("touch x3.txt");
    system("touch x4.txt");
    system("echo Test files created!");

    // GET TESTS  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    system("echo GET tests  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");

    system("curl -s http://localhost:8080/y1.txt -o x1.txt");
    system("sleep 0.5");
    t1 = system("diff y1.txt x1.txt");
    if (t1 == 0)
    {
        printf("Test 1 PASSED\n");
    }
    else
    {
        printf("Test 1 failed\n");
    }

    system("curl -s http://localhost:8080/y1.txt -o x1.txt & curl -s http://localhost:8080/y2.txt -o x2.txt");
    system("sleep 0.5");
    t1 = system("diff y1.txt x1.txt");
    t2 = system("diff y2.txt x2.txt");
    if (t1 == 0 && t2 == 0)
    {
        printf("Test 2 PASSED\n");
    }
    else
    {
        printf("Test 2 failed\n");
    }

    system("curl -s http://localhost:8080/y3.txt -o x3.txt & curl -s http://localhost:8080/y4.txt -o x2.txt");
    system("sleep 0.5");
    t1 = system("diff y3.txt x3.txt");
    t2 = system("diff y4.txt x2.txt");
    if (t1 == 0 && t2 == 0)
    {
        printf("Test 3 PASSED\n");
    }
    else
    {
        printf("Test 3 failed\n");
    }

    system("curl -s http://localhost:8080/y5.txt -o x1.txt & curl -s http://localhost:8080/y6.txt -o x2.txt & curl -s http://localhost:8080/y7.txt -o x3.txt");
    system("sleep 0.5");
    t1 = system("diff y5.txt x1.txt");
    t2 = system("diff y6.txt x2.txt");
    t3 = system("diff y7.txt x3.txt");
    if (t1 == 0 && t2 == 0 && t3 == 0)
    {
        printf("Test 4 PASSED\n");
    }
    else
    {
        printf("Test 4 failed\n");
    }

    system("curl -s http://localhost:8080/y1.txt -o x1.txt & curl -s http://localhost:8080/y2.txt -o x2.txt & curl -s http://localhost:8080/y3.txt -o x3.txt & curl -s http://localhost:8080/y4.txt -o x4.txt");
    system("sleep 0.5");
    t1 = system("diff y1.txt x1.txt");
    t2 = system("diff y2.txt x2.txt");
    t3 = system("diff y3.txt x3.txt");
    t4 = system("diff y4.txt x4.txt");
    if (t1 == 0 && t2 == 0 && t3 == 0 && t4 == 0)
    {
        printf("Test 5 PASSED\n");
    }
    else
    {
        printf("Test 5 failed\n");
    }

    system("curl -s http://localhost:8080/y4.txt -o x1.txt & curl -s http://localhost:8080/y5.txt -o x2.txt & curl -s http://localhost:8080/y6.txt -o x3.txt & curl -s http://localhost:8080/y7.txt -o x4.txt");
    system("sleep 0.5");
    t1 = system("diff y4.txt x1.txt");
    t2 = system("diff y5.txt x2.txt");
    t3 = system("diff y6.txt x3.txt");
    t4 = system("diff y7.txt x4.txt");
    if (t1 == 0 && t2 == 0 && t3 == 0 && t4 == 0)
    {
        printf("Test 6 PASSED\n");
    }
    else
    {
        printf("Test 6 failed\n");
    }

    system("curl -s http://localhost:8080/y1.txt -o x1.txt & curl -s http://localhost:8080/y2.txt -o x2.txt & curl -s http://localhost:8080/y3.txt -o x3.txt");
    system("sleep 0.5");
    t1 = system("diff y1.txt x1.txt");
    t2 = system("diff y2.txt x2.txt");
    t3 = system("diff y3.txt x3.txt");
    if (t1 == 0 && t2 == 0 && t3 == 0)
    {
        printf("Test 7 PASSED\n");
    }
    else
    {
        printf("Test 7 failed\n");
    }

    // PUT TESTS  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    system("echo PUT tests  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");

    system("curl -s -T y5.txt http://localhost:8080/x2.txt");
    system("sleep 0.5");
    t1 = system("diff y5.txt x2.txt");
    if (t1 == 0)
    {
        printf("Test 1 PASSED\n");
    }
    else
    {
        printf("Test 1 failed\n");
    }

    system("curl -s -T y6.txt http://localhost:8080/x3.txt");
    system("sleep 0.5");
    t1 = system("diff y6.txt x3.txt");
    if (t1 == 0)
    {
        printf("Test 2 PASSED\n");
    }
    else
    {
        printf("Test 2 failed\n");
    }

    system("curl -s -T y1.txt http://localhost:8080/x1.txt & curl -s -T y2.txt http://localhost:8080/x2.txt");
    system("sleep 0.5");
    t1 = system("diff y1.txt x1.txt");
    t2 = system("diff y2.txt x2.txt");
    if (t1 == 0 && t2 == 0)
    {
        printf("Test 3 PASSED\n");
    }
    else
    {
        printf("Test 3 failed\n");
    }

    system("curl -s -T y3.txt http://localhost:8080/x3.txt & curl -s -T y4.txt http://localhost:8080/x4.txt");
    system("sleep 0.5");
    t1 = system("diff y3.txt x3.txt");
    t2 = system("diff y4.txt x4.txt");
    if (t1 == 0 && t2 == 0)
    {
        printf("Test 4 PASSED\n");
    }
    else
    {
        printf("Test 4 failed\n");
    }

    system("curl -s -T y5.txt http://localhost:8080/x1.txt & curl -s -T y6.txt http://localhost:8080/x2.txt");
    system("sleep 0.5");
    t1 = system("diff y5.txt x1.txt");
    t2 = system("diff y6.txt x2.txt");
    if (t1 == 0 && t2 == 0)
    {
        printf("Test 5 PASSED\n");
    }
    else
    {
        printf("Test 5 failed\n");
    }

    system("curl -s -T y3.txt http://localhost:8080/x1.txt & curl -s -T y4.txt http://localhost:8080/x2.txt & curl -s -T y5.txt http://localhost:8080/x3.txt & curl -s -T y6.txt http://localhost:8080/x4.txt");
    system("sleep 0.5");
    t1 = system("diff y3.txt x1.txt");
    t2 = system("diff y4.txt x2.txt");
    t3 = system("diff y5.txt x3.txt");
    t4 = system("diff y6.txt x4.txt");
    if (t1 == 0 && t2 == 0 && t3 == 0 && t4 == 0)
    {
        printf("Test 6 PASSED\n");
    }
    else
    {
        printf("Test 6 failed\n");
    }

    system("curl -s -T y1.txt http://localhost:8080/x5.txt & curl -s -T y2.txt http://localhost:8080/x6.txt");
    system("sleep 0.5");
    t1 = system("diff y1.txt x5.txt");
    t2 = system("diff y2.txt x6.txt");
    if (t1 == 0 && t2 == 0)
    {
        printf("Test 7 PASSED\n");
    }
    else
    {
        printf("Test 7 failed\n");
    }

    system("curl -s -T y3.txt http://localhost:8080/x7.txt & curl -s -T y4.txt http://localhost:8080/x8.txt");
    system("sleep 0.5");
    t1 = system("diff y3.txt x7.txt");
    t2 = system("diff y4.txt x8.txt");
    if (t1 == 0 && t2 == 0)
    {
        printf("Test 8 PASSED\n");
    }
    else
    {
        printf("Test 8 failed\n");
    }

    system("curl -s -T y3.txt http://localhost:8080/x9.txt & curl -s -T y4.txt http://localhost:8080/x10.txt & curl -s -T y5.txt http://localhost:8080/x11.txt & curl -s -T y6.txt http://localhost:8080/x12.txt");
    system("sleep 0.5");
    t1 = system("diff y3.txt x9.txt");
    t2 = system("diff y4.txt x10.txt");
    t3 = system("diff y5.txt x11.txt");
    t4 = system("diff y6.txt x12.txt");
    if (t1 == 0 && t2 == 0 && t3 == 0 && t4 == 0)
    {
        printf("Test 9 PASSED\n");
    }
    else
    {
        printf("Test 9 failed\n");
    }

    system("curl -s -T y7.txt http://localhost:8080/x1.txt & curl -s -T y4.txt http://localhost:8080/x2.txt & curl -s -T y5.txt http://localhost:8080/x13.txt & curl -s -T y6.txt http://localhost:8080/x14.txt");
    system("sleep 0.5");
    t1 = system("diff y7.txt x1.txt");
    t2 = system("diff y4.txt x2.txt");
    t3 = system("diff y5.txt x13.txt");
    t4 = system("diff y6.txt x14.txt");
    if (t1 == 0 && t2 == 0 && t3 == 0 && t4 == 0)
    {
        printf("Test 10 PASSED\n");
    }
    else
    {
        printf("Test 10 failed\n");
    }

    // PUT + GET AT SAME TIME
    system("echo PUT and GET at same time tests  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");

    system("curl -s  http://localhost:8080/y1.txt -o x1.txt & curl -s -T y2.txt http://localhost:8080/x2.txt");
    system("sleep 0.5");
    t1 = system("diff y1.txt x1.txt");
    t2 = system("diff y2.txt x2.txt");
    if (t1 == 0 && t2 == 0)
    {
        printf("Test 1 PASSED\n");
    }
    else
    {
        printf("Test 1 failed\n");
    }

    system("curl -s  http://localhost:8080/y3.txt -o x3.txt & curl -s -T y4.txt http://localhost:8080/x4.txt");
    system("sleep 0.5");
    t1 = system("diff y3.txt x3.txt");
    t2 = system("diff y4.txt x4.txt");
    if (t1 == 0 && t2 == 0)
    {
        printf("Test 2 PASSED\n");
    }
    else
    {
        printf("Test 2 failed\n");
    }

    system("curl -s http://localhost:8080/x1.txt -o x13.txt & curl -s -T y1.txt http://localhost:8080/x1.txt");
    system("sleep 1.5");
    t1 = system("diff x1.txt x13.txt");
    t2 = system("diff y1.txt x1.txt");
    if (t1 == 0 && t2 == 0)
    {
        printf("Test 3 PASSED\n");
    }
    else
    {
        printf("Test 3 failed\n");
    }

    system("curl -s -T y2.txt http://localhost:8080/x2.txt & curl -s http://localhost:8080/x2.txt -o x13.txt");
    system("sleep 1.5");
    t1 = system("diff y2.txt x2.txt");
    t2 = system("diff x2.txt x13.txt");
    if (t1 == 0 && t2 == 0)
    {
        printf("Test 4 PASSED\n");
    }
    else
    {
        printf("Test 4 failed\n");
    }

    return 0;
}