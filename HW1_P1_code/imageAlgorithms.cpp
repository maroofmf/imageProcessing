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

