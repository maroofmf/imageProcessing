//
// Created by Maroof Mohammed Farooq on 9/4/16.
//

// Header files
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <iterator>
#include <math.h>
#include "imageData.h"
#include "imageAlgorithms.h"
#include "matrix.h"

// Namespace
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
// Image Bytes per pixel getter:
int imageData::getBytesPerPixel(){
    return(BytesPerPixel);
}
//----------------------------------------------------------------------------------------------------------------//
// Image Width getter:
int imageData::getImageWidth(){
    return(imageWidth);
}
//----------------------------------------------------------------------------------------------------------------//
// Image Height getter:
int imageData::getImageHeight(){
    return(imageHeight);
}
//----------------------------------------------------------------------------------------------------------------//
// I. All pixel values getter:
vector<unsigned char> imageData::getPixelValues(){
    return(pixelData);
}
//----------------------------------------------------------------------------------------------------------------//
// II. Pixel values getter by row column and depth:
unsigned char imageData::getPixelValues(int row, int column, int depth){
    return(accessPixelValue(row,column,depth));
}
//----------------------------------------------------------------------------------------------------------------//
// III. Pixel values getter by index value:
unsigned char imageData::getPixelValues(int index){
    return(pixelData[index]);
}
//----------------------------------------------------------------------------------------------------------------//
// I. Pixel value setter: Set values of all the pixels at once
void imageData::setPixelValues(vector<unsigned char> newPixelData){
    pixelData = newPixelData;
}
//----------------------------------------------------------------------------------------------------------------//
// II. Pixel value setter: Set values of one pixel in 2/3d coordinate system
void imageData::setPixelValues(unsigned char newPixelData,int row,int column,int depth){
    pixelData[row*imageWidth*BytesPerPixel+column*BytesPerPixel+depth] = newPixelData;
}
//----------------------------------------------------------------------------------------------------------------//
// III. Pixel value setter: Set values of one pixel in 1d coordinate system
void imageData::setPixelValues(unsigned char newPixelData,int index){
    pixelData[index] = newPixelData;
}
//----------------------------------------------------------------------------------------------------------------//
// Resizing Image
imageData imageData::resizeImage(int newWidth, int newHeight){

    // Initilize an empty image
    imageData resizedImage(BytesPerPixel,newWidth,newHeight);

    // initilize variable to temporarily store retrieved pixel values
    unsigned char pixelValue;

    // Fill the pixels into the resized image
    // Check if iamge is being shrunk or enlarged
    if(newWidth<imageWidth){
        // Caluculate the resizing ratio
        double ratio = (double)newWidth/imageWidth;
        for(int depthIndex=0; depthIndex< BytesPerPixel;depthIndex++ ){
            for(double rowIndex=0; rowIndex<newHeight;rowIndex++) {
                for (double columnIndex = 0; columnIndex < newWidth; columnIndex++) {
                    // Copy values
                    pixelValue = accessPixelValue((int)ceil(rowIndex/ratio),(int)columnIndex/ratio,depthIndex);
                    resizedImage.setPixelValues(pixelValue,rowIndex,columnIndex,depthIndex);
                }
            }
        }
    }else if(newWidth>imageWidth){
        // Initialize variables
        double fractionRight, fractionLeft, fractionTop, fractionBottom, rowFraction, columnFraction;
        double originalRowIndex, originalColumnIndex;
        unsigned char topRightPixelValue,bottomRightPixelValue,topLeftPixelValue,bottomLeftPixelValue;
        // Caluculate the resizing ratio
        double ratio = (double)(imageWidth-1)/(newWidth-1);
        int extendBy = 1;
        imageData extendedImage = extendImage(extendBy);
        for(int depthIndex=0; depthIndex< BytesPerPixel;depthIndex++ ){
            for(double rowIndex=0; rowIndex<newHeight;rowIndex++) {
                for (double columnIndex = 0; columnIndex < newWidth; columnIndex++) {
                    rowFraction = (double)rowIndex * ratio;
                    columnFraction = (double)columnIndex * ratio;
                    fractionBottom = modf(rowFraction,&originalRowIndex);
                    fractionTop = 1-fractionBottom;
                    fractionRight = modf(columnFraction, &originalColumnIndex);
                    fractionLeft = 1-fractionRight;
                    topLeftPixelValue = extendedImage.accessPixelValue(originalRowIndex+extendBy,originalColumnIndex+extendBy,depthIndex);
                    bottomLeftPixelValue = extendedImage.accessPixelValue(originalRowIndex+extendBy+1,originalColumnIndex+extendBy,depthIndex);
                    topRightPixelValue = extendedImage.accessPixelValue(originalRowIndex+extendBy,originalColumnIndex+extendBy+1,depthIndex);
                    bottomRightPixelValue = extendedImage.accessPixelValue(originalRowIndex+extendBy+1,originalColumnIndex+extendBy+1,depthIndex);

                    // Bilinear interpolation
                    pixelValue = (unsigned char)((topLeftPixelValue*fractionTop*fractionLeft+ topRightPixelValue*fractionTop*fractionRight+
                    bottomRightPixelValue*fractionBottom*fractionRight + bottomLeftPixelValue*fractionBottom*fractionLeft));

                    resizedImage.setPixelValues(pixelValue,rowIndex,columnIndex,depthIndex);


                }
            }
        }
    }else{
        return(*this);
    }
    return(resizedImage);
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
// Extend Image
imageData imageData::extendImage(int extendBy){

    // Declare variables
    imageData extendedImage(BytesPerPixel, imageWidth+2*extendBy, imageHeight+2*extendBy);
    unsigned char pixelValue;

    // Copy the original image into the extended image
    for(int depthIndex=0; depthIndex<BytesPerPixel;depthIndex++){
        for(int rowIndex=0; rowIndex<imageHeight;rowIndex++){
            for(int columnIndex=0; columnIndex<imageWidth;columnIndex++){
                pixelValue = accessPixelValue(rowIndex,columnIndex,depthIndex);
                extendedImage.setPixelValues(pixelValue,rowIndex+extendBy,columnIndex+extendBy,depthIndex);
            }
        }
    }

    // Interpolate the pixel values of top and bottom rows
    int extendedIndex = imageHeight + extendBy;
    for(int depthIndex=0; depthIndex<BytesPerPixel;depthIndex++){
        for(int rowIndex=0; rowIndex<extendBy;rowIndex++){
            for(int columnIndex=0; columnIndex<imageWidth;columnIndex++){
                // top rows
                pixelValue = accessPixelValue(rowIndex,columnIndex,depthIndex);
                extendedImage.setPixelValues(pixelValue,extendBy-rowIndex-1,columnIndex+extendBy,depthIndex);
                // bottom rows
                pixelValue = accessPixelValue(imageHeight-rowIndex-1,columnIndex,depthIndex);
                extendedImage.setPixelValues(pixelValue,extendedIndex,columnIndex+extendBy,depthIndex);
            }
            extendedIndex+=1;
        }
        extendedIndex = imageHeight + extendBy;
    }

    // Interpolate the pixel values of right and left column
    extendedIndex = imageWidth + extendBy;
    for(int depthIndex=0; depthIndex<BytesPerPixel;depthIndex++){
        for(int rowIndex=0; rowIndex<imageHeight;rowIndex++){
            for(int columnIndex=0; columnIndex<extendBy;columnIndex++){
                // left column
                pixelValue = accessPixelValue(rowIndex,columnIndex,depthIndex);
                extendedImage.setPixelValues(pixelValue,rowIndex+extendBy,extendBy-columnIndex-1,depthIndex);
                // right column
                pixelValue = accessPixelValue(rowIndex,imageWidth-columnIndex-1,depthIndex);
                extendedImage.setPixelValues(pixelValue,rowIndex+extendBy,extendedIndex,depthIndex);
                extendedIndex+=1;
            }
            extendedIndex = imageWidth + extendBy;
        }
        extendedIndex = imageWidth + extendBy;
    }

    for(int depthIndex=0; depthIndex<BytesPerPixel;depthIndex++) {
        for (int rowIndex = 0; rowIndex < extendBy; rowIndex++) {
            for (int columnIndex = 0; columnIndex < extendBy; columnIndex++) {

                // top left corner
                pixelValue = extendedImage.getPixelValues(rowIndex+extendBy,columnIndex,depthIndex);
                extendedImage.setPixelValues(pixelValue,rowIndex,columnIndex,depthIndex);

                // bottom left corner
                pixelValue = extendedImage.getPixelValues(rowIndex+imageHeight,columnIndex,depthIndex);
                extendedImage.setPixelValues(pixelValue,rowIndex+imageHeight+extendBy,columnIndex,depthIndex);

                // top right corner
                pixelValue = extendedImage.getPixelValues(rowIndex+extendBy,columnIndex+imageWidth+extendBy,depthIndex);
                extendedImage.setPixelValues(pixelValue,rowIndex,columnIndex+imageWidth+extendBy,depthIndex);

                // bottom right corner
                pixelValue = extendedImage.getPixelValues(rowIndex+imageHeight,columnIndex+imageWidth+extendBy,depthIndex);
                extendedImage.setPixelValues(pixelValue,rowIndex+imageHeight+extendBy,columnIndex+imageWidth+extendBy,depthIndex);

            }
        }
    }


    return(extendedImage);
}
//----------------------------------------------------------------------------------------------------------------//
// RGB to CMY
imageData imageData::rgb2cmy(bool replaceColorSpaceFlag){
    // Check if the image is gray scale
    if(BytesPerPixel < 3){
        cout << "Cannot use color transform on grayscale images"<< endl;
        exit(1);
    }
    imageData cmyImage(BytesPerPixel,imageWidth,imageHeight);
    cmyImage.setPixelValues(pixelData);
    double pixelValue;
    for(int depthIndex=0; depthIndex<BytesPerPixel;depthIndex++){
        for(int rowIndex=0; rowIndex<imageHeight;rowIndex++){
            for(int columnIndex=0; columnIndex<imageWidth;columnIndex++){
                pixelValue = (1-((double)cmyImage.accessPixelValue(rowIndex,columnIndex,depthIndex))/255.0)*255.0;
                cmyImage.setPixelValues((unsigned char)pixelValue,rowIndex,columnIndex,depthIndex);
            }
        }
    }
    if(replaceColorSpaceFlag){
        pixelData = cmyImage.getPixelValues();
        return *this;
    }else{
        return(cmyImage);
    }
}
//----------------------------------------------------------------------------------------------------------------//
// RGB to HSL:
imageData imageData::rgb2hsl(bool replaceColorSpaceFlag){

    if(BytesPerPixel < 3){
        cout << "Cannot use color transform on grayscale images"<< endl;
        exit(1);
    }
    imageData hslImage(BytesPerPixel,imageWidth,imageHeight);
    hslImage.setPixelValues(pixelData);
    double h_pixelValue,s_pixelValue, l_pixelValue, m_value, M_value, c_value, r_value,g_value, b_value;
        for(int rowIndex=0; rowIndex<imageHeight;rowIndex++){
            for(int columnIndex=0; columnIndex<imageWidth;columnIndex++){
                r_value = accessPixelValue(rowIndex,columnIndex,0)/255.0;
                g_value = accessPixelValue(rowIndex,columnIndex,1)/255.0;
                b_value = accessPixelValue(rowIndex,columnIndex,2)/255.0;
                M_value = max(max(r_value,g_value),b_value);
                m_value = min(min(r_value,g_value),b_value);
                c_value = M_value - m_value;

                // Compute L value
                l_pixelValue = (M_value+m_value)/2.0;

                // Compute H Value
                if(c_value==0){
                    h_pixelValue = 0;
                }else if(M_value==r_value){
                    h_pixelValue = 60*(fmod(((g_value-b_value)/c_value),6));
                }else if(M_value==g_value){
                    h_pixelValue = 60*(((b_value-r_value)/c_value)+2);
                }else if(M_value==b_value){
                    h_pixelValue = 60*(((r_value-g_value)/c_value)+4);
                }else{
                    cout << "Couldn't compute h value in rgb2hsl" <<endl;
                    exit(-2);
                }

                //Compute  S Value
                if(l_pixelValue == 0){
                    s_pixelValue =0;
                }else if(l_pixelValue<0.5&&l_pixelValue>0){
                    s_pixelValue = c_value/(2*l_pixelValue);
                }else{
                    s_pixelValue = c_value/(2-2*l_pixelValue);
                }
                // Renormalizing:
                h_pixelValue = h_pixelValue*(255.0/360.0);
                s_pixelValue = s_pixelValue*(255);
                l_pixelValue = l_pixelValue*(255);

                hslImage.setPixelValues((unsigned char)h_pixelValue,rowIndex,columnIndex,0);
                hslImage.setPixelValues((unsigned char)s_pixelValue,rowIndex,columnIndex,1);
                hslImage.setPixelValues((unsigned char)l_pixelValue,rowIndex,columnIndex,2);
            }
        }
    if(replaceColorSpaceFlag){
        pixelData = hslImage.getPixelValues();
        return *this;
    }else{
        return(hslImage);
    }
}
//----------------------------------------------------------------------------------------------------------------//
// HSL to RGB:
imageData imageData::hsl2rgb(bool replaceColorSpaceFlag) {
    if (BytesPerPixel < 3) {
        cout << "Cannot use color transform on grayscale images" << endl;
        exit(1);
    }
    imageData rgbImage(BytesPerPixel, imageWidth, imageHeight);
    rgbImage.setPixelValues(pixelData);
    double h_pixelValue, s_pixelValue, l_pixelValue, m_value, x_value, c_value, r_value, g_value, b_value;
    for (int rowIndex = 0; rowIndex < imageHeight; rowIndex++) {
        for (int columnIndex = 0; columnIndex < imageWidth; columnIndex++) {
            h_pixelValue = accessPixelValue(rowIndex, columnIndex, 0) * (360.0 / 255.0);
            s_pixelValue = accessPixelValue(rowIndex, columnIndex, 1) * (1.0 / 255.0);
            l_pixelValue = accessPixelValue(rowIndex, columnIndex, 2) * (1.0 / 255.0);

            c_value = (1 - fabs(2.0 * l_pixelValue - 1.0)) * s_pixelValue;
            x_value = c_value * (1.0 - fabs(fmod((h_pixelValue / 60.0), 2.0) - 1));
            m_value = l_pixelValue - c_value / 2.0;

            if (h_pixelValue >= 0 && h_pixelValue < 60) {
                r_value = c_value;
                g_value = x_value;
                b_value = 0;
            } else if(h_pixelValue >= 60 && h_pixelValue < 120){
                r_value = x_value;
                g_value = c_value;
                b_value = 0;
            }else if(h_pixelValue >= 120 && h_pixelValue < 180){
                r_value = 0;
                g_value = c_value;
                b_value = x_value;
            }else if(h_pixelValue >= 180 && h_pixelValue < 240){
                r_value = 0;
                g_value = x_value;
                b_value = c_value;
            }else if(h_pixelValue >= 240 && h_pixelValue < 300){
                r_value = x_value;
                g_value = 0;
                b_value = c_value;
            }else if(h_pixelValue >= 300 && h_pixelValue < 360){
                r_value = c_value;
                g_value = 0;
                b_value = x_value;
            }

            r_value = (unsigned char)(255.0*(r_value+m_value));
            g_value = (unsigned char)(255.0*(g_value+m_value));
            b_value = (unsigned char)(255.0*(b_value+m_value));

            rgbImage.setPixelValues((unsigned char)r_value,rowIndex,columnIndex,0);
            rgbImage.setPixelValues((unsigned char)g_value,rowIndex,columnIndex,1);
            rgbImage.setPixelValues((unsigned char)b_value,rowIndex,columnIndex,2);
        }
    }

    if(replaceColorSpaceFlag){
        pixelData = rgbImage.getPixelValues();
        return *this;
    }else{
        return(rgbImage);
    }

}

//----------------------------------------------------------------------------------------------------------------//
// Convert each channel to grayscale:
vector<imageData> imageData::seperateChannels(){

    if(BytesPerPixel<3){
        cout << "Image is in grayscale!" <<endl;
        exit(-1);
    }

    vector<imageData> decomposedChannels;
    imageData firstChannel(1,imageWidth,imageHeight);
    imageData secondChannel(1,imageWidth,imageHeight);
    imageData thirdChannel(1,imageWidth,imageHeight);

    for(int index = 0; index<imageHeight*imageWidth;index++){
        firstChannel.setPixelValues(pixelData[3*index+0],index);
        secondChannel.setPixelValues(pixelData[3*index+1],index);
        thirdChannel.setPixelValues(pixelData[3*index+2],index);

    }

    decomposedChannels.push_back(firstChannel);
    decomposedChannels.push_back(secondChannel);
    decomposedChannels.push_back(thirdChannel);

    return decomposedChannels;
}
//----------------------------------------------------------------------------------------------------------------//
// Combine channels to form color image:
void imageData::concatenateChannels(vector<imageData> colorChannels) {

    unsigned char redValue;
    unsigned char greenValue;
    unsigned char blueValue;

    for(int rowIndex=0; rowIndex<imageHeight;rowIndex++) {
        for (int columnIndex = 0; columnIndex < imageWidth; columnIndex++) {

            redValue = colorChannels[0].getPixelValues(rowIndex,columnIndex,0);
            greenValue = colorChannels[1].getPixelValues(rowIndex,columnIndex,0);
            blueValue = colorChannels[2].getPixelValues(rowIndex,columnIndex,0);
            setPixelValues(redValue,rowIndex,columnIndex,0);
            setPixelValues(greenValue,rowIndex,columnIndex,1);
            setPixelValues(blueValue,rowIndex,columnIndex,2);

        }
    }
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
//    cout<< (int)pixelData[0] << endl;
    fwrite(&pixelData[0], sizeof(unsigned char), imageWidth*imageHeight*BytesPerPixel, outputFile);
    fclose(outputFile);
}
//----------------------------------------------------------------------------------------------------------------//
