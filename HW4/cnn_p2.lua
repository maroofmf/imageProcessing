-- Name: Maroof Mohammed Farooq
-- Project 4
-- CNN testing on negative image samples
-------------------------------------------------------------------------
-- Initialize packages

require 'nn'
require 'optim'
require 'image'
require 'gnuplot'

-------------------------------------------------------------------------
-- Loading Network and datasets:

trainedNetwork = torch.load('models/trainedNet_1.t7');
trainSet = torch.load('mnist-p1b-train.t7')
testSet = torch.load('mnist-p1b-test.t7');
X_train = trainSet.data:double():mul(1/255);
y_train = trainSet.label:double();
X_test = testSet.data:double():mul(1/255);
y_test = testSet.label:double();

-------------------------------------------------------------------------
-- Predicting labels for negative samples

function negativeSampleTest()

	local classes = {'0','1','2','3','4','5','6','7','8','9'};
	local confusion_negative = optim.ConfusionMatrix(classes);
	local output = trainedNetwork:forward(X_test:clone():mul(-1):add(1));
	confusion_negative:batchAdd(output,y_test);
	confusion_negative:updateValids()
	print(string.format('\27[34m Accuracy on negative test samples = %.2f, Mean Accuracy Precision = %.2f',100*confusion_negative.totalValid, 100* confusion_negative.averageValid))
	torch.save('metadata/confusion_negative.t7',confusion_negative);
end

negativeSampleTest();
---------------------------------------------------------------------------
-- Develop the network for both negative and positive samples

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
-- Develop data set for negative and positive samples

X_train_negative = X_train:clone():mul(-1):add(1)
y_train_negative = y_train:clone()
X_test_negative = X_test:clone():mul(-1):add(1)
y_test_negative = y_test:clone()

X_train = torch.cat(X_train,X_train_negative,1)
y_train = torch.cat(y_train, y_train_negative,1)
X_test = torch.cat(X_test,X_test_negative,1)
y_test = torch.cat(y_test,y_test_negative,1)

perm1 = torch.randperm(X_train:size(1)):long()
perm2 = torch.randperm(X_test:size(1)):long()

X_train:index(1,perm1)
y_train:index(1,perm1)
X_test:index(1,perm2)
y_test:index(1,perm2)

-------------------------------------------------------------------------
-- Define test-train system paramters:

numberOfEpochs = 7;
trainingSize = X_train:size(1)
modelParams,gradParams = network:getParameters()
epochAccuracy_train = torch.DoubleTensor(1,numberOfEpochs);
epochAccuracy_test = torch.DoubleTensor(1,numberOfEpochs);
batchSize = 50;
config = {learningRate = 0.06}

-------------------------------------------------------------------------
-- Testing

-- Compute accuracy on test and save test confusion matrix:
function testData(epochNumber)

	-- Confusion matrix
	local classes = {'0','1','2','3','4','5','6','7','8','9'}
	local confusion_test = optim.ConfusionMatrix(classes);

	-- Compute:
	local testOutput = network:forward(X_test);
	confusion_test:batchAdd(testOutput,y_test);
	confusion_test:updateValids()
	print(string.format('\27[34m Accuracy on Test Set = %.2f, Mean Accuracy Precision = %.2f',100*confusion_test.totalValid,100*confusion_test.averageValid))
	epochAccuracy_test[1][epochNumber] = 100*confusion_test.totalValid
	torch.save('metadata/confusion_mixed_'..tostring(epochNumber)..'.t7',confusion_test)

end

-------------------------------------------------------------------------
-- Training

for epochNumber = 1,numberOfEpochs do

	totalLoss = 0;
	sign = 0;

	-- Confusion matrix
	classes = {'0','1','2','3','4','5','6','7','8','9'}
	confusion_train = optim.ConfusionMatrix(classes)
	confusion_test = optim.ConfusionMatrix(classes);

	-- Data shuffling:
	perm = torch.randperm(X_train:size(1)):long()
	X_train:index(1,perm)
	y_train:index(1,perm)

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
			return errorValue,gradParams
		end

		-- Update system parameters
		optim.sgd(feval,modelParams, config)
		io.write(string.format("\27[34m progress: %4d/%4d\r",iteration,trainingSize))
		io.flush()
	end

	-- Compute total loss in training and state the accuracy
	totalLoss = totalLoss/ trainingSize;
	confusion_train:updateValids()
	print(string.format('\27[34m epoch = %2d/%2d, loss = %.2f, accuracy = %.2f, mean accuracy precision = %.2f',epochNumber,numberOfEpochs, totalLoss, 100*confusion_train.totalValid, 100*confusion_train.averageValid))
	epochAccuracy_train[1][epochNumber] = 100*confusion_train.totalValid;
	torch.save('metadata/confusion_train_mixed_'..tostring(epochNumber)..'.t7',confusion_train)

	-- Compute accuracy on test data
	testData(epochNumber)

end

-------------------------------------------------------------------------
-- Plotting epoch - accuracy results:

epochs = torch.linspace(1,numberOfEpochs,numberOfEpochs)
gnuplot.pdffigure('plots/training_2.pdf')
gnuplot.plot({'Training',epochs,epochAccuracy_train[1]},{'Testing',epochs,epochAccuracy_test[1]})
gnuplot.xlabel('Epochs')
gnuplot.ylabel('Accuracy')
gnuplot.plotflush()

-------------------------------------------------------------------------
-- Save network and accuracy values

network:clearState();
torch.save('models/trainedNet_mixed.t7', network)
torch.save('metadata/trainAccuracy_mixed.t7',epochAccuracy_train)
torch.save('metadata/testAccuracy_mixed.t7',epochAccuracy_test)
