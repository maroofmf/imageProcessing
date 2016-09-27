//
// Created by Maroof Mohammed Farooq on 9/26/16.
//
/*
 This class contains all basic matrix operations that will be utilized in image processing.
 All the operations are described below:

 (1)

 */

// Header files
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <iterator>
#include <math.h>
#include <map>
#include <stdarg.h>
#include "imageData.h"
#include "imageAlgorithms.h"
#include "matrix.h"

using namespace std;
//----------------------------------------------------------------------------------------------------------------//
// Matrix constructor
template <class indexDataType, class valueDataType>
matrix<indexDataType,valueDataType>::matrix(indexDataType width1, indexDataType height1, indexDataType depth1){
    matWidth = width1;
    matHeight = height1;
    matDepth = depth1;
    matData.resize(matWidth*matHeight*matDepth,0);
}

//----------------------------------------------------------------------------------------------------------------//
// Matrix destructor
template <class indexDataType, class valueDataType>
matrix<indexDataType,valueDataType>::~matrix(void){
}

//----------------------------------------------------------------------------------------------------------------//
// Setter matrix values from a vector

template <class indexDataType, class valueDataType>
void matrix<indexDataType,valueDataType>::setMatrixValues(vector<valueDataType> inputValues){
    matData = inputValues;
}

//----------------------------------------------------------------------------------------------------------------//
// Setter matrix values from a Image

template <class indexDataType, class valueDataType>
void matrix<indexDataType,valueDataType>::setMatrixValues(imageData inputImage,indexDataType startRow,indexDataType startColumn, indexDataType startDepth, indexDataType totalDepth){

    for(indexDataType rowIndex = startRow; rowIndex< startRow+matHeight; rowIndex++){
        for(indexDataType columnIndex = startColumn; columnIndex< startColumn+matWidth; columnIndex++){
            for(indexDataType depthIndex = startDepth; depthIndex < startDepth+totalDepth; depthIndex++){
                matData.push_back(inputImage.getPixelValues((int)rowIndex,(int)columnIndex,(int)depthIndex));
            }
        }
    }
}
//----------------------------------------------------------------------------------------------------------------//
// Setter matrix values for row,col and depth
template <class indexDataType, class valueDataType>
void matrix<indexDataType,valueDataType>::setMatrixValues(valueDataType matValue, indexDataType row, indexDataType column, indexDataType depth){
    matData[row*matWidth*matDepth+column*matDepth+depth] = matValue;
}

//----------------------------------------------------------------------------------------------------------------//
// Setter matrix values from input values

template <class indexDataType, class valueDataType>
void matrix<indexDataType,valueDataType>::setMatrixByValues(indexDataType numberOfArguments,...){

    va_list ap;
    va_start(ap,numberOfArguments);
    for(int index=0;index<numberOfArguments ; index++){
        matData[index] = va_arg(ap,valueDataType);
    }

};
//----------------------------------------------------------------------------------------------------------------//
// Get matrix values for row,col and depth
template <class indexDataType, class valueDataType>
valueDataType matrix<indexDataType,valueDataType>::getMatrixValues(indexDataType row, indexDataType column, indexDataType depth){
    return(matData[row*matWidth*matDepth+column*matDepth+depth]);
}

//----------------------------------------------------------------------------------------------------------------//
// Get matrix width
template <class indexDataType, class valueDataType>
indexDataType matrix<indexDataType,valueDataType>::getWidth(){
    return matWidth;
}

//----------------------------------------------------------------------------------------------------------------//
// Get matrix height
template <class indexDataType, class valueDataType>
indexDataType matrix<indexDataType,valueDataType>::getHeight(){
    return matHeight;
}

//----------------------------------------------------------------------------------------------------------------//
// Get matrix depth
template <class indexDataType, class valueDataType>
indexDataType matrix<indexDataType,valueDataType>::getDepth(){
    return matDepth;
}

//----------------------------------------------------------------------------------------------------------------//
// Multiply each element by
template <class indexDataType, class valueDataType>
void matrix<indexDataType,valueDataType>::multiplyEachValueBy(valueDataType value){

    for(int index = 0; index<matData.size(); index++){
        matData[index] *= value;
    }
}

//----------------------------------------------------------------------------------------------------------------//
// Access matrix values from row, column, depth

template <class indexDataType, class valueDataType>
valueDataType matrix<indexDataType,valueDataType>::accessMatrixValue(indexDataType row,indexDataType column,indexDataType depth){

    if(depth>=matDepth||row>=matHeight||column>=matWidth){
        cout << "Index out of range" <<endl;
        exit(1);
    }
    return(matData[row*matWidth*matDepth+column*matDepth+depth]);
}
//----------------------------------------------------------------------------------------------------------------//
// Print matrix in text file
template <class indexDataType, class valueDataType>
void matrix<indexDataType,valueDataType>::printMatrix(){

    // Open text file
    ofstream outputFile;
    remove("matrixData.txt");
    outputFile.open ("matrixData.txt", ofstream::out | ofstream::app);

    for(indexDataType depthIndex = 0; depthIndex < matDepth; depthIndex++) {
        for (indexDataType rowIndex = 0; rowIndex < matHeight; rowIndex++) {
            for (indexDataType columnIndex = 0; columnIndex < matWidth; columnIndex++) {

                outputFile <<  matData[rowIndex * matWidth * matDepth + columnIndex * matDepth + depthIndex] <<
                "\t";
            }
            outputFile << "\n";
        }
        outputFile << "\n \n";
    }

    outputFile.close();

}
//----------------------------------------------------------------------------------------------------------------//
// Acceptable templates

template class matrix<int,unsigned char>;
template class matrix<int,int>;
template class matrix<int,double>;
template class matrix<int,float>;
template class matrix<int,long>;

