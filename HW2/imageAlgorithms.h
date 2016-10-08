//
// Created by Maroof Mohammed Farooq on 9/11/16.
//

#ifndef CPP_IMAGEALGORITHMS_H
#define CPP_IMAGEALGORITHMS_H

#include "matrix.h"
#include <map>

using namespace std;

class imageAlgorithms{
public:
    imageAlgorithms(imageData* imageObject1);
    ~imageAlgorithms(void);

    unsigned char bilinearInterpolation(double rowIndex, double columnIndex,double depthIndex=0.0);
    void histEqualization_tf();
    void histEqualization_cdf();
    void equalizationBasedOnReference(imageData referenceObject);
    void histEqualization_gaussian(double mean, double std);
    void psnr(imageData originalImage);
    void meanFilter(int windowSize);
    void medianFilter(int windowSize);
    void NLMFilter(int windowSize, int searchWindowSize, double decayFactor);
    void dithering(int indexMatrixSize,bool useGivenMatrix);
    void fourLevelDithering();
    void errorDiffusion(string algorithm);
    void binarize(double threshold, bool invert = false);
    void binarizeBasedOnMode(double radius,bool invert = false);
    void adaptiveBinarization(int windowSize);
    matrix<int,bool> charToBoolean();
    matrix<int,unsigned char> booleanToChar(matrix<int,bool> inputMatrix);

    matrix<int,double>  filterApply(imageData frame, matrix<int,double>  window,string algorithm);
    map<int,vector<int> > harrisCornerDetector(double RThreshold);

    matrix<int,bool> shrinking(matrix<int,bool> binaryImageData);

private:
    imageData* imageObject;
    matrix<int,int> bayerMat(int size);

    bool conditionalPatternMatching(string operation,int obtainedPattern);
    bool unconditionalPatternMatching(string operation,int obtainedPattern);
    static map<int,int > hitMapDeveloper(vector<int> patternValues1,vector<int> patternValues2);
    static map<int,int > maskMapDeveloper(vector<int> maskValues);
    int bound(int obtainedPattern);

};

#endif //CPP_IMAGEALGORITHMS_H
