// Name: Maroof Mohammed Farooq
// HW P2
// Image Processing

#include "headers.h"
#include "imageData.h"
#include "imageAlgorithms.h"
#include "matrix.h"

using namespace std;
using namespace cv;

int main(int argc, char *argv[]) {
	// Define file pointer and variables
	int BytesPerPixel;
	int imageWidth;
	int imageHeight;
	int problemNumber;
//----------------------------------------------------------------------------------------------------------------//
	// Check for proper syntax
	if (argc < 3) {
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout <<
		"program_name input_image.raw output_image.raw [BytesPerPixel = 1] [imageWidth = 256] [imageHeight = 256]" <<
		endl;
		return 0;
	}

	// Check if image is grayscale or color
	if (argc < 4) {
		BytesPerPixel = 1;// default is grey image
		imageHeight = 256;
		imageWidth = 256;
	}
	else {
		BytesPerPixel = atoi(argv[3]);
		// Check if size is specified
		if (argc >= 5) {
			imageWidth = atoi(argv[4]);
			imageHeight = atoi(argv[5]);
		}
		else {
			imageHeight = 256;
			imageWidth = 256;
		}
	}
	problemNumber = atoi(argv[6]);

//----------------------------------------------------------------------------------------------------------------//
// input and output file names
	string inputFileName = argv[1];
	string outputFileName = argv[2];

//----------------------------------------------------------------------------------------------------------------//
// 1(a) Texture Classification

	if (problemNumber == 1) {

		// Reading additional arguments:
		if(argc!=8){
			cout<< "Incorrect number of arguments for problem 1!" <<endl;
			exit(0);
		}

		int trainingSize = atoi(argv[7]);

		// Read 12 training images

		vector<imageData*> trainingImages;
		vector<Mat> matFiles;
		for(int i = 1; i<=trainingSize; i++){
			imageData* trainImage = new imageData(BytesPerPixel,imageWidth,imageHeight);
			trainImage->imageRead(("HW3 Images/P1/Texture"+to_string(i)+".raw").c_str());
			Mat tempImage = trainImage->convertToMat();
			trainingImages.push_back(trainImage);
			matFiles.push_back((tempImage));
		}

		// Create object for image algorithms
		imageAlgorithms lawsAlgorithm(trainingImages[0]);

		// Step 1: Remove DC Component from filter
		vector<matrix<int,double> > subtractedDCImages;
		for_each(trainingImages.begin(),trainingImages.end(),[&](imageData* image){
			subtractedDCImages.push_back(lawsAlgorithm.subtractDC(*image));
		});

		// Step 2: Create Law's filter
		map<int,matrix<int,double> > filterBank = lawsAlgorithm.getLawsFilter();

		// Step 3: Convolve laws filter with input image
		map<int, matrix<int,double> > outputImages;
		unordered_map<int, vector<double> > featureVectors;
		featureVectors.reserve(trainingSize);
		matrix<int,double> tempMatrix(imageHeight,imageWidth,BytesPerPixel);
		double energyValue = 0;
		int imageNumber = 0;

		// Convolve all images
		for_each(subtractedDCImages.begin(), subtractedDCImages.end(), [&](matrix<int,double> inputImage){

			// Apply filter and calculate energy value
			for_each(filterBank.begin(),filterBank.end(),[&](pair<int,matrix<int,double> > filter){
				tempMatrix = lawsAlgorithm.filterApply(&inputImage,&get<1>(filter),"convolutionWithoutAbsolute");
				energyValue = lawsAlgorithm.imageEnergy(tempMatrix);
				featureVectors[imageNumber].push_back(energyValue);
			});

			imageNumber++;

		});

		// Step 4: Perform PCA:

		// Create a feature matrix
		Mat dataPoints(12,25,CV_32F);

		for(int rowIndex = 0; rowIndex < 12; rowIndex++){
			for(int columnIndex = 0; columnIndex < 25; columnIndex++){

				dataPoints.at<float>(rowIndex,columnIndex) = featureVectors[rowIndex][columnIndex];

			}
		}

		// Calculate PCA:
		PCA pca(dataPoints, Mat(), CV_PCA_DATA_AS_ROW, 3);
		Mat mean = pca.mean.clone();
		Mat eigenvalues = pca.eigenvalues.clone();
		Mat eigenvectors = pca.eigenvectors.clone();

		// Project PCA points:
		Mat projected;
		pca.project(dataPoints,projected);

		// Export PCA points to matlab
		matrix<int,float> matlabOut(12,3,1);
		matlabOut = matlabOut.mat2matrix<int,float>(projected);
		matlabOut.printForMatlab();

		// Export Original data points to matlab:
		matrix<int,float> matlabOut1(12,25,1);
		matlabOut1 = matlabOut1.mat2matrix<int,float>(dataPoints);
		matlabOut1.printForMatlab();

		// Step 5: Applying K means:

		// K means Parameters
		TermCriteria criteria = TermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 3, 0.001 );
		int kValue = 4;
		int numberOfAttempts = 1;
		Mat labels,centers;

		// Apply K means:
		kmeans(dataPoints ,kValue, labels, criteria, numberOfAttempts, KMEANS_RANDOM_CENTERS, centers);

		cout<< labels << endl;

	}

