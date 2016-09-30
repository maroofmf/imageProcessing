//
// Created by Maroof Mohammed Farooq on 9/4/16.
//

#ifndef CPP_IMAGEDATA_H
#define CPP_IMAGEDATA_H

using namespace std;

class imageData{
public:
    imageData(int BytesPerPixel1, int imageWidth1, int imageHeight1);
    imageData();
    ~imageData(void);
    int getBytesPerPixel();
    int getImageWidth();
    int getImageHeight();
    vector<unsigned char> getPixelValues();
    unsigned char getPixelValues(int row, int column, int depth);
    unsigned char getPixelValues(int index);
    void setPixelValues(vector<unsigned char> newPixelData);
    void setPixelValues(unsigned char newPixelData,int row,int column,int depth);
    void setPixelValues(unsigned char newPixelData,int index);
    imageData resizeImage(int newWidth, int newHeight);
    void cropImage(imageData orignalImage,int cropRow,int cropColumn,int cropWidth,int cropHeight);
    imageData extendImage(int extendBy);
    imageData rgb2cmy(bool replaceColorSpaceFlag);
    imageData rgb2hsl(bool replaceColorSpaceFlag);
    imageData hsl2rgb(bool replaceColorSpaceFlag);
    vector<imageData> seperateChannels();
    void concatenateChannels(vector<imageData> colorChannels);

    vector<double> imageToCartesian(double pixelRow, double pixelColumn);
    vector<double> cartesianToImage(double x, double y);
    unsigned char getPixelValuesFrom_xy(double x, double y);

    unsigned char accessPixelValue(int row, int column, int depth);
    void saveImage(const char* outputFileName);
    void imageRead(const char* inputFileName);

private:
    int BytesPerPixel;
    int imageWidth;
    int imageHeight;
    vector<unsigned char>  pixelData;
    vector<double> pixelCoordinateConversion;
};

#endif //CPP_IMAGEDATA_H
