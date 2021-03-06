#include "Calibrator.h"
#include <bitset>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>


/**************** FS ************
const unsigned char order[64]={62, 63, 59, 60, 61, 56, 57, 58, 52, 53, 54, 55, 49, 50, 51, 45,
			       46, 47, 48, 42, 43, 44, 38, 39, 40, 41, 35, 36, 37, 31, 32, 33,
			       34, 28, 29, 30, 24, 25, 26, 27, 21, 22, 23, 17, 18, 19, 20, 14,
			       15, 16, 10, 11, 12,  7,  3,  0,  8,  4,  1,  9,  5,  2, 13,  6};

const unsigned char order[64]={1, 0, 4, 3, 2, 7, 6, 5, 11, 10, 9, 8, 14, 13, 12, 18,
			       17, 16, 15, 21, 20, 19, 25, 24, 23, 22, 28, 27, 26, 32, 31, 30,
			       29, 35, 34, 33, 39, 38, 37, 36, 42, 41, 40, 46, 45, 44, 43, 49,
			       48, 47, 62, 52, 51, 50, 59, 56, 53, 63, 60, 57, 54, 61, 58, 55};
********************/


// NEW order array (inverse indexes)
const unsigned char order[64]={1, 0, 4, 3, 2, 7, 6, 5, 11, 10, 9, 8, 14, 13, 12, 18, 
			       17, 16, 15, 21, 20, 19, 25, 24, 23, 22, 28, 27, 26, 32, 31, 30, 
			       29, 35, 34, 33, 39, 38, 37, 36, 42, 41, 40, 46, 45, 44, 43, 49, 
			       48, 47, 53, 52, 51, 56, 60, 63, 55, 59, 62, 54, 58, 61, 50, 57}; 




void Calibrator::Close(){
  //close root file if opened
  Write();
}

//the heart of the matter
void Calibrator::Process(Unpacker & my_unp_data){

  ResetData(); //set values of unp_data to defaults (zero)

  //channel will be set to default values for some bits of data where they don't apply (SYNC pulse)
  SetModule( my_unp_data.GetFee64Id() );
  //FS: meaningless for disc!
  SetChannel( my_unp_data.GetChId() );

  //FS: already checked in unpacker  if( IsValidChannel(GetModule(), GetChannel() )){ //should already be checked if valid in Unpacker?
    
  //FS in unpacker SetBValidCh( true ); //not used! can be commented out ...
  //FS: in unpacker  SetBEnabled( b_ch_enabled[GetModule()][GetChannel()] );
     
  //FS: done already else return; //now ensure it will be valid, no need to check again in each step....?

  SetDataType( my_unp_data.GetDataType() );


  //------------------------------------------------------------------
  //
  //   if INFO DATA: should only be DISC. and CORR SCALARS
  //
  //-----------------------------------------------------------------
  if(GetDataType()==2){

    if(my_unp_data.GetInfoCode()==6){ //FAST DISCRIMINATOR hit
      
      SetTmStpDisc(my_unp_data.GetTmStp()); 

    }

    //if CORRELATION SCALER 
    else if(my_unp_data.GetInfoCode()==8){

      //FS: any module
      //if(  GetModule()==13 ||GetModule()==14 ||GetModule()==16  ){

      int64_t offset=  4*my_unp_data.GetCorrScaler() - my_unp_data.GetTmStp(); //AIDA= 100MHz Corr scaler= 25MHz

      SetBCorrStatus(true);
      SetTmStpOffset(offset);

      if(b_debug){
	std::cout << " OFFFFFFFFFFFFFFFFFFFFF: " << offset<<std::endl;
	std::cout << "MODULE:"<<int(my_unp_data.GetFee64Id())<<"::  TS(aida), CORR: " << my_unp_data.GetTmStp() << ",  "<< my_unp_data.GetCorrScaler(); //<<",  "<< my_unp_data.GetCorrScaler()<<std::endl;
	printf("  INFO 0x%lX \n", my_unp_data.GetInfoField());
      }
      //     } //check module 13, 14 or 16

      SetInfoCode(my_unp_data.GetInfoCode());
      //FS: ony adc data makes it forward... SetBPushData(true); //do we want to send data from corr scaler?
      // SetBFillTree(true);
    }
  }


  //------------------------------------------------------------------
  //
  //   if ADC DATA: 
  //-----------------------------------------------------------------
  if(GetDataType()==3){

    SetAdcData(my_unp_data.GetAdcData());
    SetAdcRange(my_unp_data.GetAdcRange()) ;

    //FS    if(SetGeometry()){ //calculates DSSD, side, strip, ...
    SetGeometry();

    //time discriminator set after calculation of time-stamp
    //this should be: if above threshold! In CalibrateAdc()
    //FS: in Unpacker if( GetBEnabled() ) SetBPushData(true); //if not a dissabled chanel (e.g. noisy ch)
    //FS: only push adc_data above software threshold
    if(CalibrateAdc()) SetBPushData(true); 
    // SetBFillTree(true);
    //FS }
  }


  //if we're interested in this event
  if(GetBPushData() || GetBRootTree() ){

    //timing only if it makes sense...
    SetTimeAida(my_unp_data.GetTmStp());

    if(GetDataType()==3) SetDiscFlag(SetTimeDisc()); // check if thre is a fast disc. value for this ADC hit

    SetCorrFlag();
    SetTimeExternal();

	/******* FS *************
    if(b_debug){
      if(GetDataType()==2 && (my_unp_data.GetInfoCode()==8 || my_unp_data.GetInfoCode()==4 )){
	if(GetModule()==13 ||GetModule()==14 ||GetModule()==16 ){
	  std::cout << " TIME AIDA: "<< GetTimeAida() << "  TIME EXT: "<< GetTimeExternal() 
		    <<  "   offset: "<<tm_stp_corr_offset <<std::endl;
	}
      }
    }
	****************************/
  }


  if( GetBRootTree() && GetBFillTree()){
    
    //save this entry to TTree
    out_root_tree->Fill();
  }

  /************* fs *********** 
  if(GetBHistograms()){
    //Fill histograms here
    //Do we need any of the time-stamp variables calculated only for GetBPushData() or GetBFillTree()?
    FillHistograms(); 
  } 
 ********************************/

}


