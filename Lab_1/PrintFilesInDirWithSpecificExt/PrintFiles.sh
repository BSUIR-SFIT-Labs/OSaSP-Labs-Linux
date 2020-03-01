#!/bin/bash

if [[ -d "$2" ]];
then
    files=$(find "$2" -type f -name "*.$3")
    echo "${files}" > "$1"
else
    echo "Directory "$2" not found!"
fi

