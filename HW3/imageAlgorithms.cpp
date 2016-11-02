//
// Created by Maroof Mohammed Farooq on 9/11/16.
//

#include "headers.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "imageData.h"
#include "imageAlgorithms.h"
#include "matrix.h"

using namespace std;
using namespace cv;

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
// Bilinear interpolation for 1d image:
unsigned char imageAlgorithms::bilinearInterpolation(double rowIndex, double columnIndex,double depthIndex){

    // Local variables declaration
    unsigned char pixelValue;
    double originalRow,originalColumn;
    double fractionRight, fractionLeft, fractionTop, fractionBottom;
    unsigned char topRightPixelValue,bottomRightPixelValue,topLeftPixelValue,bottomLeftPixelValue;

    fractionBottom = modf(rowIndex,&originalRow);
    fractionTop = 1-fractionBottom;
    fractionRight = modf(columnIndex, &originalColumn);
    fractionLeft = 1-fractionRight;
    topLeftPixelValue = imageObject->accessPixelValue(originalRow,originalColumn,depthIndex);
    bottomLeftPixelValue = imageObject->accessPixelValue(originalRow+1,originalColumn,depthIndex);
    topRightPixelValue = imageObject->accessPixelValue(originalRow,originalColumn,depthIndex);
    bottomRightPixelValue = imageObject->accessPixelValue(originalRow+1,originalColumn,depthIndex);

    // Bilinear interpolation
    pixelValue = (unsigned char)((topLeftPixelValue*fractionTop*fractionLeft+ topRightPixelValue*fractionTop*fractionRight+
                                  bottomRightPixelValue*fractionBottom*fractionRight + bottomLeftPixelValue*fractionBottom*fractionLeft));

    return pixelValue;

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
    double numberOfPixels = originalPixelData.size();
    double mseValue = 0;
    double psnrValue = 0;

    for(int index = 0; index < numberOfPixels; index++){
        mseValue += (1.0/numberOfPixels)*((double)noisyPixelData[index] - (double)originalPixelData[index])*((double)noisyPixelData[index] - (double)originalPixelData[index]);

    }
    psnrValue = 10.00*log10(pow(255.0,2)/mseValue);
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

                // Accumulate all window values
                for(int x_windowIndex = 0; x_windowIndex<windowSize; x_windowIndex++ ){
                    for(int y_windowIndex = 0; y_windowIndex<windowSize; y_windowIndex++ ){
                        windowValues.push_back(extendedNoisyImage.getPixelValues(rowIndex+x_windowIndex,columnIndex+y_windowIndex,depthIndex));
                    }
                }

                // Calculate mean
                meanValue = accumulate(windowValues.begin(),windowValues.end(),0.0)/windowValues.size();
                windowValues.clear();
                filteredImage.setPixelValues((unsigned char)meanValue,rowIndex,columnIndex,depthIndex);

            }
        }
    }

    imageObject->setPixelValues(filteredImage.getPixelValues());
}
////----------------------------------------------------------------------------------------------------------------//
// Median filter:
void imageAlgorithms::medianFilter(int windowSize) {

    // Initialize variables
    vector<unsigned char> noisyPixelData = imageObject->getPixelValues();
    int BytesPerPixel = imageObject->getBytesPerPixel();
    int imageWidth = imageObject->getImageWidth();
    int imageHeight = imageObject->getImageHeight();
    double medianValue;
    vector<unsigned char> windowValues(windowSize*windowSize,0);

    // Extend noisy image to implement filter
    imageData extendedNoisyImage = imageObject->extendImage(floor(windowSize/2));

    // Create output image
    imageData filteredImage(BytesPerPixel,imageWidth,imageHeight);

    //Apply filter

    for(int depthIndex=0; depthIndex<BytesPerPixel;depthIndex++) {
        for (int rowIndex = 0; rowIndex < imageHeight; rowIndex++) {
            for (int columnIndex = 0; columnIndex < imageWidth; columnIndex++) {

//                 Accumulate all window values
//                 Box window
                for(int x_windowIndex = 0; x_windowIndex<windowSize; x_windowIndex++ ){
                    for(int y_windowIndex = 0; y_windowIndex<windowSize; y_windowIndex++ ){
                        windowValues.push_back(extendedNoisyImage.getPixelValues(rowIndex+x_windowIndex,columnIndex+y_windowIndex,depthIndex));
                    }
                }

                // Calculate median
                sort(windowValues.begin(),windowValues.end());
                medianValue = windowValues[floor(windowValues.size()/2)];
                windowValues.clear();

                filteredImage.setPixelValues((unsigned char)medianValue,rowIndex,columnIndex,depthIndex);

            }
        }
    }

    imageObject->setPixelValues(filteredImage.getPixelValues());
}
//----------------------------------------------------------------------------------------------------------------//
//// Median filter with plus:
//void imageAlgorithms::medianFilter(int windowSize) {
//
//    // Initialize variables
//    vector<unsigned char> noisyPixelData = imageObject->getPixelValues();
//    int BytesPerPixel = imageObject->getBytesPerPixel();
//    int imageWidth = imageObject->getImageWidth();
//    int imageHeight = imageObject->getImageHeight();
//    double medianValue;
//    vector<unsigned char> windowValues(windowSize*windowSize,0);
//
//    // Extend noisy image to implement filter
//    imageData extendedNoisyImage = imageObject->extendImage(2);
//
//    // Create output image
//    imageData filteredImage(BytesPerPixel,imageWidth,imageHeight);
//
//    //Apply filter
//
//    for(int depthIndex=0; depthIndex<BytesPerPixel;depthIndex++) {
//        for (int rowIndex = 0; rowIndex < imageHeight; rowIndex++) {
//            for (int columnIndex = 0; columnIndex < imageWidth; columnIndex++) {
//
////                 Accumulate all window values
////                 Plus window
//                for(int x_windowIndex=-2;x_windowIndex<3;x_windowIndex++){
//                    windowValues.push_back(extendedNoisyImage.getPixelValues(rowIndex+x_windowIndex,columnIndex+2,depthIndex));
//                }
//                for(int y_windowIndex=-2;y_windowIndex<3;y_windowIndex++){
//                    windowValues.push_back(extendedNoisyImage.getPixelValues(rowIndex+2,columnIndex+y_windowIndex,depthIndex));
//                }
//
//                // Calculate median
//                sort(windowValues.begin(),windowValues.end());
//                medianValue = windowValues[floor(windowValues.size()/2)];
//                windowValues.clear();
//
//                filteredImage.setPixelValues((unsigned char)medianValue,rowIndex,columnIndex,depthIndex);
//
//            }
//        }
//    }
//
//    imageObject->setPixelValues(filteredImage.getPixelValues());
//}

//----------------------------------------------------------------------------------------------------------------//
// NLM Filtering:

void imageAlgorithms::NLMFilter(int windowSize, int searchWindowSize, double decayFactor){

    // Initialize variables
    int BytesPerPixel = imageObject->getBytesPerPixel();
    int imageWidth = imageObject->getImageWidth();
    int imageHeight = imageObject->getImageHeight();
    imageData filteredImage(BytesPerPixel,imageWidth,imageHeight);
    double pixelValue;
    imageData searchImage(1,searchWindowSize,searchWindowSize);
    imageData windowImage(1,windowSize,windowSize);
    int extendBy = floor((searchWindowSize)/2);
    int extendBy_window = floor(windowSize/2);
    double weightValue;
    vector<double> weightVector(searchWindowSize*searchWindowSize,0);
    double size = BytesPerPixel*imageHeight*imageWidth;
    double status = 0;

    // Resize the image;
    imageData noisyImage = imageObject->extendImage(extendBy);

    // Iterate through the image
    for(int depthIndex=0; depthIndex<BytesPerPixel;depthIndex++) {
        for (int rowIndex = 0; rowIndex < imageHeight; rowIndex++) {
            for (int columnIndex = 0; columnIndex < imageWidth; columnIndex++) {

                // Fill search Image pixels
                for(int searchImageRow = 0; searchImageRow < searchWindowSize; searchImageRow++ ){
                    for(int searchImageColumn = 0; searchImageColumn < searchWindowSize; searchImageColumn++ ){
                        pixelValue = noisyImage.getPixelValues(searchImageRow+rowIndex,searchImageColumn+columnIndex,depthIndex);
                        searchImage.setPixelValues(pixelValue,searchImageRow,searchImageColumn,0);
                    }
                }

                // Fill window Image pixels
                for(int windowRow = 0; windowRow < windowSize; windowRow++ ){
                    for(int windowColumn = 0; windowColumn < windowSize; windowColumn++ ){
                        int windowRowIndex = windowRow+rowIndex+extendBy-floor(windowSize/2);
                        int windowColumnIndex = windowColumn + columnIndex + extendBy - floor(windowSize/2);
                        pixelValue = noisyImage.getPixelValues(windowRowIndex,windowColumnIndex,depthIndex);
                        windowImage.setPixelValues(pixelValue,windowRow,windowColumn,0);
                    }
                }

                // Finding weights
                weightVector.clear();
                imageData extendedSearchWindow = searchImage.extendImage(extendBy_window);
                for(int searchImageRow = 0; searchImageRow < searchWindowSize; searchImageRow++ ){
                    for(int searchImageColumn = 0; searchImageColumn < searchWindowSize; searchImageColumn++ ){

                        // Find square of differences value
                        // Box window
                        double movingWindowValues = 0,fixedWindowValue = 0;
                        double dValue = 0;
                        for(int windowRow = 0; windowRow < windowSize; windowRow++ ) {
                            for (int windowColumn = 0; windowColumn < windowSize; windowColumn++) {
                                movingWindowValues = (double)(extendedSearchWindow.getPixelValues(searchImageRow+windowRow,searchImageColumn+windowColumn,0));
                                fixedWindowValue = (double)windowImage.getPixelValues(windowRow,windowColumn,0);
                                dValue += pow(movingWindowValues-fixedWindowValue,2);
                            }
                        }

                        weightValue = exp(-dValue/(decayFactor*decayFactor));
                        weightVector.push_back(weightValue);
                    }
                }

                // Normalizing all weights
                double weightSum = accumulate(weightVector.begin(),weightVector.end(),0.0);
                for(int index=0; index< weightVector.size(); index++){
                    weightVector[index] = weightVector[index]/weightSum;
                }

                pixelValue = 0;
                for(int searchImageRow = 0; searchImageRow < searchWindowSize; searchImageRow++ ) {
                    for (int searchImageColumn = 0; searchImageColumn < searchWindowSize; searchImageColumn++) {
                        pixelValue += weightVector[searchImageRow*searchWindowSize+searchImageColumn]*(double)searchImage.getPixelValues(searchImageRow,searchImageColumn,0);
                    }
                }
                filteredImage.setPixelValues((unsigned char)pixelValue,rowIndex,columnIndex,depthIndex);
            }
        }
    }

    imageObject->setPixelValues(filteredImage.getPixelValues());

}
//----------------------------------------------------------------------------------------------------------------//
// Dithering:

