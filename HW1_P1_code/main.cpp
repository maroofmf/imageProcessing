// Name: Maroof Mohammed Farooq// HW1 P1(a)// Image Processing// Basic Image manipulation#include <stdio.h>#include <iostream>#include <stdlib.h>#include <vector>#include <string>#include "imageData.h"using namespace std;int main(int argc, char *argv[]){	// Define file pointer and variables	int BytesPerPixel;	int imageWidth;	int imageHeight;//----------------------------------------------------------------------------------------------------------------//	// Check for proper syntax	if (argc < 3){		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;		cout << "program_name input_image.raw output_image.raw [BytesPerPixel = 1] [imageWidth = 256] [imageHeight = 256]" << endl;		return 0;	}		// Check if image is grayscale or color	if (argc < 4) {		BytesPerPixel = 1;// default is grey image		imageHeight = 256;		imageWidth = 256;	}	else {		BytesPerPixel = atoi(argv[3]);		// Check if size is specified		if (argc >= 5){			imageWidth = atoi(argv[4]);			imageHeight = atoi(argv[5]);		}		else{			imageHeight = 256;			imageWidth = 256;		}	}//----------------------------------------------------------------------------------------------------------------//// input and output file names	string inputFileName = argv[1];	string outputFileName = argv[2];//----------------------------------------------------------------------------------------------------------------//	// Cropping	// Open input image	imageData orignalImage(BytesPerPixel,imageWidth, imageHeight);	orignalImage.imageRead(inputFileName.c_str());//	 Cropping parameters	int cropRow = 66; // Rebel= 28	int cropColumn = 40; // 33	int cropWidth = 150;	int cropHeight = 150;	// Create blank image	imageData croppedImage(BytesPerPixel,cropWidth, cropHeight);	croppedImage.cropImage(orignalImage,cropRow,cropColumn,cropWidth, cropHeight);	croppedImage.saveImage(outputFileName.c_str());	// Resizing	imageData resizedImage = croppedImage.resizeImage(200,200);	resizedImage.saveImage("resized.raw");//----------------------------------------------------------------------------------------------------------------//	// Changing color space to cmy//	imageData orignalImage(BytesPerPixel,imageWidth,imageHeight);//	orignalImage.imageRead(inputFileName.c_str());//	imageData cmyImage = orignalImage.rgb2cmy(false);//	cmyImage.saveImage("cmy_image.raw");//	vector<imageData> decomposedChannels;//	decomposedChannels = cmyImage.seperateChannels();//	decomposedChannels[0].saveImage("c.raw");//	decomposedChannels[1].saveImage("m.raw");//	decomposedChannels[2].saveImage("y.raw");//----------------------------------------------------------------------------------------------------------------////	// Changing color space to hsv////	imageData orignalImage(BytesPerPixel,imageWidth,imageHeight);//	orignalImage.imageRead(inputFileName.c_str());//	imageData hsvImage = orignalImage.rgb2hsl(false);//	hsvImage.saveImage(outputFileName.c_str());////	vector<imageData> decomposedChannels;//	decomposedChannels = hsvImage.seperateChannels();//	decomposedChannels[0].saveImage("h.raw");//	decomposedChannels[1].saveImage("s.raw");//	decomposedChannels[2].saveImage("v.raw");//----------------------------------------------------------------------------------------------------------------//	return 0;}