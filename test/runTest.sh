#!/bin/bash

testFolder=$1

cd ../build #sad sam u buildu od generatora
cmake ..
make # kompajlaj
./semantic < ../test/"$testFolder"/test.in # na ulazu stablo
cd .. #u rootu
outputFile="build/output.out"  
testFile="test/"$testFolder"/test.out" 
if cmp -s "$outputFile" "$testFile"; then
	printf 'Test passed\n'
else
	printf 'Test failed\n'
fi


