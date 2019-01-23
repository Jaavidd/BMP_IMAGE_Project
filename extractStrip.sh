#!/bin/bash
## Two arguments are taken: the strip width and the directory.

for i in $2/*
do
	if [ -x $i ] ## If a file is executable, it is skipped.
	then
		continue
	fi
	if [[ $i =~ \.BMP$ || $i =~ \.bmp$ ]] ## If a file is a BMP file it is processed.
	then
		./extractStrip $1 $i
	fi
done



for i in $2/*_S*.bmp
do
	mv $i ./ExtractedImages
done
