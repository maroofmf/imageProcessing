// Name: Maroof Mohammed Farooq
// HW1 P1
// Image Processing
// Basic Image manipulation

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
////----------------------------------------------------------------------------------------------------------------//
	// Cropping
	if(problemNumber == 1){

		// Checking if the number of arguments are for problem 1
		cout << "Checking for parameters" << endl;
		if(argc != 11){
			cout << "Improper usage" << endl;
			cout << "Correct usage: inputImagePath outputImageName BytesPerPixel imageWidth imageHeight "
							"problemNumber cropRow cropColumn cropWidth cropHeight" << endl;
			exit(-1);
		}
		cout << "Parameters sucessfully tested" << endl;
		cout << "Cropping:" + inputFileName << endl;

		// Open input image
		imageData orignalImage(BytesPerPixel,imageWidth, imageHeight);
		orignalImage.imageRead(inputFileName.c_str());

		// Cropping parameters
		int cropRow = atoi(argv[7]);
		int cropColumn = atoi(argv[8]);
		int cropWidth = atoi(argv[9]);
		int cropHeight = atoi(argv[10]);

		// Create blank image and crop image
		imageData croppedImage(BytesPerPixel,cropWidth, cropHeight);
		croppedImage.cropImage(orignalImage,cropRow,cropColumn,cropWidth, cropHeight);
		croppedImage.saveImage(("p1_a_output/" + outputFileName+".raw").c_str());

		// Resizing the cropped image into three sizes
		cout << "Resizing:" + inputFileName << endl;
		imageData resizedImage1 = croppedImage.resizeImage(100,100);
		resizedImage1.saveImage(("p1_a_output/"+outputFileName+"_resized_100.raw").c_str());
		imageData resizedImage2 = croppedImage.resizeImage(200,200);
		resizedImage2.saveImage(("p1_a_output/"+outputFileName+"_resized_200.raw").c_str());
		imageData resizedImage3 = croppedImage.resizeImage(300,300);
		resizedImage3.saveImage(("p1_a_output/"+outputFileName+"_resized_300.raw").c_str());

		cout<< "Cropping and resizing successfully done "<<endl;
	}
//----------------------------------------------------------------------------------------------------------------//
//	 Changing color space to cmy

	if(problemNumber == 2) {

		// Checking if the number of arguments are for problem 1
		cout << "Checking for parameters" << endl;
		if (argc != 7) {
			cout << "Improper usage" << endl;
			cout << "Correct usage: inputImagePath outputImageName BytesPerPixel imageWidth imageHeight problemNumber" << endl;
			exit(-1);
		}
		cout << "Parameters sucessfully tested" << endl;
		cout << "Changing "+inputFileName+" to cmy color space" << endl;

		// read original image
		imageData orignalImage(BytesPerPixel,imageWidth,imageHeight);
		orignalImage.imageRead(inputFileName.c_str());

		// Convert original image into cmy
		imageData cmyImage = orignalImage.rgb2cmy(false);
		cmyImage.saveImage(("p1_b_output/"+outputFileName+".raw").c_str());

		// Decompose Channels
		vector<imageData> decomposedChannels;
		decomposedChannels = cmyImage.seperateChannels();
		decomposedChannels[0].saveImage(("p1_b_output/"+outputFileName+"_c.raw").c_str());
		decomposedChannels[1].saveImage(("p1_b_output/"+outputFileName+"_m.raw").c_str());
		decomposedChannels[2].saveImage(("p1_b_output/"+outputFileName+"_y.raw").c_str());

		cout<< "RGB to CMY successfully done "<<endl;

	}


//----------------------------------------------------------------------------------------------------------------//
// Changing color space to hsl

	if(problemNumber == 3) {

		// Checking if the number of arguments are for problem 1
		cout << "Checking for parameters" << endl;
		if (argc != 7) {
			cout << "Improper usage" << endl;
			cout <<
			"Correct usage: inputImagePath outputImageName BytesPerPixel imageWidth imageHeight problemNumber" << endl;
			exit(-1);
		}
		cout << "Parameters sucessfully tested" << endl;
		cout << "Changing " + inputFileName + " to hsl color space" << endl;

		imageData orignalImage(BytesPerPixel, imageWidth, imageHeight);
		orignalImage.imageRead(inputFileName.c_str());
		imageData hsvImage = orignalImage.rgb2hsl(false);
		hsvImage.saveImage(("p1_b_output/" + outputFileName + ".raw").c_str());

		// Decompose channels
		vector <imageData> decomposedChannels;
		decomposedChannels = hsvImage.seperateChannels();
		decomposedChannels[0].saveImage(("p1_b_output/" + outputFileName + "_h.raw").c_str());
		decomposedChannels[1].saveImage(("p1_b_output/" + outputFileName + "_s.raw").c_str());
		decomposedChannels[2].saveImage(("p1_b_output/" + outputFileName + "_l.raw").c_str());

		cout<< "RGB to HSL successfully done "<<endl;

	}
