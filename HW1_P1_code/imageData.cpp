//
// Created by Maroof Mohammed Farooq on 9/4/16.
//

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <iterator>
#include <math.h>
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
// I. All pixel values getter:
vector<unsigned char> imageData::getPixelValues(){
    return(pixelData);
}
//----------------------------------------------------------------------------------------------------------------//
// II. pixel values getter by row column and depth:
unsigned char imageData::getPixelValues(int row, int column, int depth){
    return(accessPixelValue(row,column,depth));
}
//----------------------------------------------------------------------------------------------------------------//
// III. pixel values getter by index value:
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

    // initilize pixelValues to temporarily store retrieved pixel values
    unsigned char pixelValue;

    // Caluculate the resizing ratio
    double ratio = (double)newWidth/imageWidth;

    // Fill the pixels into the resized image
    if(newWidth<imageWidth){
        for(double rowIndex=0; rowIndex<newHeight;rowIndex++) {
            for (double columnIndex = 0; columnIndex < newWidth; columnIndex++) {
                // R channel
                pixelValue = accessPixelValue((int)ceil(rowIndex/ratio),(int)columnIndex/ratio,0 );
                resizedImage.setPixelValues(pixelValue,rowIndex,columnIndex,0);
                // G channel
                pixelValue = accessPixelValue((int)ceil(rowIndex/ratio),(int)columnIndex/ratio,1);
                resizedImage.setPixelValues(pixelValue,rowIndex,columnIndex,1);
                // B channel
                pixelValue = accessPixelValue((int)ceil(rowIndex/ratio),(int)columnIndex/ratio,2 );
                resizedImage.setPixelValues(pixelValue,rowIndex,columnIndex,2);
            }
        }
    }else if(newWidth>imageWidth){
        for(double rowIndex=0; rowIndex<imageHeight;rowIndex++) {
            for (double columnIndex = 0; columnIndex < imageWidth; columnIndex++) {
                // R channel
                pixelValue = accessPixelValue(rowIndex,columnIndex,0);
                resizedImage.setPixelValues(pixelValue,(int)ceil(rowIndex*ratio),(int)ceil(ratio*columnIndex),0);
                // G channel
                pixelValue = accessPixelValue(rowIndex,columnIndex,1);
                resizedImage.setPixelValues(pixelValue,(int)ceil(rowIndex*ratio),(int)ceil(ratio*columnIndex),1);
                // B channel
                pixelValue = accessPixelValue(rowIndex,columnIndex,2);
                resizedImage.setPixelValues(pixelValue,(int)ceil(rowIndex*ratio),(int)ceil(ratio*columnIndex),2);
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
// RGB to CMY
imageData imageData::rgb2cmy(bool replaceColorSpaceFlag){
    imageData cmyImage(BytesPerPixel,imageWidth,imageHeight);
//TODO: Check the color space for image first!
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
//        delete cmyImage;
        return *this;
    }else{
        return(cmyImage);
    }
}
//----------------------------------------------------------------------------------------------------------------//
// RGB to HSL:
imageData imageData::rgb2hsl(bool replaceColorSpaceFlag){
    imageData hslImage(BytesPerPixel,imageWidth,imageHeight);
//TODO: Check the color space for image first!
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
