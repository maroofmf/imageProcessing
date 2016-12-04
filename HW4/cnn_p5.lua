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