//----------------------------------------------------------------------------------------------------------------//
//// Histogram equalization
	if(problemNumber == 4) {

		// Checking if the number of arguments are for problem 1
		cout << "Checking for parameters" << endl;
		if (argc != 7) {
			cout << "Improper usage" << endl;
			cout <<
			"Correct usage: inputImagePath outputImageName BytesPerPixel imageWidth imageHeight problemNumber" << endl;
			exit(-1);
		}

		cout << "Parameters sucessfully tested" << endl;
		cout << "Performing histogram equalization on: " + inputFileName << endl;

		imageData orignalImage(BytesPerPixel,imageWidth,imageHeight);
		orignalImage.imageRead(inputFileName.c_str());

		imageData enhancedImage_tf = orignalImage;
		imageAlgorithms imageAlgo_tf(&enhancedImage_tf);
		imageAlgo_tf.histEqualization_tf();
		enhancedImage_tf.saveImage(("p2_a_output/"+outputFileName+"_tf.raw").c_str());

		imageData enhancedImage_cdf = orignalImage;
		imageAlgorithms imageAlgo_cdf(&enhancedImage_cdf);
		imageAlgo_cdf.histEqualization_cdf();
		enhancedImage_cdf.saveImage(("p2_a_output/"+outputFileName+"_cdf.raw").c_str());
		cout << "Sucessfully completed " << endl;

	}


//----------------------------------------------------------------------------------------------------------------//
// Histogram equalization for color image

	if(problemNumber == 5) {

		// Checking if the number of arguments are for problem 1
		cout << "Checking for parameters" << endl;
		if (argc != 7) {
			cout << "Improper usage" << endl;
			cout <<
			"Correct usage: inputImagePath outputImageName BytesPerPixel imageWidth imageHeight problemNumber" << endl;
			exit(-1);
		}

		cout << "Parameters sucessfully tested" << endl;
		cout << "Performing histogram equalization on: " + inputFileName << endl;

		imageData orignalImage(BytesPerPixel, imageWidth, imageHeight);
		orignalImage.imageRead(inputFileName.c_str());

		// seperate image channels and use tf method

		vector <imageData> seperatedChannels_a = orignalImage.seperateChannels();
		imageAlgorithms redChannel_a(&seperatedChannels_a[0]);
		imageAlgorithms greenChannel_a(&seperatedChannels_a[1]);
		imageAlgorithms blueChannel_a(&seperatedChannels_a[2]);
		redChannel_a.histEqualization_tf();
		greenChannel_a.histEqualization_tf();
		blueChannel_a.histEqualization_tf();

		imageData manipulatedImage_a(BytesPerPixel, imageWidth, imageHeight);
		manipulatedImage_a.concatenateChannels(seperatedChannels_a);

		manipulatedImage_a.saveImage(("p2_b_output/"+outputFileName+"_tf.raw").c_str());

		// Using cdf method
		vector <imageData> seperatedChannels_b = orignalImage.seperateChannels();
		imageAlgorithms redChannel_b(&seperatedChannels_b[0]);
		imageAlgorithms greenChannel_b(&seperatedChannels_b[1]);
		imageAlgorithms blueChannel_b(&seperatedChannels_b[2]);
		redChannel_b.histEqualization_cdf();
		greenChannel_b.histEqualization_cdf();
		blueChannel_b.histEqualization_cdf();

		imageData manipulatedImage_b(BytesPerPixel, imageWidth, imageHeight);
		manipulatedImage_b.concatenateChannels(seperatedChannels_b);
		manipulatedImage_b.saveImage(("p2_b_output/"+outputFileName+"_cdf.raw").c_str());
			cout << "Sucessfully completed " << endl;

	}
