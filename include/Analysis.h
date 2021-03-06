#ifndef _Analysis_H //what is going on here?
#define _Analysis_H

#include <iostream>
#include <TTree.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TH1.h>
#include "Calibrator.h"
#include "DataSource.h"
#include "Common.h"

//FS
#include <queue>

class Analysis{
 private:

  bool b_debug;
  bool b_histograms;

  bool b_root_tree; //do we write data to file
  TTree * out_root_tree;

  // ----- correlations in time of different types -----
  //  static const int n_update= 10000000;

  std::queue<common::calib_data_struct> q;

  //-----------------------------------------------
  //   BEGIN PARAMETERS
  //-----------------------------------------------
  //
  // ----- parameters: Set/(Get?) should be private functions and only performed once when initializing step -----
  bool b_mod_enabled[33]; 
  int geo_detector[33]; // 0:3
  int geo_side[33]; // 0:3
  int geo_strip[33]; // 0:3

  int p_opt_tm_stp;
  double event_time_window;

  int p_N_MAX[2];
  int p_DELTA_MAX;
  int p_N_DET_MAX;
 
  //-----------------------------------------------
  //   END PARAMETERS
  //-----------------------------------------------

  int event_count;
  double t_low_prev;
  double t_high_prev;
  double t_disc_prev;

  bool b_pulser;

  unsigned char event_range;

  // !!!------------------------------------!!!
  //
  // note: if new variables included, they must also be added to definition
  // of output_root_tree Branch in InitCalibrator()
  //
  // !!!-------------------------------------!!!

  /**************** FS *************************
  //FS: this is replaced by struct aida_event
  struct event_struct{

    int64_t e_i[4][2];
    int64_t e_d[4][2];

    int64_t t0;
    int64_t t0_ext;
    int64_t dt;

    int multiplicity;

    int n_side_i[4][2];
    int n_side_d[4][2];

    int n_det_i[4];
    int n_det_d[4];

    int x_i[4];
    int y_i[4];
    int x_d[4];
    int y_d[4];

    int dx_d[4];
    int dy_d[4];

    unsigned char decay_flag;
    unsigned char implant_flag;

  } evt_data;
  *********** FS ******************/



  common::analysis_data_struct hit;
  common::aida_event evt_data;

 public:

