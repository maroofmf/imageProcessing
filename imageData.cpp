//
// Created by Maroof Mohammed Farooq on 9/4/16.
//

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <iterator>
#include "imageData.h"

using namespace std;

//----------------------------------------------------------------------------------------------------------------//
// Image constructor I: Used to initialize a black image
imageData::imageData(int BytesPerPixel1, int imageWidth1, int imageHeight1){
    BytesPerPixel = BytesPerPixel1;
    imageWidth = imageWidth1;
    imageHeight = imageHeight1;
    pixelData.resize(imageHeight*imageWidth*BytesPerPixel,0);
}
//----------------------------------------------------------------------------------------------------------------//
// Destructor
imageData::~imageData(void){
}
//----------------------------------------------------------------------------------------------------------------//
// pixel values getter:
vector<unsigned char> imageData::getPixelValues(){
    return(pixelData);
}
//----------------------------------------------------------------------------------------------------------------//
// pixel value setter:
void imageData::setPixelValues(vector<unsigned char> newPixelData){
    pixelData = newPixelData;
}
//----------------------------------------------------------------------------------------------------------------//
void imageData::setPixelValues(unsigned char newPixelData,int row,int column,int depth){
    pixelData[row*imageWidth*BytesPerPixel+column*BytesPerPixel+depth] = newPixelData;
}
//----------------------------------------------------------------------------------------------------------------//
// Resizing Image
void imageData::resizeImage(int newWidth, int newHeight){
}
//----------------------------------------------------------------------------------------------------------------//
// Crop Image
void imageData::cropImage(imageData orignalImage,int cropRow,int cropColumn,int cropWidth,int cropHeight){
    int index =0;

    for(int depthIndex=0; depthIndex<BytesPerPixel;depthIndex++){
        for(int rowIndex=0; rowIndex<imageHeight;rowIndex++){
            for(int columnIndex=0; columnIndex<imageWidth;columnIndex++){
                index = rowIndex*imageWidth*BytesPerPixel+columnIndex*BytesPerPixel+depthIndex;
                pixelData[index] = orignalImage.accessPixelValue(cropRow+rowIndex,cropColumn+columnIndex,depthIndex);
            }
        }
    }
}
//----------------------------------------------------------------------------------------------------------------//
// RGB to CMY
imageData imageData::rgb2cmy(bool replaceColorSpaceFlag){
    imageData cmyImage(BytesPerPixel,imageWidth,imageHeight);
//TODO: Check the color space for image first!
    cmyImage.setPixelValues(pixelData);
    double pixelValue;
    for(int depthIndex=0; depthIndex<BytesPerPixel;depthIndex++){
        for(int rowIndex=0; rowIndex<imageHeight;rowIndex++){
            for(int columnIndex=0; columnIndex<imageWidth;columnIndex++){
                pixelValue = (1-((double)cmyImage.accessPixelValue(rowIndex,columnIndex,depthIndex))/255)*255;
                cmyImage.setPixelValues((unsigned char)pixelValue,rowIndex,columnIndex,depthIndex);
            }
        }
    }
    if(replaceColorSpaceFlag){
        pixelData = cmyImage.getPixelValues();
//        delete cmyImage;
        return *this;
    }
    return(cmyImage);
}
//----------------------------------------------------------------------------------------------------------------//
// Access image data 3d to 1d
// rows: 0--(imageHeight-1)
// columns: 0--(imageWidth-1)
// depth = 0--(BytesPerPixel-1)
unsigned char imageData::accessPixelValue(int row, int column, int depth){
    if(depth>BytesPerPixel||row>=imageHeight||column>=imageWidth){
        cout << "Index out of range" <<endl;
        exit(1);
    }
    return(pixelData[row*imageWidth*BytesPerPixel+column*BytesPerPixel+depth]);
}
//----------------------------------------------------------------------------------------------------------------//
// Load image from file
void imageData::imageRead(const char* inputFileName){
    FILE* inputFile;
    if (!(inputFile=fopen(inputFileName,"rb"))) {
        cout << "Cannot open file: " << inputFile <<endl;
        exit(1);
    }
    fread(&pixelData[0], sizeof(unsigned char), imageWidth*imageHeight*BytesPerPixel, inputFile);
    fclose(inputFile);
}
//----------------------------------------------------------------------------------------------------------------//
// Save image to .raw file
void imageData::saveImage(const char* outputFileName){
    FILE* outputFile;
    if (!(outputFile=fopen(outputFileName,"wb"))) {
        cout << "Cannot open file: " << outputFileName << endl;
        exit(1);
    }
    fwrite(&pixelData[0], sizeof(unsigned char), imageWidth*imageHeight*BytesPerPixel, outputFile);
    fclose(outputFile);
}
//----------------------------------------------------------------------------------------------------------------//
