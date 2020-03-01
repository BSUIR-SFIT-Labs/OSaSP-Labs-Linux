#!/bin/bash

if [[ $# > 0 ]];
then
    echo "Arguments entered:" $@
    echo "Arguments entered:" $@ > Arguments.txt
    echo "Path to the file with the arguments:" $(realpath Arguments.txt) 
else
    echo "Enter at least one argument!"
fi


