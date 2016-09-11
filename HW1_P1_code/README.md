# Name: Maroof Mohammed Farooq 
# 
#

# Macros for makefile
OBJS = main.o imageData.o imageAlgorithms.o
cc = g++
DEBUG = -g
CFLAGS = -c
ARGS = P1/Anna.raw annaout.raw 3 250 300
ARGS1 = P2/Beach_dark.raw hist.raw 1 500 375
ARGS2 = P1/Clownfish.raw colorSpace.raw 3 600 400
ARGS3 = P1/Turtle.raw hsvspace.raw 3 400 250 

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
	rm *.o hw1_exe  *.raw *.out

run:
	@echo "Running the executable file"
	./hw1_exe $(ARGS)