void imageAlgorithms::dithering(int indexMatrixSize,bool useGivenMatrix){

    // Local variables
    double thresholdValue;
    int imageHeight = imageObject->getImageHeight();
    int imageWidth = imageObject->getImageWidth();
    int BytesPerPixel = imageObject->getBytesPerPixel();
    imageData ditheredImage(BytesPerPixel,imageWidth,imageHeight);
    unsigned char pixelValue;

    // Pre-defined matrices:
    matrix<int,int> givenMat(4,4,1);
    givenMat.setMatrixByValues(16,14,10,11,15,9,3,0,4,8,2,1,5,13,7,6,12);

    // Check if the image is grayscale
    if(BytesPerPixel!=1){
        cout<< " Image should be grayscale for dithering" << endl;
        exit(-1);
    }

    // Computing the bayer index matrix
    matrix<int,int> bayerIndexMat = bayerMat(indexMatrixSize);
    matrix<int,unsigned char> thresholdMat(indexMatrixSize,indexMatrixSize,1);
    if(useGivenMatrix){
        bayerIndexMat = givenMat;
    }

    // Calculate threshold matrix
    for(int rowIndex=0; rowIndex< indexMatrixSize; rowIndex++){
        for(int columnIndex = 0; columnIndex < indexMatrixSize; columnIndex++){
            thresholdValue = 255.0*(((double)bayerIndexMat.getMatrixValues(rowIndex,columnIndex,0)+0.5)/(double)pow(indexMatrixSize,2.0));
            thresholdMat.setMatrixValues((unsigned char)thresholdValue,rowIndex,columnIndex,0);
        }
    }

    thresholdMat.printMatrix();

    // Dither the image
    for(int rowIndex=0; rowIndex< imageWidth; rowIndex++){
        for(int columnIndex = 0; columnIndex < imageHeight; columnIndex++){
            pixelValue = imageObject->getPixelValues(rowIndex,columnIndex,0);
            if(pixelValue > thresholdMat.getMatrixValues(rowIndex%indexMatrixSize,columnIndex%indexMatrixSize,0)){
                pixelValue = 255;
            }else{
                pixelValue = 0;
            }

            ditheredImage.setPixelValues(pixelValue,rowIndex,columnIndex,0);
        }
    }

    imageObject->setPixelValues(ditheredImage.getPixelValues());

}

//----------------------------------------------------------------------------------------------------------------//
// Helper function for dithering. (Used to get bayer index matrix)
matrix<int,int> imageAlgorithms::bayerMat(int size){

    matrix<int,int> bayerMatrix(size,size,1);
    matrix<int,int> basicIndexMatrix(2,2,1);
    basicIndexMatrix.setMatrixByValues(4,0,2,3,1);
    if(size == 2){
        bayerMatrix = basicIndexMatrix;
    }else {

        // Check if size is correct
        if (size % 2 != 0) {
            cout << "Wrong bayer matrix size" << endl;
            exit(0);
        }

        // Recurse till you get the final output
        matrix<int, int> returnMat = bayerMat(size/2);
        int matValue;

        //Multiply the bayerMartix
        for (int rowIndex = 0; rowIndex < size / 2; rowIndex++) {
            for (int columnIndex = 0; columnIndex < size / 2; columnIndex++) {
                matValue = 4*returnMat.getMatrixValues(rowIndex,columnIndex,0);
                bayerMatrix.setMatrixValues(matValue,rowIndex,columnIndex,0);
            }
        }

        for (int rowIndex = 0; rowIndex < size / 2; rowIndex++) {
            for (int columnIndex = 0; columnIndex < size / 2; columnIndex++) {
                matValue = 4*returnMat.getMatrixValues(rowIndex,columnIndex,0)+2;
                bayerMatrix.setMatrixValues(matValue,rowIndex,columnIndex+size/2,0);
            }
        }

        for (int rowIndex = 0; rowIndex < size / 2; rowIndex++) {
            for (int columnIndex = 0; columnIndex < size / 2; columnIndex++) {
                matValue = 4*returnMat.getMatrixValues(rowIndex,columnIndex,0)+3;
                bayerMatrix.setMatrixValues(matValue,rowIndex+size/2,columnIndex,0);
            }
        }

        for (int rowIndex = 0; rowIndex < size / 2; rowIndex++) {
            for (int columnIndex = 0; columnIndex < size / 2; columnIndex++) {
                matValue = 4*returnMat.getMatrixValues(rowIndex,columnIndex,0)+1;
                bayerMatrix.setMatrixValues(matValue,rowIndex+size/2,columnIndex+size/2,0);
            }
        }

    }

    return(bayerMatrix);

}
//----------------------------------------------------------------------------------------------------------------//
// Four level dithering
void imageAlgorithms::fourLevelDithering(){

    // Local variables
    double thresholdValue;
    int imageHeight = imageObject->getImageHeight();
    int imageWidth = imageObject->getImageWidth();
    int BytesPerPixel = imageObject->getBytesPerPixel();
    imageData ditheredImage(BytesPerPixel,imageWidth,imageHeight);
    unsigned char pixelValue;

    // Dither the image
    for(int rowIndex=0; rowIndex< imageWidth; rowIndex++){
        for(int columnIndex = 0; columnIndex < imageHeight; columnIndex++){
            pixelValue = imageObject->getPixelValues(rowIndex,columnIndex,0);


            if((pixelValue <= 85 )){
                pixelValue = 85;
            }else if((pixelValue <= 170)&&(pixelValue > 85 )){
                pixelValue = 170;
            }else if((pixelValue <= 255)&&(pixelValue > 170 )){
                pixelValue = 255;
            }

//            if(pixelValue <= 43 ){
//                pixelValue = 0;
//            }else if((pixelValue <= 127)&&(pixelValue > 43 )){
//                pixelValue = 85;
//            }else if((pixelValue <= 212)&&(pixelValue > 127 )){
//                pixelValue = 170;
//            }else if((pixelValue <= 255)&&(pixelValue > 212 )){
//                pixelValue = 255;
//            }

            ditheredImage.setPixelValues(pixelValue,rowIndex,columnIndex,0);
        }
    }

    imageObject->setPixelValues(ditheredImage.getPixelValues());

}
////----------------------------------------------------------------------------------------------------------------//
//// Error diffusion halftoning:
//
//void imageAlgorithms::errorDiffusion(string algorithm){
//
//    // Local Variables
//    int imageHeight = imageObject->getImageHeight();
//    int imageWidth = imageObject->getImageWidth();
//    int BytesPerPixel = imageObject->getBytesPerPixel();
//    imageData extendedImage;
//    imageData ditheredImage(BytesPerPixel,imageWidth,imageHeight);
//    matrix<int,double> errorDiffusionMatrix;
//    int oldPixelValue, newPixelValue, tempPixelValue;
//    int pixelDiff;
//    int threshold = 127;
//    int extendBy;
//
//
//    if(algorithm=="floyd-steinberg"){
//        // Set values for diffusion matrix
//        errorDiffusionMatrix.setMatrixDimentions(3,3,1);
//        errorDiffusionMatrix.setMatrixByValues(9,0.0,0.0,0.0,0.0,0.0,7.0,3.0,5.0,1.0);
//        errorDiffusionMatrix.multiplyEachValueBy(1/16.0);
//        // Extend image
//        extendBy = 1;
//
//    }else if(algorithm=="JJN"){
//        // Set values for diffusion matrix
//        errorDiffusionMatrix.setMatrixDimentions(5,5,1);
//        errorDiffusionMatrix.setMatrixByValues(25,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,7.0,5.0,3.0,5.0,7.0,5.0,3.0,1.0,3.0,5.0,3.0,1.0);
//        errorDiffusionMatrix.multiplyEachValueBy(1/48.0);
//        // Extend image
//        extendBy = 2;
//
//
//    }else if(algorithm=="stucki"){
//        // Set values for diffusion matrix
//        errorDiffusionMatrix.setMatrixDimentions(5,5,1);
//        errorDiffusionMatrix.setMatrixByValues(25,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,7.0,5.0,3.0,5.0,7.0,5.0,3.0,1.0,3.0,5.0,3.0,1.0);
//        errorDiffusionMatrix.multiplyEachValueBy(1/42.0);
//        // Extend image
//        extendBy = 2;
//    }
//
//    extendedImage = imageObject->extendImage(extendBy);
//
//    // Serpentine scanning:
//    int matrixSize = errorDiffusionMatrix.getHeight();
//    int columnIndexStart=0, columnIndexEnd=imageWidth;
//
//    for(int rowIndex = 0; rowIndex < imageHeight; rowIndex++){
//        for(int columnIndex = columnIndexStart; columnIndex < columnIndexEnd; columnIndex++){
//
//            // Get pixel value
//            oldPixelValue = extendedImage.getPixelValues(rowIndex+extendBy,abs(columnIndex)+extendBy,0);
//            newPixelValue = ((oldPixelValue < threshold)?0:255);
//            pixelDiff = oldPixelValue - newPixelValue;
//
//            // Set pixel value
//            ditheredImage.setPixelValues((unsigned char)newPixelValue,rowIndex,abs(columnIndex),0);
//
//            // Error Diffusion
//            if(((rowIndex+1)%2 == 0)||(rowIndex==0)){
//
//                for(int filterRowIndex = 0; filterRowIndex < matrixSize; filterRowIndex++){
//                    for(int filterColumnIndex = 0; filterColumnIndex < matrixSize; filterColumnIndex++){
//
//                        tempPixelValue = extendedImage.getPixelValues(rowIndex+filterRowIndex,abs(columnIndex)+filterColumnIndex,0);
//                        tempPixelValue = tempPixelValue + floor(((double)pixelDiff)*errorDiffusionMatrix.getMatrixValues(filterRowIndex,filterColumnIndex,0));
//
//                        if(tempPixelValue<0){
//                            tempPixelValue = 0;
//                        } else if(tempPixelValue>255){
//                            tempPixelValue = 255;
//                        }
//
//                        extendedImage.setPixelValues((unsigned char)tempPixelValue,rowIndex+filterRowIndex,abs(columnIndex)+filterColumnIndex,0);
//                    }
//                }
//
//            }else{
//                for(int filterRowIndex = 0; filterRowIndex < matrixSize; filterRowIndex++){
//                    for(int filterColumnIndex = 0; filterColumnIndex < matrixSize; filterColumnIndex++){
//
//                        tempPixelValue = extendedImage.getPixelValues(rowIndex+filterRowIndex,abs(columnIndex)+filterColumnIndex,0);
//                        tempPixelValue = tempPixelValue + floor((double)pixelDiff*(errorDiffusionMatrix.getMatrixValues(filterRowIndex,matrixSize-filterColumnIndex-1,0)));
//
//                        if(tempPixelValue<0){
//                            tempPixelValue = 0;
//                        } else if(tempPixelValue>255){
//                            tempPixelValue = 255;
//                        }
//
//                        extendedImage.setPixelValues((unsigned char)tempPixelValue,rowIndex+filterRowIndex,abs(columnIndex)+filterColumnIndex,0);
//                    }
//                }
//
//            }
//        }
//
//        // Check if row index is even
//        if((rowIndex+1)%2 == 0){
//            columnIndexStart = 0;
//            columnIndexEnd = imageWidth;
//        }else{
//            columnIndexStart = -imageWidth + 1;
//            columnIndexEnd = 1;
//        }
//
//    }
//
//    imageObject->setPixelValues(ditheredImage.getPixelValues());
//
//}

