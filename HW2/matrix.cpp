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
// Default constructor
template <class indexDataType, class valueDataType>
matrix<indexDataType,valueDataType>::matrix(){
}

//----------------------------------------------------------------------------------------------------------------//
// Matrix constructor
template <class indexDataType, class valueDataType>
matrix<indexDataType,valueDataType>::matrix(indexDataType height1, indexDataType width1, indexDataType depth1){
    if(depth1==0){depth1=1;}
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
// Setter matrix dimentions
template <class indexDataType, class valueDataType>
void matrix<indexDataType,valueDataType>::setMatrixDimentions(indexDataType height1, indexDataType width1, indexDataType depth1){

    if(depth1==0){depth1=1;}
    matWidth = width1;
    matHeight = height1;
    matDepth = depth1;
    matData.resize(matWidth*matHeight*matDepth,0);
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
// Get all matrix values
template <class indexDataType, class valueDataType>
vector<valueDataType> matrix<indexDataType,valueDataType>::getMatrixValues(){
    return(matData);
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
// Trace of matrix:
template <class indexDataType, class valueDataType>
double matrix<indexDataType,valueDataType>::trace(matrix<indexDataType,valueDataType> inputMatrix){

    // Local Variables
    double traceValue = 0;
    int matrixSize = inputMatrix.getWidth();

    // Check if the matrix is square:
    if((matrixSize!=inputMatrix.getHeight())||(inputMatrix.getDepth()!=1)){
        cout<< "Wrong matrix size for calculating trace "<<endl;
        exit(-2);
    }

    // Calculate trace of a matrix
    for(int index = 0; index < matrixSize; index++) {
        traceValue += inputMatrix.getMatrixValues(index,index,0);
    }

    return traceValue;

}

//----------------------------------------------------------------------------------------------------------------//
// Determinant of matrix:
template <class indexDataType, class valueDataType>
double matrix<indexDataType,valueDataType>::determinant(matrix<indexDataType,valueDataType> inputMatrix) {

    if(inputMatrix.getHeight()!=inputMatrix.getWidth()){
        cout<< "Cannot calculate determinant for this matrix " << endl;
        exit(-1);
    }

    indexDataType sizeOfMatrix = inputMatrix.getHeight();
    matrix<indexDataType,valueDataType> subMatrix(inputMatrix.getWidth()-1,inputMatrix.getWidth()-1,1);
    double d;

    int subi, j, subj;
    if (sizeOfMatrix == 2)
    {
        return (inputMatrix.getMatrixValues(0,0,0)*inputMatrix.getMatrixValues(1,1,0) - inputMatrix.getMatrixValues(1,0,0)*inputMatrix.getMatrixValues(0,1,0));
    }else{

        for(int c = 0; c < sizeOfMatrix; c++){
            subi = 0;
            for(int i = 1; i < sizeOfMatrix; i++){
                subj = 0;
                for(j = 0; j < sizeOfMatrix; j++){
                    if (j == c){
                        continue;
                    }
                    subMatrix.setMatrixValues(inputMatrix.getMatrixValues(i,j,0),subi,subj,0);
                    subj++;
                }
                subi++;
            }
            d = d + (pow(-1 ,c) * inputMatrix.getMatrixValues(0,c,0) * determinant(subMatrix));
        }
    }
    return d;
}
//----------------------------------------------------------------------------------------------------------------//
// Cofactor matrix:
template <class indexDataType, class valueDataType>
matrix<indexDataType,valueDataType> matrix<indexDataType,valueDataType>::cofactorMatrix(matrix<indexDataType,valueDataType> inputMatrix,indexDataType row, indexDataType column) {

    indexDataType outputHeight = inputMatrix.getHeight() - 1;
    indexDataType outputWidth = inputMatrix.getWidth() -1;
    matrix<indexDataType,valueDataType> outputMatrix(outputHeight,outputWidth,1);
    vector<valueDataType> outputVector;

    // Loop thorugh the elements
    for(int rowIndex = 0; rowIndex < outputHeight+1; rowIndex++){
        for(int columnIndex = 0; columnIndex< outputWidth+1; columnIndex++){
            if((rowIndex != row)&&(columnIndex != column)){
                outputVector.push_back(inputMatrix.getMatrixValues(rowIndex,columnIndex,0));
            }
        }
    }

    // Set the output
    outputMatrix.setMatrixValues(outputVector);
    return(outputMatrix);
}

//----------------------------------------------------------------------------------------------------------------//
// Adjoint matrix:
template <class indexDataType, class valueDataType>
matrix<indexDataType,valueDataType> matrix<indexDataType,valueDataType>::adjoint(matrix<indexDataType,valueDataType> inputMatrix) {

    // local variables
    indexDataType outputHeight = inputMatrix.getHeight();
    indexDataType outputWidth = inputMatrix.getWidth();
    matrix<indexDataType,valueDataType> outputMatrix(outputHeight,outputWidth,1);
    int powerCount = 0;
    valueDataType matValue;

    // find output values
    for(int rowIndex = 0; rowIndex < outputHeight; rowIndex++){
        for(int columnIndex = 0; columnIndex< outputWidth; columnIndex++){
            matValue = pow(-1,powerCount++)*determinant(cofactorMatrix(inputMatrix,rowIndex,columnIndex));
            outputMatrix.setMatrixValues(matValue,rowIndex,columnIndex,0);
        }
    }

    return(outputMatrix);


}


//----------------------------------------------------------------------------------------------------------------//
// Moore Penrose Pseudo inverse:

template <class indexDataType, class valueDataType>
matrix<indexDataType,valueDataType> matrix<indexDataType,valueDataType>::pseudoInverse() {

    // Initialize variables
    matrix<indexDataType,valueDataType> outputMatrix(matHeight,matWidth,1);
    matrix<indexDataType,valueDataType> multipliedMatrix(matHeight,matWidth,1);
    matrix<indexDataType,valueDataType> transposedMatrix(matHeight,matWidth,1);
    valueDataType matValue;
    double det;

    transposedMatrix = transpose();
    multipliedMatrix = transposedMatrix;
    multipliedMatrix = multipliedMatrix.multiplyWith(*this);
    det = determinant(multipliedMatrix);
    outputMatrix = adjoint(multipliedMatrix);
    outputMatrix.multiplyEachValueBy(1/det);
    outputMatrix = outputMatrix.multiplyWith(transposedMatrix);

    return (outputMatrix);

}

//----------------------------------------------------------------------------------------------------------------//
// Matrix Transpose
template <class indexDataType, class valueDataType>
matrix<indexDataType,valueDataType> matrix<indexDataType,valueDataType>::transpose() {

    // Initialize variables
    matrix<indexDataType,valueDataType> outputMatrix(matWidth,matHeight,1);
    valueDataType matValue;

    for(indexDataType rowIndex = 0; rowIndex < matHeight; rowIndex++){
        for(indexDataType columnIndex = 0; columnIndex < matWidth; columnIndex++) {

            matValue = getMatrixValues(rowIndex,columnIndex,0);
            outputMatrix.setMatrixValues(matValue,columnIndex,rowIndex,0);
        }
    }
    return outputMatrix;

}
//----------------------------------------------------------------------------------------------------------------//
// Matrix multiplication
template <class indexDataType, class valueDataType>
matrix<indexDataType,valueDataType> matrix<indexDataType,valueDataType>::multiplyWith(matrix<indexDataType,valueDataType> otherMatrix){

    // Check the depth of matrix
    if(otherMatrix.getDepth()!=1){
        cout<< "3D Matrix multiplication not supported at this time"<<endl;
        exit(-1);
    }

    // Check matrix dimentions
    if(matWidth!=otherMatrix.getHeight()){
        cout<< "Incorrect matrix sizes" <<endl;
        exit(-1);
    }

    // Output matrix
    matrix<indexDataType,valueDataType> outputMatrix(matHeight,otherMatrix.getWidth(),1);
    valueDataType matValue = 0;

    // Calculate output matrix

    for(indexDataType rowIndex = 0; rowIndex < matHeight; rowIndex++){
        for(indexDataType columnIndex = 0; columnIndex < otherMatrix.getWidth(); columnIndex++){

            matValue = 0;
            // Calculate the product
            for(indexDataType localIndex = 0; localIndex < matWidth; localIndex++){
                    matValue += getMatrixValues(rowIndex,localIndex,0)*otherMatrix.getMatrixValues(localIndex,columnIndex,0);
            }
            outputMatrix.setMatrixValues(matValue,rowIndex,columnIndex,0);
        }
    }
    return (outputMatrix);
}

//----------------------------------------------------------------------------------------------------------------//
// Operator overload ==

template <class indexDataType, class valueDataType>
bool matrix<indexDataType,valueDataType>::operator==(matrix<indexDataType,valueDataType> otherMatrix){

    bool same = 0;
    vector<valueDataType> otherMatrixData = otherMatrix.getMatrixValues();

    if((matWidth!=otherMatrix.getWidth())||(matHeight!=otherMatrix.getHeight())||(matDepth!=otherMatrix.getDepth())){
        return same;
    }

    for(int index = 0; index<matData.size(); index++){

        if(otherMatrixData[index] != matData[index]){
            return same;
        }
    }

    same = 1;
    return same;

}


//----------------------------------------------------------------------------------------------------------------//
// Operator overload !=

template <class indexDataType, class valueDataType>
bool matrix<indexDataType,valueDataType>::operator!=(matrix<indexDataType,valueDataType> otherMatrix){

    bool same = 1;
    vector<valueDataType> otherMatrixData = otherMatrix.getMatrixValues();

    if((matWidth!=otherMatrix.getWidth())||(matHeight!=otherMatrix.getHeight())||(matDepth!=otherMatrix.getDepth())){
        return same;
    }

    for(int index = 0; index<matData.size(); index++){

        if(otherMatrixData[index] != matData[index]){
            return same;
        }
    }

    same = 0;
    return same;

}

//----------------------------------------------------------------------------------------------------------------//
// Count number of non zero elements

template <class indexDataType, class valueDataType>
int matrix<indexDataType,valueDataType>::countNumberOfNonZero(){

    int count = 0;

    for(int index = 0; index<matData.size(); index++){

        if(matData[index]>0){
            count++;
        }
    }

    return count;

}

//----------------------------------------------------------------------------------------------------------------//
// Multiply each element by
template <class indexDataType, class valueDataType>
void matrix<indexDataType,valueDataType>::multiplyEachValueBy(valueDataType value){

    for(int index = 0; index<matData.size(); index++){
        matData[index] = matData[index]*value;
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
template class matrix<int,bool>;