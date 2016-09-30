############################### Description ###############################
# EE 569 Homework 2
# Date: 9 October 2016
# Name: Maroof Mohammed Farooq 
# USCID : 7126869945 
# Email : maroofmf@usc.edu
# Operating System: OSX El Capitan -v 10.11.6
# Compiler: g++

############################# README #########################################
# (1)  The input folder  (HW 2 images) from the question should be included 
#      seperately in the source folder. Please note that the paths for images are passed to
#      the program.
#
# (2) To compile and run the program: make -f README.md
#
# (3) To clean all the folders and object files in the folder run: make -f README.md clean
#
# (4) The output images will be stored in the folder with names based on the question number.
#     Please change the arguments if new test images are used. 

################################ Macros ######################################
OBJS = main.o imageData.o imageAlgorithms.o matrix.o
cc = g++
DEBUG = -g
CFLAGS = -c
OUTPUTFOLDERS = p2_a_output p2_b_output p1_a_output p1_b_output p1_c_output

################################### Aruguments #################################
# Format of arguments: inputImagePath outputImageName BytesPerPixel imageWidth imageHeight problemNumber *additional problem parameters*

ARGS1 = HW\ 2\ images/house.raw houseOut 1 512 512 2
ARGS2 = HW\ 2\ images/Kitten_1.raw kitten_1_out 3 300 300 1
ARGS3 = HW\ 2\ images/Kitten_2.raw kitten_2_out 3 300 300 1

################################ Code Execution and Linking ##################
all: hw2_exe run	

hw2_exe: $(OBJS)
	@echo "Linking the code"
	$(cc) $(OBJS) -o hw2_exe

main.o: main.cpp
	@echo "Compiling main.cpp in progress"
	$(cc) $(CFLAGS) main.cpp

imageData.o: imageData.h imageData.cpp
	$(cc) $(CFLAGS) imageData.cpp

imageAlgorithms.o: imageAlgorithms.h imageAlgorithms.cpp
	$(cc) $(CFLAGS) imageAlgorithms.cpp

matrix.o: matrix.h matrix.cpp
	$(cc) $(CFLAGS) matrix.cpp
clean:
	@echo "Cleaning folder by deleting object and executable files"
	rm -rf $(OUTPUTFOLDERS)
	rm -rf *.o hw2_exe  *.raw *.out 
	

run:
	@echo "Deleting all output folders"
	rm -rf $(OUTPUTFOLDERS)
	@echo "Running the executable files"
	mkdir $(OUTPUTFOLDERS)
	./hw2_exe $(ARGS1)
	./hw2_exe $(ARGS2)
	#./hw2_exe $(ARGS3)
	
############################### END ################################################
