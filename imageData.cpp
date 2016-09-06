//
// Created by Maroof Mohammed Farooq on 9/4/16.
//

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include "imageData.h"

using namespace std;

// Image constructor
imageData::imageData(FILE* file, int BytesPerPixel1, int imageWidth1, int imageHeight1){
    inputFile = file;
    BytesPerPixel = BytesPerPixel1;
    imageWidth = imageWidth1;
    imageHeight = imageHeight1;
    pixelData.resize(imageHeight*imageWidth*BytesPerPixel);
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
