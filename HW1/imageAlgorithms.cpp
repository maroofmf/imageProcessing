//
// Created by Maroof Mohammed Farooq on 9/11/16.
//

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <iterator>
#include <math.h>
#include <map>
#include <numeric>
#include "imageData.h"
#include "imageAlgorithms.h"

using namespace std;

//----------------------------------------------------------------------------------------------------------------//
// Image Algorithms constructor
imageAlgorithms::imageAlgorithms(imageData* imageObject1){
    imageObject = imageObject1;
}
//----------------------------------------------------------------------------------------------------------------//
// Destructor
imageAlgorithms::~imageAlgorithms(void){
}
//----------------------------------------------------------------------------------------------------------------//
// Transfer function based Histogram Equalization
void imageAlgorithms::histEqualization_tf(){
    // Get a histogram of pixel values
    // Initialize pixel data vector that contains all the pixel values
    // It is hard copy. Data changed in pixelData wont be stored in imageObject
    vector<unsigned char> pixelData = imageObject->getPixelValues();

    // Initializing a hash map to store the frequencies of pixel values
    map<unsigned char, double> pixelFrequency;
    double numberOfPixels = pixelData.size();

    //Initilize hash table with all possible pixel values
    for(int pixelValue = 0; pixelValue < 256; pixelValue++){
        pixelFrequency[(unsigned char)pixelValue] = 0.0;
    }

//    // Count the frequency of each pixel value and normalize it
    for(int pixelDataIndex = 0; pixelDataIndex < pixelData.size(); pixelDataIndex++){
        pixelFrequency[pixelData[pixelDataIndex]]+=1.0/numberOfPixels;
    }

//     Calculate cumulative histogram
    for(int pixelValue = 0; pixelValue < 256; pixelValue++){
        pixelFrequency[(unsigned char)pixelValue]+=pixelFrequency[(unsigned char)(pixelValue-1)];
    }

//     Create a mapping table
    for(int pixelDataIndex = 0; pixelDataIndex < pixelData.size(); pixelDataIndex++){
        pixelData[pixelDataIndex] = (unsigned char)ceil(pixelFrequency[pixelData[pixelDataIndex]]*255);
    }

    // Set pixel values in image Object
    imageObject->setPixelValues(pixelData);
}
//----------------------------------------------------------------------------------------------------------------//
// Transfer function based on Cumulative probability:
void imageAlgorithms::histEqualization_cdf(){
    // Get a histogram of pixel values
    // Initialize pixel data vector that contains all the pixel values
    // It is hard copy. Data changed in pixelData wont be stored in imageObject
    vector<unsigned char> pixelData = imageObject->getPixelValues();

    // Initializing a hash map to store the frequencies of pixel values
    map<unsigned char, vector<int> > pixelLocation;
    int numberOfPixels = pixelData.size();
    vector<int> concatenatedPixelLocations;

    //Initilize hash table with all possible pixel values
    for(int pixelValue = 0; pixelValue < 256; pixelValue++){
        pixelLocation[(unsigned char)pixelValue] = vector<int>();
    }

    // Store indexes in a vector
    for(int pixelDataIndex = 0; pixelDataIndex< numberOfPixels; pixelDataIndex++){
        pixelLocation[pixelData[pixelDataIndex]].push_back(pixelDataIndex);
    }

    //Form a single vector of locations sorted based on pixel values
    for(int pixelValue = 0; pixelValue < 256; pixelValue++){
        // Check if the pixel value exists:
        if(pixelLocation[pixelValue].size()!=0) {
            concatenatedPixelLocations.insert(concatenatedPixelLocations.end(), pixelLocation[pixelValue].begin(),
                                              pixelLocation[pixelValue].end());
        }
    }

    // Retrieve indexes and assign a value
    for(int pixelLocationIndex = 0; pixelLocationIndex < numberOfPixels; pixelLocationIndex++){
        pixelData[concatenatedPixelLocations[pixelLocationIndex]] = ceil((255.0/(numberOfPixels-1))*pixelLocationIndex);
    }

//     Set pixel values in image Object
    imageObject->setPixelValues(pixelData);
}
//----------------------------------------------------------------------------------------------------------------//
// Equalization based on reference image
void imageAlgorithms::equalizationBasedOnReference(imageData referenceObject){

    // Initialize variables
    vector<unsigned char> orignalPixelData = imageObject->getPixelValues();
    vector<unsigned char> referencePixelData = referenceObject.getPixelValues();
    double originalPDF;
    double referencePDF = 0;
    int referencePixelValue=0;
    unsigned char maxReference = *max_element(referencePixelData.begin(),referencePixelData.end());

    // Initializing a hash maps to store the frequencies of pixel values
    map<unsigned char, double> originalPixelFrequency;
    map<unsigned char, double> referencePixelFrequency;
    int numberOfPixels_original = orignalPixelData.size();
    int numberOfPixels_reference = referencePixelData.size();

    //Initilize hash table with all possible pixel values
    for(int pixelValue = 0; pixelValue < 256; pixelValue++){
        originalPixelFrequency[(unsigned char)pixelValue] = 0.0;
        referencePixelFrequency[(unsigned char)pixelValue] = 0.0;
    }

    // Count the frequency of each pixel value and normalize it
    for(int pixelDataIndex = 0; pixelDataIndex < numberOfPixels_original; pixelDataIndex++) {
        originalPixelFrequency[orignalPixelData[pixelDataIndex]] += 1.0 / numberOfPixels_original;
    }

    for(int pixelDataIndex = 0; pixelDataIndex < numberOfPixels_reference; pixelDataIndex++){
        referencePixelFrequency[referencePixelData[pixelDataIndex]]+=1.0/numberOfPixels_reference;
    }

    // Caluculate the mode value of reference pixel data
    unsigned char mode = 0;
    for(int pixelValue = 1; pixelValue < 256; pixelValue++){
        if(referencePixelFrequency[(unsigned char)pixelValue]>referencePixelFrequency[(unsigned char)(pixelValue-1)]){
            mode = (unsigned char)pixelValue;
        }
    }

//     Calculate cumulative histogram

    for(int pixelValue = 1; pixelValue < 256; pixelValue++){
        originalPixelFrequency[(unsigned char)pixelValue]+=originalPixelFrequency[(unsigned char)(pixelValue-1)];
        referencePixelFrequency[(unsigned char)pixelValue]+=referencePixelFrequency[(unsigned char)(pixelValue-1)];
    }

//     Create a mapping table
    for(int pixelDataIndex = 0; pixelDataIndex < numberOfPixels_original; pixelDataIndex++){
        originalPDF = originalPixelFrequency[orignalPixelData[pixelDataIndex]];

        // Scan through the corresponding cdf in reference image
        for(int referencePixelIndex = 0;referencePixelIndex< int(maxReference); referencePixelIndex++){
            referencePDF = referencePixelFrequency[referencePixelIndex];
            referencePixelValue = referencePixelIndex;
            if(referencePDF>originalPDF){
                break;
            }
        }
        if(referencePDF<originalPDF){
            referencePixelValue = mode;
        }


        //Use referencePixelIndex to replace the original pixel value

        orignalPixelData[pixelDataIndex] = (unsigned char)referencePixelValue;
    }

    // Set pixel values in image Object
    imageObject->setPixelValues(orignalPixelData);

}
//----------------------------------------------------------------------------------------------------------------//
// Equalization based on Gaussian function

