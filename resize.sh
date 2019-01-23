#!/bin/bash

for i in $1/*
do
	if [[ -x $i  ]] ## If a file is executable, it is skipped.
	then
		continue
	fi

	if [[ $i =~ \.BMP$ || $i =~ \.bmp$   ]] ## If a file is a BMP file it is processed.
	then
		./resize $i $2 
        
	fi
done


for i in $1/*Resized.bmp
do
	mv $i ./ResizedImages
done
