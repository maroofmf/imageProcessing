
# Macros:
OBJS = main.o imageData.o imageAlgorithms.o matrix.o
cc = g++
DEBUG = -g
CFLAGS = -c
OUTPUTFOLDERS = p3_a_output p3_b_output  p2_a_output p2_b_output p1_a_output p1_b_output p1_c_output

# List of arguments:
ARGS1 = HW\ 2\ images/house.raw houseOut 1 512 512 4
ARGS2 = HW\ 2\ images/Kitten_1.raw kitten_1_out 3 300 300 1
ARGS3 = HW\ 2\ images/Kitten_2.raw kitten_2_out 3 300 300 1
ARGS4 = HW\ 2\ images/piece.raw piece_out 3 500 500 2 HW\ 2\ images/Hillary.raw HW\ 2\ images/Trump.raw 3 512 512
ARGS5 = HW\ 2\ images/field.raw field_out 3 972 648 3 HW\ 2\ images/tartans.raw HW\ 2\ images/trojans.raw 3 350 146
ARGS6 = HW\ 2\ images/Rice.raw rice_out 3 690 500 5
ARGS7 = HW\ 2\ images/Butterfly.raw mpeg_out 1 335 320 6 HW\ 2\ images/Fly.raw 1 222 223 HW\ 2\ images/Probe.raw 1 496 502

# Code Execution:
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
	rm -rf *.o hw2_exe  *.raw *.out *.txt

run:
	@echo "Deleting all output folders"
	rm -rf $(OUTPUTFOLDERS)
	@echo "Running the executable files"
	mkdir $(OUTPUTFOLDERS)
	./hw2_exe $(ARGS1)
	./hw2_exe $(ARGS2)
	./hw2_exe $(ARGS3)
	./hw2_exe $(ARGS4)
	./hw2_exe $(ARGS5)
	./hw2_exe $(ARGS6)
	./hw2_exe $(ARGS7)
