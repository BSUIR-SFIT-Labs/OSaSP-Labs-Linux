#!/bin/bash

fileExtension="$1"
directory="$2"

if [[ -d $directory ]];
then
    files=$(find "$directory" -type f -name "*.$fileExtension")
    
    for file in ${files}
    do
        fileName=$(basename -- $file)
        ln $file $directory$fileName
    done

    echo "Script execution is complete!"
else
    echo "Directory $directory not found!"
fi