void Calibrator::FillHistograms(){
  //fee 1 to 16:
  if(GetModule()>0 && GetModule()<17){

    if(GetAdcRange()==0){ //decay
      hCalElCh[GetModule()-1]->Fill(GetChannel(),GetAdcData());
    }
    else if(GetAdcRange()==1){ //decay
      hCalEhCh[GetModule()-1]->Fill(GetChannel(),GetAdcData());
    }

    hist_fill_count++;
    if( ( hist_fill_count%n_update)==0 ) UpdateHistograms();
  }
}

void Calibrator::UpdateHistograms(){
  if(GetBHistograms()){
    
    for(int i=0;i<16;i++){
      cCal1->cd(i+1); hCalEhCh[i]->Draw("colz");
      cCal2->cd(i+1); hCalElCh[i]->Draw("colz");
    }
    cCal1->Update();
    cCal2->Update();
    if(GetBDebug()){
      std::cout << "   Calibrator::UpdateHistograms() at cnt " << hist_fill_count << std::endl;
    }

  }
}

void Calibrator::ResetHistograms(){
  if(GetBHistograms()){
    std::cout << ".... Calibrator:: and here we reset histograms; not implemented yet"<<std::endl;
  }
}

bool Calibrator::IsValidChannel(int module, int channel){
  if(module >=0 && module < common::N_FEE64){
    if(channel >=0 && channel < common::N_CHANNEL){
      return true; //common:n_fee64?
    }
  }

  return false;
}


