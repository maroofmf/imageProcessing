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

// Image constructor I: Used when data is read by a file
imageData::imageData(FILE* file, int BytesPerPixel1, int imageWidth1, int imageHeight1){
    inputFile = file;
    BytesPerPixel = BytesPerPixel1;
    imageWidth = imageWidth1;
    imageHeight = imageHeight1;
    pixelData.resize(imageHeight*imageWidth*BytesPerPixel,0);
}

// Image constructor II: Used to initialize a black image
imageData::imageData(int BytesPerPixel1, int imageWidth1, int imageHeight1){
    BytesPerPixel = BytesPerPixel1;
    imageWidth = imageWidth1;
    imageHeight = imageHeight1;
    pixelData.resize(imageHeight*imageWidth*BytesPerPixel,0);
}

// pixel values getter:
vector<unsigned char> imageData::getPixelValues(){
    return(pixelData);
}

// pixel value setter:
void imageData::setPixelValues(vector<unsigned char> newPixelData){
    pixelData = newPixelData;
}

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



// Read image into 1d vector
void imageData::imageRead(){
    fread(&pixelData[0], sizeof(unsigned char), imageWidth*imageHeight*BytesPerPixel, inputFile);
    fclose(inputFile);
}

// Save image to .raw file
void imageData::saveData(FILE* outputFileName){
    fwrite(&pixelData[0], sizeof(unsigned char), imageWidth*imageHeight*BytesPerPixel, outputFileName);
    fclose(outputFileName);
}
