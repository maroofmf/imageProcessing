############################### Description ###############################
# EE 569 Homework 1
# Date: 18 September 2016
# Name: Maroof Mohammed Farooq 
# USCID : 7126869945 
# Email : maroofmf@usc.edu
# Operating System: OSX El Capitan -v 10.11.6
# Compiler: g++

############################# README #########################################
# (1)  All the input image folders (P1,P2,P3) from the question should be included 
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
OBJS = main.o imageData.o imageAlgorithms.o
cc = g++
DEBUG = -g
CFLAGS = -c
OUTPUTFOLDERS = p1_a_output p1_b_output p2_a_output p2_b_output p2_c_output p2_d_output p3_a_output p3_b_output

################################### Aruguments #################################
# Format of arguments: inputImagePath outputImageName BytesPerPixel imageWidth imageHeight problemNumber cropRow cropColumn cropWidth cropHeight 
ARGS1 = P1/Anna.raw annaOut 3 250 300 1 66 40 150 150
ARGS2 = P1/Rebel.raw rebelOut 3 200 300 1 28 33 150 150
ARGS3 = P1/Clownfish.raw clownOut 3 600 400 2
ARGS4 = P1/Octopus.raw octopusOut 3 500 374 2
ARGS5 = P1/Turtle.raw turtleOut 3 400 250 3
ARGS6 = P1/Jellyfish.raw jellyOut 3 500 333 3
ARGS7 = P2/Beach_bright.raw beach_bright 1 500 375 4
ARGS8 = P2/Beach_dark.raw beach_dark 1 500 375 4
ARGS9 = P2/Skyline.raw skylineOut 3 600 400 5
ARGS10 = P2/Trojan.raw trojanOut 3 450 300 6
ARGS11 = P2/Park.raw parkOut 3 259 194 6
ARGS12 = P2/Student_1.raw student_1_out 1 500 332 7
ARGS13 = P2/Student_2.raw student_2_out 1 500 375 7
ARGS14 = P3/Lena.raw lenaOut 3 512 512 8 P3/Lena_noisy.raw 
ARGS15 = P3/Buildings.raw buildingsOut 3 512 512 8 P3/Buildings_noisy.raw 
ARGS16 = P3/Trees.raw treesOut 3 774 518 8 P3/Trees_noisy.raw 
ARGS17 = P3/Lena.raw lenaOut 3 512 512 9 P3/Lena_noisy.raw 
ARGS18 = P3/Buildings.raw buildingsOut 3 512 512 9 P3/Buildings_noisy.raw 
ARGS19 = P3/Trees.raw treesOut 3 774 518 9 P3/Trees_noisy.raw 

################################ Code Execution and Linking ##################
all: hw1_exe run	

hw1_exe: $(OBJS)
	@echo "Linking the code"
	$(cc) $(OBJS) -o hw1_exe

main.o: main.cpp
	@echo "Compiling main.cpp in progress"
	$(cc) $(CFLAGS) main.cpp

imageData.o: imageData.h imageData.cpp
	$(cc) $(CFLAGS) imageData.cpp

imageAlgorithms.o: imageAlgorithms.h imageAlgorithms.cpp
	$(cc) $(CFLAGS) imageAlgorithms.cpp

clean:
	@echo "Cleaning folder by deleting object and executable files"
	rm -rf $(OUTPUTFOLDERS)
	rm -rf *.o hw1_exe  *.raw *.out 
	

run:
	@echo "Deleting all output folders"
	rm -rf $(OUTPUTFOLDERS)
	@echo "Running the executable files"
	mkdir $(OUTPUTFOLDERS)
	./hw1_exe $(ARGS1)
	./hw1_exe $(ARGS2)
	./hw1_exe $(ARGS3)
	./hw1_exe $(ARGS4)
	./hw1_exe $(ARGS5)
	./hw1_exe $(ARGS6)
	./hw1_exe $(ARGS7)
	./hw1_exe $(ARGS8)
	./hw1_exe $(ARGS9)
	./hw1_exe $(ARGS10)
	./hw1_exe $(ARGS11)
	./hw1_exe $(ARGS12)
	./hw1_exe $(ARGS13)
	./hw1_exe $(ARGS14)
	./hw1_exe $(ARGS15)
	./hw1_exe $(ARGS16)
	./hw1_exe $(ARGS17)
	./hw1_exe $(ARGS18)
	./hw1_exe $(ARGS19)
	
############################### END ################################################
