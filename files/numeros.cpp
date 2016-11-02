//
// C++ include.
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>


using namespace std;


const int N_DSSD=24+1; //naming of AIDA modules begins from 1



const int order[64]={62, 63, 59, 60, 61, 56, 57, 58, 52, 53, 54, 55, 49, 50, 51, 45,
		     46, 47, 48, 42, 43, 44, 38, 39, 40, 41, 35, 36, 37, 31, 32, 33,
		     34, 28, 29, 30, 24, 25, 26, 27, 21, 22, 23, 17, 18, 19, 20, 14,
		     15, 16, 10, 11, 12,  7,  3,  0,  8,  4,  1,  9,  5,  2, 13,  6};



/****
18      55      1       0       1       0       0.6103515625    7488
18      58      1       1       0       0       0.6103515625    2018
18      61      1       2       1       -1.1416058608   0.6103515625    1020
18      54      1       3       1       -63.6309856863  0.6103515625    740
18      57      1       4       1       36.2247349324   0.6103515625    578
18      60      1       5       1       -92.3562677173  0.6103515625    787
18      63      1       6       1       76.4847263904   0.6103515625    499
18      53      1       7       1       17.9052395002   0.6103515625    680
18      56      1       8       1       -1072.7171180286        0.6103515625    1091
18      59      1       9       1       59.6214641505   0.6103515625    692
18      50      1       10      1       104.5775168857  0.6103515625    423
18      51      1       11      1       -336.6294880421 0.6103515625    652
18      52      1       12      1       -66.1003710877  0.6103515625    688
18      62      1       13
*******/

const int inv_order[64]={1, 0, 4, 3, 2, 7, 6, 5, 11, 10, 9, 8, 14, 13, 12, 18,
			 17, 16, 15, 21, 20, 19, 25, 24, 23, 22, 28, 27, 26, 32, 31, 30,
			 29, 35, 34, 33, 39, 38, 37, 36, 42, 41, 40, 46, 45, 44, 43, 49,
			 48, 47, 62, 52, 51, 50, 59, 56, 53, 63, 60, 57, 54, 61, 58, 55};




//
// Setup for Nov 2016 BRIKEN test
//
//     A                       (coordinate
//    --------             ^    system used)
//   |        |D           | 
//   |   X    |          y |     X
//   | (beam) |            |   (beam)
//  B|        |            |
//    --------             +----------->
//          C                    x
// DSSSD #   FEE64 #
//
//            A  B  C  D
// 1         17 20 22 23   UPSTREAM
// 2         18 19 21 24
// 3          9 12 14 15
// 4         10 11 13 16
// 5          1  4  6  7
// 6          2  3  5  8   DOWNSTREAM
//
//  assignment of channels in Phong's code?
//  |-------|-------|------|------|
//  0       64     128    192     256 [ch]
//       A      C       B     D
//
// module = 0: not used in NNAIDA


//bool check(int mod, int ch, int s);

int main(){

  cout << endl;
  //  int inv_order[64];
  for(int i=0; i<64; i++){
    //inv_order[i]= 63- order[i];
    cout << "("<<i<<": "<<inv_order[i] <<")  ";
  }
  cout << endl;


  cout << endl;
  //int inv_order[64];
  for(int i=0; i<64; i++){
    //inv_order[i]= 63- order[i];
    cout << ", "<<inv_order[i];
    if((i+1)%16 == 0 ) cout <<endl;
  }
  cout << endl;



  for(int i=0; i<64; i++){
    cout << inv_order[i] << endl;
  }

  cout << endl << endl;

  for(int i=0; i<64; i++){
    cout << inv_order[63-i] << endl;
  }


  for(int i=0; i<64; i++){
    printf("%3i",inv_order[i]);
    //    cout << inv_order[i] << " ";
  }

  cout << endl << endl;

  for(int i=0; i<64; i++){
    printf("%3i",inv_order[63-i]);
    //    cout << inv_order[63-i] << endl;
  }
  cout << endl << endl;


  cout << endl << "\n NEW order array )inverse indexes)"<<endl;
  //int inv_order[64];
  for(int i=0; i<64; i++){
    //inv_order[i]= 63- order[i];
    for(int j=0;j<64; j++){
      if(inv_order[j]==i)  cout << j <<", ";
    }
    if((i+1)%16 == 0 ) cout <<endl;
  }
  cout << endl;


  return 1;
}