void imageAlgorithms::histEqualization_gaussian(double mean,double std){
    // Get a histogram of pixel values
    // Initialize pixel data vector that contains all the pixel values
    // It is hard copy. Data changed in pixelData wont be stored in imageObject
    vector<unsigned char> pixelData = imageObject->getPixelValues();

    // Initializing a hash map to store the frequencies of pixel values
    map<unsigned char, vector<int> > pixelLocation;
    map<unsigned char, double> desiredPixelFrequency;
    int numberOfPixels = pixelData.size();
    vector<int> concatenatedPixelLocations;

    //Initilize hash table with all possible pixel values
    for(int pixelValue = 0; pixelValue < 256; pixelValue++){
        pixelLocation[(unsigned char)pixelValue] = vector<int>();
        desiredPixelFrequency[(unsigned char)pixelValue] = 0;
    }

    // Store indexes in a vector
    for(int pixelDataIndex = 0; pixelDataIndex< numberOfPixels; pixelDataIndex++){
        pixelLocation[pixelData[pixelDataIndex]].push_back(pixelDataIndex);
    }

    //Caluculate the desired pixel frequency
    //Calculate cumulative histogram and desired pixel frequency
    desiredPixelFrequency[0] = (1/sqrt(2*M_PI*(pow(std,2))))*exp(-(pow((0-mean),2))/(2*pow(std,2)));
    for (int pixelValue = 1; pixelValue < 256; pixelValue++) {
        desiredPixelFrequency[pixelValue] = desiredPixelFrequency[pixelValue-1]+(1/sqrt(2*M_PI*(pow(std,2.0))))*exp(-(pow(((double)pixelValue-mean),2))/(2*pow(std,2)));
    }

    // Renormalizing the cdf
    double maxDesiredPixelFrequency = desiredPixelFrequency[255];
    for (int pixelValue = 0; pixelValue < 256; pixelValue++) {
        desiredPixelFrequency[pixelValue] = (int)((desiredPixelFrequency[pixelValue]/maxDesiredPixelFrequency)*numberOfPixels);
    }

    // Caluculating the number of pixels for each bin
    map<unsigned char,int> pixelsPerBin;
    pixelsPerBin[0] = desiredPixelFrequency[0];
    for (int pixelValue = 1; pixelValue < 256; pixelValue++) {
        pixelsPerBin[pixelValue] = desiredPixelFrequency[pixelValue] - desiredPixelFrequency[pixelValue-1];
    }

    //Form a single vector of locations sorted based on pixel values
    for(int pixelValue = 0; pixelValue < 256; pixelValue++){
        // Check if the pixel value exists:
        if(pixelLocation[pixelValue].size()!=0) {
            concatenatedPixelLocations.insert(concatenatedPixelLocations.end(), pixelLocation[pixelValue].begin(),
                                              pixelLocation[pixelValue].end());
        }
    }

    // Retrieve indexes and assign a value
    int binNumber = 0;
    for(int pixelLocationIndex = 0; pixelLocationIndex < numberOfPixels; pixelLocationIndex++){

        // Check if the bin is empty
        if(pixelsPerBin[binNumber] == 0){
            binNumber+=1;
        }
        // Set the pixel value
        pixelData[concatenatedPixelLocations[pixelLocationIndex]] = (unsigned char)binNumber;
        // Decrement the pixel per bin value
        pixelsPerBin[binNumber] -= 1;

    }

//     Set pixel values in image Object
    imageObject->setPixelValues(pixelData);
}
//----------------------------------------------------------------------------------------------------------------//
// PSNR Calcluation:
void imageAlgorithms::psnr(imageData originalImage){

    // Initialize variables
    vector<unsigned char> noisyPixelData = imageObject->getPixelValues();
    vector<unsigned char> originalPixelData = originalImage.getPixelValues();
    int numberOfPixels = originalPixelData.size();
    double mseValue = 0;
    double psnrValue = 0;

    for(int index = 0; index < numberOfPixels; index++){
        mseValue += (1.0/numberOfPixels)*((double)noisyPixelData[index] - (double)originalPixelData[index])*((double)noisyPixelData[index] - (double)originalPixelData[index]);

    }
    psnrValue = 10*log10((255^2)/mseValue);
    cout << psnrValue << endl;

}
//----------------------------------------------------------------------------------------------------------------//
// Mean filter:
void imageAlgorithms::meanFilter(int windowSize) {

    // Initialize variables
    vector<unsigned char> noisyPixelData = imageObject->getPixelValues();
    int BytesPerPixel = imageObject->getBytesPerPixel();
    int imageWidth = imageObject->getImageWidth();
    int imageHeight = imageObject->getImageHeight();
    double meanValue;
    vector<unsigned char> windowValues(windowSize*windowSize,0);

    // Extend noisy image to implement filter
    imageData extendedNoisyImage = imageObject->extendImage(floor(windowSize/2));

    // Create output image
    imageData filteredImage(BytesPerPixel,imageWidth,imageHeight);

    //Apply filter

    for(int depthIndex=0; depthIndex<BytesPerPixel;depthIndex++) {
        for (int rowIndex = 0; rowIndex < imageHeight; rowIndex++) {
            for (int columnIndex = 0; columnIndex < imageWidth; columnIndex++) {

                for(int vectorIndex = 0; vectorIndex < windowSize*windowSize; vectorIndex++){

                }

                filteredImage[rowIndex][columnIndex][depthIndex] = (unsigned char)meanValue;

            }
        }
    }
}



