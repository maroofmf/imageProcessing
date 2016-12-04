-- Name: Maroof Mohammed Farooq
-- Project 4
-- Train CNN
-------------------------------------------------------------------------
-- Initialize packages

require 'nn'
require 'optim'
require 'image'
require 'gnuplot'

-------------------------------------------------------------------------
-- Load training and testing DataSet

torch.manualSeed(0);
training_dataSet = torch.load('mnist-p1b-train.t7');
testing_dataSet = torch.load('mnist-p1b-test.t7');
X_train = training_dataSet.data:double():mul(1/255);
y_train = training_dataSet.label:double():clone();
X_test = testing_dataSet.data:double():mul(1/255);
y_test = testing_dataSet.label:double():clone();

-------------------------------------------------------------------------
-- Develop the network

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

--print('LeNet-5 \n' .. network:__tostring());

-------------------------------------------------------------------------
-- Define cost function:

criterion = nn.ClassNLLCriterion();

-------------------------------------------------------------------------
-- Network Initialization: From : https://github.com/e-lab/torch-toolbox/blob/master/Weight-init/weight-init.lua
-- Courtesy: jhjin, hohoCode

-- "Efficient backprop"
-- Yann Lecun, 1998
local function w_init_heuristic(fan_in, fan_out)
	return math.sqrt(1/(3*fan_in))
end


-- "Understanding the difficulty of training deep feedforward neural networks"
-- Xavier Glorot, 2010
local function w_init_xavier(fan_in, fan_out)
	return math.sqrt(2/(fan_in + fan_out))
end


-- "Understanding the difficulty of training deep feedforward neural networks"
-- Xavier Glorot, 2010
local function w_init_xavier_caffe(fan_in, fan_out)
	return math.sqrt(1/fan_in)
end


-- "Delving Deep into Rectifiers: Surpassing Human-Level Performance on ImageNet Classification"
-- Kaiming He, 2015
local function w_init_kaiming(fan_in, fan_out)
	return math.sqrt(4/(fan_in + fan_out))
end


local function w_init(net, arg)
	-- choose initialization method
	local method = nil
	if     arg == 'heuristic'    then method = w_init_heuristic
	elseif arg == 'xavier'       then method = w_init_xavier
	elseif arg == 'xavier_caffe' then method = w_init_xavier_caffe
	elseif arg == 'kaiming'      then method = w_init_kaiming
	else
		assert(false)
	end

	-- loop over all convolutional modules
	for i = 1, #net.modules do
		local m = net.modules[i]
		if m.__typename == 'nn.SpatialConvolution' then
			m:reset(method(m.nInputPlane*m.kH*m.kW, m.nOutputPlane*m.kH*m.kW))
		elseif m.__typename == 'nn.SpatialConvolutionMM' then
			m:reset(method(m.nInputPlane*m.kH*m.kW, m.nOutputPlane*m.kH*m.kW))
		elseif m.__typename == 'cudnn.SpatialConvolution' then
			m:reset(method(m.nInputPlane*m.kH*m.kW, m.nOutputPlane*m.kH*m.kW))
		elseif m.__typename == 'nn.LateralConvolution' then
			m:reset(method(m.nInputPlane*1*1, m.nOutputPlane*1*1))
		elseif m.__typename == 'nn.VerticalConvolution' then
			m:reset(method(1*m.kH*m.kW, 1*m.kH*m.kW))
		elseif m.__typename == 'nn.HorizontalConvolution' then
			m:reset(method(1*m.kH*m.kW, 1*m.kH*m.kW))
		elseif m.__typename == 'nn.Linear' then
			m:reset(method(m.weight:size(2), m.weight:size(1)))
		elseif m.__typename == 'nn.TemporalConvolution' then
			m:reset(method(m.weight:size(2), m.weight:size(1)))
		end

		if m.bias then
			m.bias:zero()
		end
	end
	return net
end

network = w_init(network,'xavier')
-------------------------------------------------------------------------
-- Define test-train system paramters:

numberOfEpochs = 7;
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
	local classes = {'0','1','2','3','4','5','6','7','8','9'}
	local confusion_test = optim.ConfusionMatrix(classes);

	-- Compute:
	local testOutput = network:forward(X_test);
	confusion_test:batchAdd(testOutput,y_test);
	confusion_test:updateValids()
	print(string.format('\27[34m Accuracy on Test Set = %.2f, Mean Accuracy Precision = %.2f',100*confusion_test.totalValid,100*confusion_test.averageValid))
	epochAccuracy_test[1][epochNumber] = 100*confusion_test.totalValid
	torch.save('metadata/confusion_test_'..tostring(epochNumber)..'.t7',confusion_test)
	collectgarbage();
	return
end

-------------------------------------------------------------------------
-- Training

for epochNumber = 1,numberOfEpochs do

	totalLoss = 0;

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
	torch.save('metadata/confusion_train_'..tostring(epochNumber)..'.t7',confusion_train)

	-- Compute accuracy on test data
	testData(epochNumber)

end

-------------------------------------------------------------------------
-- Plotting epoch - accuracy results:

epochs = torch.linspace(1,numberOfEpochs,numberOfEpochs)
gnuplot.pdffigure('plots/training_1.pdf')
gnuplot.plot({'Training',epochs,epochAccuracy_train[1]},{'Testing',epochs,epochAccuracy_test[1]})
gnuplot.xlabel('Epochs')
gnuplot.ylabel('Accuracy')
gnuplot.plotflush()

-------------------------------------------------------------------------
-- Save network and accuracy values

network:clearState();
torch.save('models/trainedNet_1.t7', network)
torch.save('metadata/trainAccuracy.t7',epochAccuracy_train)
torch.save('metadata/testAccuracy.t7',epochAccuracy_test)
collectgarbage();
