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

// Image constructor
imageData::imageData(FILE* file, int BytesPerPixel1, int imageWidth1, int imageHeight1){
    inputFile = file;
    BytesPerPixel = BytesPerPixel1;
    imageWidth = imageWidth1;
    imageHeight = imageHeight1;
    pixelData.resize(imageHeight*imageWidth*BytesPerPixel,0);
}





// Access image data
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
    cout << (int)accessPixelValue(imageHeight-1,imageWidth-1,1) <<endl;
//    cout << (int)pixelData[] << endl;
}

// Save image to .raw file
void imageData::saveData(FILE* outputFileName){
    fwrite(&pixelData[0], sizeof(unsigned char), imageWidth*imageHeight*BytesPerPixel, outputFileName);
    fclose(outputFileName);
}
