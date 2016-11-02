
# Macros:
OBJS = main.o imageData.o imageAlgorithms.o matrix.o
cc = clang++ -std=c++11 -w
DEBUG = -g
CFLAGS1 = `pkg-config --cflags opencv`
CFLAGS = -c -g
LIBS = `pkg-config --libs opencv`
OUTPUTFOLDERS = metadata p3_a_output p3_b_output p3_c_output  p2_a_output p2_b_output p2_c_output p1_a_output p1_b_output p1_c_output

# List of arguments:
# Format: $(PATH_TO_INPUT_IMAGE) $(OUTPUT_RAW_FILENAME) $(CHANNELS) $(IMAGE_WIDTH) $(IMAGE_HEIGHT) $(PROBLEM_NUMBER) $(ADDITIONAL_ARGUMENTS)

ARGS1 = HW3\ Images/P1/Texture.raw texture 1 128 128 1 12
ARGS2 = HW3\ Images/P1/Comb_1.raw combOut_1 1 500 425 2
ARGS3 = HW3\ Images/P1/Comb_2.raw combOut_2 1 512 512 2
ARGS4 = HW3\ Images/P1/Comb_2.raw combOut_2 1 512 512 3
ARGS5 = HW3\ Images/P2/jeep.jpg jeepOut 3 500 380 4
ARGS5 = HW3\ Images/P2/bus.jpg busOut 3 450 248 4
ARGS6 = HW3\ Images/P2/ matchOut 3 500 256 5
ARGS7 = HW3\ Images/P2/ matchOut 3 500 256 6
ARGS8 = HW3\ Images/P3/Zebra.raw zebraOut 3 481 321 7
ARGS9 = HW3\ Images/P3/Jaguar.raw jaguarOut 3 481 321 7
ARGS10 = HW3\ Images/P3/ SEOut 3 481 321 8

# Code Execution:
all: hw3_exe run

hw3_exe: $(OBJS)
	@echo "Linking the code"
	$(cc) $(LIBS) $(OBJS) -o hw3_exe

main.o: main.cpp
	@echo "Compiling main.cpp in progress"
	$(cc) $(LIBS) $(CFLAGS) main.cpp

imageData.o: imageData.h imageData.cpp
	$(cc) $(LIBS) $(CFLAGS) imageData.cpp

imageAlgorithms.o: imageAlgorithms.h imageAlgorithms.cpp
	$(cc) $(LIBS) $(CFLAGS) imageAlgorithms.cpp

matrix.o: matrix.h matrix.cpp
	$(cc) $(LIBS) $(CFLAGS) matrix.cpp

clean:
	@echo "Cleaning folder by deleting object and executable files"
	rm -rf $(OUTPUTFOLDERS)
	rm -rf *.o hw3_exe  *.raw *.out *.txt

run:
	@echo "Deleting all output folders"
	rm -rf $(OUTPUTFOLDERS)
	@echo "Running the executable files"
	mkdir $(OUTPUTFOLDERS)
	./hw3_exe $(ARGS1)
	#./hw3_exe $(ARGS2)
	#./hw3_exe $(ARGS3)
	#./hw3_exe $(ARGS4)
	#./hw3_exe $(ARGS5)
	#./hw3_exe $(ARGS6)
	#./hw3_exe $(ARGS7)
	#./hw3_exe $(ARGS8)
	#./hw3_exe $(ARGS9)
	#./hw3_exe $(ARGS10)

timeit:
	@echo "Timing the code"
	valgrind --tool=callgrind ./hw3_exe $(ARGS1)