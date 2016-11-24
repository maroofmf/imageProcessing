-- Name: Maroof Mohammed Farooq
-- Project 4
--
-------------------------------------------------------------------------
-- Initialize packages

require 'nn'
require 'optim'
require 'image'

-------------------------------------------------------------------------
-- Load Network and data set

trainedNetwork = torch.load('networkOut.t7');
dataSet = torch.load('mnist-p1b-test.t7');
X_data = dataSet.data:double():mul(1/255):clone();
y_data = dataSet.label:double():clone();

-------------------------------------------------------------------------

classes = {'0','1','2','3','4','5','6','7','8','9'};
confusion = optim.ConfusionMatrix(classes);
output = trainedNetwork:forward(X_data);
confusion:batchAdd(output,y_data);
confusion:updateValids()
print(string.format('accuracy = %.2f',100*confusion.totalValid))

