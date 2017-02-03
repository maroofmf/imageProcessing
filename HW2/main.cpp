// Name: Maroof Mohammed Farooq
// HW P2
// Image Processing

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <iterator>
#include <math.h>
#include <map>
#include "imageData.h"
#include "imageAlgorithms.h"
#include "matrix.h"

using namespace std;

int main(int argc, char *argv[])



{
	// Define file pointer and variables
	int BytesPerPixel;
	int imageWidth;
	int imageHeight;
	int problemNumber;
//----------------------------------------------------------------------------------------------------------------//
	// Check for proper syntax
	if (argc < 3){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw output_image.raw [BytesPerPixel = 1] [imageWidth = 256] [imageHeight = 256]" << endl;
		return 0;
	}
	
	// Check if image is grayscale or color
	if (argc < 4) {
		BytesPerPixel = 1;// default is grey image
		imageHeight = 256;
		imageWidth = 256;
	}
	else {
		BytesPerPixel = atoi(argv[3]);
		// Check if size is specified
		if (argc >= 5){
			imageWidth = atoi(argv[4]);
			imageHeight = atoi(argv[5]);
		}
		else{
			imageHeight = 256;
			imageWidth = 256;
		}
	}
	problemNumber = atoi(argv[6]);
//----------------------------------------------------------------------------------------------------------------//
// input and output file names
	string inputFileName = argv[1];
	string outputFileName = argv[2];

//----------------------------------------------------------------------------------------------------------------//
// (1a) Geometric warping

	if(problemNumber ==1){

		// Read Image
		imageData originalImage(BytesPerPixel,imageWidth,imageHeight);
		originalImage.imageRead(inputFileName.c_str());

		// Seperate color channels
		vector<imageData> colorChannels = originalImage.seperateChannels();

		// Save image for geometrical modification:
		// Perform diamond warping
		imageData manipulatedImage(BytesPerPixel,imageWidth,imageHeight);
		double start_x,end_x,start_y,end_y;
		start_x = manipulatedImage.imageToCartesian(0,0)[0];
		start_y = manipulatedImage.imageToCartesian(0,0)[1];
		end_x = manipulatedImage.imageToCartesian(imageHeight-1,imageWidth-1)[0];
		end_y = manipulatedImage.imageToCartesian(imageHeight-1,imageWidth-1)[1];
		unsigned char redPixelValue, greenPixelValue, bluePixelValue;
		int row,column;

		for(double yValue = start_y; yValue>=end_y; yValue--){
			for(double xValue = start_x; xValue<=end_x; xValue++){

				redPixelValue = colorChannels[0].diamondWarping(xValue,yValue);
				greenPixelValue = colorChannels[1].diamondWarping(xValue,yValue);
				bluePixelValue = colorChannels[2].diamondWarping(xValue,yValue);

				row = manipulatedImage.cartesianToImage(xValue,yValue)[0];
				column = manipulatedImage.cartesianToImage(xValue,yValue)[1];

				manipulatedImage.setPixelValues(redPixelValue,row,column,0);
				manipulatedImage.setPixelValues(greenPixelValue,row,column,1);
				manipulatedImage.setPixelValues(bluePixelValue,row,column,2);
			}
		}

		manipulatedImage.saveImage(("p1_a_output/"+outputFileName+".raw").c_str());

	}
//----------------------------------------------------------------------------------------------------------------//
// (1b) Puzzle matching

	if(problemNumber == 2) {

		// Input more arguments
		if(argc!=12){
			cout<< "Wrong number of inputs! Please check the arguments1" <<endl;
			exit(1);
		}

		// Reading command line for extra arguments
		string inputFileName_Puzzle1 = argv[7];
		string inputFileName_Puzzle2 = argv[8];
		int puzzleBytesPerPixel = atoi(argv[9]);
		int puzzleImageWidth = atoi(argv[10]);
		int puzzleImageHeight = atoi(argv[11]);

		//Read pieces images
		imageData piecesImage(BytesPerPixel,imageWidth,imageHeight);
		imageData image_Puzzle1(puzzleBytesPerPixel,puzzleImageWidth,puzzleImageHeight);
		imageData image_Puzzle2(puzzleBytesPerPixel,puzzleImageWidth,puzzleImageHeight);
		piecesImage.imageRead(inputFileName.c_str());
		image_Puzzle1.imageRead(inputFileName_Puzzle1.c_str());
		image_Puzzle2.imageRead(inputFileName_Puzzle2.c_str());

		// Color channel decomposition
		vector<imageData> colorChannels_puzzle1 = image_Puzzle1.seperateChannels();
		vector<imageData> colorChannels_puzzle2 = image_Puzzle2.seperateChannels();
		vector<imageData> colorChannels_pieces = piecesImage.seperateChannels();


		// Convert to gray
		imageData gray_pieces = piecesImage.colorToGrayscale();
		imageData gray_puzzle1 = image_Puzzle1.colorToGrayscale();
		imageData gray_puzzle2 = image_Puzzle2.colorToGrayscale();

		//Binarize the image
		imageAlgorithms on_pieces(&gray_pieces);
		imageAlgorithms on_puzzle1(&gray_puzzle1);
		imageAlgorithms on_puzzle2(&gray_puzzle2);
		on_pieces.binarize(254.00);
		on_puzzle1.binarize(254.00);
		on_puzzle2.binarize(254.00);

		//Finding corners
		map<int,vector<int> > corners_pieces;
		map<int,vector<int> > corners_puzzle1;
		map<int,vector<int> > corners_puzzle2;
		corners_pieces = on_pieces.harrisCornerDetector(4650000000000);
		corners_puzzle1 = on_puzzle1.harrisCornerDetector(1000000000000);
		corners_puzzle2 = on_puzzle2.harrisCornerDetector(1000000000000);

		// Calculating the star and end of cartesian coordinates of puzzle 1
		double start_x,end_x,start_y,end_y;
		start_x = image_Puzzle1.imageToCartesian(corners_puzzle1[0][0]-1,corners_puzzle1[0][1]-1)[0];
		start_y = image_Puzzle1.imageToCartesian(corners_puzzle1[0][0]-1,corners_puzzle1[0][1]-1)[1];
		end_x = image_Puzzle1.imageToCartesian(corners_puzzle1[3][0]+1,corners_puzzle1[3][1]+1)[0];
		end_y =image_Puzzle1.imageToCartesian(corners_puzzle1[3][0]+1,corners_puzzle1[3][1]+1)[1];

		unsigned char redPixelValue, greenPixelValue, bluePixelValue;
		int row,column;
		bool boundary;

		// Fill in the white space
		for(double yValue = start_y; yValue>=end_y; yValue--){
			for(double xValue = start_x; xValue<=end_x; xValue++){

				redPixelValue = colorChannels_puzzle1[0].puzzleMatching1(xValue,yValue,&colorChannels_pieces[0]);
				greenPixelValue = colorChannels_puzzle1[1].puzzleMatching1(xValue,yValue,&colorChannels_pieces[1]);
				bluePixelValue = colorChannels_puzzle1[2].puzzleMatching1(xValue,yValue,&colorChannels_pieces[2]);

				row = image_Puzzle1.cartesianToImage(xValue,yValue)[0];
				column = image_Puzzle1.cartesianToImage(xValue,yValue)[1];

				image_Puzzle1.setPixelValues(redPixelValue,row,column,0);
				image_Puzzle1.setPixelValues(greenPixelValue,row,column,1);
				image_Puzzle1.setPixelValues(bluePixelValue,row,column,2);
			}
		}

		// Save output image for puzzle1
		image_Puzzle1.saveImage(("p1_b_output/"+outputFileName+"_puzzle1.raw").c_str());

		// Calculating the star and end of cartesian coordinates of puzzle 2
		start_x = image_Puzzle2.imageToCartesian(corners_puzzle2[0][0]-1,corners_puzzle2[0][1]-1)[0];
		start_y = image_Puzzle2.imageToCartesian(corners_puzzle2[0][0]-1,corners_puzzle2[0][1]-1)[1];
		end_x = image_Puzzle2.imageToCartesian(corners_puzzle2[3][0]+1,corners_puzzle2[3][1]-1)[0];
		end_y =image_Puzzle2.imageToCartesian(corners_puzzle2[3][0]+1,corners_puzzle2[3][1]+1)[1];

		// Fill in the white space for puzzle 2
		for(double yValue = start_y; yValue>=end_y; yValue--){
			for(double xValue = start_x; xValue<=end_x; xValue++){

				redPixelValue = colorChannels_puzzle1[0].puzzleMatching2(xValue,yValue,&colorChannels_pieces[0]);
				greenPixelValue = colorChannels_puzzle1[1].puzzleMatching2(xValue,yValue,&colorChannels_pieces[1]);
				bluePixelValue = colorChannels_puzzle1[2].puzzleMatching2(xValue,yValue,&colorChannels_pieces[2]);

				row = image_Puzzle2.cartesianToImage(xValue,yValue)[0];
				column = image_Puzzle2.cartesianToImage(xValue,yValue)[1];

				image_Puzzle2.setPixelValues(redPixelValue,row,column,0);
				image_Puzzle2.setPixelValues(greenPixelValue,row,column,1);
				image_Puzzle2.setPixelValues(bluePixelValue,row,column,2);
			}
		}

		// Save output image for puzzle1
		image_Puzzle2.saveImage(("p1_b_output/"+outputFileName+"_puzzle2.raw").c_str());

	}

//----------------------------------------------------------------------------------------------------------------//
// (1c) Homographic Transformation and image overlay

	if(problemNumber == 3) {

		// Input more arguments
		if(argc!=12){
			cout<< "Wrong number of inputs! Please check the arguments1" <<endl;
			exit(1);
		}

		// Reading command line for extra arguments
		string inputFileName_text1 = argv[7];
		string inputFileName_text2 = argv[8];
		int textBytesPerPixel = atoi(argv[9]);
		int textImageWidth = atoi(argv[10]);
		int textImageHeight = atoi(argv[11]);

		//Read images
		imageData* hostImage = new imageData(BytesPerPixel,imageWidth,imageHeight);
		imageData* image_text1 = new imageData(textBytesPerPixel,textImageWidth,textImageHeight);
		imageData* image_text2 = new imageData(textBytesPerPixel,textImageWidth,textImageHeight);
		hostImage->imageRead(inputFileName.c_str());
		image_text1->imageRead(inputFileName_text1.c_str());
		image_text2->imageRead(inputFileName_text2.c_str());

		// Color channel decomposition
		vector<imageData> colorChannels_host = hostImage->seperateChannels();
		vector<imageData> colorChannels_text1 = image_text1->seperateChannels();
		vector<imageData> colorChannels_text2 = image_text2->seperateChannels();

		// Calculating the star and end of cartesian coordinates of host image
		double start_x,end_x,start_y,end_y;
		start_x = 356.5;
		start_y = 406.5;
		end_x = 775.5;
		end_y = 20.5;

		unsigned char redPixelValue, greenPixelValue, bluePixelValue;
		int row,column;

		// Fill in the mapped values to host image for text 1
		for(double yValue = start_y; yValue>=end_y; yValue--){
			for(double xValue = start_x; xValue<=end_x; xValue++){

				redPixelValue = colorChannels_host[0].textEmbedding1(xValue,yValue,&colorChannels_text1[0]);
				greenPixelValue = colorChannels_host[1].textEmbedding1(xValue,yValue,&colorChannels_text1[1]);
				bluePixelValue = colorChannels_host[2].textEmbedding1(xValue,yValue,&colorChannels_text1[2]);

				// Check for background
				if((redPixelValue<=20)&&(greenPixelValue<=20)&&(bluePixelValue<=20)){
					redPixelValue = colorChannels_host[0].getPixelValuesFrom_xy(xValue,yValue);
					greenPixelValue = colorChannels_host[1].getPixelValuesFrom_xy(xValue,yValue);
					bluePixelValue = colorChannels_host[2].getPixelValuesFrom_xy(xValue,yValue);
				}

				row = hostImage->cartesianToImage(xValue,yValue)[0];
				column = hostImage->cartesianToImage(xValue,yValue)[1];

				hostImage->setPixelValues(redPixelValue,row,column,0);
				hostImage->setPixelValues(greenPixelValue,row,column,1);
				hostImage->setPixelValues(bluePixelValue,row,column,2);
			}
		}

		// Save output image for text 1
		hostImage->saveImage(("p1_c_output/"+outputFileName+"_text1.raw").c_str());

		// Fill in the mapped values to host image for text 2
		for(double yValue = start_y; yValue>=end_y; yValue--){
			for(double xValue = start_x; xValue<=end_x; xValue++){

				redPixelValue = colorChannels_host[0].textEmbedding1(xValue,yValue,&colorChannels_text2[0]);
				greenPixelValue = colorChannels_host[1].textEmbedding1(xValue,yValue,&colorChannels_text2[1]);
				bluePixelValue = colorChannels_host[2].textEmbedding1(xValue,yValue,&colorChannels_text2[2]);

				// Check for background
				if((redPixelValue>100)&&(greenPixelValue>100)&&(bluePixelValue>100)){
					redPixelValue = colorChannels_host[0].getPixelValuesFrom_xy(xValue,yValue);
					greenPixelValue = colorChannels_host[1].getPixelValuesFrom_xy(xValue,yValue);
					bluePixelValue = colorChannels_host[2].getPixelValuesFrom_xy(xValue,yValue);
				}

				// Check for pixels out of range
				if((redPixelValue<=20)&&(greenPixelValue<=20)&&(bluePixelValue<=20)){
					redPixelValue = colorChannels_host[0].getPixelValuesFrom_xy(xValue,yValue);
					greenPixelValue = colorChannels_host[1].getPixelValuesFrom_xy(xValue,yValue);
					bluePixelValue = colorChannels_host[2].getPixelValuesFrom_xy(xValue,yValue);
				}

				row = hostImage->cartesianToImage(xValue,yValue)[0];
				column = hostImage->cartesianToImage(xValue,yValue)[1];

				hostImage->setPixelValues(redPixelValue,row,column,0);
				hostImage->setPixelValues(greenPixelValue,row,column,1);
				hostImage->setPixelValues(bluePixelValue,row,column,2);
			}
		}

		// Save output image for text 1
		hostImage->saveImage(("p1_c_output/"+outputFileName+"_text2.raw").c_str());
	}

//----------------------------------------------------------------------------------------------------------------//
// (2) Digital Halftoning

	if(problemNumber == 4) {

		// Read image
		imageData originalImage(BytesPerPixel,imageWidth,imageHeight);
		originalImage.imageRead(inputFileName.c_str());

		// Dither the image using 2x2 bayer index matrix
		imageData ditheredImage = originalImage;
		imageAlgorithms ditheringAlgo(&ditheredImage);
		ditheringAlgo.dithering(2,false);
		ditheredImage.saveImage(("p2_a_output/"+outputFileName+"_2x2"+".raw").c_str());

		// Dither the image using 8x8 bayer index matrix
		ditheredImage = originalImage;
		ditheringAlgo.dithering(8,false);
		ditheredImage.saveImage(("p2_a_output/"+outputFileName+"_8x8"+".raw").c_str());

		// Dither the image using 4x4 bayer index matrix
		ditheredImage = originalImage;
		ditheringAlgo.dithering(4, false);
		ditheredImage.saveImage(("p2_a_output/"+outputFileName+"_4x4"+".raw").c_str());

		// Dither the image using given bayer index matrix
		ditheredImage = originalImage;
		ditheringAlgo.dithering(4,true);
		ditheredImage.saveImage(("p2_a_output/"+outputFileName+"_a4"+".raw").c_str());

		// Dithering for four intensity levels
		ditheredImage = originalImage;
		ditheringAlgo.fourLevelDithering();
		ditheredImage.saveImage(("p2_a_output/"+outputFileName+"_4Level"+".raw").c_str());

		//  Halftoning using error diffusion (floyd-steinberg)
		ditheredImage = originalImage;
		ditheringAlgo.errorDiffusion("floyd-steinberg");
		ditheredImage.saveImage(("p2_b_output/"+outputFileName+"_floyd-steinberg"+".raw").c_str());

		//	Halftoning using error diffusion (JJN)
		ditheredImage = originalImage;
		ditheringAlgo.errorDiffusion("JJN");
		ditheredImage.saveImage(("p2_b_output/"+outputFileName+"_jjn"+".raw").c_str());

		// Halftoning using error diffusion (stucki)
		ditheredImage = originalImage;
		ditheringAlgo.errorDiffusion("stucki");
		ditheredImage.saveImage(("p2_b_output/"+outputFileName+"_stucki"+".raw").c_str());

	}


//----------------------------------------------------------------------------------------------------------------//
// (3a) Rice grain inspection:

	if(problemNumber == 5) {


		// Read input image
		imageData* riceImage = new imageData(BytesPerPixel,imageWidth,imageHeight);
		riceImage->imageRead(inputFileName.c_str());
		vector<imageData> colorChannels = riceImage->seperateChannels();

		// Color algorithms init
		imageAlgorithms onRedChannel(&colorChannels[0]);
		imageAlgorithms onGreenChannel(&colorChannels[1]);
		imageAlgorithms onBlueChannel(&colorChannels[2]);

		// Apply algorithms for color
		onRedChannel.medianFilter(3);
		onRedChannel.binarizeBasedOnMode(15,true);

		onGreenChannel.medianFilter(3);
		onGreenChannel.binarizeBasedOnMode(15,true);

		onBlueChannel.medianFilter(3);
		onBlueChannel.binarizeBasedOnMode(15,true);

		// Concatenate color channels
		imageData riceProcessed(BytesPerPixel,imageWidth,imageHeight);
		riceProcessed.concatenateChannels(colorChannels);

		// Convert image to grayscale
		imageData riceGrayImage = riceProcessed.colorToGrayscale();

		// Grayscale image to binary image
		imageAlgorithms onGrayImage(&riceGrayImage);
		onGrayImage.medianFilter(5);
		onGrayImage.binarizeBasedOnMode(15,true);

		// Get binary image data
		matrix<int,bool> binaryImageData = onGrayImage.charToBoolean();

		// Count the number of rice grains by applying shrinking:
		matrix<int,bool> shrinkedImageData = onGrayImage.morphologicalAlgorithms(binaryImageData,"Shrinking");
		int numberOfRiceGrains = shrinkedImageData.countNumberOfNonZero();
		cout<< "The  number of rice grains are: "<< numberOfRiceGrains <<endl;
		matrix<int,unsigned char> printableImageData = onGrayImage.booleanToChar(shrinkedImageData);
		imageData shrinkedImage(BytesPerPixel,imageWidth,imageHeight);
		shrinkedImage.setPixelValues(printableImageData.getMatrixValues());

		// Apply thinning to image
		matrix<int,bool> thinnedImageData = onGrayImage.morphologicalAlgorithms(binaryImageData,"Thinning");
		printableImageData = onGrayImage.booleanToChar(thinnedImageData);
		imageData thinnedImage(BytesPerPixel,imageWidth,imageHeight);
		thinnedImage.setPixelValues(printableImageData.getMatrixValues());

		// Save output image
		riceGrayImage.saveImage(("p3_a_output/"+outputFileName+"_binarized.raw").c_str());
		shrinkedImage.saveImage(("p3_a_output/"+outputFileName+"_shrinked.raw").c_str());
		thinnedImage.saveImage(("p3_a_output/"+outputFileName+"_thinned.raw").c_str());
	}

//----------------------------------------------------------------------------------------------------------------//
// (3b) MPEG - 7 Dataset:

	if(problemNumber == 6) {

		// Input more arguments
		if(argc!=15){
			cout<< "Wrong number of inputs! Please check the arguments for ARGS7" <<endl;
			exit(1);
		}

		// Reading command line for extra arguments
		string inputFileName_image2 = argv[7];
		int BytesPerPixel_image2 = atoi(argv[8]);
		int imageWidth_image2 = atoi(argv[9]);
		int imageHeight_image2 = atoi(argv[10]);

		string inputFileName_testImage = argv[11];
		int BytesPerPixel_testImage = atoi(argv[12]);
		int imageWidth_testImage = atoi(argv[13]);
		int imageHeight_testImage = atoi(argv[14]);



		// Read input image
		imageData* butterflyImage = new imageData(BytesPerPixel,imageWidth,imageHeight);
		imageData* flyImage = new imageData(BytesPerPixel_image2,imageWidth_image2,imageHeight_image2);
		imageData* probeImage = new imageData(BytesPerPixel_testImage,imageWidth_testImage,imageHeight_testImage);
		butterflyImage->imageRead(inputFileName.c_str());
		flyImage->imageRead(inputFileName_image2.c_str());
		probeImage->imageRead(inputFileName_testImage.c_str());

		//-----------------------------------------------------------------------//
		//Butterfly image:

		imageData butterflyOutput(BytesPerPixel,imageWidth,imageHeight);
		butterflyOutput = *butterflyImage;

		// Initialize algorithms for butterfly
		imageAlgorithms onButterflyImage(&butterflyOutput);

		// Hole filling operation
		matrix<int,bool> butterflyHoleFilled = onButterflyImage.holeFilling();
		matrix<int,unsigned char> tempCharData = onButterflyImage.booleanToChar(butterflyHoleFilled);
		butterflyOutput.setPixelValues(tempCharData.getMatrixValues());
		butterflyOutput.saveImage(("p3_b_output/"+outputFileName+"_butterfly_holeFilled.raw").c_str());

		// Boundary smoothening using dilation and erosion:
		matrix<int,bool> butterflyBoundarySmoothen = onButterflyImage.dilation(butterflyHoleFilled,2);
		butterflyBoundarySmoothen = onButterflyImage.erosion(butterflyBoundarySmoothen,1);
		tempCharData = onButterflyImage.booleanToChar(butterflyBoundarySmoothen);
		butterflyOutput.setPixelValues(tempCharData.getMatrixValues());
		butterflyOutput.saveImage(("p3_b_output/"+outputFileName+"_butterfly_smoothened.raw").c_str());

		// Skeletonization
		matrix<int,bool> butterflySkeletonizing = onButterflyImage.morphologicalAlgorithms(butterflyBoundarySmoothen,"Skeletonizing");
		tempCharData = onButterflyImage.booleanToChar(butterflySkeletonizing);
		butterflyOutput.setPixelValues(tempCharData.getMatrixValues());
		butterflyOutput.saveImage(("p3_b_output/"+outputFileName+"_butterfly_skeletonized.raw").c_str());

		// Thinning
		matrix<int,bool> butterflyThinning = onButterflyImage.morphologicalAlgorithms(butterflyBoundarySmoothen,"Thinning");
		tempCharData = onButterflyImage.booleanToChar(butterflyThinning);
		butterflyOutput.setPixelValues(tempCharData.getMatrixValues());
		butterflyOutput.saveImage(("p3_b_output/"+outputFileName+"_butterfly_thinning.raw").c_str());
		//-----------------------------------------------------------------------//
		//Fly image:

		imageData flyOutput(BytesPerPixel_image2,imageWidth_image2,imageHeight_image2);
		flyOutput.setPixelValues(flyImage->getPixelValues());

		// Save fly image in matrix:
		matrix<int, unsigned char> flyMatrix(flyImage->getImageHeight(),flyImage->getImageWidth(),1);
		flyMatrix.setMatrixValues(flyImage->getPixelValues());

		// Extend matrices:
		flyMatrix.zeroPad(1);
		imageData flyPadded(flyMatrix.getDepth(),flyMatrix.getWidth(),flyMatrix.getHeight());
		flyPadded.setPixelValues(flyMatrix.getMatrixValues());

		// Initialize algorithms:
		imageAlgorithms onFlyPadded(&flyPadded);
		imageAlgorithms onFly(&flyOutput);

		// Apply hole filling on fly
		matrix<int,bool> flyHoleFilled = onFlyPadded.holeFilling();
		flyHoleFilled.removeZeroPadding(1);
		tempCharData = onFlyPadded.booleanToChar(flyHoleFilled);
		flyOutput.setPixelValues(tempCharData.getMatrixValues());
		flyOutput.saveImage(("p3_b_output/"+outputFileName+"_fly_holeFilled.raw").c_str());

		// Boundary smoothening using dilation and erosion:
		matrix<int,bool> flyBoundarySmoothen = onFly.erosion(flyHoleFilled,2);
		flyBoundarySmoothen = onFly.dilation(flyBoundarySmoothen,1);
		tempCharData = onFly.booleanToChar(flyBoundarySmoothen);
		flyOutput.setPixelValues(tempCharData.getMatrixValues());
		flyOutput.saveImage(("p3_b_output/"+outputFileName+"_fly_smoothened.raw").c_str());

		// Skeletonization
		matrix<int,bool> flySkeletonizing = onFly.morphologicalAlgorithms(flyBoundarySmoothen,"Skeletonizing");
		tempCharData = onFly.booleanToChar(flySkeletonizing);
		flyOutput.setPixelValues(tempCharData.getMatrixValues());
		flyOutput.saveImage(("p3_b_output/"+outputFileName+"_fly_skeletonized.raw").c_str());

		// Thinning
		matrix<int,bool> flyThinning = onFly.morphologicalAlgorithms(flyBoundarySmoothen,"Thinning");
		tempCharData = onFly.booleanToChar(flyThinning);
		flyOutput.setPixelValues(tempCharData.getMatrixValues());
		flyOutput.saveImage(("p3_b_output/"+outputFileName+"_fly_thinning.raw").c_str());

		//-----------------------------------------------------------------------//
		//Probe image:

		imageData probeOutput(BytesPerPixel_testImage,imageWidth_testImage,imageHeight_testImage);
		probeOutput.setPixelValues(probeImage->getPixelValues());

		// Save fly image in matrix:
		matrix<int, unsigned char> probeMatrix(probeImage->getImageHeight(),probeImage->getImageWidth(),1);
		probeMatrix.setMatrixValues(probeImage->getPixelValues());

		// Extend matrices:
		probeMatrix.zeroPad(1);
		imageData probePadded(probeMatrix.getDepth(),probeMatrix.getWidth(),probeMatrix.getHeight());
		probePadded.setPixelValues(probeMatrix.getMatrixValues());

		// Initialize algorithms:
		imageAlgorithms onProbePadded(&probePadded);
		imageAlgorithms onProbe(&probeOutput);

		// Apply hole filling on probe
		matrix<int,bool> probeHoleFilled = onProbePadded.holeFilling();
		probeHoleFilled.removeZeroPadding(1);
		tempCharData = onProbePadded.booleanToChar(probeHoleFilled);
		probeOutput.setPixelValues(tempCharData.getMatrixValues());
		probeOutput.saveImage(("p3_b_output/"+outputFileName+"_probe_holeFilled.raw").c_str());

		// Boundary smoothening using dilation and erosion:
		matrix<int,bool> probeBoundarySmoothen = onProbe.dilation(probeHoleFilled,3);
		probeBoundarySmoothen = onProbe.erosion(probeBoundarySmoothen,2);
		tempCharData = onProbe.booleanToChar(probeBoundarySmoothen);
		probeOutput.setPixelValues(tempCharData.getMatrixValues());
		probeOutput.saveImage(("p3_b_output/"+outputFileName+"_probe_smoothened.raw").c_str());

		// Skeletonization
		matrix<int,bool> probeSkeletonizing = onProbe.morphologicalAlgorithms(probeBoundarySmoothen,"Skeletonizing");
		tempCharData = onProbe.booleanToChar(probeSkeletonizing);
		probeOutput.setPixelValues(tempCharData.getMatrixValues());
		probeOutput.saveImage(("p3_b_output/"+outputFileName+"_probe_skeletonized.raw").c_str());

		// Thinning
		matrix<int,bool> probeThinning = onProbe.morphologicalAlgorithms(probeBoundarySmoothen,"Thinning");
		tempCharData = onProbe.booleanToChar(probeThinning);
		probeOutput.setPixelValues(tempCharData.getMatrixValues());
		probeOutput.saveImage(("p3_b_output/"+outputFileName+"_probe_thinning.raw").c_str());

	}

	return 0;
}

//----------------------------------------------------------------------------------------------------------------//

