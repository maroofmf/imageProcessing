## Description:

> **EE 569 Homework 4**  
> **Date:** 4 December 2016  
> **Name:** Maroof Mohammed Farooq   
> **USCID:** 7126869945  
> **Email:** maroofmf@usc.edu  
> **Operating System:** OSX El Capitan -v 10.11.6  
> **Dependencies:** nn, optim, image, gnuplot

## Programming file description:

* cnn_p1:
    * Training and testing LeNet-5 architecture on MNIST dataset.
    * Plotting accuracy vs epochs graphs on training and testing sets.
    
* cnn_p2:
    * Testing the trained model from cnn_p1 on negative images
    * Training a new network for better performance on original and   
    negative images.
    * Plotting accuracy vs epochs graphs on training and testing sets.
    
* cnn_p3:
    * Training and testing LeNet-5 architecture on colored mnist dataset.
    * Plotting accuracy vs epochs graphs on training and testing sets.
    
* cnn_p4:
    * Testing Translation Invariance of model trained from cnn_p1.
    * Plotting accuracy vs translational invariances on testing data.
    
* cnn_p5:
Upda
 
## Guidelines for running the code:
 
* This code uses torch. Make sure you have installed the latest version  
 of torch on your machine. 
* The input data (mnist test and train) must be contained in the source  
  folder.
* To compile and start training just type:   
> make 
* To clean all the folders and object files in the folder run:       
> make clean
* The output models are stored in models folder created by the program.  
* Input and output images als
in .t7 format.