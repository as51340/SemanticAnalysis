#!/bin/bash

cd ../build
cmake ..
make
cd ../test
#TEST
for test in *; do
	if [ -d "$test" ]; then
		cd ../build
		fileToTestIn=../test/"$test"/test.in
		fileToTestOut=../test/"$test"/test.out
		if [[ ! -f "$fileToTestIn" ]]; then
			fileToTestIn=../test/"$test"/"$test".in
			fileToTestOut=../test/"$test"/"$test".out
		fi
		./semantic < "$fileToTestIn"
		treeParsingFile="output.out"
		if cmp -s "$treeParsingFile" "$fileToTestIn"; then
			outputFile="errors.out"
			cmp -s "$outputFile" "$fileToTestOut"
			status=$?
			if [[ $status != 0 ]]; then
				printf "Test $test\n"
				printf 'Test failed\n'
				exit 1
			fi
		else
			printf "Test $test\n"
			printf 'Tree not parsed correctly, not running test!\n'
			exit 1
		fi
	fi
	printf "\n"
	cd ../test
done