//----------------------------------------------------------------------------------------------------------------//
// Error diffusion halftoning:

void imageAlgorithms::errorDiffusion(string algorithm){

    // Local Variables
    int imageHeight = imageObject->getImageHeight();
    int imageWidth = imageObject->getImageWidth();
    int BytesPerPixel = imageObject->getBytesPerPixel();
    imageData extendedImage;
    imageData ditheredImage(BytesPerPixel,imageWidth,imageHeight);
    matrix<int,double> errorDiffusionMatrix;
    int oldPixelValue, newPixelValue, tempPixelValue;
    int pixelDiff;
    int threshold = 127;
    int extendBy;


    if(algorithm=="floyd-steinberg"){
        // Set values for diffusion matrix
        errorDiffusionMatrix.setMatrixDimentions(3,3,1);
        errorDiffusionMatrix.setMatrixByValues(9,0.0,0.0,0.0,0.0,0.0,7.0,3.0,5.0,1.0);
        errorDiffusionMatrix.multiplyEachValueBy(1/16.0);
        // Extend image
        extendBy = 1;

    }else if(algorithm=="JJN"){
        // Set values for diffusion matrix
        errorDiffusionMatrix.setMatrixDimentions(5,5,1);
        errorDiffusionMatrix.setMatrixByValues(25,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,7.0,5.0,3.0,5.0,7.0,5.0,3.0,1.0,3.0,5.0,3.0,1.0);
        errorDiffusionMatrix.multiplyEachValueBy(1/48.0);
        // Extend image
        extendBy = 2;


    }else if(algorithm=="stucki"){
        // Set values for diffusion matrix
        errorDiffusionMatrix.setMatrixDimentions(5,5,1);
        errorDiffusionMatrix.setMatrixByValues(25,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,7.0,5.0,3.0,5.0,7.0,5.0,3.0,1.0,3.0,5.0,3.0,1.0);
        errorDiffusionMatrix.multiplyEachValueBy(1/42.0);
        // Extend image
        extendBy = 2;
    }

    extendedImage = imageObject->extendImage(extendBy);
    matrix<int,double> extendedMatrix(extendedImage.getImageHeight(),extendedImage.getImageWidth(),1);

    vector<unsigned char> pixelValues = extendedImage.getPixelValues();
    vector<double> matrixValues;

    // Type conversion and loading matrix.
    for(int index = 0; index < pixelValues.size(); index++){
        matrixValues.push_back((double)pixelValues[index]);
    }
    extendedMatrix.setMatrixValues(matrixValues);


    // Serpentine scanning:
    int matrixSize = errorDiffusionMatrix.getHeight();
    int columnIndexStart=0, columnIndexEnd=imageWidth;

    for(int rowIndex = 0; rowIndex < imageHeight; rowIndex++){
        for(int columnIndex = columnIndexStart; columnIndex < columnIndexEnd; columnIndex++){

            // Get pixel value
            oldPixelValue = extendedMatrix.getMatrixValues(rowIndex+extendBy,abs(columnIndex)+extendBy,0);
            newPixelValue = ((oldPixelValue < threshold)?0:255);
            pixelDiff = oldPixelValue - newPixelValue;

            // Set pixel value
            ditheredImage.setPixelValues((unsigned char)newPixelValue,rowIndex,abs(columnIndex),0);

            // Error Diffusion
            if(((rowIndex)%2 == 0)||(rowIndex==0)){

                for(int filterRowIndex = 0; filterRowIndex < matrixSize; filterRowIndex++){
                    for(int filterColumnIndex = 0; filterColumnIndex < matrixSize; filterColumnIndex++){

                        tempPixelValue = extendedMatrix.getMatrixValues(rowIndex+filterRowIndex,abs(columnIndex)+filterColumnIndex,0);
                        tempPixelValue = tempPixelValue + floor(((double)pixelDiff)*errorDiffusionMatrix.getMatrixValues(filterRowIndex,filterColumnIndex,0));
                        extendedMatrix.setMatrixValues((double)tempPixelValue,rowIndex+filterRowIndex,abs(columnIndex)+filterColumnIndex,0);
                    }
                }


            }else{

                for(int filterRowIndex = 0; filterRowIndex < matrixSize; filterRowIndex++){
                    for(int filterColumnIndex = 0; filterColumnIndex < matrixSize; filterColumnIndex++){

                        tempPixelValue = extendedMatrix.getMatrixValues(rowIndex+filterRowIndex,abs(columnIndex)+filterColumnIndex,0);
                        tempPixelValue = tempPixelValue + floor((double)pixelDiff*(errorDiffusionMatrix.getMatrixValues(filterRowIndex,matrixSize-filterColumnIndex-1,0)));
                        extendedMatrix.setMatrixValues((double)tempPixelValue,rowIndex+filterRowIndex,abs(columnIndex)+filterColumnIndex,0);
                    }
                }

            }
        }

        // Check if row index is even
        if((rowIndex+1)%2 == 0){
            columnIndexStart = 0;
            columnIndexEnd = imageWidth;
        }else{
            columnIndexStart = -imageWidth + 1;
            columnIndexEnd = 1;
        }

    }

    imageObject->setPixelValues(ditheredImage.getPixelValues());

}

//----------------------------------------------------------------------------------------------------------------//
// Binarize:

void imageAlgorithms::binarize(double threshold,bool invert){

    int imageHeight = imageObject->getImageHeight();
    int imageWidth = imageObject->getImageWidth();
    int BytePerPixels = imageObject->getBytesPerPixel();
    unsigned char pixelValue;

    if(BytePerPixels!=1){
        cout<< "Error! Binarization directly from color image not tested"<<endl;
        exit(0);
    }

    for(int rowIndex=0; rowIndex<imageHeight;rowIndex++) {
        for (int columnIndex = 0; columnIndex < imageWidth; columnIndex++) {

            pixelValue = imageObject->getPixelValues(rowIndex,columnIndex,0);
            pixelValue = (pixelValue>threshold)? 0:255;

            if(invert){
                pixelValue = abs(255-pixelValue);
            }

            imageObject->setPixelValues(pixelValue,rowIndex,columnIndex,0);
        }
    }

}
//----------------------------------------------------------------------------------------------------------------//
// Binarize based on mode:
void imageAlgorithms::binarizeBasedOnMode(double radius,bool invert){

    int imageHeight = imageObject->getImageHeight();
    int imageWidth = imageObject->getImageWidth();
    int BytePerPixels = imageObject->getBytesPerPixel();
    unsigned char pixelValue;
    int modeValue = 0;

    if(BytePerPixels!=1){
        cout<< "Error! Binarization directly from color image not tested"<<endl;
        exit(0);
    }

    // Finding mode of the image:
    vector<int> histogram(255,0);
    for(int rowIndex=0; rowIndex<imageHeight;rowIndex++) {
        for (int columnIndex = 0; columnIndex < imageWidth; columnIndex++) {
            ++histogram[imageObject->getPixelValues(rowIndex,columnIndex,0)];
        }
    }

    modeValue = max_element( histogram.begin(), histogram.end() ) - histogram.begin();


    for(int rowIndex=0; rowIndex<imageHeight;rowIndex++) {
        for (int columnIndex = 0; columnIndex < imageWidth; columnIndex++) {

            pixelValue = imageObject->getPixelValues(rowIndex,columnIndex,0);

            if((pixelValue>=modeValue-radius)&&(pixelValue<=modeValue+radius)){
                pixelValue = 255;
            }else{
                pixelValue = 0;
            }

            if(invert){
                pixelValue = abs(255-pixelValue);
            }

            imageObject->setPixelValues(pixelValue,rowIndex,columnIndex,0);
        }
    }
}
//----------------------------------------------------------------------------------------------------------------//
// Char to Boolean conversion:
matrix<int,bool> imageAlgorithms::charToBoolean(){

    // Init local variables
    int imageHeight = imageObject->getImageHeight();
    int imageWidth = imageObject->getImageWidth();
    int BytePerPixels = imageObject->getBytesPerPixel();
    matrix<int,bool> boolMatrix(imageHeight,imageWidth,1);

    // Check if input has one channel only
    if(BytePerPixels!=1){
        cout<< "Error in charToBoolean! Cannot input multi channel image" <<endl;
        exit(-1);
    }

    // Get boolean values
    for(int rowIndex=0; rowIndex<imageHeight;rowIndex++) {
        for (int columnIndex = 0; columnIndex < imageWidth; columnIndex++) {

            boolMatrix.setMatrixValues((bool)imageObject->getPixelValues(rowIndex,columnIndex,0),rowIndex,columnIndex,0);
        }
    }

    return boolMatrix;
}

