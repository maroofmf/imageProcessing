-- Name: Maroof Mohammed Farooq
-- Project 4
-- CNN testing on color background!
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
training_dataSet = torch.load('mnist-p2b-train.t7');
testing_dataSet = torch.load('mnist-p2b-test.t7');
X_train = training_dataSet.data:double():mul(1/255);
y_train = training_dataSet.label:double():clone();
X_test = testing_dataSet.data:double():mul(1/255);
y_test = testing_dataSet.label:double():clone();

-------------------------------------------------------------------------
-- Develop the network

network = nn.Sequential();

-- View Layer:
network:add(nn.View(3,32,32))

-- First Layer
network:add(nn.SpatialConvolution(3,6,5,5));
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
--network:add(nn.Dropout(0.4))

-- Fully Connected Layer 1:
network:add(nn.Linear(120,84))
network:add(nn.ReLU())
--network:add(nn.Dropout(0.4))

-- Output Layer:
network:add(nn.Linear(84,11));
network:add(nn.LogSoftMax());

--print('LeNet-5 \n' .. network:__tostring());

-------------------------------------------------------------------------
-- Define cost function:

criterion = nn.ClassNLLCriterion();

-------------------------------------------------------------------------
-- Define test-train system paramters:

numberOfEpochs = 15;
trainingSize = X_train:size(1)
modelParams,gradParams = network:getParameters()
epochAccuracy_train = torch.DoubleTensor(1,numberOfEpochs);
epochAccuracy_test = torch.DoubleTensor(1,numberOfEpochs);
batchSize = 10;
config = {learningRate = 0.06, momentum = 0.9}

-------------------------------------------------------------------------
-- Testing

-- Compute accuracy on test and save test confusion matrix:
function testData(epochNumber)

	-- Confusion matrix
	local classes = {'0','1','2','3','4','5','6','7','8','9','NC'}
	local confusion_test = optim.ConfusionMatrix(classes);

	-- Compute:
	local testOutput = network:forward(X_test);
	confusion_test:batchAdd(testOutput,y_test);
	confusion_test:updateValids()
	print(string.format('\27[34m Accuracy on Test Set = %.2f, Mean Accuracy Precision = %.2f',100*confusion_test.totalValid,100*confusion_test.averageValid))
	epochAccuracy_test[1][epochNumber] = 100*confusion_test.totalValid
	torch.save('metadata/confusion_test_color'..tostring(epochNumber)..'.t7',confusion_test)
	collectgarbage();
	return
end

-------------------------------------------------------------------------
-- Training

for epochNumber = 1,numberOfEpochs do

	totalLoss = 0;

	-- Confusion matrix
	classes = {'0','1','2','3','4','5','6','7','8','9','NC'}
	confusion_train = optim.ConfusionMatrix(classes)

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
	torch.save('metadata/confusion_train_color_'..tostring(epochNumber)..'.t7',confusion_train)

	-- Compute accuracy on test data
	testData(epochNumber)
	collectgarbage();

end

-------------------------------------------------------------------------
-- Plotting epoch - accuracy results:

epochs = torch.linspace(1,numberOfEpochs,numberOfEpochs)
gnuplot.pdffigure('plots/training_3.pdf')
gnuplot.plot({'Training',epochs,epochAccuracy_train[1]},{'Testing',epochs,epochAccuracy_test[1]})
gnuplot.xlabel('Epochs')
gnuplot.ylabel('Accuracy')
gnuplot.plotflush()

-------------------------------------------------------------------------
-- Save network and accuracy values

network:clearState();
torch.save('models/trainedNet_color.t7', network)
torch.save('metadata/trainAccuracy_color.t7',epochAccuracy_train)
torch.save('metadata/testAccuracy_color.t7',epochAccuracy_test)
collectgarbage();
