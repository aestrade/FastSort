#ifndef _Calibrator_H //what is going on here?
#define _Calibrator_H

#include <fstream>
#include <iostream>
#include <string>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH2I.h>
#include "Unpacker.h"
#include "Common.h"

class Calibrator{
 private:

  bool b_debug;

  bool b_root_tree; //do we write data to file
  bool b_histograms;

  bool b_push_data;
  bool b_fill_tree;

  TTree * out_root_tree;

  // ----- correlations in time of different types -----
  unsigned long tm_stp_disc[common::N_FEE64][common::N_CHANNEL]; //latest disc tm stp!
  double tm_stp_corr_offset;

  int hist_fill_count;
  static const int n_update= 10000000;


  //-----------------------------------------------
  //   BEGIN PARAMETERS
  //-----------------------------------------------
  //
  // ++ mask modules & channels ++
  bool b_ch_enabled[common::N_FEE64][common::N_CHANNEL];
  // ++ geometry of stack ++
  int map_dssd[common::N_FEE64]; //module -> DSSD
  int map_side[common::N_FEE64]; //module -> vertical/horizontal strips
  int map_strip[common::N_FEE64]; //module -> ch(0:63)/ch(64:127)
  // ++ ADC energy calibration ++
  int adc_polarity[common::N_FEE64]; //module -> n-/p-side strips 
  double adc_offset[common::N_FEE64][common::N_CHANNEL];
  double adc_gain[common::N_FEE64][common::N_CHANNEL];
  double adc_gain_highE[common::N_FEE64]; //20GeV range: for now use simple calibration.
  int disc_time_window;
  double aida_time_calib;
  //-----------------------------------------------
  //   END PARAMETERS
  //-----------------------------------------------

  //ch enables, ch valid.....
  bool b_enabled; //if c hannel is not masked 
  bool b_valid_ch; //if .... we have set the channels for this data bit?
  bool b_corr_status; //

  // !!!------------------------------------!!!
  //
  // note: if new variables included, they must also be added to definition
  // of output_root_tree Branch in InitCalibrator()
  //
  // !!!-------------------------------------!!!
  common::calib_data_struct cal_data;


 public:

  TCanvas *cCal1;
  TCanvas *cCal2;

  TH2I *hCalEhCh[16];
  TH2I *hCalElCh[16];

  Calibrator();
  ~Calibrator(){};

  void InitCalibrator(int opt, char *file_name); //
  void Process(Unpacker & my_unp_data);
  void Close();
  void LoadParameters(char *file_name);


  void FillHistograms();
  void UpdateHistograms();
  void ResetHistograms();
  bool IsValidChannel(int module, int channel);
  void Update();

  void ResetData();
  void Write();

  //get and set: which ones!
  bool SetGeometry();
  void CalibrateAdc();
  unsigned char OrderChannel( unsigned char ch );

  //Setters...
  void SetBDebug(bool flag);
  void SetBHistograms(bool flag);
  void SetBPushData(bool flag);
  void SetBFillTree(bool flag);
  void SetBRootTree(bool flag);

  void SetBEnabled(bool flag); //forget for now...
  void SetBValidCh(bool flag);
  void SetBCorrStatus(bool flag);

  void SetTmStpDisc(unsigned long value);
  void SetTmStpOffset(int64_t value); //time difference AIDA->EXTERNAL

  //Setters... for cal_data structure
  void SetTimeAida(double value);
  bool SetTimeDisc();
  void SetTimeExternal();
  void SetAdcEnergy(double value);

  void SetAdcData(int value);
  void SetDSSD(int value);
  void SetStrip(int value);

  void SetAdcRange(unsigned char value);
  void SetSide(unsigned char value);
  void SetModule(unsigned char value);
  void SetChannel(unsigned char value);
  void SetDataType(unsigned char value);
  void SetInfoCode(unsigned char value);

  void SetCorrFlag();
  void SetDiscFlag(bool value);


  //Getters...
  bool GetBDebug();
  bool GetBHistograms();
  bool GetBPushData();
  bool GetBFillTree();
  bool GetBRootTree();

  bool GetBEnabled();
  bool GetBValidCh();
  bool GetBCorrStatus();

  unsigned long GetTmStpDisc();
  int64_t GetTmStpOffset();

  //Getters... for cal_data structure
  double GetTimeAida();
  double GetTimeDisc();
  double GetTimeExternal();
  double GetAdcEnergy();

  int GetAdcData();
  int GetDSSD();
  int GetStrip();

  unsigned char GetAdcRange();
  unsigned char GetSide();
  unsigned char GetModule();
  unsigned char GetChannel();
  unsigned char GetDataType();
  unsigned char GetInfoCode();

  bool GetCorrFlag();
  bool GetDiscFlag();

  common::calib_data_struct GetStruct();
};


#endif
