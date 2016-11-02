// AE: Oct 30, 2016
//    This code has a script to convert a table of AIDA calibration parameters to the format used by FastSport analysis code
//    The original table is provided by code developed by Vi Phong (@RIKEN)
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


bool check(int mod, int ch, int s);

int main(){


  ifstream inFile("all_cal_thr_newmapping8.txt", ifstream::in);
  ofstream outFile("param_table_FS.txt", ofstream::out);



  int dssd;
  int strip;
  int module;
  int ch;
  int enable;
  double offset;
  double gain;
  double threshold;

  double p_gain[N_DSSD][64]={{0}};
  double p_offset[N_DSSD][64]={{0}};
  int p_enabled[N_DSSD][64];
  double p_threshold[N_DSSD][64]={{0}};
  int p_map_dssd[N_DSSD];
  int p_map_side[N_DSSD];
  int p_map_strip[N_DSSD];

  double my_gain= 0.61035156;

  bool b_read[N_DSSD][64]={{0}};

  for(int i=0;i<N_DSSD;i++){
    for(int j=0;j<64;j++){
      p_enabled[i][j]=1;
    }
  }

  //Format of table in input text file
  // FEE64 channel DSSD strip channel_enable offset[KeV/ch] gain[kev/ch] threshold

  int ii=0;
  do{

    ii++;

    module= ch= strip= -1;

    inFile >> module >> ch >> dssd >> strip >> enable >> offset >> gain >> threshold;

    if(check(module, ch, strip)){

      b_read[module][ch]= true; //we god new data for this channel
      
      p_enabled[module][ch]= enable;
      p_threshold[module][ch]= threshold;
      p_offset[module][ch]= offset/my_gain; //FastSort code uses offset in channels
      //only once...
      if(ch==0){
	p_map_dssd[module]= dssd;
	if(strip<128 && strip>=0){
	  p_map_side[module]= 1; //vertical strips, p-side
	  if(strip<64) p_map_strip[module]=1;
	  else p_map_strip[module]=2;
	}
	else if(strip>127 && strip<256){
	  p_map_side[module]= 0;
	  if(strip< 128+64) p_map_strip[module]= 1;
	  else p_map_strip[module]=2;
	}
	else{
	  cout << "\n Do not recognize strip value: " << strip << endl;
	}
      }

    }
    else{
      cout << "WRONG INPUT mod, channel, strip= " << module << " " << ch << " " << strip << endl;

     
    }

  }while(!inFile.eof());
    
  cout << "\n\n READ " << ii-1 << "+1 LINES from TABLE\n"<<endl;
  
  outFile <<"#map FEE64 -> DSSD" << endl;
  for(int i=1; i<N_DSSD; i++){
    if(b_read[i][0]) outFile << "map_dssd  "<< i << "  " << p_map_dssd[i] << endl;
  }

  outFile <<"\n#map FEE64 -> DSSD side" << endl;
  for(int i=1; i<N_DSSD; i++){
    if(b_read[i][0]) outFile << "map_side  "<< i << "  " << p_map_side[i] << endl;
  }

  outFile <<"\n#map FEE64 -> DSSD strips (group of 64 strips)" << endl;
  for(int i=1; i<N_DSSD; i++){
    if(b_read[i][0]) outFile << "map_strip  "<< i << "  " << p_map_strip[i] << endl;
  }




  outFile <<"\n#ADC offsets [ch]" << endl;
  for(int i=1; i<N_DSSD; i++){
    for(int j=0; j<64; j++){
      if(b_read[i][j]) outFile << "adc_offset  "<< i << "  " << j << "  " << p_offset[i][j] << endl;
    }
  }

  outFile <<"\n#ADC threshold [keV]" << endl;
  for(int i=1; i<N_DSSD; i++){
    for(int j=0; j<64; j++){
      if(b_read[i][j]) outFile << "energy_threshold  "<<i << "  " << j << "  " << p_threshold[i][j] << endl;
    }
  }

  outFile <<"#ADC channels dissabled (1: enable, 0: dissable)" << endl;
  for(int i=1; i<N_DSSD; i++){
    for(int j=0; j<64; j++){
      //only print dissabled channels
      if(b_read[i][j] && p_enabled[i][j]==0) outFile << "b_ch_enabled  "<<i << "  " << j << "  " << p_enabled[i][j] << endl;
    }
  }
  outFile << "\n#END READ DATA FROM TABLE"<<endl;
  outFile.close();
  inFile.close();


  return 0;
}

bool check(int mod, int ch, int s){


  if(mod<0) return false;
  if(mod>=N_DSSD) return false; //1->24
  if(ch<0) return false;
  if(ch>63) return false;
  if(s<0) return false;
  if(s>255) return false;

  return true;
}
