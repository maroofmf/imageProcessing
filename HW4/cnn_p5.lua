-- Name: Maroof Mohammed Farooq
-- Project 4
-- CNN training on translation invariance!
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
network = torch.load('models/trainedNet_1.t7');
X_train = training_dataSet.data:double():mul(1/255);
y_train = training_dataSet.label:double():clone();
X_test = testing_dataSet.data:double():mul(1/255);
y_test = testing_dataSet.label:double():clone();


-------------------------------------------------------------------------
-- Creating translationally invariant data:

originalX_train = X_train:clone();
originaly_train = y_train:clone();

function getData()

	-- reset data:
	X_train = originalX_train:clone();
	y_train = originaly_train:clone();

	-- data shuffling:
	local perm1 = torch.randperm(X_train:size(1)):long()
	X_train:index(1,perm1)
	y_train:index(1,perm1)

	-- translate 50% of images

	for i = 1,30000,1000 do

		-- get translation parameters:
		xTranslate = math.floor(torch.uniform(-5,5))
		yTranslate = math.floor(torch.uniform(-5,5))

		X_train[{{i,i+999},{},{},{}}] = (image.translate(X_train:narrow(1,i,1000):view(1000,32,32),xTranslate,yTranslate)):view(1000,1,32,32);

	end

	-- data shuffling
	local perm2 = torch.randperm(X_train:size(1)):long()
	X_train:index(1,perm2)
	y_train:index(1,perm2)
	collectgarbage();
	return
end

---------------------------------------------------------------------------
-- Develop the network for translation invariance

network = nn.Sequential();

-- View Layer:
network:add(nn.View(1,32,32))

-- First Layer
network:add(nn.SpatialConvolution(1,6,5,5));
network:add(nn.SpatialMaxPooling(2,2,2,2))
network:add(nn.ReLU());
--network:add(nn.Dropout(0.1))

-- Second Layer
network:add(nn.SpatialConvolution(6,16,5,5));
network:add(nn.SpatialMaxPooling(2,2,2,2))
network:add(nn.ReLU());
--network:add(nn.Dropout(0.1))

-- View Layer
network:add(nn.View(16*5*5))

-- Fully Connected Layer 1:
network:add(nn.Linear(16*5*5,120))
network:add(nn.ReLU())
--network:add(nn.Dropout(0.2))

-- Fully Connected Layer 1:
network:add(nn.Linear(120,84))
network:add(nn.ReLU())
--network:add(nn.Dropout(0.2))

-- Output Layer:
network:add(nn.Linear(84,10));
network:add(nn.LogSoftMax());

-------------------------------------------------------------------------
-- Define cost function:

criterion = nn.ClassNLLCriterion();


-------------------------------------------------------------------------
-- Define test-train system paramters:

numberOfEpochs = 7;
trainingSize = X_train:size(1)
modelParams,gradParams = network:getParameters()
epochAccuracy_train = torch.DoubleTensor(1,numberOfEpochs);
batchSize = 10;
config = {learningRate = 0.06, momentum = 0.9 }

-------------------------------------------------------------------------
-- Training

for epochNumber = 1,numberOfEpochs do

	totalLoss = 0;
	sign = 0;

	-- Confusion matrix
	classes = {'0','1','2','3','4','5','6','7','8','9'}
	confusion_train = optim.ConfusionMatrix(classes)
	confusion_test = optim.ConfusionMatrix(classes);

	-- Get translated and shuffled data:
	getData();

	-- Batch training
	for iteration = 1,trainingSize,batchSize do

		-- Slice training and testing data:
		batchSize_new = math.min(trainingSize-iteration+1,batchSize)
		trainingBatch = X_train:narrow(1,iteration,batchSize_new)
		labelBatch = y_train:narrow(1,iteration,batchSize_new)

		-- Function for forward pass:
		local function feval()
			gradParams:zero()
			local networkOutput = network:forward(trainingBatch)
			local errorValue = criterion:forward(networkOutput, labelBatch)
			totalLoss = totalLoss + batchSize_new*errorValue;
			local gradOutput = criterion:backward(networkOutput, labelBatch)
			local gradInput = network:backward(trainingBatch,gradOutput)
			confusion_train:batchAdd(networkOutput,labelBatch);
			collectgarbage();
			return errorValue,gradParams
		end

		-- Update system parameters
		optim.sgd(feval,modelParams, config)
		io.write(string.format("\27[34m progress: %4d/%4d\r",iteration,trainingSize))
		io.flush()
		collectgarbage();
	end

	-- Compute total loss in training and state the accuracy
	totalLoss = totalLoss/ trainingSize;
	confusion_train:updateValids()
	print(string.format('\27[34m epoch = %2d/%2d, loss = %.2f, accuracy = %.2f, mean accuracy precision = %.2f',epochNumber,numberOfEpochs, totalLoss, 100*confusion_train.totalValid, 100*confusion_train.averageValid))
	epochAccuracy_train[1][epochNumber] = 100*confusion_train.totalValid;
	torch.save('metadata/confusion_train_translated_'..tostring(epochNumber)..'.t7',confusion_train)

end

-------------------------------------------------------------------------
-- Plotting epoch - accuracy results:

epochs = torch.linspace(1,numberOfEpochs,numberOfEpochs)
gnuplot.pdffigure('plots/training_translated.pdf')
gnuplot.plot({'Training',epochs,epochAccuracy_train[1]})
gnuplot.xlabel('Epochs')
gnuplot.ylabel('Accuracy')
gnuplot.plotflush()

-------------------------------------------------------------------------
-- Save network and accuracy values

network:clearState();
torch.save('models/trainedNet_translated.t7', network)
torch.save('metadata/trainAccuracy_translated.t7',epochAccuracy_train)
collectgarbage();

-------------------------------------------------------------------------
-- Testing Network:
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




