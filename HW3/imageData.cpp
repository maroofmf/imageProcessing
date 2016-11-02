//
// Created by Maroof Mohammed Farooq on 9/4/16.
//

// Header files
#include "headers.h"
#include "imageData.h"
#include "imageAlgorithms.h"
#include "matrix.h"

// Namespace
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------//
// Image constructor I: Used to initialize a black image
imageData::imageData(int BytesPerPixel1, int imageWidth1, int imageHeight1){
    BytesPerPixel = BytesPerPixel1;
    imageWidth = imageWidth1;
    imageHeight = imageHeight1;
    pixelData.resize(imageHeight*imageWidth*BytesPerPixel,0);

    // Initialize
    const double dataArray[9] = {0.0, 1.0, 0.5, -1, 0.0, imageHeight - 0.5, 0.0, 0.0, 1.0};
    copy(&dataArray[0], &dataArray[9], back_inserter(toCartesianVector));

    // Initialize to image vector
    matrix<int,double> toImageMatrix(3,3,1);
    toImageMatrix.setMatrixValues(toCartesianVector);
    toImageMatrix = toImageMatrix.pseudoInverse();
    toImageVector = toImageMatrix.getMatrixValues();
}
//----------------------------------------------------------------------------------------------------------------//
// Image constructor II: Default initializer
imageData::imageData(){
}

//----------------------------------------------------------------------------------------------------------------//
// Destructor
imageData::~imageData(void){
}
//----------------------------------------------------------------------------------------------------------------//
// Image Bytes per pixel getter:
int imageData::getBytesPerPixel(){
    return(BytesPerPixel);
}
//----------------------------------------------------------------------------------------------------------------//
// Image Width getter:
int imageData::getImageWidth(){
    return(imageWidth);
}
//----------------------------------------------------------------------------------------------------------------//
// Image Height getter:
int imageData::getImageHeight(){
    return(imageHeight);
}
//----------------------------------------------------------------------------------------------------------------//
// Cartesian vector converter getter:
vector<double> imageData::getToCartesianVector(){
    return toCartesianVector;
}

//----------------------------------------------------------------------------------------------------------------//
// Image vector converter getter:
vector<double> imageData::getToImageVector(){
    return toImageVector;
}
//----------------------------------------------------------------------------------------------------------------//
// I. All pixel values getter:
vector<unsigned char> imageData::getPixelValues(){
    return(pixelData);
}
//----------------------------------------------------------------------------------------------------------------//
// II. Pixel values getter by row column and depth:
unsigned char imageData::getPixelValues(int row, int column, int depth){
    return(accessPixelValue(row,column,depth));
}
//----------------------------------------------------------------------------------------------------------------//
// III. Pixel values getter by index value:
unsigned char imageData::getPixelValues(int index){
    return(pixelData[index]);
}
//----------------------------------------------------------------------------------------------------------------//
// III. Pixel values getter by index value:
void imageData::setToCartesianVector(vector<double> newToCartesianVector){
    toCartesianVector = newToCartesianVector;
}