void Calibrator::InitCalibrator(int opt, char *file_name){

  //file name-> to load parameters


  // if first of 'opt' is 1, enable histogramming
  if( (opt & 0x01) == 1){

    SetBHistograms(true);
    
    cCal1= new TCanvas("cCal1","cCal1",50,50,900,900); cCal1->Divide(4,4);
    cCal2= new TCanvas("cCal2","cCal2",90,90,900,900); cCal2->Divide(4,4);
    
    char hname[256];
    char htitle[256];
    for(int i=0;i<16;i++){
      sprintf(hname,"hCalEhCh%i",i+1);
      sprintf(htitle,"NNAIDA%i [20GeV range];ch;ADC data",i+1);
      hCalEhCh[i]= new TH2I(hname, htitle, 64, 0, 64, 512, 0, 65536);

      cCal1->cd(i+1); hCalEhCh[i]->Draw("colz");  //here, or specific Draw function?

      sprintf(hname,"hCalElCh%i",i+1);
      sprintf(htitle,"NNAIDA%i [20MeV range];ch;ADC data",i+1);
      hCalElCh[i]= new TH2I(hname, htitle, 64, 0, 64, 512, 0, 65536);

      cCal2->cd(i+1); hCalElCh[i]->Draw("colz");  //here, or specific Draw function?


    }
  
    if(b_debug) std::cout << "db    Calibrator.cpp: Initialized histograms and canvas for this step"<<std::endl;
  }

  //if second bit of 'opt' is 1, enable TTree for output
  if( ( (opt & 0x02) >> 1)  == 1){

    std::cout << " ***     Calibrator::InitCalibrator(): initializing TTree" << std::endl;
    //Initialize TTree
    out_root_tree = new TTree("AIDA_calibrate","AIDA_calibrate");
    out_root_tree->Branch("entry_calibrate",&cal_data,"time_aida/D:time_disc/D:time_external/D:adc_energy/D:adc_data/I:dssd/I:strip/I:adc_range/b:side/b:module/b:channel/b:data_type/b:info_code/b:corr_flag/O:disc_flag/O");
                                
    out_root_tree->SetAutoSave(-500000000);
    out_root_tree->SetAutoFlush(-50000000);


    std::cout << "  !!!!! Calibrator:: TTrree->GetAutoSave(): "<< out_root_tree->GetAutoSave() <<" !!! "<<std::endl;

    SetBRootTree(true);
  }

  LoadParameters(file_name);

  ResetData(); //sets values of unp_data structure to default values

}


void Calibrator::LoadParameters(char * file_name){

  //char* file_name= (char*)"test.dat";

  std::ifstream param_file;

  param_file.open(file_name,std::ios::in); //need to specify as text?


  const int MAX_LINES= 40000; //4176 number of parameters used so far...
  int line_count=0;
  int par_count=0;

  char line[1024]; // string or char array?
  std::string my_line;
  char ch_first;
  std::string par_name;
  int module, channel;
  double data;
  double x, y, z;
  //  bool b_data;

  if(param_file.is_open()){

    for(;;){

      if(std::getline(param_file, my_line)){

	if(GetBDebug()) std::cout << " * LoadParam() - line: " << my_line << std::endl;

	//skip empty lines and comments
	if(!my_line.empty() && '\n' != my_line[0] && '#' != my_line[0]){
	
	  std::istringstream iss(my_line);  
	  y = -1; //?? o a valid number
	  x = -1;
	  z= -1;
	  // par_name[0]= '\0';

	  iss >> par_name >> x >> y >> z;


	  if(GetBDebug()) std::cout << " * LoadParameter() - values read: " << par_name << " "<< x << y << z << std::endl;

	  if(par_name=="map_dssd"){
	    module= x; data= y;
	    if(IsValidChannel(module, 0)){
	      par_count++;
	      map_dssd[module]= int(data);
	    }
	  }
	  else if(par_name=="map_side"){
	    module= x; data= y;
	    if(IsValidChannel(module, 0)){
	      par_count++;
	      map_side[module]= int(data);
	      //FS: also get the polarity form the side
	      // side= 0 => polarity= 1
	      // side= 1 => polarity= -1
	      if(map_side[module]==0) adc_polarity[module]= 1;
	      else if(map_side[module]==1) adc_polarity[module]= -1;
	    }
	  }
	  else if(par_name=="map_strip"){
	    module= x; data= y;
	    if(IsValidChannel(module, 0)){
	      par_count++;
	      map_strip[module]= int(data);
	    }
	  }
	  else if(par_name=="adc_polarity"){
	    module= x; data= y;
	    if(IsValidChannel(module, 0)){
	      par_count++;
	      adc_polarity[module]= int(data);
	    }
	  }
	  else if(par_name=="adc_gain_highE"){
	    module= x; data= y;
	    if(IsValidChannel(module, 0)){
	      par_count++;
	      adc_gain_highE[module]= data;
	    }
	  }
	  /********* FS: b_ch_enabled moved to Unpacker *******
	  else if(par_name=="b_ch_enabled"){
	    module= x; channel= y;
	    data= z;
	    if(IsValidChannel(module, channel)){
	      par_count++;
	      b_ch_enabled[module][channel]= int( data );
	    }
	  }
	  *******************************************/
	  else if(par_name=="adc_offset"){
	    module= x; channel= y;
	    data= z;
	    if(IsValidChannel(module, channel)){
	      par_count++;
	      adc_offset[module][channel]= data;
	    }
	  }
	  else if(par_name=="adc_gain"){
	    module= x; channel= y;
	    data= z;
	    if(IsValidChannel(module, channel)){
	      par_count++;
	      adc_gain[module][channel]= data;
	    }
	  }
	  else if(par_name=="energy_threshold"){
	    module= x; channel= y;
	    data= z;
	    if(IsValidChannel(module, channel)){
	      par_count++;
	      energy_threshold[module][channel]= data;
	    }
	  }
	  else if(par_name=="disc_time_window"){
	    //module= x; channel= y;
	    data= x;
	    disc_time_window= x;
	  }
	  else if(par_name=="aida_time_calib"){
	    //module= x; channel= y;
	    data= x;
	    aida_time_calib= x;
	  }

	  else{
	    if(b_debug) std::cout << " Calibrator.cpp:: parameter name not recognized: " << par_name << "." <<std::endl;
	  }
	}
	

      }
      else{
	std::cout << " Calibrator.cpp:: LoadParameters() reached end of parameter file. Loaded new value for "<< par_count << " parameters." << std::endl;
	break;
      }
      //check forever loop not running amok
      line_count++;
      if(line_count>MAX_LINES){

	std::cout << " Calibrator.cpp:: count of lines read by LoadParameters() exceeds maximum allowed value (" << MAX_LINES << "). Parameters loaded= " << par_count<<std::endl;
	std::cout << "                  Break out of loop and continue... *****************"  << std::endl << std::endl;
	break;
      }
    }

  }
  else{
    std::cout << " Calibrator.cpp:: Error opening parameter file: " << file_name << " *****************"  << std::endl << std::endl;
  }
  return;

}