//----------------------------------------------------------------------------------------------------------------//
// Boolean to char conversion:
matrix<int,unsigned char> imageAlgorithms::booleanToChar(matrix<int,bool> inputMatrix){

    // Init local variables
//    int imageHeight = imageObject->getImageHeight();
//    int imageWidth = imageObject->getImageWidth();
//    int BytePerPixels = imageObject->getBytesPerPixel();

    int imageHeight = inputMatrix.getHeight();
    int imageWidth = inputMatrix.getWidth();
    int BytePerPixels = inputMatrix.getDepth();

    matrix<int,unsigned char> pixelValueMatrix(imageHeight,imageWidth,1);
    unsigned char pixelValue;

    // Check if input has one channel only
    if(BytePerPixels!=1){
        cout<< "Error in booleanToChar! Cannot input multi channel image" <<endl;
        exit(-1);
    }

    // Get unsigned char values
    for(int rowIndex=0; rowIndex<imageHeight;rowIndex++) {
        for (int columnIndex = 0; columnIndex < imageWidth; columnIndex++) {

            pixelValue = 255*inputMatrix.getMatrixValues(rowIndex,columnIndex,0);
            pixelValueMatrix.setMatrixValues(pixelValue,rowIndex,columnIndex,0);
        }
    }

    return pixelValueMatrix;

}

//----------------------------------------------------------------------------------------------------------------//
//Adaptive Binarization:
void imageAlgorithms::adaptiveBinarization(int windowSize){

    // Local variables
    int imageHeight = imageObject->getImageHeight();
    int imageWidth = imageObject->getImageWidth();
    int BytePerPixels = imageObject->getBytesPerPixel();
    imageData* outputImage = new imageData(BytePerPixels,imageWidth,imageHeight);
    int numberOfLoops_row = ceil(imageHeight/windowSize);
    int numberOfLoops_columns = ceil(imageWidth/windowSize);

    double thresholdValue = 0;
    unsigned char pixelValue = 0;
    double numberOfElements=0;

    // Loop throught the output elements
    for(int rowIndex = 0; rowIndex <= numberOfLoops_row; rowIndex++){
        for(int columnIndex = 0; columnIndex <= numberOfLoops_columns; columnIndex++){

            // Calculate threshold first
            thresholdValue = 0;
            numberOfElements = 0;
            for(int windowRowIndex = rowIndex*windowSize; windowRowIndex < (rowIndex+1)*windowSize; windowRowIndex++) {
                for (int windowColumnIndex = columnIndex*windowSize; windowColumnIndex < (columnIndex+1)*windowSize; windowColumnIndex++) {

                    // Check if the index is out of bound;
                    if((windowRowIndex>=imageHeight)||(windowColumnIndex>=imageWidth)){
                        continue;
                    }
                    numberOfElements++;
                    thresholdValue += (imageObject->getPixelValues(windowRowIndex,windowColumnIndex,0));
                }
            }
            thresholdValue = thresholdValue/numberOfElements;
//            cout<< thresholdValue <<endl;

            // Binarize the image
            for(int windowRowIndex = rowIndex*windowSize; windowRowIndex < (rowIndex+1)*windowSize; windowRowIndex++) {
                for (int windowColumnIndex = columnIndex*windowSize; windowColumnIndex < (columnIndex+1)*windowSize; windowColumnIndex++) {

                    // Check if the index is out of bound;
                    if((windowRowIndex>=imageHeight)||(windowColumnIndex>=imageWidth)){
                        continue;
                    }

                    pixelValue = imageObject->getPixelValues(windowRowIndex,windowColumnIndex,0);
                    pixelValue = (pixelValue>thresholdValue)?0:255;
                    outputImage->setPixelValues(pixelValue,windowRowIndex,windowColumnIndex,0);
                }
            }

        }
    }

    imageObject->setPixelValues(outputImage->getPixelValues());

}

//----------------------------------------------------------------------------------------------------------------//
// I. Filter applyer:
matrix<int,double> imageAlgorithms::filterApply(imageData frame, matrix<int,double> window,string algorithm){

    // Local Variables
    int imageWidth = frame.getImageWidth();
    int imageHeight = frame.getImageHeight();
    int windowSize = window.getHeight();
    int extendBy = floor(windowSize/2);

    // Test if the input is accurate
    if(windowSize!=window.getWidth()){
        cout<< "Please re-enter the matrix! Wrong matrix size" <<endl;
        exit(-2);
    }
    if(frame.getBytesPerPixel()!=1){
        cout<< "Please re-enter the image! Wrong image size" <<endl;
        exit(-2);
    }


    // Set input and output images
    matrix<int,double> outputFrame(imageHeight,imageWidth,1);
    imageData extendedImage = frame.extendImage(extendBy);
    double outValue = 0.0;

    for(int rowIndex = 0;rowIndex < imageHeight; rowIndex++){
        for(int columnIndex = 0; columnIndex < imageWidth; columnIndex++){

            for(int windowRow = 0;windowRow < windowSize; windowRow++) {
                for (int windowColumn = 0; windowColumn < windowSize; windowColumn++) {

                    if(algorithm =="convolutionWithAbsolute"){
                        outValue += window.getMatrixValues(windowRow,windowColumn,0)*extendedImage.accessPixelValue(rowIndex+windowRow,columnIndex+windowColumn,0);
                    }

                    if(algorithm=="convolutionWithoutAbsolute"){
                        outValue += window.getMatrixValues(windowRow,windowColumn,0)*extendedImage.accessPixelValue(rowIndex+windowRow,columnIndex+windowColumn,0);
                    }

                }
            }

            if(algorithm =="convolutionWithAbsolute"){
                outputFrame.setMatrixValues(fabs(outValue),rowIndex,columnIndex,0);
                outValue=0.0;
            }

            if(algorithm=="convolutionWithoutAbsolute"){
                outputFrame.setMatrixValues(outValue,rowIndex,columnIndex,0);
                outValue=0.0;
            }

        }
    }

    return  outputFrame;

}

////----------------------------------------------------------------------------------------------------------------//
//// II. Filter applyer:
//matrix<int,double> imageAlgorithms::filterApply(matrix<int,double> frame, matrix<int,double> window,string algorithm){
//
//    // Local Variables
//    int imageWidth = frame.getWidth();
//    int imageHeight = frame.getHeight();
//    int windowSize = window.getHeight();
//    int extendBy = floor(windowSize/2);
//
//    // Test if the input is accurate
//    if(windowSize!=window.getWidth() || windowSize%2==0){
//        cout<< "Please re-enter the matrix! Wrong matrix size" <<endl;
//        exit(-2);
//    }
//    if(frame.getDepth()!=1){
//        cout<< "Please re-enter the image! Wrong image size" <<endl;
//        exit(-2);
//    }
//
//    // Set input and output images
//    matrix<int,double> outputFrame(imageHeight,imageWidth,1);
//
//    // Extend Image
//    matrix<int,double> extendedMatrix = frame.extendMatrix(extendBy);
//    double outValue = 0.0;
//
//    for(int rowIndex = 0;rowIndex < imageHeight; rowIndex++){
//        for(int columnIndex = 0; columnIndex < imageWidth; columnIndex++){
//
//            for(int windowRow = 0;windowRow < windowSize; windowRow++) {
//                for (int windowColumn = 0; windowColumn < windowSize; windowColumn++) {
//
//                    if(algorithm =="convolutionWithAbsolute"){
//                        outValue += window.getMatrixValues(windowRow,windowColumn,0)*extendedMatrix.accessMatrixValue(rowIndex+windowRow,columnIndex+windowColumn,0);
//                    }
//
//                    if(algorithm=="convolutionWithoutAbsolute"){
//                        outValue += window.getMatrixValues(windowRow,windowColumn,0)*extendedMatrix.accessMatrixValue(rowIndex+windowRow,columnIndex+windowColumn,0);
//                    }
//
//                }
//            }
//
//            if(algorithm =="convolutionWithAbsolute"){
//                outputFrame.setMatrixValues(fabs(outValue),rowIndex,columnIndex,0);
//                outValue=0.0;
//            }
//
//            if(algorithm=="convolutionWithoutAbsolute"){
//                outputFrame.setMatrixValues(outValue,rowIndex,columnIndex,0);
//                outValue=0.0;
//            }
//
//        }
//    }
//
//    return  outputFrame;
//
//}

//----------------------------------------------------------------------------------------------------------------//
// II. Filter applyer:
matrix<int,double> imageAlgorithms::filterApply(matrix<int,double>* frame, matrix<int,double>* window,string algorithm){

    // Local Variables
    int imageWidth = frame->getWidth();
    int imageHeight = frame->getHeight();
    int windowSize = window->getHeight();
    int extendBy = floor(windowSize/2);

    // Test if the input is accurate
    if(windowSize!=window->getWidth() || windowSize%2==0){
        cout<< "Please re-enter the matrix! Wrong matrix size" <<endl;
        exit(-2);
    }
    if(frame->getDepth()!=1){
        cout<< "Please re-enter the image! Wrong image size" <<endl;
        exit(-2);
    }

    // Set input and output images
    matrix<int,double> outputFrame(imageHeight,imageWidth,1);

    // Extend Image
    matrix<int,double> extendedMatrix = frame->extendMatrix(extendBy);
    double outValue = 0.0;

    for(int rowIndex = 0;rowIndex < imageHeight; rowIndex++){
        for(int columnIndex = 0; columnIndex < imageWidth; columnIndex++){

            for(int windowRow = 0;windowRow < windowSize; windowRow++) {
                for (int windowColumn = 0; windowColumn < windowSize; windowColumn++) {

                        outValue += window->getMatrixValues(windowRow,windowColumn,0)*extendedMatrix.accessMatrixValue(rowIndex+windowRow,columnIndex+windowColumn,0);

                }
            }

            if(algorithm =="convolutionWithAbsolute"){
                outputFrame.setMatrixValues(fabs(outValue),rowIndex,columnIndex,0);
                outValue=0.0;
            }

            if(algorithm=="convolutionWithoutAbsolute"){
                outputFrame.setMatrixValues(outValue,rowIndex,columnIndex,0);
                outValue=0.0;
            }

        }
    }

    return  outputFrame;

}
//----------------------------------------------------------------------------------------------------------------//
// Corner detection algorithm (Harris algorithm):
// TODO: Not fully developed. Only good for this problem
// Assumes four corners for one object;

