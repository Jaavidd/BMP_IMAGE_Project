
There are some code for extract strip from the middle of the bmp image with the given width.
For extract,write in command:   make  makefile_for_extract,then ./extractStrip.sh -Width DirectoryName
or ./extractStrip -Width FileName 

You can resize image by taking different psf(Pixel Scaling Factor) between two or more images.
For this,type: make  makefile_for_resize,then ./resize.sh DirectoryName PsfValues.txt
or ./resize FileName PsfValues.txt

Also,after extracting and resizing,you can glue all images together by typing: glue.sh

Here you can see 2 bmp images and test them as an example.