//----------------------------------------------------------------------------------------------------------------//
// III. Pixel values getter by index value:
void imageData::setToImageVector(vector<double> newToImageVector){
    toImageVector = newToImageVector;
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

    // initilize variable to temporarily store retrieved pixel values
    unsigned char pixelValue;

    // Fill the pixels into the resized image
    // Check if iamge is being shrunk or enlarged
    if(newWidth<imageWidth){
        // Caluculate the resizing ratio
        double ratio = (double)newWidth/imageWidth;
        for(int depthIndex=0; depthIndex< BytesPerPixel;depthIndex++ ){
            for(double rowIndex=0; rowIndex<newHeight;rowIndex++) {
                for (double columnIndex = 0; columnIndex < newWidth; columnIndex++) {
                    // Copy values
                    pixelValue = accessPixelValue((int)ceil(rowIndex/ratio),(int)columnIndex/ratio,depthIndex);
                    resizedImage.setPixelValues(pixelValue,rowIndex,columnIndex,depthIndex);
                }
            }
        }
    }else if(newWidth>imageWidth){
        // Initialize variables
        double fractionRight, fractionLeft, fractionTop, fractionBottom, rowFraction, columnFraction;
        double originalRowIndex, originalColumnIndex;
        unsigned char topRightPixelValue,bottomRightPixelValue,topLeftPixelValue,bottomLeftPixelValue;
        // Caluculate the resizing ratio
        double ratio = (double)(imageWidth-1)/(newWidth-1);
        int extendBy = 1;
        imageData extendedImage = extendImage(extendBy);
        for(int depthIndex=0; depthIndex< BytesPerPixel;depthIndex++ ){
            for(double rowIndex=0; rowIndex<newHeight;rowIndex++) {
                for (double columnIndex = 0; columnIndex < newWidth; columnIndex++) {
                    rowFraction = (double)rowIndex * ratio;
                    columnFraction = (double)columnIndex * ratio;
                    fractionBottom = modf(rowFraction,&originalRowIndex);
                    fractionTop = 1-fractionBottom;
                    fractionRight = modf(columnFraction, &originalColumnIndex);
                    fractionLeft = 1-fractionRight;
                    topLeftPixelValue = extendedImage.accessPixelValue(originalRowIndex+extendBy,originalColumnIndex+extendBy,depthIndex);
                    bottomLeftPixelValue = extendedImage.accessPixelValue(originalRowIndex+extendBy+1,originalColumnIndex+extendBy,depthIndex);
                    topRightPixelValue = extendedImage.accessPixelValue(originalRowIndex+extendBy,originalColumnIndex+extendBy+1,depthIndex);
                    bottomRightPixelValue = extendedImage.accessPixelValue(originalRowIndex+extendBy+1,originalColumnIndex+extendBy+1,depthIndex);

                    // Bilinear interpolation
                    pixelValue = (unsigned char)((topLeftPixelValue*fractionTop*fractionLeft+ topRightPixelValue*fractionTop*fractionRight+
                    bottomRightPixelValue*fractionBottom*fractionRight + bottomLeftPixelValue*fractionBottom*fractionLeft));

                    resizedImage.setPixelValues(pixelValue,rowIndex,columnIndex,depthIndex);


                }
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
// Extend Image
imageData imageData::extendImage(int extendBy){

    // Declare variables
    imageData extendedImage(BytesPerPixel, imageWidth+2*extendBy, imageHeight+2*extendBy);
    unsigned char pixelValue;

    // Copy the original image into the extended image
    for(int depthIndex=0; depthIndex<BytesPerPixel;depthIndex++){
        for(int rowIndex=0; rowIndex<imageHeight;rowIndex++){
            for(int columnIndex=0; columnIndex<imageWidth;columnIndex++){
                pixelValue = accessPixelValue(rowIndex,columnIndex,depthIndex);
                extendedImage.setPixelValues(pixelValue,rowIndex+extendBy,columnIndex+extendBy,depthIndex);
            }
        }
    }

    // Interpolate the pixel values of top and bottom rows
    int extendedIndex = imageHeight + extendBy;
    for(int depthIndex=0; depthIndex<BytesPerPixel;depthIndex++){
        for(int rowIndex=0; rowIndex<extendBy;rowIndex++){
            for(int columnIndex=0; columnIndex<imageWidth;columnIndex++){
                // top rows
                pixelValue = accessPixelValue(rowIndex,columnIndex,depthIndex);
                extendedImage.setPixelValues(pixelValue,extendBy-rowIndex-1,columnIndex+extendBy,depthIndex);
                // bottom rows
                pixelValue = accessPixelValue(imageHeight-rowIndex-1,columnIndex,depthIndex);
                extendedImage.setPixelValues(pixelValue,extendedIndex,columnIndex+extendBy,depthIndex);
            }
            extendedIndex+=1;
        }
        extendedIndex = imageHeight + extendBy;
    }

    // Interpolate the pixel values of right and left column
    extendedIndex = imageWidth + extendBy;
    for(int depthIndex=0; depthIndex<BytesPerPixel;depthIndex++){
        for(int rowIndex=0; rowIndex<imageHeight;rowIndex++){
            for(int columnIndex=0; columnIndex<extendBy;columnIndex++){
                // left column
                pixelValue = accessPixelValue(rowIndex,columnIndex,depthIndex);
                extendedImage.setPixelValues(pixelValue,rowIndex+extendBy,extendBy-columnIndex-1,depthIndex);
                // right column
                pixelValue = accessPixelValue(rowIndex,imageWidth-columnIndex-1,depthIndex);
                extendedImage.setPixelValues(pixelValue,rowIndex+extendBy,extendedIndex,depthIndex);
                extendedIndex+=1;
            }
            extendedIndex = imageWidth + extendBy;
        }
        extendedIndex = imageWidth + extendBy;
    }

    for(int depthIndex=0; depthIndex<BytesPerPixel;depthIndex++) {
        for (int rowIndex = 0; rowIndex < extendBy; rowIndex++) {
            for (int columnIndex = 0; columnIndex < extendBy; columnIndex++) {

                // top left corner
                pixelValue = extendedImage.getPixelValues(rowIndex+extendBy,columnIndex,depthIndex);
                extendedImage.setPixelValues(pixelValue,rowIndex,columnIndex,depthIndex);

                // bottom left corner
                pixelValue = extendedImage.getPixelValues(rowIndex+imageHeight,columnIndex,depthIndex);
                extendedImage.setPixelValues(pixelValue,rowIndex+imageHeight+extendBy,columnIndex,depthIndex);

                // top right corner
                pixelValue = extendedImage.getPixelValues(rowIndex+extendBy,columnIndex+imageWidth+extendBy,depthIndex);
                extendedImage.setPixelValues(pixelValue,rowIndex,columnIndex+imageWidth+extendBy,depthIndex);

                // bottom right corner
                pixelValue = extendedImage.getPixelValues(rowIndex+imageHeight,columnIndex+imageWidth+extendBy,depthIndex);
                extendedImage.setPixelValues(pixelValue,rowIndex+imageHeight+extendBy,columnIndex+imageWidth+extendBy,depthIndex);

            }
        }
    }


    return(extendedImage);
}
//----------------------------------------------------------------------------------------------------------------//
// RGB to CMY
imageData imageData::rgb2cmy(bool replaceColorSpaceFlag){
    // Check if the image is gray scale
    if(BytesPerPixel < 3){
        cout << "Cannot use color transform on grayscale images"<< endl;
        exit(1);
    }
    imageData cmyImage(BytesPerPixel,imageWidth,imageHeight);
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
        return *this;
    }else{
        return(cmyImage);
    }
}
//----------------------------------------------------------------------------------------------------------------//
// RGB to HSL:
imageData imageData::rgb2hsl(bool replaceColorSpaceFlag){

    if(BytesPerPixel < 3){
        cout << "Cannot use color transform on grayscale images"<< endl;
        exit(1);
    }
    imageData hslImage(BytesPerPixel,imageWidth,imageHeight);
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
// HSL to RGB:
imageData imageData::hsl2rgb(bool replaceColorSpaceFlag) {
    if (BytesPerPixel < 3) {
        cout << "Cannot use color transform on grayscale images" << endl;
        exit(1);
    }
    imageData rgbImage(BytesPerPixel, imageWidth, imageHeight);
    rgbImage.setPixelValues(pixelData);
    double h_pixelValue, s_pixelValue, l_pixelValue, m_value, x_value, c_value, r_value, g_value, b_value;
    for (int rowIndex = 0; rowIndex < imageHeight; rowIndex++) {
        for (int columnIndex = 0; columnIndex < imageWidth; columnIndex++) {
            h_pixelValue = accessPixelValue(rowIndex, columnIndex, 0) * (360.0 / 255.0);
            s_pixelValue = accessPixelValue(rowIndex, columnIndex, 1) * (1.0 / 255.0);
            l_pixelValue = accessPixelValue(rowIndex, columnIndex, 2) * (1.0 / 255.0);

            c_value = (1 - fabs(2.0 * l_pixelValue - 1.0)) * s_pixelValue;
            x_value = c_value * (1.0 - fabs(fmod((h_pixelValue / 60.0), 2.0) - 1));
            m_value = l_pixelValue - c_value / 2.0;

            if (h_pixelValue >= 0 && h_pixelValue < 60) {
                r_value = c_value;
                g_value = x_value;
                b_value = 0;
            } else if(h_pixelValue >= 60 && h_pixelValue < 120){
                r_value = x_value;
                g_value = c_value;
                b_value = 0;
            }else if(h_pixelValue >= 120 && h_pixelValue < 180){
                r_value = 0;
                g_value = c_value;
                b_value = x_value;
            }else if(h_pixelValue >= 180 && h_pixelValue < 240){
                r_value = 0;
                g_value = x_value;
                b_value = c_value;
            }else if(h_pixelValue >= 240 && h_pixelValue < 300){
                r_value = x_value;
                g_value = 0;
                b_value = c_value;
            }else if(h_pixelValue >= 300 && h_pixelValue < 360){
                r_value = c_value;
                g_value = 0;
                b_value = x_value;
            }

            r_value = (unsigned char)(255.0*(r_value+m_value));
            g_value = (unsigned char)(255.0*(g_value+m_value));
            b_value = (unsigned char)(255.0*(b_value+m_value));

            rgbImage.setPixelValues((unsigned char)r_value,rowIndex,columnIndex,0);
            rgbImage.setPixelValues((unsigned char)g_value,rowIndex,columnIndex,1);
            rgbImage.setPixelValues((unsigned char)b_value,rowIndex,columnIndex,2);
        }
    }

    if(replaceColorSpaceFlag){
        pixelData = rgbImage.getPixelValues();
        return *this;
    }else{
        return(rgbImage);
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
// Combine channels to form color image:
void imageData::concatenateChannels(vector<imageData> colorChannels) {

    unsigned char redValue;
    unsigned char greenValue;
    unsigned char blueValue;

    for(int rowIndex=0; rowIndex<imageHeight;rowIndex++) {
        for (int columnIndex = 0; columnIndex < imageWidth; columnIndex++) {

            redValue = colorChannels[0].getPixelValues(rowIndex,columnIndex,0);
            greenValue = colorChannels[1].getPixelValues(rowIndex,columnIndex,0);
            blueValue = colorChannels[2].getPixelValues(rowIndex,columnIndex,0);
            setPixelValues(redValue,rowIndex,columnIndex,0);
            setPixelValues(greenValue,rowIndex,columnIndex,1);
            setPixelValues(blueValue,rowIndex,columnIndex,2);

        }
    }
}

//----------------------------------------------------------------------------------------------------------------//
// Color to grayscale transform:
imageData imageData::colorToGrayscale(){

    if(BytesPerPixel!=3){
        cout<< "Cannot convert to grayscale. Please check the input" <<endl;
        exit(-1);
    }

    // Local variables
    imageData grayScaleImage(1,imageWidth,imageHeight);
    unsigned  char pixelValue;

    // Convert to grayscale
    for(int rowIndex=0; rowIndex<imageHeight;rowIndex++){
        for(int columnIndex=0; columnIndex<imageWidth;columnIndex++){

            pixelValue = (unsigned char)(0.299*(double)accessPixelValue(rowIndex,columnIndex,0) + 0.587*(double)accessPixelValue(rowIndex,columnIndex,1) + 0.114*(double)accessPixelValue(rowIndex,columnIndex,2));
            grayScaleImage.setPixelValues(pixelValue,rowIndex,columnIndex,0);

        }
    }

    return grayScaleImage;

}

//----------------------------------------------------------------------------------------------------------------//
// Image Diamond Warping:
unsigned char imageData::diamondWarping(double x, double y, unsigned char defaultBackgroundValue){

    // Initialize all warping matrices
    static matrix<int,double> diamondWarpingMatrix1(3,3,1);
    diamondWarpingMatrix1.setMatrixByValues(9,2.0,0.0,-150.5,-1.0,1.0,150.5,0.0,0.0,1.0); // Found from matlab
    static matrix<int,double> diamondWarpingMatrix2(3,3,1);
    diamondWarpingMatrix2.setMatrixByValues(9,2.0135,0.0,-152.5338,1.0135,1.0000,-152.5338,0.0,0.0,1.0);
    static matrix<int,double> diamondWarpingMatrix3(3,3,1);
    diamondWarpingMatrix3.setMatrixByValues(9,1.0,1.0135,-152.5338,0.00, 2.0135,-152.5338,0.0,0.0,1.0000);
    static matrix<int,double> diamondWarpingMatrix4(3,3,1);
    diamondWarpingMatrix4.setMatrixByValues(9,1.0000,-1.0000,150.5000,0.0000,2.0000,-150.5000,0.0,0.0,1.0000);
    static matrix<int,double> diamondWarpingMatrix5(3,3,1);
    diamondWarpingMatrix5.setMatrixByValues(9,2.0135,0.0,-152.5338,-1.0135,1.0000,152.5338,0.00,0.0,1.0000);
    static matrix<int,double> diamondWarpingMatrix6(3,3,1);
    diamondWarpingMatrix6.setMatrixByValues(9,2.0000,0.0,-150.5000,1.0000,1.0000,-150.5000,0.0000,0.0,1.0000);
    static matrix<int,double> diamondWarpingMatrix7(3,3,1);
    diamondWarpingMatrix7.setMatrixByValues(9,1.0000,1.0000,-150.5000,0.0,2.0000,-150.5000,0.0,0.0000,1.0000);
    static matrix<int,double> diamondWarpingMatrix8(3,3,1);
    diamondWarpingMatrix8.setMatrixByValues(9,1.0000,-1.0135,152.5338,0.0,2.0135,-152.5338,0.0,0.0000,1.0000);

    // Initialize output and input coordinates:
    vector<double> outputCartesianCoordinates;
    matrix<int,double> inputCartesianCoordinates(3,1,1);
    inputCartesianCoordinates.setMatrixByValues(3,x,y,1.0);

    // Generate hypothesis value
    double yNormalized = y-150.5;
    double xNormalized = x-150.5;
    double hypothesisValue = (atan((yNormalized)/(xNormalized)))*(180.0/M_PI);

    if((xNormalized<0)&&(yNormalized>=0)){
        hypothesisValue +=180;
    } else if((xNormalized<0)&&(yNormalized<0)){
        hypothesisValue +=180;
    } else if((xNormalized>=0)&&(yNormalized<0)){
        hypothesisValue += 360;
    }

    // Applying suitable transformation matrix
    if((hypothesisValue<=135)&&(hypothesisValue>=90)){
        outputCartesianCoordinates = (diamondWarpingMatrix1.multiplyWith(inputCartesianCoordinates)).getMatrixValues();
    } else if((hypothesisValue<=90)&&(hypothesisValue>=45)){
        outputCartesianCoordinates = (diamondWarpingMatrix2.multiplyWith(inputCartesianCoordinates)).getMatrixValues();
    }else if((hypothesisValue<=45)&&(hypothesisValue>=0)){
        outputCartesianCoordinates = (diamondWarpingMatrix3.multiplyWith(inputCartesianCoordinates)).getMatrixValues();
    }else if((hypothesisValue<=360)&&(hypothesisValue>=315)){
        outputCartesianCoordinates = (diamondWarpingMatrix4.multiplyWith(inputCartesianCoordinates)).getMatrixValues();
    }else if((hypothesisValue<=315)&&(hypothesisValue>=270)){
        outputCartesianCoordinates = (diamondWarpingMatrix5.multiplyWith(inputCartesianCoordinates)).getMatrixValues();
    }else if((hypothesisValue<=270)&&(hypothesisValue>=225)){
        outputCartesianCoordinates = (diamondWarpingMatrix6.multiplyWith(inputCartesianCoordinates)).getMatrixValues();
    }else if((hypothesisValue<=225)&&(hypothesisValue>=180)){
        outputCartesianCoordinates = (diamondWarpingMatrix7.multiplyWith(inputCartesianCoordinates)).getMatrixValues();
    }else if((hypothesisValue<=180)&&(hypothesisValue>=135)){
        outputCartesianCoordinates = (diamondWarpingMatrix8.multiplyWith(inputCartesianCoordinates)).getMatrixValues();
    }else{
        outputCartesianCoordinates = inputCartesianCoordinates.getMatrixValues();
    }

    return getPixelValuesFrom_xy(outputCartesianCoordinates[0],outputCartesianCoordinates[1],0);
}

//----------------------------------------------------------------------------------------------------------------//
// Puzzle matching 1
unsigned char imageData::puzzleMatching1(double x, double y,imageData* piecesImage,unsigned char defaultBackgroundValue){

    // Static variables
    static matrix<int,double> mapping_puzzle1(3,3,1);
    mapping_puzzle1.setMatrixByValues(9,1.3861,0.3762,-283.1485,-0.3812,1.3911,-17.8589,-0.0000,0.0000,1.0000);

    // Local variables
    vector<double> outputCartesianCoordinates;
    matrix<int,double> inputCartesianCoordinates(3,1,1);
    inputCartesianCoordinates.setMatrixByValues(3,x,y,1.0);

    // Calculate output coordinates
    vector<double> outputCartesianCoordinate = (mapping_puzzle1.multiplyWith(inputCartesianCoordinates)).getMatrixValues();

    return piecesImage->getPixelValuesFrom_xy(outputCartesianCoordinate[0],outputCartesianCoordinate[1],0);

}

//----------------------------------------------------------------------------------------------------------------//
// Puzzle matching 2
unsigned char imageData::puzzleMatching2(double x, double y, imageData* piecesImage,unsigned char defaultBackgroundValue){

    // Static variables
    static matrix<int,double> mapping_puzzle2(3,3,1);
    mapping_puzzle2.setMatrixByValues(9,-0.0594,-0.7030,514.8812,0.6980,-0.0644,33.3540,-0.0000,-0.0000,1.0000);

    // Local variables
    vector<double> outputCartesianCoordinates;
    matrix<int,double> inputCartesianCoordinates(3,1,1);
    inputCartesianCoordinates.setMatrixByValues(3,x,y,1.0);

    // Calculate output coordinates
    vector<double> outputCartesianCoordinate = (mapping_puzzle2.multiplyWith(inputCartesianCoordinates)).getMatrixValues();

    return piecesImage->getPixelValuesFrom_xy(outputCartesianCoordinate[0],outputCartesianCoordinate[1],0);

}

//----------------------------------------------------------------------------------------------------------------//
// Image overlay 1:
unsigned char imageData::textEmbedding1(double x, double y, imageData* textImage,unsigned char defaultBackgroundValue){

    // Static variables
    static matrix<int,double> mapping_text1(3,3,1);
    mapping_text1.setMatrixByValues(9,0.0684,0.8874,-66.3633,-0.6006,0.4089,304.3954,0.0001 ,-0.0028, 1.0);

    // Local variables
    vector<double> outputCartesianCoordinates;
    matrix<int,double> inputCartesianCoordinates(3,1,1);
    inputCartesianCoordinates.setMatrixByValues(3,x,y,1.0);

    // Calculate output coordinates
    vector<double> outputCartesianCoordinate = (mapping_text1.multiplyWith(inputCartesianCoordinates)).getMatrixValues();

    return textImage->getPixelValuesFrom_xy(outputCartesianCoordinate[0]/outputCartesianCoordinate[2],outputCartesianCoordinate[1]/outputCartesianCoordinate[2],defaultBackgroundValue);
}

//----------------------------------------------------------------------------------------------------------------//
// Image Translation:
void imageData::translation(double delta_x ,double delta_y){

    matrix<int,double> transformer(3,3,1);
    matrix<int,double> translationMatrix(3,3,1);

    transformer.setMatrixValues(toCartesianVector);
    translationMatrix.setMatrixByValues(9,1.0,0.0,delta_x,0.0,1.0,delta_y,0.0,0.0,1.0);

    translationMatrix = translationMatrix.multiplyWith(transformer);

    toCartesianVector = translationMatrix.getMatrixValues();
    toImageVector = (translationMatrix.pseudoInverse()).getMatrixValues();

}

//----------------------------------------------------------------------------------------------------------------//
// Image Rotation:

void imageData::rotation(double theta){

    matrix<int,double> transformer(3,3,1);
    matrix<int,double> rotationMatrix(3,3,1);

    transformer.setMatrixValues(toCartesianVector);
    rotationMatrix.setMatrixByValues(9,cos(M_PI*theta/180.0),-sin(M_PI*theta/180.0),0.0,sin(M_PI*theta/180.0),cos(M_PI*theta/180.0),0.0,0.0,0.0,1.0);

    rotationMatrix = rotationMatrix.multiplyWith(transformer);

    toCartesianVector = rotationMatrix.getMatrixValues();
    toImageVector = (rotationMatrix.pseudoInverse()).getMatrixValues();

}

//----------------------------------------------------------------------------------------------------------------//
// Image Scaling:

void imageData::scaling(double xScaleFactor, double yScaleFactor){

    matrix<int,double> transformer(3,3,1);
    matrix<int,double> scalingMatrix(3,3,1);

    transformer.setMatrixValues(toCartesianVector);
    scalingMatrix.setMatrixByValues(9,xScaleFactor,0.0,0.0,0.0,yScaleFactor,0.0,0.0,0.0,1.0);

    scalingMatrix = scalingMatrix.multiplyWith(transformer);

    toCartesianVector = scalingMatrix.getMatrixValues();
    toImageVector = (scalingMatrix.pseudoInverse()).getMatrixValues();

}

//----------------------------------------------------------------------------------------------------------------//
// Image coordinate to x,y coordinate:
vector<double> imageData::imageToCartesian(double pixelRow, double pixelColumn){

    // Initialize matrices
    matrix<int,double> toCartesianMatrix(3,3,1);
    matrix<int,double> imageCoordinates(3,1,1);
    matrix<int,double> cartesianCoordinates(3,1,1);

    // Set matrix values
    toCartesianMatrix.setMatrixValues(toCartesianVector);
    imageCoordinates.setMatrixByValues(3,pixelRow,pixelColumn,1.0);

    // Calculate cartetian value
    cartesianCoordinates = toCartesianMatrix.multiplyWith(imageCoordinates);

    // Return matrix vector
    return cartesianCoordinates.getMatrixValues();

}

//----------------------------------------------------------------------------------------------------------------//
// x,y to image coordinate:
vector<double> imageData::cartesianToImage(double x, double y){

    // Initialize matrices
    matrix<int,double> toImageMatrix(3,3,1);
    matrix<int,double> imageCoordinates(3,1,1);
    matrix<int,double> cartesianCoordinates(3,1,1);

    // Set transformer values
    toImageMatrix.setMatrixValues(toImageVector);
//    transformer = transformer.pseudoInverse();

    cartesianCoordinates.setMatrixByValues(3,x,y,1.0);

    // Calculate image index value
    imageCoordinates = toImageMatrix.multiplyWith(cartesianCoordinates);

    // Return matrix vector
    return imageCoordinates.getMatrixValues();

}

//----------------------------------------------------------------------------------------------------------------//
// Return pixel values from x,y coordinate:

unsigned char imageData::getPixelValuesFrom_xy(double x, double y, unsigned char defaultBackgroundValue){

    // Local Variables
    vector<double> imageCoordinates = cartesianToImage(x,y);
    double intPart;

    // Check if the access is beyond the indices
    if((imageCoordinates[0]>=imageHeight)||(imageCoordinates[1]>=imageWidth)||(imageCoordinates[0]<0)||(imageCoordinates[1]<0)){
        return defaultBackgroundValue;
    }

    // Check if the coordinates are at the edges
    else if((ceil(imageCoordinates[0])>=imageHeight-1)||(ceil(imageCoordinates[1])>=imageWidth-1)){
        return accessPixelValue(imageCoordinates[0],imageCoordinates[1],0);
    }

    // Check if the coordinates are fractional. Apply bilinear interpolation if so.
    else if((modf(imageCoordinates[0],&intPart)!=0)||(modf(imageCoordinates[1],&intPart)!=0)){
        imageAlgorithms interpolateValues(this);
        unsigned char pixelValue;
        pixelValue = interpolateValues.bilinearInterpolation(imageCoordinates[0],imageCoordinates[1],0.0);
        return pixelValue;
    }
    // Else return the accessed values.
    else{
        return accessPixelValue(imageCoordinates[0],imageCoordinates[1],0);
    }

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
// Save binary Image:

void imageData::saveBinaryImage(const char* outputFileName){
    FILE* outputFile;
    if (!(outputFile=fopen(outputFileName,"wb"))) {
        cout << "Cannot open file: " << outputFileName << endl;
        exit(1);
    }

    // Convert binary to unsigned char
    vector<unsigned char> convertedPixelData(imageWidth*imageHeight*BytesPerPixel,0);

    for(int index = 0; index < convertedPixelData.size(); index++){
        convertedPixelData[index] = 255*((unsigned char)pixelData[index]);
    }

    fwrite(&convertedPixelData[0], sizeof(unsigned char), imageWidth*imageHeight*BytesPerPixel, outputFile);
    fclose(outputFile);
}
//----------------------------------------------------------------------------------------------------------------//
// Convert an image to Mat file
Mat imageData::convertToMat(){

    // Test for number of channels
    if(BytesPerPixel == 3){

        Mat outputContainer(imageHeight,imageWidth,CV_8UC3);
        Vec3b intensities;
        // Fill in image values into mat file:
        for(int rowIndex = 0; rowIndex < imageHeight; rowIndex++){
            for(int columnIndex = 0; columnIndex < imageWidth; columnIndex++){

                intensities.val[0] = accessPixelValue(rowIndex,columnIndex,2);
                intensities.val[1] = accessPixelValue(rowIndex,columnIndex,1);
                intensities.val[2] = accessPixelValue(rowIndex,columnIndex,0);
                outputContainer.at<Vec3b>(rowIndex,columnIndex) = intensities;
            }
        }
        return outputContainer;

    }else if(BytesPerPixel == 1){

        Mat outputContainer(imageHeight,imageWidth,CV_8UC1);
        // Fill in image values into mat file:
        for(int rowIndex = 0; rowIndex < imageHeight; rowIndex++){
            for(int columnIndex = 0; columnIndex < imageWidth; columnIndex++){
                outputContainer.at<unsigned char>(rowIndex,columnIndex) = accessPixelValue(rowIndex,columnIndex,0);
            }
        }
        return outputContainer;

    }else{
        cout<< " Bytes per pixel incorrect while converting to mat!" <<endl;
        exit(0);
    }
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