map<int,vector<int> > imageAlgorithms::harrisCornerDetector(double RThreshold){

    // Initialize all local variables
    int imageWidth = imageObject->getImageWidth();
    int imageHeight = imageObject->getImageHeight();
    map<int,vector<int> > corners;
    matrix<int,double> sobelOperator_x(3,3,1);
    sobelOperator_x.setMatrixByValues(9,-1.0,0.0,1.0,-2.0,0.0,2.0,-1.0,0.0,1.0);
    matrix<int,double> sobelOperator_y(3,3,1);
    sobelOperator_y = sobelOperator_x.transpose();
    int extendBy = 1;
    int windowSize = 3;
    int i = 0;
    map<int,vector<double> > pointsOfInterest;

    // Prepare image
    imageData extendedImage = imageObject->extendImage(extendBy);


    // Apply sobel operator
    matrix<int,double> imageGradient_x = filterApply(extendedImage,sobelOperator_x,"convolutionWithAbsolute");
    matrix<int,double> imageGradient_y = filterApply(extendedImage,sobelOperator_y,"convolutionWithAbsolute");

    // Run harris corner detection algorithm
    matrix<int,double> RMatrix(imageHeight-1,imageWidth-1,1);
    matrix<int,double> MMatrix(2,2,1);
    double RValue = 0.0;
    double value_xx = 0, value_yy = 0,value_xy = 0;

    for(int rowIndex = 1;rowIndex < imageHeight-1; rowIndex++) {
        for (int columnIndex = 1; columnIndex < imageWidth-1; columnIndex++) {

            value_xx = 0.0;
            value_xy = 0.0;
            value_yy = 0.0;

            for (int windowRow = -1; windowRow < 2; windowRow++) {
                for (int windowColumn = -1; windowColumn < 2; windowColumn++) {

                    value_xx += imageGradient_x.getMatrixValues(rowIndex+windowRow,columnIndex+windowColumn,0) * imageGradient_x.getMatrixValues(rowIndex+windowRow,columnIndex+windowColumn,0);
                    value_xy += imageGradient_y.getMatrixValues(rowIndex+windowRow,columnIndex+windowColumn,0) * imageGradient_x.getMatrixValues(rowIndex+windowRow,columnIndex+windowColumn,0);
                    value_yy += imageGradient_y.getMatrixValues(rowIndex+windowRow,columnIndex+windowColumn,0) * imageGradient_y.getMatrixValues(rowIndex+windowRow,columnIndex+windowColumn,0);

                }
            }

            // Setting up MMatrix and calculating R value
            MMatrix.setMatrixByValues(4,value_xx,value_xy,value_xy,value_yy);
            RValue = MMatrix.determinant(MMatrix) - 0.04*pow(MMatrix.trace(MMatrix),2.0);

            // Thresholding R value
            if(RValue > RThreshold){

//                cout << rowIndex <<" " << columnIndex<< " " << RValue <<endl;
                pointsOfInterest[i].push_back(rowIndex);
                pointsOfInterest[i].push_back(columnIndex);
                pointsOfInterest[i].push_back(RValue);
                pointsOfInterest[i].push_back(0.0);
                i++;
            }

        }
    }

    // Non max suppression
    int sizeOfMap = pointsOfInterest.size();
    int nonMaxRadius = 5;
    int maximumIndex;
    int cornerIndex = 0;
    int current_x=0,current_y=0;


    for(int iter= 0; iter<sizeOfMap;iter++){

        if(pointsOfInterest[iter][3]==1){
            continue;
        }

        current_x = pointsOfInterest[iter][0];
        current_y = pointsOfInterest[iter][1];
        pointsOfInterest[iter][3] = 1.0;
        maximumIndex = iter;

        // Calculate maximum RValue and taking it as the corner
        for(int internalIter= 0; internalIter<sizeOfMap;internalIter++){

            if(pointsOfInterest[internalIter][3]==1){
                continue;
            }

            bool withinRadius_x = fabs(pointsOfInterest[internalIter][0] - current_x) < nonMaxRadius;
            bool withinRadius_y = fabs(pointsOfInterest[internalIter][1] - current_y)  < nonMaxRadius;
            bool greaterThanCurrent = pointsOfInterest[internalIter][2] > pointsOfInterest[maximumIndex][2];

            // Set Visited flag
            if(withinRadius_x && withinRadius_y){

                pointsOfInterest[internalIter][3] = 1.0;
                if(greaterThanCurrent){
                    maximumIndex = internalIter;
                }
            }
        }

        corners[cornerIndex].push_back(pointsOfInterest[maximumIndex][0]);
        corners[cornerIndex].push_back(pointsOfInterest[maximumIndex][1]);
        cornerIndex++;

    }

    return corners;

}

//----------------------------------------------------------------------------------------------------------------//
// Morphological algorithm:

matrix<int,bool> imageAlgorithms::morphologicalAlgorithms(matrix<int,bool> binaryImageData, string operation){

    // Local parameters
    int matrixHeight = binaryImageData.getHeight();
    int matrixWidth = binaryImageData.getWidth();
    matrix<int,bool> postShrinking(matrixHeight,matrixWidth,1);
    matrix<int,bool> intermediateMatrix = binaryImageData;
    matrix<int,bool> preShrinking = binaryImageData;
    bool matrixValue;
    int windowBitStream = 0;
    bool mValue = 0;
    bool postValue = 0;
    bool firstIteration = 1;

    // Check if the operation specified is correct!
    if((operation!="Shrinking")&&((operation!="Thinning"))&&((operation!="Skeletonizing"))){
        cout<< "Please specify the correct operation for morphological Algorithms" <<endl;
        exit(0);
    }



    while(postShrinking!=preShrinking){

        if(!firstIteration){
            preShrinking.setMatrixValues(postShrinking.getMatrixValues());
        }
        firstIteration = 0;

        //Pass one
        for(int rowIndex = 1; rowIndex< matrixHeight-1; rowIndex++ ){
            for(int columnIndex = 1; columnIndex< matrixWidth-1; columnIndex++ ){

                matrixValue = preShrinking.getMatrixValues(rowIndex,columnIndex,0);

                // Check if matrix value is 0 or 1
                if(!matrixValue){
                    intermediateMatrix.setMatrixValues(matrixValue,rowIndex,columnIndex,0);
                    continue;
                }

                // Get all neighbouring pixels
                int bitSetter = 0x100;
                bool bitValue = 0;
                windowBitStream = 0;

                for(int windowRowIndex = -1; windowRowIndex<2;windowRowIndex++){
                    for(int windowColumnIndex = -1; windowColumnIndex < 2;windowColumnIndex++){

                        bitValue = preShrinking.getMatrixValues(rowIndex+windowRowIndex,columnIndex+windowColumnIndex,0);

                        if(bitValue){
                            windowBitStream |= bitSetter;
                        }

                        bitSetter = bitSetter >> 1;

                    }
                }


                // Check the conditional pattern
                mValue = conditionalPatternMatching(operation,windowBitStream);

                // Set mValue for intermediate matrix
                intermediateMatrix.setMatrixValues(mValue,rowIndex,columnIndex,0);
            }
        }

        // Pass two
        for(int rowIndex = 1; rowIndex< matrixHeight-1; rowIndex++ ) {
            for (int columnIndex = 1; columnIndex < matrixWidth - 1; columnIndex++) {

                mValue = intermediateMatrix.getMatrixValues(rowIndex,columnIndex,0);

                // Check if matrix value is 0 or 1
                if(!mValue){
                    postShrinking.setMatrixValues(preShrinking.getMatrixValues(rowIndex,columnIndex,0),rowIndex,columnIndex,0);
                    continue;
                }

                // Get all neighbouring pixels
                int bitSetter = 0x100;
                bool bitValue = 0;
                windowBitStream = 0;

                for(int windowRowIndex = -1; windowRowIndex<2;windowRowIndex++){
                    for(int windowColumnIndex = -1; windowColumnIndex < 2;windowColumnIndex++){
                        bitValue = intermediateMatrix.getMatrixValues(rowIndex+windowRowIndex,columnIndex+windowColumnIndex,0);

                        if(bitValue){
                            windowBitStream |= bitSetter;
                        }
                        bitSetter = bitSetter >>1;
                    }
                }

                // Check the conditional pattern
                postValue = unconditionalPatternMatching(operation,windowBitStream);

                // Set postValue for postShrinking matrix
                postShrinking.setMatrixValues(postValue,rowIndex,columnIndex,0);

            }
        }

    }

    return postShrinking;

}

//----------------------------------------------------------------------------------------------------------------//
// conditional pattern matching function
// Private method to determine if conditional pattern is true

