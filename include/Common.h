#ifndef _Common_H
#define _Common_H


namespace common{


  const int N_CHANNEL  = 64; //# channels per FEE64 module (will not change! so hard coded in some parts of code)

  //modules in AIDA setup
  const int N_FEE64 = 24+1; //numbering of AIDA Fee64 modules in raw data begins from 1
  //number of DSSD in detector stack
  const int N_DSSD = 6;
  
  const int ADC_ZERO = 32768; //2**15 


  struct calib_data_struct{

    double time_aida;
    double time_disc;
    double time_external;
    double adc_energy;

    int adc_data;
    int dssd;
    int strip;

    unsigned char adc_range;
    unsigned char side;
    unsigned char module;
    unsigned char channel;
    unsigned char data_type;
    unsigned char info_code;

    bool corr_flag; //if we have a good calculation of offset EXT->AIDA
    bool disc_flag; //if we have good DISC value for ADC hit

  };


  struct analysis_data_struct{
    //    ULong64_t t;
    //ULong64_t t_fast;
    unsigned long long t;
    unsigned long long t_fast;

    double e;
    double e_x;
    double e_y;
    double x;
    double y;
    double z;
    int n;
    int n_x;
    int n_y;
    int n_z;
    unsigned char type;
  };


  /****/
  struct aida_event{
    unsigned long long t;
    unsigned long long t_fast;
    double e;
    double e_x;
    double e_y;
    double x;
    double y;
    double z;
    //  int n;
    //  int nx;
    //  int ny;
    //  int nz;
    unsigned char type;
  };
  /*****/

  /*** FS: for debug **********
  struct aida_event{
    unsigned long long t;
    unsigned long long t_fast;
    int e;
    int e_x;
    int e_y;
    int x;
    int y;
    int z;
    //  int n;
    //  int nx;
    //  int ny;
    //  int nz;
    unsigned char type;
  };
  ***************/
}

#endif
