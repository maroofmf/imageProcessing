-- This program provides you an example of how to use torch.
-- It is not yet a complete machine learning program.
-- For example, you need to evaluate on the testing dataset
-- to know the true performance of your network when applied
-- to unseen data. You also need to write code to compute 
-- the classification accuracy.
    
-- Training on the full dataset costs a pretty long time.
-- Make sure your program works well everywhere before make such training.
-- Use small amount of samples for debugging.
-- You should also print some information to get the statistics.

----------------------------------------------


-- 10/28/2016 Created By Zhehang Ding

require "nn"
require "optim"
require "image"

function main()
  -- Fix the random seed for debugging.
  torch.manualSeed(0)
  
  -- Let us create some 8x8 gray-level images.
  -- Each image belongs to one of two patterns: "x" or "+".
  -- Each image has its pattern on a noisy background.
  -- You don't have to understand this part of code.
  -- In your homework you just load the MNIST data and do preprocessing.
  -- ( dataset = torch.load('XXX.t7') )
  do
  local N = 100
  -- In this example, only training set is created.
  -- In your homework, there are training and testing datasets.
  -- You should use the training dataset to train the network,
  -- and use testing dataset to see the true performance of it.
  local perm = torch.randperm(N):long()
  local pat1=torch.Tensor({{{{1,0,0,1},{0,1,1,0},{0,1,1,0},{1,0,0,1}}}}):expand(N/2,1,4,4);
  local pat2=torch.Tensor({{{{0,1,0,1},{0,1,0,0},{1,1,1,1},{0,1,0,0}}}}):expand(N/2,1,4,4);
  local pattern = torch.cat(pat1,pat2,1):index(1,perm)
  trainset = {}
  trainset.data  = torch.randn(N,1,4,4) * 0.15 + pattern
  trainset.label = torch.cat(torch.zeros(N/2),torch.ones(N/2)):index(1,perm)+1
  -- Preview the dataset if you want.
  --image.save('patterns.png',image.toDisplayTensor(trainset.data:narrow(1,1,20),1,10))
  end

  torch.save('dataset.t7',trainset) 

  -- Create the network and the criterion.
  -- This is just an example which contains all layers you will be using.
  -- However, the architecture and the parameters we should use are different.
  print('Creating the network and the criterion...')
  network   = nn.Sequential()
  criterion = nn.ClassNLLCriterion()

  -- A view layer so the network recognize a batch.
  -- View layers do not perform any computation.
  -- They tell the network how to look at the data.
  network:add(nn.View(1,4,4):setNumInputDims(3))
  -- A 5x5 conv Layer with 3 kernels.
  network:add(nn.SpatialConvolution(1,3,2,2,1,1))
  -- A ReLU activation layer.
  network:add(nn.ReLU())
  -- A maxpooling layer.
  network:add(nn.SpatialMaxPooling(2,2,2,2))
  -- A view layer that convert 2D images to 1D vectors.
  -- Also we tell it to recognize a batch.
  network:add(nn.View(-1):setNumInputDims(3))
  -- A linear transformation layer.
  network:add(nn.Linear(1*1*3,2))
  -- A log-softmax layer produces the log probabilities.
  network:add(nn.LogSoftMax())
  
  -- Extract the parameters and arrange them linearly in memory.
  -- So we have a large vector containing all the parameters.
  parameters,gradParameters = network:getParameters()
  
  --
  print('Training...')
  local nEpoch = 20
  for e = 1,nEpoch do
    -- Number of training samples.
    local size  = trainset.data:size()[1]
    -- Batch size. We use a batch of samples to "smooth" the gradients.
    local bsize = 10
    -- Total loss.
    local tloss = 0

    -- Confusion matrix. This is a helpful tool.
    local classes = {'x','+'}
    local confusion = optim.ConfusionMatrix(classes)
    
    for t  = 1,size,10 do
      local bsize = math.min(bsize,size-t+1)
      local input  = trainset.data:narrow(1,t,bsize)
      local target = trainset.label:narrow(1,t,bsize)
      -- Reset the gradients to zero.
      gradParameters:zero()
      -- Forward the data and compute the loss.
      local output = network:forward(input)
      local loss   = criterion:forward(output,target)
      -- Collect Statistics
      tloss = tloss + loss * bsize
      confusion:batchAdd(output,target)
      -- Backward. The gradient wrt the parameters are internally computed.
      local gradOutput = criterion:backward(output,target)
      local gradInput  = network:backward(input,gradOutput)
      -- The optim module accepts a function for evaluation.
      -- For simplicity I made the computation outside, and
      -- this function is used only to return the result.
      local function feval()
        return loss,gradParameters
      end
      -- Specify the training parameters.
      local config = {
        learningRate = 0.06,
      }
      -- We use the SGD method.
      optim.sgd(feval, parameters, config)
      -- Show the progress.
      io.write(string.format("progress: %4d/%4d\r",t,size))
      io.flush()
    end
    -- Compute the average loss.
    tloss = tloss / size
    -- Update the confusion matrix.
    confusion:updateValids()
    -- Let us print the loss and the accuracy.
    -- You should see the loss decreases and the accuracy increases as the training progresses.
    print(string.format('epoch = %2d/%2d  loss = %.2f accuracy = %.2f',e,nEpoch,tloss,100*confusion.totalValid))
    -- You can print the confusion matrix if you want.
    -- print(confusion)
  end
  
  -- Clean temporary data to reduce the size of the network file.
  network:clearState()
  -- Save the network.
  torch.save('output.t7',network)
end

main()

