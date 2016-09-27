//
// Created by Maroof Mohammed Farooq on 9/26/16.
//

#ifndef CPP_MATRIX_H
#define CPP_MATRIX_H

using namespace std;

template <class indexDataType, class valueDataType>
class matrix{

public:
    matrix(indexDataType width1, indexDataType height1, indexDataType depth1);
    ~matrix(void);

    void setMatrixValues(vector<valueDataType> inputValues);
    void setMatrixValues(imageData inputImage,indexDataType startRow,indexDataType startColumn, indexDataType startDepth, indexDataType totalDepth);
    void setMatrixValues(valueDataType matValue, indexDataType row, indexDataType column, indexDataType depth);
    void setMatrixByValues(indexDataType numberOfArguments,...);
    valueDataType getMatrixValues(indexDataType row, indexDataType column, indexDataType depth);
    indexDataType getWidth();
    indexDataType getHeight();
    indexDataType getDepth();
    void multiplyEachValueBy(valueDataType value);

    valueDataType accessMatrixValue(indexDataType row,indexDataType column,indexDataType depth);
    void printMatrix();

private:
    indexDataType matWidth;
    indexDataType matHeight;
    indexDataType matDepth;
    vector<valueDataType> matData;
};

#endif //CPP_MATRIX_H
