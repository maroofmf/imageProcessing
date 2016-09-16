#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

int main( int argc, char** argv )
{
    Mat img;
    FILE *fp = NULL;
    char *imagedata = NULL;
    int framesize = IMAGE_WIDTH * IMAGE_HEIGHT;

//Open raw Bayer image.
    fp = fopen("Trojan.raw", "rb");

//Memory allocation for bayer image data buffer.
    imagedata = (char*) malloc (sizeof(char) * framesize);

//Read image data and store in buffer.
    fread(imagedata, sizeof(char), framesize, fp);

//Image dimension.
    imageSize.height = IMAGE_WIDTH;
    imageSize.width = IMAGE_HEIGHT;

//Create Opencv mat structure for image dimension. For 8 bit bayer, type should be CV_8UC1.
    img.create(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);

    memcpy(img.data, imagedata, framesize);

    free(imagedata);

    fclose(fp);
}