//
// Created by Maroof Mohammed Farooq on 9/4/16.
//

#ifndef CPP_IMAGEDATA_H
#define CPP_IMAGEDATA_H

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>


using namespace std;

class imageData{
public:
    imageData(FILE* file, int BytesPerPixel1, int imageWidth1, int imageHeight1);
    void saveData(FILE* outputFileName);
    void imageRead();
    unsigned char accessPixelValue(int row, int column, int depth);

private:
    FILE *inputFile;
    int BytesPerPixel;
    int imageWidth;
    int imageHeight;
    vector<unsigned char>  pixelData;
};

#endif //CPP_IMAGEDATA_H