void Calibrator::ResetData(){



  
  b_push_data = false;
  b_fill_tree = true;

  //reset this?
  b_enabled= false;
  b_valid_ch= false;


  cal_data.time_aida= 0;
  cal_data.time_disc= 0;
  cal_data.time_external= 0;

  cal_data.adc_energy= 0;
  cal_data.dssd= 0;
  cal_data.strip= 0;

  cal_data.adc_range= 0;
  cal_data.side= 0;
  cal_data.module= 0;
  cal_data.channel= 0;
  cal_data.data_type= 0;
  cal_data.info_code= 0;

  cal_data.corr_flag= false; 
  cal_data.disc_flag= false; 


}


void Calibrator::Write(){
  if(GetBRootTree()){


    out_root_tree->Write(0,TObject::kOverwrite);
    std::cout << "\n *** writing ng Clibrator TTree .... "<< std::endl;
    out_root_tree->Print();
  }
  if(GetBHistograms()){

    for(int i=0;i<16;i++){
      hCalEhCh[i]->Write();
      hCalElCh[i]->Write();
    }
  }
}

//FS:fast! bool Calibrator::SetGeometry(){
void Calibrator::SetGeometry(){


    cal_data.dssd = map_dssd[GetModule()];

    // higher number channels for strips at center of detector (FEE for each half)
    if(map_strip[GetModule()] == 1){
      //FS     cal_data.strip = OrderChannel( GetChannel() );
     cal_data.strip = 63 - OrderChannel( GetChannel() );
    } else if(map_strip[GetModule()]== 2){
      //FS      cal_data.strip = 127 -  OrderChannel( GetChannel() ); 
      cal_data.strip = 64 +  OrderChannel( GetChannel() ); 
    } else {
      cal_data.strip = -1;
    }

    cal_data.side = map_side[GetModule()];

    //FS return true;

    //return false;
}