  /************
  //-----ADC data singles
  TH2I * hADClowCh[common::N_FEE64];
  TH2I * hADCdiscCh[common::N_FEE64];
  TH2I * hADChighCh[common::N_FEE64];
  TH1I * hCh_ADClow[common::N_FEE64];
  TH1I * hCh_ADChigh[common::N_FEE64];
  TH1I * hCh_ADCdisc[common::N_FEE64];


  //energy sum
  TH1I * hElow[common::N_FEE64];
  TH1I * hEdisc[common::N_FEE64];
  TH1I * hEhigh[common::N_FEE64];



  //----------Monitor performance
  TH1I * hSyncFEE;
  TH1I * hPauseFEE;
  TH1I * hResumeFEE;
  TH1I * hADClowFEE;
  TH1I * hADChighFEE;
  TH1I * hADCdiscFEE;

  //----------- time distributions
  TH1I * hTimeADClow[common::N_DSSD];
  TH1I * hTimeADCdisc[common::N_DSSD];
  TH1I * hTimeADChigh[common::N_DSSD];

  TH1I * hTimeStamp;
  TH1I * hTimeStampExt;
  TH1I * hTimeStampFlag;

  //------------Event building
  TH2I * hEvt_ExEy_if[4];
  TH2I * hEvt_XY_if[4];
  TH1I * hEvt_Eside_if[4][2];

  TH2I * hEvt_ExEy_df[4];
  TH2I * hEvt_XY_df[4];
  TH1I * hEvt_Eside_df[4][2];

  TH2I * hEvt_ExEy_df2[4];
  TH2I * hEvt_XY_df2[4];
  TH1I * hEvt_Eside_df2[4][2];


  TH1I * hEvt_Eside[4][2];
  TH1I * hEvt_Eaida;
  TH1I * hEvt_Eaida_gE;
  TH1I * hEvt_Eaida_gX;

  TH2I * hEvt_ExEy[4];
  TH2I * hEvt_EdE;
  
  TH1I * hEvt_X[4];
  TH1I * hEvt_Y[4];
  TH1I * hEvt_dX[3]; // 1:2, 1:3, 2:3
  TH1I * hEvt_dY[3];

  TH2I * hEvt_XY[4];
  TH2I * hEvt_dXdX;
  TH2I * hEvt_dYdY;

  TH1I * hEvt_Multi[4][2];
  TH2I * hEvt_MultiSide[4];

  TH1I * hEvt_HitsSide;
  TH1I * hEvt_HitsDet;
  TH1I * hEvt_HitsDet_gE;
  TH1I * hEvt_HitsDet_gX;

  TH1I * hEvt_TmStpDist[4];

  //
  // decay events
  //
  TH1I * hEvt_Eside_d[4][2];
  TH2I * hEvt_ExEy_d[4]; //
  TH2I * hEvt_ExEy_sum_d[4]; //

  TH2I * hEvt_EPulser_d;
  
  TH1I * hEvt_X_d[4];
  TH1I * hEvt_Y_d[4];
  TH1I * hEvt_dX_d[4]; // 1:2, 1:3, 2:3
  TH1I * hEvt_dY_d[4];

  TH2I * hEvt_XY_d[4];

  TH1I * hEvt_MultiDet_d; //how many detectors have decay info
  TH2I * hEvt_MultiSide_d[4]; //how many sides have decay info (with implant, det==1)
  TH1I * hEvt_MultiStrip_d[4][2]; //how many strips have decay data ()

  TH2I * hEvt_MultidX_d[4][2]; // dX/dY vs multiplicity (side)

  TH2I * hEvt_MultiID;
  TH1I * hEvt_HitsFlag;



  TCanvas *cADClow[2];
  TCanvas *cADCdisc[2];
  TCanvas *cADChigh[2];
  TCanvas *cEall[2];
  TCanvas *cTimeDist[common::N_DSSD];

  TCanvas *cEvtE1;
  TCanvas *cEvtE2;
  TCanvas *cEvtXY;
  TCanvas *cEvtdXdY;
  TCanvas *cEvtMulti;
  TCanvas *cEvtHits;
  TCanvas *cEvtTS;

  TCanvas *cEvtE_d;
  TCanvas *cEvtXY_d;
  TCanvas *cEvtXY2_d;
  TCanvas *cEvtMulti_d;
  TCanvas *cEvtHit_d;
*****************/

  Analysis();
  ~Analysis(){};



  void InitAnalysis(int opt); //
  void Process(DataSource & my_source, Calibrator & my_cal_data);
  void Close();

  bool BuildEvent(Calibrator & my_cal_data);
  bool CloseEvent();
  void InitEvent(Calibrator & my_cal_data);


  void FillHistogramsSingles(Calibrator & my_cal_data);
  void FillHistogramsEvent();
  void UpdateHistograms();
  void ResetHistograms();

  void ResetEvent();
  void WriteHistograms();

  void WriteOutBuffer(DataSource & my_source);

  bool IsChEnabled(Calibrator & my_cal_data);

  //get and set: which ones!

  bool SetEventTimeWindow(double value);

  void PrintEvent();

  //Setters...
  void SetBDebug(bool flag);
  void SetBHistograms(bool flag);
  void SetBPushData(bool flag);
  void SetBFillTree(bool flag);
  void SetBRootTree(bool flag);


  //Getters...
  double GetEventTimeWindow();
  inline bool GetBDebug() { return b_debug; };
  bool GetBHistograms();
  bool GetBPushData();
  bool GetBFillTree();
  bool GetBRootTree();
  int GetMultiplicity();

};


#endif
