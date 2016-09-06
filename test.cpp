#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
//using namespace vector;
using namespace std;

int arraym(int*** param){
    cout<< param[0][0][0]<<endl;
    return 0;
}


int main(){

//    int a[2][2] ={{1,2},{3,4}};
//    int* b = arraym(a);
//    cout<< b[0]<<endl;

//    int rows = 3;
//    int columns = 3;
//    int depth = 3;
//    vector<vector<vector<int> > > myVect(rows, vector<vector<int> >(columns, vector<int>(depth)));
//
//    cout<< myVect[0][3][1] << endl;
//    cout << "0. size: " << myints.size() << '\n';
//
//    for (int i=0; i<10; i++) myints.push_back(i);
//    cout << "1. size: " << myints.size() << '\n';
//
//    myints.insert (myints.end(),10,100);
//    cout << "2. size: " << myints.size() << '\n';
//
//    myints.pop_back();
//    cout << "3. size: " << myints.size() << '\n';



    int a[5][5][2];
    int b[5][5][2];
    for(int i=0;i<5;i++){
        for(int j=0;j<5;j++){
            for(int z=0;z!=2;z++){
                a[i][j][z] = i+j+z;
            }
        }
    }

    for(int i = 0;i!=3;i++){
        cout << i << endl;
    }

    for(int i=0;i<5;i++) {
        for (int j = 0; j < 5; j++) {
            for (int z = 0; z < 2; z++) {
                b[i][j][z] = a[i + 1][j + 1][z + 1];
            }
        }
    }


//    arraym();
    cout << a << endl;
    return 0;
}