//----------------------------------------------------------------------------------------------------------------//
// 1(b) Texture Segmentation:

	if(problemNumber == 2){

		// Read input Image
		imageData inputImage(BytesPerPixel,imageWidth,imageHeight);
		inputImage.imageRead(inputFileName.c_str());

		// Input image matrix:
		matrix<int,double> inputImageMatrix(imageHeight,imageWidth,BytesPerPixel);
		inputImageMatrix.setMatrixValues(inputImage,0,0,0,1);

		// Create image algorithms:
		imageAlgorithms segmentation_pipeline(&inputImage);

		// Create Laws filter:
		map<int,matrix<int,double> > filterBank = segmentation_pipeline.getLawsFilter();

		// Convlolve all filters:
		map<int, matrix<int,double> > outputImages;
		int index = 0;
		for_each(filterBank.begin(),filterBank.end(),[&](pair<int,matrix<int,double>> filter){
			outputImages[index++] = segmentation_pipeline.filterApply(&inputImageMatrix,&get<1>(filter),"convolutionWithoutAbsolute");
		});

		// K means:
		int windowSize = 15;

		// Compute data points
		Mat dataPoints(imageHeight*imageWidth,25,CV_32F);
		for_each(outputImages.begin(),outputImages.end(),[&](pair<int,matrix<int,double>> imagePair){
			(segmentation_pipeline.energyPerPixel(&get<1>(imagePair),windowSize)).copyTo(dataPoints.col(get<0>(imagePair)));
		});

		// Apply K means
		TermCriteria criteria = TermCriteria( CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001 );
		int kValue = atoi(argv[7]);
		int numberOfAttempts = 5;
		Mat labels,centers;

		// Apply K means:
		kmeans(dataPoints ,kValue, labels, criteria, numberOfAttempts, KMEANS_RANDOM_CENTERS, centers);

		// Output Labels
		Mat outputImage(imageHeight,imageWidth,CV_8UC1);
		unsigned char value = 0;
		unsigned char graylevel[6] = {0, 51, 102, 153, 204, 255};

		for(int rowIndex = 0; rowIndex < imageHeight; rowIndex++){
			for(int columnIndex = 0; columnIndex < imageWidth; columnIndex++){
				outputImage.at<unsigned char>(rowIndex,columnIndex) = (unsigned char)((labels.at<int>(rowIndex*imageWidth+columnIndex,0))*255/kValue);
			}
		}

		// Display image
		imwrite("p1_b_output/"+outputFileName+".jpg",outputImage);
//		imshow("test",outputImage);
//		waitKey(0);

	}
//----------------------------------------------------------------------------------------------------------------//
// 2(a): Extraction and Description of Salient Points:

	if(problemNumber ==4) {

		imageAlgorithms featureExtract;
		featureExtract.saveSalientPoints(inputFileName,"p2_a_output/"+outputFileName);

	}

