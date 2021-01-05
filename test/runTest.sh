
#!/bin/bash

testFolder=$1

cd ../build #sad sam u buildu od generatora
cmake .. #currFolder = build
make # kompajlaj, currFolder = build
./semantic < ../test/"$testFolder"/test.in # na ulazu stablo, najprije provjeri da li je dobro stablo isparsirano, currFolder = build
treeParsingFile="output.out" #u buildu current folder
treeParsingFileCorrect="../test/"$testFolder"/test.in"
if cmp -s "$treeParsingFile" "$treeParsingFileCorrect"; then
	printf 'Tree parsed correctly!\n'
else
	printf 'Tree not parsed correctly, not running test!\n'
	exit 1
fi
cd .. #u rootu
outputFile="build/errors.out"
testFile="test/"$testFolder"/test.out"
if cmp -s "$outputFile" "$testFile"; then
	printf 'Test passed\n'
else
	printf 'Test failed\n'
fi
