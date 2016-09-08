# 
#
#

# Macros for makefile
OBJS = main.o imageData.o
cc = g++
DEBUG = -g
CFLAGS = -c
ARGS1 = P1/Anna.raw test.raw 3 250 300
ARGS = P1/Clownfish.raw colorSpace.raw 600 400 3
 
p1_a_exe: $(OBJS)
	@echo "Linking the code"
	$(cc) $(OBJS) -o p1_a_exe

main.o: main.cpp
	@echo "Compiling main.cpp in progress"
	$(cc) $(CFLAGS) main.cpp

imageData.o: imageData.h imageData.cpp
	$(cc) $(CFLAGS) imageData.cpp

clean:
	@echo "Cleaning folder by deleting object and executable files"
	rm *.o p1_a_exe  *.raw

run:
	@echo "Running the executable file"
	./p1_a_exe $(ARGS)