//----------------------------------------------------------------------------------------------------------------//
// 2(b): Image matching

	if (problemNumber==5){

		// Read input files
		Mat rav4_1 = imread(inputFileName+"rav4_1.jpg", 0);
		Mat rav4_2 = imread(inputFileName+"rav4_2.jpg", 0);
		Mat jeep = imread(inputFileName+"jeep.jpg", 0);
		Mat bus = imread(inputFileName+"bus.jpg", 0);

		// Extract and display points of interest for rav images
		imageAlgorithms featureExtract;
		featureExtract.saveSalientPoints(inputFileName+"rav4_1.jpg","p2_b_output/rav4_1");
		featureExtract.saveSalientPoints(inputFileName+"rav4_2.jpg","p2_b_output/rav4_2");

		// Image Compare
		featureExtract.featureMatching(rav4_1,rav4_2,"p2_b_output/"+outputFileName,2);
		featureExtract.featureMatching(rav4_1,jeep,"p2_b_output/"+outputFileName,2);
		featureExtract.featureMatching(rav4_1,bus,"p2_b_output/"+outputFileName,2);

	}

//----------------------------------------------------------------------------------------------------------------//
// 2(c): Bag of Words

	if (problemNumber==6){

		// Read images
		Mat rav4_1 = imread(inputFileName+"rav4_1.jpg", 0);
		Mat rav4_2 = imread(inputFileName+"rav4_2.jpg", 0);
		Mat jeep = imread(inputFileName+"jeep.jpg", 0);
		Mat bus = imread(inputFileName+"bus.jpg", 0);

		// Add descriptors to bag of words
		BOWKMeansTrainer bagOfWords(8);
		imageAlgorithms bog_pipeline;
		bog_pipeline.addDescriptor(rav4_1, bagOfWords);
		bog_pipeline.addDescriptor(jeep, bagOfWords);
		bog_pipeline.addDescriptor(bus, bagOfWords);

		// Cluster the bag of words to form vocabulary
		Mat vocabulary = bagOfWords.cluster();

		// Set descriptor BOW
		Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");
		Ptr<DescriptorExtractor> extractor = new SiftDescriptorExtractor();
		BOWImgDescriptorExtractor dextract( extractor, matcher);

		// Set vocabulary
		dextract.setVocabulary(vocabulary);

		// Create vector of training images:
		vector<Mat> trainImages;
		trainImages.push_back(rav4_1);
		trainImages.push_back(jeep);
		trainImages.push_back(bus);

		// Create vector of testing images
		vector<Mat> testImages;
		testImages.push_back(rav4_2);

		//train and test the image using BOW:
		Mat hypothesisValues = bog_pipeline.compareUsingBOW(trainImages,testImages,dextract);

		//Print hypothesis Values
		cout<< "Hypothesis Values are:" <<endl;
		cout<< hypothesisValues <<endl;

		// Find the classified label
		string labels[] = {"Rav4_1", "Jeep", "Bus"};
		double min = 1000;
		int index = 0;
		for(int i=0; i< hypothesisValues.cols;i++){
			if(min>hypothesisValues.at<float>(0,i)){
				min = hypothesisValues.at<float>(0,i);
				index = i;
			}
		}
		cout<< "The correct class for test image is: "  << labels[index] <<endl;

	}


//----------------------------------------------------------------------------------------------------------------//
// 3(a) Canny Edge detector:

	if(problemNumber == 7){

//		// Read input image and convert to Mat
		imageData inputImage(BytesPerPixel,imageWidth,imageHeight);
		inputImage.imageRead(inputFileName.c_str());

		//Canny Edge detector
		imageAlgorithms cannyEdge(&inputImage);
		Mat cannyAuto = cannyEdge.auto_cannyEdge(0.33);

		// Save image
		imwrite("p3_a_output/"+outputFileName+".jpg",cannyAuto );

	}

	return 0;
}

