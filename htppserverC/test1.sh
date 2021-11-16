port=8080
if (( "$#" == 1 )) && (( "$1" > 1023 )); then
	port="$1"
elif (( "$#" == 1 )); then
	echo "Warning: Port numbers less than 1024 are reserved. Defaulting to port 3010..."
elif [[ "$#" -ne 0 ]]; then
	echo "asgn0-test: Program takes up to 1 argument (port number). Exiting..."
	exit 1
fi

check_GET_Diff () {
	i=0
	while [ $i -lt $3 ]; do
		out=$(diff $1 <(timeout 5 curl -s localhost:$port/r"$(($2 + $i))".txt))

		if [ ! "$out" = "" ]; then
			printf "command: diff $1 <(curl -s localhost:$port/r$(($2 + $i)).txt)\n"
			break
		fi
		((++i))
	done
	
	if [ $? -ne 124 ]; then
		echo $out
	else
		echo timed out
	fi
}

printf "====Cleaning up files from previous runs if they exist, and clearing log_file====\n"
NUM_IN_FILES=2
iter=$(($NUM_IN_FILES+1))
while [ $iter -le $((3*$NUM_IN_FILES)) ]; do
	if [ -f r"$iter".txt ]; then
		rm -f r"$iter".txt
	fi
	((++iter))
done
> log_file

#### Create input files ####
iter=0
if [ ! -f "r1.txt" ]; then
	touch r1.txt
	echo "Creating smaller test txt file"
	while [ $iter -le 5 ];
	do
		cat httpserver.c >> r1.txt
		((++iter))
	done
fi

if [ ! -f "r2.txt" ]; then
	touch r2.txt
	echo "Creating smaller binary test file"
	cat httpserver >> r2.txt
fi

iter=0
if [ ! -f "r3.txt" ]; then
	touch r3.txt
	echo "Creating smaller Makefile test file"
	while [ $iter -le 5 ];
	do
		cat Makefile >> r3.txt
		((++iter))
	done
fi

if [ ! -f "r4.txt" ]; then
	touch r4.txt
	echo "Downloading larger test file"
	curl -s ftp://ccg.epfl.ch/epd/current/epd.seq | tac | tac | head -qn 40000 > r4.txt
fi

iter=0
if [ ! -f "r5.txt" ]; then
	touch r5.txt
	echo "Creating second large test file"
	while [ $iter -le 13 ]
	do
		cat httpserver.c >> r5.txt
		cat Makefile >> r5.txt
		((++iter))
	done
fi

if [ ! -f "r6.txt" ]; then
	touch r6.txt
	out=$(which head)
	cat $out >> r6.txt
fi

if [ ! -f "r7.txt" ]; then
	touch r7.txt
	echo "Creating larger mixed test file"
	cat r4.txt >> r7.txt
	cat r6.txt >> r7.txt
	cat r5.txt >> r7.txt
fi

iter=4
FILE=r"$iter".txt
out=$(check_GET_Diff $FILE $iter 1)

printf "Test $testCase: "
if [ "$out" = "" ]; then
    printf "PASS\n"
else
    printf "FAIL. Difference found. Called diff between $FILE and GET on $FILE\n"
fi

# testCase=1

# iter=1
# while [ $iter -le $NUM_IN_FILES ]; do
# 	FILE=r"$iter".txt
# 	out=$(check_GET_Diff $FILE $iter 1)

# 	printf "Test $testCase: "
# 	if [ "$out" = "" ]; then
# 		printf "PASS\n"
# 	else
# 		printf "FAIL. Difference found. Called diff between $FILE and GET on $FILE\n"
# 	fi

# 	((++iter))
# 	((++testCase))
# done