bool imageAlgorithms::conditionalPatternMatching(string operation,int obtainedPattern){

    // Local Variables
    bool hitOrMiss = 0;
    int boundValue = bound(obtainedPattern);

    // Static look up tables:
    static map<int,vector<int> > shrink_LUT;
    static map<int,vector<int> > thinning_LUT;
    static map<int,vector<int> > skeletonizing_LUT;

    static int shrink_1[] = {0b001010000,0b100010000,0b000010100,0b000010001};
    static vector<int> shrink_1_vec(shrink_1, shrink_1 + sizeof(shrink_1) / sizeof(shrink_1[0]));
    shrink_LUT[1] = shrink_1_vec;

    static int shrink_2[] = {0b000011000,0b010010000,0b000110000,0b000010010};
    static vector<int> shrink_2_vec(shrink_2, shrink_2 + sizeof(shrink_2) / sizeof(shrink_2[0]));
    shrink_LUT[2] = shrink_2_vec;

    static int shrink_3[] = {0b001011000,0b011010000,0b110010000,0b100110000,0b000110100,0b000010110,0b000010011,0b000011001};
    static vector<int> shrink_3_vec(shrink_3, shrink_3 + sizeof(shrink_3) / sizeof(shrink_3[0]));
    shrink_LUT[3] = shrink_3_vec;

    static int shrink_4[] = {0b001011001,0b111010000,0b100110100,0b000010111};
    static vector<int> shrink_4_vec(shrink_4, shrink_4 + sizeof(shrink_4) / sizeof(shrink_4[0]));
    shrink_LUT[4] = shrink_4_vec;

    static int shrink_5[] = {0b110011000,0b010011001,0b011110000,0b001011010,0b011011000,0b110110000,0b000110110,0b000011011};
    static vector<int> shrink_5_vec(shrink_5, shrink_5 + sizeof(shrink_5) / sizeof(shrink_5[0]));
    shrink_LUT[5] = shrink_5_vec;

    static int shrink_6[] = {0b110011001,0b011110100,0b111011000,0b011011001,0b111110000,0b110110100,0b100110110,0b000110111,0b000011111,0b001011011};
    static vector<int> shrink_6_vec(shrink_6, shrink_6 + sizeof(shrink_6) / sizeof(shrink_6[0]));
    shrink_LUT[6] = shrink_6_vec;

    static int shrink_7[] = {0b111011001,0b111110100,0b100110111,0b001011111};
    static vector<int> shrink_7_vec(shrink_7, shrink_7 + sizeof(shrink_7) / sizeof(shrink_7[0]));
    shrink_LUT[7] = shrink_7_vec;

    static int shrink_8[] = {0b011011011,0b111111000,0b110110110,0b000111111};
    static vector<int> shrink_8_vec(shrink_8, shrink_8 + sizeof(shrink_8) / sizeof(shrink_8[0]));
    shrink_LUT[8] = shrink_8_vec;

    static int shrink_9[] = {0b111011011,0b011011111,0b111111100,0b111111001,0b111110110,0b110110111,0b100111111,0b001111111};
    static vector<int> shrink_9_vec(shrink_9, shrink_9 + sizeof(shrink_9) / sizeof(shrink_9[0]));
    shrink_LUT[9] = shrink_9_vec;

    static int shrink_10[] = {0b111011111,0b111111101,0b111110111,0b101111111};
    static vector<int> shrink_10_vec(shrink_10, shrink_10 + sizeof(shrink_10) / sizeof(shrink_10[0]));
    shrink_LUT[10] = shrink_10_vec;

    // Thinning LUT:
    static int thinning_4[] = {0b010011000,0b010110000,0b000110010,0b000011010,0b001011001,0b111010000,0b100110100,0b000010111};
    static vector<int> thinning_4_vec(thinning_4, thinning_4 + sizeof(thinning_4) / sizeof(thinning_4[0]));
    thinning_LUT[4] = thinning_4_vec;

    thinning_LUT[5] = shrink_LUT[5];
    thinning_LUT[6] = shrink_LUT[6];
    thinning_LUT[7] = shrink_LUT[7];
    thinning_LUT[8] = shrink_LUT[8];
    thinning_LUT[9] = shrink_LUT[9];
    thinning_LUT[10] = shrink_LUT[10];

    // Skeletonizing LUT:

    skeletonizing_LUT[4] = thinning_LUT[4];

    static int skeletonizing_6[] = {0b111011000,0b011011001,0b111110000,0b110110100,0b100110110,0b000110111,0b000011111,0b001011011};
    static vector<int> skeletonizing_6_vec(skeletonizing_6, skeletonizing_6 + sizeof(skeletonizing_6) / sizeof(skeletonizing_6[0]));
    skeletonizing_LUT[6] = skeletonizing_6_vec;

    skeletonizing_LUT[7] = shrink_LUT[7];
    skeletonizing_LUT[8] = shrink_LUT[8];
    skeletonizing_LUT[9] = shrink_LUT[9];
    skeletonizing_LUT[10] = shrink_LUT[10];

    static int skeletonizing_11[] = {0b111111011,0b111111110,0b110111111,0b011111111};
    static vector<int> skeletonizing_11_vec(skeletonizing_11, skeletonizing_11 + sizeof(skeletonizing_11) / sizeof(skeletonizing_11[0]));
    skeletonizing_LUT[11] = skeletonizing_11_vec;

    // Test for shrinking
    if(operation == "Shrinking"){

        if( shrink_LUT.find(boundValue) == shrink_LUT.end()){
            hitOrMiss = 0;
            return hitOrMiss;
        }

        if( find(shrink_LUT[boundValue].begin(),shrink_LUT[boundValue].end(),obtainedPattern) != shrink_LUT[boundValue].end()){

            hitOrMiss = 1;
            return hitOrMiss;
        }

    }

    // test for thinning
    if(operation == "Thinning"){

        if( thinning_LUT.find(boundValue) == thinning_LUT.end()){
            hitOrMiss = 0;
            return hitOrMiss;
        }

        if( find(thinning_LUT[boundValue].begin(),thinning_LUT[boundValue].end(),obtainedPattern) != thinning_LUT[boundValue].end()){

            hitOrMiss = 1;
            return hitOrMiss;
        }

    }

    // test for skeletonizing
    if(operation == "Skeletonizing"){

        if( skeletonizing_LUT.find(boundValue) == skeletonizing_LUT.end()){
            hitOrMiss = 0;
            return hitOrMiss;
        }

        if( find(skeletonizing_LUT[boundValue].begin(),skeletonizing_LUT[boundValue].end(),obtainedPattern) != skeletonizing_LUT[boundValue].end()){

            hitOrMiss = 1;
            return hitOrMiss;
        }

    }

    return hitOrMiss;

}

//----------------------------------------------------------------------------------------------------------------//
// unconditional pattern matching function
// Private method to determine if conditional pattern is true

bool imageAlgorithms::unconditionalPatternMatching(string operation,int obtainedPattern){

    // Initialize LUT for unconditional pattern matching
    static int unconditionalPatternsWithoutD[]={0b001010000,0b100010000, // Spur
                                0b000010010,0b000011000, // Single 4-connection
                                0b001011000,0b011010000,0b110010000,0b100110000, //LCluster
                                0b000110100,0b000010110,0b000010011,0b000011001,
                                0b011110000,0b110011000,0b010011001,0b001011010, // 4-connected offset
                                0b011011100,0b001011100,0b011010100, // Spur corner Cluster
                                0b110110001,0b100110001,0b110010001,
                                0b001110110,0b001110100,0b001010110,
                                0b100011011,0b100011001,0b100010011
    };

    static int unconditionalPatternsWithD[]={0b110110000,// Corner Cluster
                             0b010111000,// TeeBranch
                             0b010111000,
                             0b000111010,
                             0b000111010,
                             0b010110010,
                             0b010110010,
                             0b010011010,
                             0b010011010,
                             0b101010001,0b101010010,0b101010011,0b101010100,0b101010101,0b101010110,0b101010111,// VeeBranch
                             0b100010101,0b100011100,0b100011101,0b101010100,0b101010101,0b101011100,0b101011101,
                             0b001010101,0b010010101,0b011010101,0b100010101,0b101010101,0b110010101,0b111010101,
                             0b001010101,0b001110001,0b001110101,0b101010001,0b101010101,0b101110001,0b101110101,
                             0b010011100,// DiagonalBranch
                             0b010110001,
                             0b001110010,
                             0b100011010
    };


    static int mask_shrink_thinning[]={0b110110000,// Corner Cluster
                             0b011111011,// TeeBranch
                             0b110111110,
                             0b110111110,
                             0b011111011,
                             0b010111111,
                             0b111111010,
                             0b111111010,
                             0b010111111,
                             0b101010111,0b101010111,0b101010111,0b101010111,0b101010111,0b101010111,0b101010111,// VeeBranch
                             0b101011111,0b101011111,0b101011111,0b101011111,0b101011111,0b101011111,0b101011111,
                             0b111010101,0b111010101,0b111010101,0b111010101,0b111010101,0b111010101,0b111010101,
                             0b101110101,0b101110101,0b101110101,0b101110101,0b101110101,0b101110101,0b101110101,
                             0b011111110,// DiagonalBranch
                             0b110111011,
                             0b011111110,
                             0b110111011
    };

    static int unconditionalPatterns_skel_withoutD[]={0b000010001,0b000010100,0b001010000,0b100010000, // Spur
                               0b000010010,0b000011000,0b000110000,0b010010000, // Single 4-connection
                               0b010011000,0b010110000,0b000011010,0b000110010// LCorner
    };

    static int unconditionalPatterns_skel_withD[]={
            0b110110000,// Corner Cluster
            0b000011011,
            0b010111000,// Tee Branch
            0b010110010,
            0b000111010,
            0b010011010,
            0b101010001,0b101010010,0b101010011,0b101010100,0b101010101,0b101010110,0b101010111,// Vee Branch
            0b100010101,0b100011100,0b100011101,0b101010100,0b101010101,0b101011100,0b101011101,
            0b001010101,0b010010101,0b011010101,0b100010101,0b101010101,0b110010101,0b111010101,
            0b001010101,0b001110001,0b001110101,0b101010001,0b101010101,0b101110001,0b101110101,
            0b010011100,// Diagonal Branch
            0b010110001,
            0b001110010,
            0b100011010,
    };

    static int unconditionalMask_skel[]={
                            0b110110000,// Corner Cluster
                            0b000011011,
                            0b010111000,// Tee Branch
                            0b010110010,
                            0b000111010,
                            0b010011010,
                            0b101010111,0b101010111,0b101010111,0b101010111,0b101010111,0b101010111,0b101010111,// Vee Branch
                            0b101011111,0b101011111,0b101011111,0b101011111,0b101011111,0b101011111,0b101011111,
                            0b111010101,0b111010101,0b111010101,0b111010101,0b111010101,0b111010101,0b111010101,
                            0b101110101,0b101110101,0b101110101,0b101110101,0b101110101,0b101110101,0b101110101,
                            0b011111110,// Diagonal Branch
                            0b110111011,
                            0b011111110,
                            0b110111011
    };


    // For thinning and shrinking
    static vector<int> unconditionalPatternsWithoutD_vec(unconditionalPatternsWithoutD, unconditionalPatternsWithoutD + sizeof(unconditionalPatternsWithoutD) / sizeof(unconditionalPatternsWithoutD[0]));
    static vector<int> unconditionalPatternsWithD_vec(unconditionalPatternsWithD, unconditionalPatternsWithD + sizeof(unconditionalPatternsWithD) / sizeof(unconditionalPatternsWithD[0]));
    static vector<int> mask_shrink_thinning_vec(mask_shrink_thinning, mask_shrink_thinning + sizeof(mask_shrink_thinning) / sizeof(mask_shrink_thinning[0]));
    static map<int,int> hitPatternsMap = hitMapDeveloper(unconditionalPatternsWithD_vec,unconditionalPatternsWithoutD_vec);
    static map<int,vector<int> > maskMap = maskMapDeveloper(mask_shrink_thinning_vec,unconditionalPatternsWithD_vec);


    // For skeletonizing:
    static vector<int> unconditionalPatternsWithoutD_skeletonize_vec(unconditionalPatterns_skel_withoutD, unconditionalPatterns_skel_withoutD + sizeof(unconditionalPatterns_skel_withoutD) / sizeof(unconditionalPatterns_skel_withoutD[0]));
    static vector<int> unconditionalPatternsWithD_skeletonize_vec(unconditionalPatterns_skel_withD, unconditionalPatterns_skel_withD + sizeof(unconditionalPatterns_skel_withD) / sizeof(unconditionalPatterns_skel_withD[0]));
    static vector<int> mask_skeletonize_vec(unconditionalMask_skel, unconditionalMask_skel + sizeof(unconditionalMask_skel) / sizeof(unconditionalMask_skel[0]));
    static map<int,int> hitPatternsMap_skeletonize = hitMapDeveloper(unconditionalPatternsWithoutD_skeletonize_vec,unconditionalPatternsWithD_skeletonize_vec);
    static map<int,vector<int> > maskMap_skeletonize = maskMapDeveloper(mask_skeletonize_vec,unconditionalPatternsWithD_skeletonize_vec);


    // Check for patterns
    bool hitOrMiss = 0;
    int maskedPattern = 0;

    // Check for shrinking or thinning

    if((operation=="Shrinking")||(operation=="Thinning")){

        if(hitPatternsMap[obtainedPattern]==1){
            hitOrMiss = 1;
        }else{

            // Else try different masks!
            for(int index = 0; index< maskMap.size(); index++){

                maskedPattern = maskMap[index][0]&obtainedPattern;
                // Check if masked pattern is available in dictionary
                if(maskedPattern == maskMap[index][1]){
                    hitOrMiss = 1;
                    break;
                }

            }
        }

    }

    // Check for skeletonizing

    if(operation=="Skeletonizing"){

        if(hitPatternsMap_skeletonize[obtainedPattern]==1){
            hitOrMiss = 1;
        }else{

            // Else try different masks!
            for(int index = 0; index< maskMap_skeletonize.size(); index++){

                maskedPattern = maskMap_skeletonize[index][0]&obtainedPattern;
                // Check if masked pattern is available in dictionary
                if(maskedPattern == maskMap_skeletonize[index][1]){
                    hitOrMiss = 1;
                    break;
                }

            }
        }

    }

    return hitOrMiss;

}

