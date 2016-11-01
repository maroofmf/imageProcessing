
# Macros:
OBJS = main.o imageData.o imageAlgorithms.o matrix.o
cc = clang++ -std=c++11 -w
DEBUG = -g
CFLAGS1 = `pkg-config --cflags opencv`
CFLAGS = -c -g
LIBS = `pkg-config --libs opencv`
OUTPUTFOLDERS = metadata p3_a_output p3_b_output p3_c_output  p2_a_output p2_b_output p2_c_output p1_a_output p1_b_output p1_c_output

# List of arguments:
ARGS1 = HW3\ Images/P1/Texture.raw texture 1 128 128 1 12
ARGS2 = HW3\ Images/P2/jeep.jpg jeepOut 3 500 380 4
ARGS3 = HW3\ Images/P3/Zebra.raw zebraOut 3 481 321 5
ARGS4 = HW\ 2\ images/Kitten_2.raw kitten_2_out 3 300 300 1
ARGS4 = HW\ 2\ images/piece.raw piece_out 3 500 500 2 HW\ 2\ images/Hillary.raw HW\ 2\ images/Trump.raw 3 512 512
ARGS5 = HW\ 2\ images/field.raw field_out 3 972 648 3 HW\ 2\ images/tartans.raw HW\ 2\ images/trojans.raw 3 350 146
ARGS6 = HW\ 2\ images/Rice.raw rice_out 3 690 500 5
ARGS7 = HW\ 2\ images/Butterfly.raw mpeg_out 1 335 320 6 HW\ 2\ images/Fly.raw 1 222 223 HW\ 2\ images/Probe.raw 1 496 502

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
	#./hw3_exe $(ARGS1)
	#./hw3_exe $(ARGS2)
	./hw3_exe $(ARGS3)
	#./hw2_exe $(ARGS4)
	#./hw2_exe $(ARGS5)
	#./hw2_exe $(ARGS6)
	#./hw2_exe $(ARGS7)

timeit:
	@echo "Timing the code"
	valgrind --tool=callgrind ./hw3_exe $(ARGS1)

