#!/bin/bash

sourceFile="$1"
exeFile="$2"

if gcc "$sourceFile" -o "$exeFile";
then
    echo "File $sourceFile compiled successfully!"
    echo "Runing $exeFile..."
    ./"$exeFile"
else
    echo "Errors were find in $sourceFile... Resolve errors and run the script!"
fi