bool Calibrator::CalibrateAdc(){

  if(GetAdcRange() == 0){ //decay
    int x;
    double e;

    //  x = GetAdcData() - common::ADC_ZERO - adc_offset[GetModule()][GetChannel()];
    //  e = adc_polarity[GetModule()] * adc_gain[GetModule()][GetChannel()] * x;
    x = adc_polarity[GetModule()] * (GetAdcData() - common::ADC_ZERO); //
    e = adc_gain[GetModule()][GetChannel()] * ( x + adc_offset[GetModule()][GetChannel()] );

    SetAdcEnergy(e); //adc_polarity[GetModule()] * adc_gain[GetModule()][GetChannel()] * value);
    if(e >= energy_threshold[GetModule()][GetChannel()]) return true;
    else return false;
  }
  else if(GetAdcRange() == 1){
    int x;
    x = adc_polarity[GetModule()] * (GetAdcData() - common::ADC_ZERO );
    SetAdcEnergy( adc_gain_highE[GetModule()] * x );
    return true;
  }
  else{
    SetAdcEnergy(-9999);
    return false;
  }
}

 
unsigned char Calibrator::OrderChannel( unsigned char ch ){
  //assumes we have already checked channel is within valid range!... likely in enabled channels check.
  return order[ch];
}


void Calibrator::SetBDebug(bool flag){
  b_debug= flag;
}
void Calibrator::SetBHistograms(bool flag){
  b_histograms= flag;
}
void Calibrator::SetBPushData(bool flag){
  b_push_data= flag;
}
void Calibrator::SetBFillTree(bool flag){
  b_fill_tree= flag;
}

void Calibrator::SetBRootTree(bool flag){
  if(flag){
    if( out_root_tree != NULL){
      b_root_tree= true;
      std::cout << " ***       Calibrator::SetBRootTree(): set to TRUE"<<std::endl;
    }
    else {
      std::cout << " ** WARNING ** Calibrator.cpp: attempted to set b_root_tree as valid when TTree is not initialized! *** " << std::endl;
      b_root_tree= false;
    }
  }
  else b_root_tree= false;
}

/****** FS: unpacker******
void Calibrator::SetBEnabled(bool flag){
  b_enabled= flag;
}
************************/

void Calibrator::SetBValidCh(bool flag){
  b_valid_ch= flag;
}

void Calibrator::SetBCorrStatus(bool flag){
  b_corr_status= flag;
}


void Calibrator::SetTmStpDisc(unsigned long value){
    tm_stp_disc[GetModule()][GetChannel()]= value;
}

void Calibrator::SetTmStpOffset(int64_t value){ //diff AIDA->EXTERNAL tm-stp

  tm_stp_corr_offset= value; 

}




void Calibrator::SetTimeAida(double value){
  cal_data.time_aida= value;
}

bool Calibrator::SetTimeDisc(){


  return false;
}


void Calibrator::SetTimeExternal(){
  if(GetCorrFlag()){  

    cal_data.time_external=(cal_data.time_aida + tm_stp_corr_offset)/  aida_time_calib; 
  }
  else cal_data.time_external= 0;
}

void Calibrator::SetAdcEnergy(double value){
  cal_data.adc_energy= value;
}

void Calibrator::SetAdcData(int value){
  cal_data.adc_data= value;
}
void Calibrator::SetDSSD(int value){
  cal_data.dssd= value;
}

void Calibrator::SetStrip(int value){
  cal_data.strip= value;
}

void Calibrator::SetAdcRange(unsigned char value){
  cal_data.adc_range= value;
}

void Calibrator::SetSide(unsigned char value){
  cal_data.side= value;
}

void Calibrator::SetModule(unsigned char value){
  cal_data.module= value;
}

void Calibrator::SetChannel(unsigned char value){
  cal_data.channel= value;
}

void Calibrator::SetDataType(unsigned char value){
  cal_data.data_type= value;
}

void Calibrator::SetInfoCode(unsigned char value){
  cal_data.info_code= value;
}

void Calibrator::SetCorrFlag(){

  cal_data.corr_flag= GetBCorrStatus() ;
}

void Calibrator::SetDiscFlag(bool value){
  cal_data.disc_flag= value;
}








  //Getters...
