-- Name: Maroof Mohammed Farooq
-- Project 4
-- 
-------------------------------------------------------------------------
-- Initialize packages

require 'nn'
require 'optim'
require 'image'

-------------------------------------------------------------------------
-- Load training and testing DataSet

torch.manualSeed(0);
training_dataSet = torch.load('mnist-p1b-train.t7');
testing_dataSet = torch.load('mnist-p1b-test.t7');
X_train = training_dataSet.data:double():mul(1/255):clone();
y_train = training_dataSet.label:double():clone();
X_test = testing_dataSet.data:double():clone();
y_test = testing_dataSet.data:double():clone();

-------------------------------------------------------------------------
-- Develop the network

network = nn.Sequential();

-- View Layer:
network:add(nn.View(1,32,32))

-- First Layer
network:add(nn.SpatialConvolution(1,6,5,5));
network:add(nn.SpatialMaxPooling(2,2,2,2))
network:add(nn.ReLU());

-- Second Layer
network:add(nn.SpatialConvolution(6,16,5,5));
network:add(nn.SpatialMaxPooling(2,2,2,2))
network:add(nn.ReLU());

-- View Layer
network:add(nn.View(16*5*5))

-- Fully Connected Layer 1:
network:add(nn.Linear(16*5*5,120))
network:add(nn.ReLU())

-- Fully Connected Layer 1:
network:add(nn.Linear(120,84))
network:add(nn.ReLU())

-- Output Layer:
network:add(nn.Linear(84,10));
network:add(nn.LogSoftMax());

print('LeNet-5 \n' .. network:__tostring());

-------------------------------------------------------------------------
-- Define cost function:

criterion = nn.ClassNLLCriterion();

-------------------------------------------------------------------------
-- Training

numberOfEpochs = 30;
trainingSize = X_train:size(1)
modelParams,gradParams = network:getParameters()


for epochNumber = 1,numberOfEpochs do
	batchSize = 10;
	totalLoss = 0;

	-- Confusion matrix
	classes = {'0','1','2','3','4','5','6','7','8','9'}
	confusion = optim.ConfusionMatrix(classes)

	for iteration = 1,trainingSize,batchSize do
		batchSize_new = math.min(trainingSize-iteration+1,batchSize)
		trainingBatch = X_train:narrow(1,iteration,batchSize_new)
		labelBatch = y_train:narrow(1,iteration,batchSize_new)

		local function feval()
			gradParams:zero()
			local networkOutput = network:forward(trainingBatch)
			local errorValue = criterion:forward(networkOutput, labelBatch)
			totalLoss = totalLoss + batchSize_new*errorValue;
			local gradOutput = criterion:backward(networkOutput, labelBatch)
			local gradInput = network:backward(trainingBatch,gradOutput)
			confusion:batchAdd(networkOutput,labelBatch);
			return errorValue,gradParams
		end
		config = {learningRate = 0.06}
		optim.sgd(feval,modelParams, config)
		io.write(string.format("progress: %4d/%4d\r",iteration,trainingSize))
		io.flush()
	end

	totalLoss = totalLoss/ trainingSize;
	confusion:updateValids()
	print(string.format('epoch = %2d/%2d, loss = %.2f, accuracy = %.2f',epochNumber,numberOfEpochs, totalLoss, 100*confusion.totalValid))

end

-------------------------------------------------------------------------
-- Save network

network:clearState();
torch.save('networkOut.t7', network)


