## Description:

> **EE 569 Homework 3**  
> **Date:** 6 November 2016  
> **Name:** Maroof Mohammed Farooq   
> **USCID:** 7126869945  
> **Email:** maroofmf@usc.edu  
> **Operating System:** OSX El Capitan -v 10.11.6  
> **Compiler:** g++    

## Guidelines for running the code:

* This code uses opencv 3.1. Make sure it is installed on your computer.
* The input folder (HW3 Images) from the question should be included
seperately in the source folder. Please note that the paths for images
are passed to the program.
* To compile and run the program just type:   
> make 
* To clean all the folders and object files in the folder run:       
> make clean
* The output images will be stored in the folder with names based on 
the question number.
* Please change the arguments in makefile if new test images are used.
* Input and output images are in .raw format.

## Troubleshooting:

* If you get the following error because of the makefile:  
> ld: library not found for -lippicv  
* This just means that ippicv is not found inside /usr/local/lib. To fix
this, please find "libippicv.a" in your system by running:  
> find /usr/local -name "libippicv.a"
* This will give you the location for libippicv.a. Then symbolically
link it to /usr/local/lib by running:  
> ln -s OUTPUT_FIND /usr/local/lib/
* Make sure to replace OUTPUT_FIND with the result in the previous step!