//----------------------------------------------------------------------------------------------------------------//
// Private method to build hit pattern map
map<int,int> imageAlgorithms::hitMapDeveloper(vector<int> patternValues1,vector<int> patternValues2){

    map<int,int> hitMap;

    // Fill it in with hit patterns
    for(int index = 0; index<patternValues1.size();index++){
        hitMap[patternValues1[index]] = 1;
    }

    for(int index = 0; index<patternValues2.size();index++){
        hitMap[patternValues2[index]] = 1;
    }

    return hitMap;
}

//----------------------------------------------------------------------------------------------------------------//
// Private method to build mask map
map<int,vector<int> > imageAlgorithms::maskMapDeveloper(vector<int> maskValues, vector<int> patternValues1){

    map<int,vector<int> > maskMap;

    // Fill it in with hit patterns
    for(int index = 0; index<maskValues.size();index++){
        maskMap[index].push_back(maskValues[index]);
        maskMap[index].push_back(patternValues1[index]);
    }

    return maskMap;

}

//----------------------------------------------------------------------------------------------------------------//
// Private method to determine the bound of a pattern

int imageAlgorithms::bound(int obtainedPattern){

    int boundValue = 0;
    bool bitValue;

    // Loop through the bits of obtainedPattern
    for(int bit = 0; bit<9; bit++){

        bitValue = obtainedPattern & 0x100;

        if(bit%2==1){
            boundValue += bitValue*2;
        }else if(bit==4){
            boundValue += bitValue*0;
        }else{
            boundValue += bitValue;
        }

        obtainedPattern = obtainedPattern <<1;

    }

    return boundValue;
}

//----------------------------------------------------------------------------------------------------------------//
// Hole filling algorithm:

matrix<int,bool> imageAlgorithms::holeFilling(){

    // Local variables
    int imageWidth = imageObject->getImageWidth();
    int imageHeight = imageObject->getImageHeight();
    matrix<int,bool> inputMatrix = charToBoolean();
    matrix<int,bool> seedMatrix(imageHeight,imageWidth,1);
    seedMatrix.setMatrixValues(1,0,0,0);

    // Dilate the background
    matrix<int,bool> dilatedMatrix = dilation_holeFilling(seedMatrix,inputMatrix);

    // Find the holes in the image
    matrix<int,bool> holesMatrix = !(inputMatrix|dilatedMatrix);
    matrix<int,bool> outputMatrix = inputMatrix|holesMatrix;

    return outputMatrix;

}

//----------------------------------------------------------------------------------------------------------------//
// Dilation for hole filling:

matrix<int,bool> imageAlgorithms::dilation_holeFilling(matrix<int,bool> seedMatrix, matrix<int,bool> inputMatrix){

    // Define the structuring matrix:
    int imageWidth = imageObject->getImageWidth();
    int imageHeight = imageObject->getImageHeight();
    matrix<int,bool> structuringMatrix(3,3,1);
    structuringMatrix.setMatrixByValues(9,0,1,0,1,1,1,0,1,0);
    matrix<int,bool> outputMatrix(imageHeight,imageWidth,1);
    int firstIteration = 1;
    bool outputValue = 0;

    while(outputMatrix!=seedMatrix){

        if(!firstIteration){
            seedMatrix = outputMatrix;
        }
        firstIteration = 0;

        // Iter through all the elements in seed Image
        for(int rowIndex = 0; rowIndex < imageHeight; rowIndex++){
            for(int columnIndex = 0; columnIndex < imageWidth; columnIndex++){


                if(!inputMatrix.getMatrixValues(rowIndex,columnIndex,0)){

                    // Iterate through all the elements in the window:
                    outputValue = 0;
                    for(int windowRowIndex = -1; windowRowIndex < 2; windowRowIndex++) {
                        for (int windowColumnIndex = -1; windowColumnIndex < 2; windowColumnIndex++) {

                            // Check for out of bound indices
                            if((windowRowIndex+rowIndex<0)||(windowColumnIndex+columnIndex<0)||(windowColumnIndex+columnIndex>=imageWidth)||(rowIndex+windowRowIndex>=imageHeight)){
                                continue;
                            }

                            //Calculate output value
                            outputValue |= (structuringMatrix.getMatrixValues(windowRowIndex+1,windowColumnIndex+1,0) & seedMatrix.getMatrixValues(windowRowIndex+rowIndex,windowColumnIndex+columnIndex,0));

                        }
                    }

                    outputMatrix.setMatrixValues(outputValue,rowIndex,columnIndex,0);

                }
            }
        }

    }

    return outputMatrix;

}

//----------------------------------------------------------------------------------------------------------------//
// Dilation:

matrix<int,bool> imageAlgorithms::dilation(matrix<int,bool> inputMatrix, int numberOfIter){

    // Local variables
    int matHeight = inputMatrix.getHeight();
    int matWidth = inputMatrix.getWidth();
    matrix<int,bool> outputMatrix(matHeight,matWidth,1);
    matrix<int,bool> dilationKernel(3,3,1);
    dilationKernel.setMatrixByValues(9,0,1,0,1,1,1,0,1,0);
    int outputValue = 0;
    int firstIteration = 1;


    // Apply kernel for number of iter times
    for(int iter = 0; iter< numberOfIter; iter++){

        if(!firstIteration){
            inputMatrix = outputMatrix;
        }
        firstIteration = 0;

        for(int rowIndex = 0; rowIndex < matHeight; rowIndex++){
            for(int columnIndex = 0; columnIndex < matWidth; columnIndex++){

                // Iterate through all the elements in the window:
                outputValue = 0;
                for(int windowRowIndex = -1; windowRowIndex < 2; windowRowIndex++) {
                    for (int windowColumnIndex = -1; windowColumnIndex < 2; windowColumnIndex++) {

                        // Check for out of bound indices
                        if((windowRowIndex+rowIndex<0)||(windowColumnIndex+columnIndex<0)||(windowColumnIndex+columnIndex>=matWidth)||(rowIndex+windowRowIndex>=matHeight)){
                            continue;
                        }

                        //Calculate output value
                        outputValue |= (dilationKernel.getMatrixValues(windowRowIndex+1,windowColumnIndex+1,0) & inputMatrix.getMatrixValues(windowRowIndex+rowIndex,windowColumnIndex+columnIndex,0));
                    }
                }

                outputMatrix.setMatrixValues(outputValue,rowIndex,columnIndex,0);

            }
        }

    }

    return outputMatrix;

}


//----------------------------------------------------------------------------------------------------------------//
// Erosion:

