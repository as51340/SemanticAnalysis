
#!/bin/bash

test=$1

cd ../build
cmake ..
make

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
			printf 'Test failed\n'
	else
		printf 'test passed\n'
	fi
else
	printf 'Tree not parsed correctly, not running test!\n'
fi
