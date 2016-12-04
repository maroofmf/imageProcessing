-- Name: Maroof Mohammed Farooq
-- Project 4
-- CNN testing on translation invariance!
-------------------------------------------------------------------------
-- Initialize packages

require 'nn'
require 'optim'
require 'image'
require 'gnuplot'

-------------------------------------------------------------------------
-- Load training and testing DataSet

collectgarbage();
torch.manualSeed(0);
training_dataSet = torch.load('mnist-p1b-train.t7');
testing_dataSet = torch.load('mnist-p1b-test.t7');
network = torch.load('models/trainedNet_translated.t7');
X_train = training_dataSet.data:double():mul(1/255);
y_train = training_dataSet.label:double():clone();
X_test = testing_dataSet.data:double():mul(1/255);
y_test = testing_dataSet.label:double():clone();

-------------------------------------------------------------------------
-- Translate images and evaluate their accuracy on test data set

function translateAndEvaluate(xTranslate, yTranslate)

	-- Translate images:
	translatedTrain = (image.translate(X_train:view(X_train:size(1),32,32),xTranslate,yTranslate)):view(X_train:size(1),1,32,32);
	translatedTest = (image.translate(X_test:view(10000,32,32),xTranslate,yTranslate)):view(X_test:size(1),1,32,32);

	-- Initialize the confusion matrix:
	local classes = {'0','1','2','3','4','5','6','7','8','9'}
	local confusion_test = optim.ConfusionMatrix(classes);

	-- Compute:
	local testOutput = network:forward(translatedTest);
	confusion_test:batchAdd(testOutput,y_test);
	confusion_test:updateValids()
	print(string.format('\27[34m Translation(%d,%d): Accuracy on Test Set = %.2f, Mean Accuracy Precision = %.2f',xTranslate,yTranslate,100*confusion_test.totalValid,100*confusion_test.averageValid))
	collectgarbage();
	return 100*confusion_test.averageValid
end

-------------------------------------------------------------------------
-- Run testing cases:

-- Case 1: Keep yTranslate constant and vary xTranslate from -5 to 5:
rangeOfValues = torch.linspace(-5,5,11);
accuracyValues = torch.DoubleTensor(1,11);
accuracyIndex=0;

for value = -5,5 do
	accuracyIndex = accuracyIndex+1;
	accuracyValues[1][accuracyIndex] = translateAndEvaluate(value,0);
	collectgarbage();
end

-- Plotting case 1
gnuplot.pdffigure('plots/translation_case_1.pdf')
gnuplot.plot({'Testing',rangeOfValues,accuracyValues[1]})
gnuplot.title('Varying x values with y = 0')
gnuplot.xlabel('xValues')
gnuplot.ylabel('Accuracy')
gnuplot.plotflush()
collectgarbage();

-- Case 2: Keep xTranslate constant and vary yTranslate from -5 to 5:

accuracyIndex=0;

for value = -5,5 do
	accuracyIndex = accuracyIndex+1;
	accuracyValues[1][accuracyIndex] = translateAndEvaluate(0,value);
	collectgarbage();
end

-- Plotting case 2
gnuplot.pdffigure('plots/translation_case_2.pdf')
gnuplot.plot({'Testing',rangeOfValues,accuracyValues[1]})
gnuplot.title('Varying y values with x = 0')
gnuplot.xlabel('yValues')
gnuplot.ylabel('Accuracy')
gnuplot.plotflush()
collectgarbage();

-- Case 3: Varying xTranslate and yTranslate from -5 to 5:

accuracyIndex=0;

for value = -5,5 do
	accuracyIndex = accuracyIndex+1;
	accuracyValues[1][accuracyIndex] = translateAndEvaluate(value,value);
	collectgarbage();
end

-- Plotting case 2
gnuplot.pdffigure('plots/translation_case_3.pdf')
gnuplot.plot({'Testing',rangeOfValues,accuracyValues[1]})
gnuplot.title('Varying x and y values')
gnuplot.xlabel('xValues = yValues')
gnuplot.ylabel('Accuracy')
gnuplot.plotflush()
collectgarbage();