//----------------------------------------------------------------------------------------------------------------//
//// Histogram based on a reference image

	if(problemNumber == 6) {

		// Checking if the number of arguments are for problem 1
		cout << "Checking for parameters" << endl;
		if (argc != 7) {
			cout << "Improper usage" << endl;
			cout <<
			"Correct usage: inputImagePath outputImageName BytesPerPixel imageWidth imageHeight problemNumber" << endl;
			exit(-1);
		}

		cout << "Parameters sucessfully tested" << endl;
		cout << "Applying special effects on: " + inputFileName << endl;

		string referenceImagePath = "P2/Skyline.raw";
		imageData referenceImage(3, 600, 400);
		referenceImage.imageRead(referenceImagePath.c_str());

		vector <imageData> referenceImageChannels = referenceImage.seperateChannels();

		imageData originalImage(BytesPerPixel, imageWidth, imageHeight);
		originalImage.imageRead(inputFileName.c_str());

		vector <imageData> originalImageChannels = originalImage.seperateChannels();

		imageAlgorithms redChannel(&originalImageChannels[0]);
		imageAlgorithms greenChannel(&originalImageChannels[1]);
		imageAlgorithms blueChannel(&originalImageChannels[2]);

		redChannel.equalizationBasedOnReference(referenceImageChannels[0]);
		greenChannel.equalizationBasedOnReference(referenceImageChannels[1]);
		blueChannel.equalizationBasedOnReference(referenceImageChannels[2]);

		imageData manipulatedImage(BytesPerPixel, imageWidth, imageHeight);
		manipulatedImage.concatenateChannels(originalImageChannels);

		manipulatedImage.saveImage(("p2_c_output/"+outputFileName+".raw").c_str());
			cout << "Sucessfully completed " << endl;

	}
//----------------------------------------------------------------------------------------------------------------//
//// Histogram Transform based on gaussian.
	if(problemNumber == 7) {

		// Checking if the number of arguments are for problem 1
		cout << "Checking for parameters" << endl;
		if (argc != 7) {
			cout << "Improper usage" << endl;
			cout <<
			"Correct usage: inputImagePath outputImageName BytesPerPixel imageWidth imageHeight problemNumber" << endl;
			exit(-1);
		}

		// Load additional paramteres:
		double mean = 125;
		double std = 40;

		cout << "Parameters sucessfully tested" << endl;
		cout << "Applying histogram Transform based on gaussian: " + inputFileName << endl;

		imageData originalImage(BytesPerPixel,imageWidth,imageHeight);
		originalImage.imageRead(inputFileName.c_str());
		imageData manipulatedImage = originalImage;

		imageAlgorithms applyHist(&manipulatedImage);
		applyHist.histEqualization_gaussian(mean,std);
		manipulatedImage.saveImage(("p2_d_output/histMatch_gaussian_"+outputFileName+".raw").c_str());

		cout << "Sucessfully completed " << endl;

	}
// ----------------------------------------------------------------------------------------------------------------//
// Noise Removal

	if(problemNumber == 8) {

		// Checking if the number of arguments are for problem 1
		cout << "Checking for parameters" << endl;
		if (argc != 8) {
			cout << "Improper usage" << endl;
			cout <<
			"Correct usage: inputImagePath outputImageName BytesPerPixel imageWidth imageHeight problemNumber noisyImage" <<
			endl;
			exit(-1);
		}
		string noisyImageFileName = argv[7];

		cout << "Parameters sucessfully tested" << endl;
		cout << "Applying filters to : " + noisyImageFileName << endl;

		// Setup image objects
		imageData noisyImage(BytesPerPixel,imageWidth,imageHeight);
		imageData originalImage(BytesPerPixel,imageWidth,imageHeight);

		// Load images data
		noisyImage.imageRead(noisyImageFileName.c_str());
		originalImage.imageRead(inputFileName.c_str());

		// Applying filters
		imageAlgorithms filters(&noisyImage);
//		filters.psnr(originalImage);
//		filters.meanFilter(3);
		filters.medianFilter(3);
//		filters.meanFilter(5);
//		filters.meanFilter(7);
		filters.NLMFilter(3, 13, 100);
		filters.psnr(originalImage);

		noisyImage.saveImage(("p3_a_output/"+outputFileName+".raw").c_str());

	}

	return 0;
}

