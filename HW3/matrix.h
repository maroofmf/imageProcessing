//
// Created by Maroof Mohammed Farooq on 9/26/16.
//

#ifndef CPP_MATRIX_H
#define CPP_MATRIX_H

using namespace std;

template <class indexDataType, class valueDataType>
class matrix{

public:
    matrix();
    matrix(indexDataType width1, indexDataType height1, indexDataType depth1);
    ~matrix(void);


    void setMatrixDimentions(indexDataType height1, indexDataType width1, indexDataType depth1);
    void setMatrixValues(vector<valueDataType> inputValues);
    void setMatrixValues(imageData inputImage,indexDataType startRow,indexDataType startColumn, indexDataType startDepth, indexDataType totalDepth);
    void setMatrixValues(valueDataType matValue, indexDataType row, indexDataType column, indexDataType depth);
    void setMatrixByValues(indexDataType numberOfArguments,...);
    valueDataType getMatrixValues(indexDataType row, indexDataType column, indexDataType depth);
    vector<valueDataType> getMatrixValues();

    indexDataType getWidth();
    indexDataType getHeight();
    indexDataType getDepth();

    matrix<indexDataType,valueDataType> extendMatrix(indexDataType extendBy);
    double trace(matrix<indexDataType,valueDataType> inputMatrix);
    double determinant(matrix<indexDataType,valueDataType> inputMatrix);
    matrix<indexDataType,valueDataType> cofactorMatrix(matrix<indexDataType,valueDataType> inputMatrix,indexDataType row, indexDataType column);
    matrix<indexDataType,valueDataType> adjoint(matrix<indexDataType,valueDataType> inputMatrix);
    matrix<indexDataType,valueDataType> pseudoInverse();
    matrix<indexDataType,valueDataType> transpose();
    matrix<indexDataType,valueDataType> multiplyWith(matrix<indexDataType,valueDataType> otherMatrix);

    bool operator==(matrix<indexDataType,valueDataType> otherMatrix);
    bool operator!=(matrix<indexDataType,valueDataType> otherMatrix);
    matrix<indexDataType,valueDataType> operator!();
    matrix<indexDataType,valueDataType> operator|(matrix<indexDataType,valueDataType> otherMatrix);

    int countNumberOfNonZero();
    void multiplyEachValueBy(valueDataType value);
    void zeroPad(indexDataType padBy);
    void removeZeroPadding(indexDataType paddedBy);
    valueDataType accessMatrixValue(indexDataType row,indexDataType column,indexDataType depth);
    void printMatrix(string name = "matrixData");

    template <class toIndexDT, class toValueDT>
    matrix<toIndexDT,toValueDT> converter()
    {
        // Initialize variables
        vector<toValueDT> outputVector;
        matrix<toIndexDT,toValueDT> outputMatrix(matHeight,matWidth,matDepth);

        // Type Cast
        for_each(matData.begin(),matData.end(), [&](valueDataType matValue){
            outputVector.push_back((toValueDT)matValue);
        });

        // Set values of outputMatrix
        outputMatrix.setMatrixValues(outputVector);
        return outputMatrix;
    }


private:
    indexDataType matWidth;
    indexDataType matHeight;
    indexDataType matDepth;
    vector<valueDataType> matData;
};

#endif //CPP_MATRIX_H