matrix<int,bool> imageAlgorithms::erosion(matrix<int,bool> inputMatrix, int numberOfIter){

    // Local variables
    int matHeight = inputMatrix.getHeight();
    int matWidth = inputMatrix.getWidth();
    matrix<int,bool> outputMatrix(matHeight,matWidth,1);
    matrix<int,bool> erosionKernel(3,3,1);
    erosionKernel.setMatrixByValues(9,1,1,1,1,1,1,1,1,1);
    int outputValue = 1;
    int firstIteration = 1;


    // Apply kernel for number of iter times
    for(int iter = 0; iter< numberOfIter; iter++){

        if(!firstIteration){
            inputMatrix = outputMatrix;
        }
        firstIteration = 0;

        for(int rowIndex = 0; rowIndex < matHeight; rowIndex++){
            for(int columnIndex = 0; columnIndex < matWidth; columnIndex++){

                // Iterate through all the elements in the window:
                outputValue = 1;
                for(int windowRowIndex = -1; windowRowIndex < 2; windowRowIndex++) {
                    for (int windowColumnIndex = -1; windowColumnIndex < 2; windowColumnIndex++) {

                        // Check for out of bound indices
                        if((windowRowIndex+rowIndex<0)||(windowColumnIndex+columnIndex<0)||(windowColumnIndex+columnIndex>=matWidth)||(rowIndex+windowRowIndex>=matHeight)){
                            continue;
                        }

                        //Calculate output value
                        outputValue &= (erosionKernel.getMatrixValues(windowRowIndex+1,windowColumnIndex+1,0) & inputMatrix.getMatrixValues(windowRowIndex+rowIndex,windowColumnIndex+columnIndex,0));
                    }
                }

                outputMatrix.setMatrixValues(outputValue,rowIndex,columnIndex,0);

            }
        }

    }

    return outputMatrix;

}
//----------------------------------------------------------------------------------------------------------------//
// Connected Component Labelling:
//
//matrix<int,unsigned char> imageAlgorithms::connectedComponentLabelling(matrix<int,bool> inputMatrix){
//
//    // Local variables
//    int matHeight = inputMatrix.getHeight();
//    int matWidth = inputMatrix.getWidth();
//    matrix<int,double> intermediateMatrix(matHeight,matWidth,1);
//    matrix<int,double> outputMatrix(matHeight,matWidth,1);
//    matrix<int,unsigned char> returnMatrix(matHeight,matWidth,1);
//    double pixelValue = 0;
//    map<double,vector<double> > labelTable;
//    double label = 1000000000000;
//    double labelIndex = 0;
//    vector<double> labelVector;
//    double windowValue = 0;
//    vector<double> concatenateVector;
//
//    // First pass
//    for(int rowIndex = 1; rowIndex < matHeight-1; rowIndex++){
//        for(int columnIndex = 1; columnIndex < matWidth-1; columnIndex++){
//
//            pixelValue = (double)inputMatrix.getMatrixValues(rowIndex,columnIndex,0);
//
//            // Check if pixel value is zero
//            if(pixelValue==0){
//                continue;
//            }
//
//            // Get the neighbouring pixels:
//            label = 1000000000000;
//
//            for(int windowRowIndex = -1; windowRowIndex < 2; windowRowIndex++) {
//                for (int windowColumnIndex = -1; windowColumnIndex < 2; windowColumnIndex++) {
//
//                    if(((windowRowIndex==0)&&(windowColumnIndex==0))||((windowRowIndex==1))){
//                        continue;
//                    }
//
//                    windowValue = (double)intermediateMatrix.getMatrixValues(rowIndex+windowRowIndex,columnIndex+windowColumnIndex,0);
//
//                    if(windowValue>0){
//                        label = min(label,windowValue);
//                        labelVector.push_back(windowValue);
//                    }
//
//                }
//            }
//
//            // Create new label if no label found
//            if(label ==1000000000000){
//                labelIndex++;
//                labelTable[labelIndex].push_back(labelIndex);
//                intermediateMatrix.setMatrixValues(labelIndex,rowIndex,columnIndex,0);
//            }else{
//                // Update the label table and set label value
//                intermediateMatrix.setMatrixValues(label,rowIndex,columnIndex,0);
//
//                // Copy all labels to the index
//                for(int labelVectorIndex = 0; labelVectorIndex < labelVector.size(); labelVectorIndex++){
//                    concatenateVector.insert(concatenateVector.end(),labelTable[labelVector[labelVectorIndex]].begin(),labelTable[labelVector[labelVectorIndex]].end());
//                }
//
//                for(int labelVectorIndex = 0; labelVectorIndex < labelVector.size(); labelVectorIndex++){
//
//                    if(label==76 && labelVector.size() == 2){
//                        cout<< *min_element(labelTable[78].begin(),labelTable[78].end()) << endl;
//                    }
//
//                   labelTable[labelVector[labelVectorIndex]] = concatenateVector;
//                }
//
//                concatenateVector.clear();
//                labelVector.clear();
//            }
//
//
//        }
//    }
//
//    // Second pass
//    map<int,unsigned char> labelTableMap;
//    double lowestLabel;
//    double componentNumber = 0;
//
//    for(int rowIndex = 1; rowIndex < matHeight-1; rowIndex++){
//        for(int columnIndex = 1; columnIndex < matWidth-1; columnIndex++) {
//
//            pixelValue = (double)intermediateMatrix.getMatrixValues(rowIndex,columnIndex,0);
//
//            // Check if pixel value is zero
//            if(pixelValue==0){
//                continue;
//            }
//
//            lowestLabel = *min_element(labelTable[pixelValue].begin(), labelTable[pixelValue].end() );
//
//            if(labelTableMap.find(lowestLabel) == labelTableMap.end()){
//                labelTableMap[lowestLabel] = ++componentNumber;
//                cout<< componentNumber <<endl;
//            }
//            outputMatrix.setMatrixValues(labelTableMap[lowestLabel],rowIndex,columnIndex,0);
//        }
//    }
//
//    // Convert double to unsigned char
//    for(int rowIndex = 0; rowIndex < matHeight; rowIndex++) {
//        for (int columnIndex = 0; columnIndex < matWidth; columnIndex++) {
//            returnMatrix.setMatrixValues((unsigned char)outputMatrix.getMatrixValues(rowIndex,columnIndex,0), rowIndex,columnIndex,0);
//        }
//    }
//
//    return returnMatrix;
//
//}

//----------------------------------------------------------------------------------------------------------------//
// Create Laws filter:

map<int, matrix<int,double> > imageAlgorithms::getLawsFilter(){

    // Initialize variables
    static map<int, matrix<int,double>* > lawsFilter;
    static map<int, matrix<int,double> > filterBank;
    static matrix<int,double> level(5,1,1);
    static matrix<int,double> edge(5,1,1);
    static matrix<int,double> spot(5,1,1);
    static matrix<int,double> wave(5,1,1);
    static matrix<int,double> ripple(5,1,1);

    // Initialze filter values
    level.setMatrixByValues(5,1.0,4.0,6.0,4.0,1.0);
    edge.setMatrixByValues(5,-1.0,-2.0,0.0,2.0,1.0);
    spot.setMatrixByValues(5,-1.0,0.0,2.0,0.0,-1.0);
    wave.setMatrixByValues(5,-1.0,2.0,0.0,-2.0,1.0);
    ripple.setMatrixByValues(5,1.0,-4.0,6.0,-4.0,1.0);

    // Set filter values in map
    lawsFilter[0] = &level;
    lawsFilter[1] = &edge;
    lawsFilter[2] = &spot;
    lawsFilter[3] = &wave;
    lawsFilter[4] = &ripple;

    // Create filter bank:

    int filterBankIndex = 0;
    for(int filterIndex_1 = 0; filterIndex_1 < 5; filterIndex_1++){
        for(int filterIndex_2 = 0; filterIndex_2 < 5; filterIndex_2++){
            // Multiply
            matrix<int,double> tempMatrix = *(new matrix<int,double>(5,5,1));
            tempMatrix = lawsFilter[filterIndex_1]->multiplyWith(lawsFilter[filterIndex_2]->transpose());
            filterBank[filterBankIndex++] = tempMatrix;
        }
    }

    return filterBank;
}
//----------------------------------------------------------------------------------------------------------------//
// Caluculate energy value of an image:
double imageAlgorithms::imageEnergy(matrix<int,double> inputMatrix){

    // Input matrix paramters
    double energyValue;
    int matrixHeight = inputMatrix.getHeight();
    int matrixWidth = inputMatrix.getWidth();
    int matrixDepth = inputMatrix.getDepth();
    vector<double>  matrixValues = inputMatrix.getMatrixValues();
    for_each(matrixValues.begin(),matrixValues.end(), [&](double x){ energyValue += pow(x,2.0); } );

    // Calculate energy
//    for(int depthIndex = 0; depthIndex < matrixDepth; depthIndex++){
//        for(int rowIndex = 0; rowIndex < matrixHeight; rowIndex++){
//            for(int columnIndex = 0; columnIndex < matrixWidth; columnIndex++){
//                energyValue += pow((inputMatrix.accessMatrixValue(rowIndex,columnIndex,depthIndex)- DC_Component),2.0);
//            }
//        }
//    }


    // Average the energy values
    energyValue = energyValue/(matrixDepth*matrixHeight*matrixWidth);

    return energyValue;
}

//----------------------------------------------------------------------------------------------------------------//
// Caluculate DC value of an image:
double imageAlgorithms::calculateDC(imageData inputImage){

    // Input matrix paramters
    double energyValue;
    int imageHeight = inputImage.getImageHeight();
    int imageWidth = inputImage.getImageWidth();
    int imageDepth = inputImage.getBytesPerPixel();
    vector<unsigned char>  pixelValues = inputImage.getPixelValues();
    for_each(pixelValues.begin(),pixelValues.end(), [&](unsigned char x){ energyValue += x; } );

    // Average the energy values
    energyValue = energyValue/(imageDepth*imageHeight*imageWidth);

    return energyValue;
}

//----------------------------------------------------------------------------------------------------------------//
// Subtract DC value of an image:
matrix<int,double> imageAlgorithms::subtractDC(imageData inputImage){

    // Input matrix paramters
    double DCValue = calculateDC(inputImage);
    int imageHeight = inputImage.getImageHeight();
    int imageWidth = inputImage.getImageWidth();
    int imageDepth = inputImage.getBytesPerPixel();
    vector<unsigned char>  pixelValues = inputImage.getPixelValues();

    // Set output variables
    matrix<int,double> outputMatrix(imageHeight,imageWidth,imageDepth);
    vector<double>  outputPixelValues;

    // Subtract DC Values
    for_each(pixelValues.begin(),pixelValues.end(),[&](unsigned char x){ outputPixelValues.push_back((double)x-DCValue);});

    // Fill up matrix
    outputMatrix.setMatrixValues(outputPixelValues);

    return outputMatrix;
}




