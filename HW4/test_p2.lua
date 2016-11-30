-- Name: Maroof Mohammed Farooq
-- Project 4
-- CNN testing
-------------------------------------------------------------------------
-- Initialize packages

require 'nn'
require 'optim'
require 'image'
require 'gnuplot'

-------------------------------------------------------------------------
-- Load Network and data set

trainedNetwork = torch.load('models/trainedNet_1.t7');
dataSet = torch.load('mnist-p1b-test.t7');
X_data = dataSet.data:double():mul(-1/255):add(1):clone();
y_data = dataSet.label:double():clone();

-------------------------------------------------------------------------
-- Predicting labels

classes = {'0','1','2','3','4','5','6','7','8','9'};
confusion = optim.ConfusionMatrix(classes);
output = trainedNetwork:forward(X_data);
confusion:batchAdd(output,y_data);
confusion:updateValids()
print(string.format('\27[34m Accuracy on negative test samples = %.2f',100*confusion.totalValid))