bool Calibrator::GetBDebug(){ return b_debug; }
bool Calibrator::GetBHistograms(){ return b_histograms; }
bool Calibrator::GetBPushData(){ return b_push_data; }
bool Calibrator::GetBFillTree(){ return b_fill_tree; }
bool Calibrator::GetBRootTree(){ return b_root_tree; }

//FS bool Calibrator::GetBEnabled(){ return b_enabled; }
bool Calibrator::GetBValidCh(){ return b_valid_ch; }
bool Calibrator::GetBCorrStatus(){ return b_corr_status; }


unsigned long Calibrator::GetTmStpDisc(){ 
  //if(GetBValidCh()){
  return tm_stp_disc[GetModule()][GetChannel()];
  //}
  //else return 0;
}

int64_t Calibrator::GetTmStpOffset(){ return tm_stp_corr_offset; }

//Getters... for cal_data structure
double Calibrator::GetTimeAida(){ return cal_data.time_aida; }
double Calibrator::GetTimeDisc(){ return cal_data.time_disc; }
double Calibrator::GetTimeExternal(){ return cal_data.time_external; }
double Calibrator::GetAdcEnergy(){ return cal_data.adc_energy; }

int Calibrator::GetAdcData(){ return cal_data.adc_data; }
int Calibrator::GetDSSD(){ return cal_data.dssd; }
int Calibrator::GetStrip(){ return cal_data.strip; }

unsigned char Calibrator::GetAdcRange(){ return cal_data.adc_range; }
unsigned char Calibrator::GetSide(){ return cal_data.side; }
unsigned char Calibrator::GetModule(){ return cal_data.module; }
unsigned char Calibrator::GetChannel(){ return cal_data.channel; }
unsigned char Calibrator::GetDataType(){ return cal_data.data_type; }
unsigned char Calibrator::GetInfoCode(){ return cal_data.info_code; }

//bool Calibrator::GetSyncFlag(){ return cal_data.sync_flag; }
bool Calibrator::GetCorrFlag(){ return cal_data.corr_flag; }
bool Calibrator::GetDiscFlag(){ return cal_data.disc_flag; }

common::calib_data_struct Calibrator::GetStruct(){ return cal_data; }




Calibrator::Calibrator(){

  out_root_tree = NULL;

  b_debug = false;

  b_root_tree = false;
  b_histograms = false;

  b_push_data = false;
  b_fill_tree = true;

  for(int i=0;i<common::N_FEE64;i++){
    for(int j=0;j<common::N_CHANNEL;j++){
      tm_stp_disc[i][j]=0;    
    }
  }
  tm_stp_corr_offset=0;
 


  //simplest possible....
  for(int i=0;i<common::N_FEE64;i++){
    map_dssd[i]= i/4;
    if( (i%4)<2 ) map_side[i]=0; //0,1,4,5,8,9 ...
    else map_side[i]= 1; //2,3,6,7,10,11 ...
    map_strip[i]= i%2; // even -> 0, odd -> 1
  }

  for(int i=0;i<common::N_FEE64;i++){
    for(int j=0;j<common::N_CHANNEL;j++){
      adc_offset[i][j]= 0;
      adc_gain[i][j]= 0.61035; // keV/ch [Phong's cal_table_v2.txt]
      energy_threshold[i][j]= 100; // 100 keV as a nominal threshold
    }
    if( map_side[i]==0 ) adc_polarity[i]= 1;
    else adc_polarity[i]= -1;
    adc_gain_highE[i]= 0.7; // MeV/ch
  }

  disc_time_window= 50000;
  aida_time_calib= 2; //     ???????AIDA: 25MHz, BRIKEN: 50Hz

  b_enabled= false;
  b_valid_ch= false;

  b_corr_status= false;

  cal_data.time_aida= 0;
  cal_data.time_disc= 0;
  cal_data.time_external= 0;
  cal_data.adc_energy= 0;

  cal_data.adc_data= 0;
  cal_data.dssd= 0;
  cal_data.strip= 0;

  cal_data.adc_range= 0;
  cal_data.side= 0;
  cal_data.module= 0;
  cal_data.channel= 0;
  cal_data.data_type= 0;
  cal_data.info_code= 0;

  //cal_data.sync_flag= false;
  cal_data.corr_flag= false; 
  cal_data.disc_flag= false; 

}


