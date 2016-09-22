//
// Created by Maroof Mohammed Farooq on 9/11/16.
//

#ifndef CPP_IMAGEALGORITHMS_H
#define CPP_IMAGEALGORITHMS_H

using namespace std;

class imageAlgorithms{
public:
    imageAlgorithms(imageData* imageObject1);
    ~imageAlgorithms(void);
    void histEqualization_tf();
    void histEqualization_cdf();
    void equalizationBasedOnReference(imageData referenceObject);
    void histEqualization_gaussian(double mean, double std);
    void psnr(imageData originalImage);
    void meanFilter(int windowSize);
    void medianFilter(int windowSize);
    void NLMFilter(int windowSize, int searchWindowSize, double decayFactor);


private:
    imageData* imageObject;

};

#endif //CPP_IMAGEALGORITHMS_H
