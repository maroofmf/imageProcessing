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
    vector<double> getToCartesianVector();
    vector<double> getToImageVector();
    vector<unsigned char> getPixelValues();
    unsigned char getPixelValues(int row, int column, int depth);
    unsigned char getPixelValues(int index);

    void setToCartesianVector(vector<double> newToCartesianVector);
    void setToImageVector(vector<double> newToImageVector);
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
    imageData colorToGrayscale();

    unsigned char diamondWarping(double x, double y, unsigned char defaultBackgroundValue = 0);
    unsigned char puzzleMatching1(double x, double y, imageData* piecesImage,unsigned char defaultBackgroundValue = 0);
    unsigned char puzzleMatching2(double x, double y, imageData* piecesImage,unsigned char defaultBackgroundValue = 0);
    unsigned char textEmbedding1(double x, double y, imageData* textImage,unsigned char defaultBackgroundValue = 0);

    void translation(double delta_x ,double delta_y);
    void rotation(double theta);
    void scaling(double xScaleFactor, double yScaleFactor);

    vector<double> imageToCartesian(double pixelRow, double pixelColumn);
    vector<double> cartesianToImage(double x, double y);
    unsigned char getPixelValuesFrom_xy(double x, double y,unsigned char defaultBackgroundValue=0);

    unsigned char accessPixelValue(int row, int column, int depth);
    void saveImage(const char* outputFileName);
    void imageRead(const char* inputFileName);

private:
    int BytesPerPixel;
    int imageWidth;
    int imageHeight;
    vector<unsigned char>  pixelData;
    vector<double> toCartesianVector;
    vector<double> toImageVector;
};

#endif //CPP_IMAGEDATA_H
