#include "Analysis.h"
#include <iomanip>
#include <stdio.h>
#include <string>


//the heart of the matter
void Analysis::Process(DataSource & my_source, Calibrator & my_cal_data){


  //my_cal_data.time_aida might change name...
  if( my_cal_data.GetTimeAida() > ( evt_data.t + event_time_window) ){

    if(CloseEvent()) WriteOutBuffer(my_source);
    //    if(my_source.GetBSendData()) WriteOutBuffer(my_source);
    InitEvent(my_cal_data);
    //    return true; //time to start a new event
  }
  
  else{

    //if same range as current event
    if(my_cal_data.GetAdcRange()== event_range){
      q.push(my_cal_data.GetStruct());
    }
    //if event labeled as decay, but found implant data
    else if(my_cal_data.GetAdcRange() == 1){
      event_range = 1; //change to implant type of event
      q.push(my_cal_data.GetStruct());
    }
    //if decay event for implant data, skip
    //FS: too much time to cout
    //    else {
    //  std::cout << " \n---- skipping decay hit within implantation event ---- " << std::endl;
    //}
    
    //    std::cout << "CAL (dssd, ch, E, T): " << my_cal_data.GetDSSD() << " " <<my_cal_data.GetStrip() << " "
    //	      << my_cal_data.GetAdcEnergy() << " " << my_cal_data.GetTimeAida() << std::endl; 
  }



}

//the real heart of the matter
bool Analysis::BuildEvent(Calibrator & my_cal_data){

  return false;

  //-FS  if(!my_cal_data.GetAdcRange() && !my_cal_data.GetModule()==30) return false; //skip low energy range of NNAIDA#30
 
  //-FS: should have been checked already:  if(!IsChEnabled(my_cal_data)) return false; //skip channels not enabled

  //continue if good data type to create event...

  //consider windows backwards and forwards in time?
  //  if(my_cal_data.GetDiscFlag()){

  //FS; only consider forward in time... for now.
  
  //////  if( my_cal_data.GetTimeAida() > ( evt_data.t + event_time_window) ){
  //////  return true; //time to start a new event
  //////}
  
  //If good data to add to current event....


  //FS: nice to look for dt, but not here...:  evt_data.dt= my_cal_data.time_cluster - evt_data.t; //assume monotonically increasing tm-stps

  //FS.... move all this for Close event! USE QUEUE!!
  //////q.push(my_cal_data.GetStruct());

  //////std::cout << "CAL (DSSD, ch, E): " << my_cal_data.GetDSSD() << " "
  //////    << my_cal_data.GetStrip() << " " << my_cal_data.GetAdcEnergy() << std::endl; 

  /**********
  int det= my_cal_data.GetDSSD();
  int strip= my_cal_data.GetStrip();
  int side= my_cal_data.GetSide();
  double energy= my_cal_data._adc_energy; //FS: why this does not exist?: GetEnergy();
  //FS:out: int range = my_cal_data.GetAdcRange();
  *******************/

  //-FS: this should go away, is remmand of using one FEE for MUSIC/TAC in 2015 test
  //  if(det>0 ){
  //   if(side==0) energy= my_cal_data.GetAdcData() - common::ADC_ZERO;
  //  else if(side==1) energy = common::ADC_ZERO - my_cal_data.GetAdcData();
  // }
  //else if (det==0) energy = common::ADC_ZERO - my_cal_data.GetAdcData();
    
      
  //FS: later?  evt_data.n++;
    
  /************** FS: now queueue ******************8    
  //IMPLANT IMPLANT IMPLANT
  if(my_cal_data.GetAdcRange() == 1){
    multiplicity[det][side]= multiplicity[det][side]+1;
    energy_implant[det][side]= energy_implant[det][side]+energy;
    
    //if new max value for E of this event
    if(energy>max_energy_implant[det][side]){
      max_energy_implant[det][side]= energy;
      if(side==0) evt_data.y_i[det]= strip; //n-side: horizontal strips
      else if(side==1) evt_data.x_i[det]= strip; //p-side: vertical strips
    }
  }
  //DECAY DECAY DECAY
  else{
    evt_data.n_det_d[det]= evt_data.n_det_d[det]+1;
    evt_data.n_side_d[det][side]= evt_data.n_side_d[det][side]+1;
    
    e_sum_d[det][side]= e_sum_d[det][side]+energy;
    
    if(strip>strip_max_d[det][side]) strip_max_d[det][side]= strip;
    if(strip<strip_min_d[det][side]) strip_min_d[det][side]= strip;
    
    //if new max value for E of this event
    if(energy>evt_data.e_d[det][side]){
      evt_data.e_d[det][side]= energy;
      if(side==0){
	evt_data.y_d[det]= strip; //n-side: horizontal strips
      }
      else if(side==1){ 
	evt_data.x_d[det]= strip; //p-side: vertical strips
      }
    }
  }
  ***********************/
    
    
    
    //-----------------------------------------------------------
    //
    //   Fill some histograms with internal event data
    //
    //----------------------------------------------------------
    /************* FS ********************
    if(GetBHistograms()){
      hEvt_TmStpDist[0]->Fill( evt_data.dt );
      hEvt_TmStpDist[3]->Fill( my_cal_data.GetTimeAida() - evt_data.t0 );
      if(my_cal_data.GetDiscFlag()) hEvt_TmStpDist[2]->Fill( my_cal_data.GetTimeDisc() - evt_data.t0 );
    }
    *************************************/

  return false;
  
}


bool Analysis::CloseEvent(){

  //FS: this function will be called also for first entrie... when previous event has size zero!

  int Nend= q.size();

  //FS: reset to zero!!!!
  int n_hits[common::N_DSSD][2]={{0}};

  double energy[common::N_DSSD][2]={{0}};

  double max_energy[common::N_DSSD][2]={{0}};
  int ch_max_e[common::N_DSSD][2]={{0}};

  int min_ch[common::N_DSSD][2]; //for dX of strips
  int max_ch[common::N_DSSD][2]={{0}};

  for(int i=0; i<common::N_DSSD;i++){
    min_ch[i][0]= 0x200; //512
    min_ch[i][1]= 0x200; //512
  }

  if(GetBDebug()){
    std::cout << "\nHITS:"
	      << "\nRange\tDSSD\tSide\tch\tE\tTS"<<std::endl;
  }
  for(int i=0; i< Nend ; i++){

    common::calib_data_struct cal;
    cal= q.front();
    q.pop();

    //    std::cout << "QUE (det, ch, E, T): " << cal.dssd << "  "<<cal.strip << " "
    //	      << cal.adc_energy << " " << cal.time_aida << std::endl; 

    //skip decay range hit within implantation events
    if( !(cal.adc_range==0 && event_range==1) ){
      //multiplicities
      n_hits[cal.dssd][cal.side]= n_hits[cal.dssd][cal.side] +1;
      //energy sum
      energy[cal.dssd][cal.side]= energy[cal.dssd][cal.side] + cal.adc_energy;

      //strip with maximum energy for that side
      if(cal.adc_energy > max_energy[cal.dssd][cal.side]){
	max_energy[cal.dssd][cal.side]= cal.adc_energy;
	ch_max_e[cal.dssd][cal.side]= cal.strip;
      }

      //strip for lowest and highest channel number
      if(cal.strip<min_ch[cal.dssd][cal.side]) min_ch[cal.dssd][cal.side]= cal.strip;
      if(cal.strip>max_ch[cal.dssd][cal.side]) max_ch[cal.dssd][cal.side]= cal.strip;

      if(GetBDebug()){
	std::cout << int(cal.adc_range) << " \t"
		  << cal.dssd << " \t"
		  << int(cal.side) << " \t"
		  << cal.strip << " \t"
		  << cal.adc_energy << " \t"
		  << cal.time_aida << std::endl;
      }
    }


  }
  //  std::cout << "*----------------------------------------------------------------*" << std::endl;

  if(!q.empty()){
    std::cout << "\n*********************\n"
	      << " --- Analysis::CloseEvent(): ERROR - q should have been empty! "
	      << " ------> size: " << q.size() 
	      << "\n*********************" << std::endl;

  }

  /// ACA PIBE, COMIENZA TODA LA COCOA!
  int det0, det1;
  double e0=0;
  double e1=0;
  int delta0, delta1;
  int n_det=0;
  bool b_hit_xy= true;


  //find detectors with largest energy measured for X and Y sides
  for(int i=0; i< common::N_DSSD; i++){
    //find detector with largest energy for x-side strips
    if(energy[i][0]>= e0){
      det0= i; e0= energy[i][0];
    }
    //find detector with largest energy for y-side strips
    if(energy[i][1]>= e1){
      det1= i; e1= energy[i][1];
    }
    //count how many DSSDs have a hit (either in x or y)
    if(n_hits[i][0]>0 || n_hits[i][1]>0) ++n_det;
  }

  //size of cluster (max-min strip)
  delta0= max_ch[det0][0]-min_ch[det0][0] ;
  delta1= max_ch[det1][1]-min_ch[det1][1] ;


  // in case max energy appears in different DSSDs, select the DSSD
  // with the side with largest collected energy (we could try also energy per strip?)
  if(det0!=det1){
    if(energy[det0][0] > energy[det1][1]) det1= det0;
    else det0= det1; 
    b_hit_xy= false;
  }


  //-----
  //-----  Calculation of variables in evt_data structure begins here
  //-----  
  //-----
  //-----
  //check cluster size bellow maximum value (this should also reject pulser events)
  if( n_hits[det0][0] <= p_N_MAX[event_range] && n_hits[det1][1] <= p_N_MAX[event_range] ){
    //extra conditions: 
    //    delta < n_hits + DMAX
    //    multiplicity detectors... or for implants, last with hits
    //evt_data= cal.time_disc; //THIS MUST BE SET AT INIT?
    evt_data.e_x= energy[det0][0];
    evt_data.e_y= energy[det1][1];
    evt_data.e= 0.5*(evt_data.e_x+evt_data.e_y);
    evt_data.x= ch_max_e[det0][0];
    evt_data.y= ch_max_e[det1][1];
    evt_data.z = det0;
    
    //FS: here add more stringent conditions...
    //
    //this could be checked earlier, and skip writing it to TTree, if more speed is needed.
    if(n_hits[det0][0]==0 || n_hits[det1][1]==0) evt_data.type= 0xFF; 
    else if( event_range==0){
      //not too many DSSDs with hits...
      if(n_det <= p_N_DET_MAX){
	if(b_hit_xy)  evt_data.type = 5;
	else evt_data.type= 105; //max energy for x and y planes in different DSSD
      }
      else{
	evt_data.type= 0xFF; // bad value, will return false
	//FS: could put a return value here, and would skip saving this event in TTree
	//return false;
      }
    }
    //ion
    else if(b_hit_xy) evt_data.type = 4;
    else if(!b_hit_xy) evt_data.type = 104; //max energy for x and y planes in different DSSD
  

    //if we're saving things to Root TTree, get values for hit structure
    if(GetBRootTree()){

      //    if(evt_data.implant_flag>0){
      //      int det= evt_data.implant_flag%10;
      
      hit.t= evt_data.t;
      hit.t_fast= evt_data.t_fast;
      hit.e= evt_data.e;
      hit.e_x= evt_data.e_x;
      hit.e_y= evt_data.e_y;
      hit.x= evt_data.x; 
      hit.y= evt_data.y;
      hit.z= evt_data.z;
      //--- new in analysis_data_struc (vs aida_event)
      hit.n=0;
      for(int i=0; i< common::N_DSSD ; i++){
	hit.n= hit.n + n_hits[i][0] + n_hits[i][1];
      }
      hit.n_x= n_hits[det0][0];
      hit.n_y= n_hits[det1][1];
      hit.n_z= n_det; 
      //------------------------
      hit.type= evt_data.type; 
      
      out_root_tree->Fill();
    }

    if(GetBDebug()){
      std::cout << " *** EVENT:\n"
		<< "TYPE \tDSSD \tX \tY \tE \tE_x \tE_y \tTS \tTS_fast\n"
		<< int(evt_data.type) << " \t"
		<< evt_data.z << " \t"
		<< evt_data.x << " \t"
		<< evt_data.y << " \t"
		<< evt_data.e << " \t"
		<< evt_data.e_x << " \t"
		<< evt_data.e_y << " \t"
		<< evt_data.t << " \t"
		<< evt_data.t_fast << "\n---------------------------------------------------------------------" << std::endl;
    }

    if(evt_data.type == 0xFF) return false; //bad event (still saved to tree, but not file)
    return true; //this was a good event (e.g. from cluster size conditions)
  }

  //if data don't fulfill conditions for a good aida event
  return false;

 
}


void Analysis::InitEvent(Calibrator & my_cal_data){
    
  ResetEvent();

  evt_data.t= my_cal_data.GetTimeAida(); //FS: THIS MUST BE GET TIME CLUSTER>>> LATER
  evt_data.t_fast= my_cal_data.GetTimeAida(); //FS: THIS MUST BE GET TIME CLUSTER FAST>>> LATER

  event_range= 0; //assume this is a decay event

  /********************** FS ***********************8
  //check again just in case we're trying to initialize with wrong data
  if(!my_cal_data.GetAdcRange() || !IsChEnabled(my_cal_data)){
    evt_data.t0= -9999999; //bogus timestamp... should take care of things
  }

  if(my_cal_data.GetDiscFlag()){
    evt_data.t0= my_cal_data.GetTimeDisc();
    evt_data.t0_ext= my_cal_data.GetTimeExternal();

  }
  else{
    evt_data.t0= my_cal_data.GetTimeAida();
    evt_data.t0_ext= my_cal_data.GetTimeExternal();

  }
  *************************************************/
  //FS: BuildEvent is defunct now...
  //  BuildEvent(my_cal_data);

}


void Analysis::WriteOutBuffer(DataSource & my_source){

  //FS int s_double= sizeof(double);
  //FS int s_int= sizeof(int);
  //FS int s_char= sizeof(char);


  /*********
  std::cout << "\n FAST DEBUG!! " 
	  << "\n size matters:"
	  << "\n    - unsigned long long: " << sizeof(unsigned long long)
	  << "\n    - double: "<< sizeof(double)
	  << "\n    - int: "<< sizeof(int)
	  << "\n    - unsigned char: " << sizeof(unsigned char)
	  << "\n    - aida_event: "<< sizeof(evt_data) 
	    << "\n    - aida_event: "<< sizeof(common::aida_event) 
	  <<std::endl;
  *******************/

  int offset=my_source.GetBuffOffset(); 

  // FS: debug values for output
  //  evt_data.t=0xFFEEFFEE000041D4;
  //  evt_data.t_fast=0xAABBCCDD;
  //  evt_data.e= 1;
  //  evt_data.e_x= 2;
  //  evt_data.e_y=3;
  //  evt_data.x=4;
  //  evt_data.y=5;
  // evt_data.z=6;
  // evt_data.type= 0xEE;


  //FS: debug values for output
  //  memcpy(my_source.BufferOut+offset, (char*) &aida_id, sizeof(int32_t) );
  //memcpy(my_source.BufferOut+offset+sizeof(int32_t), (char*) &evt_data, sizeof(evt_data) );
  // remove aida_id from data file
  memcpy(my_source.BufferOut+offset, (char*) &evt_data, sizeof(evt_data) );

  //FS: remove... if needed
  //  if(GetBDebug()){
  //  std::cout << "\n size of evt_data: "<< sizeof(evt_data) << std::endl ;
  //    int j=0;
  //  for(int i= offset; i< offset+sizeof(evt_data); i++){
  ///    if((j%16)==0) std::cout<< std::endl <<" -- "; //printf("\n");
  //    if( (j%4)==0) std::cout << " 0x";      
  //    printf("%02hhx",my_source.BufferOut[i]);
  //    j++;
  //  }
  // }

  //FS  offset= offset + sizeof(evt_data)+sizeof(int32_t);
  offset= offset + sizeof(evt_data);

  my_source.SetBuffOffset(offset);
  my_source.WriteBuffer();
  //  my_source.TransferBuffer(evt_data.t);

  return;

}



void Analysis::FillHistogramsSingles(Calibrator & my_cal_data){

  int mod= my_cal_data.GetModule();
  int det= my_cal_data.GetDSSD();
  int ch= my_cal_data.GetChannel();
  int side= my_cal_data.GetSide();
  int range= my_cal_data.GetAdcRange();

  /********
  if(b_debug) std::cout << " about to fill histograms-singles....mod= "<< mod << " range= "<<range  << std::endl;

  if(b_mod_enabled[mod]){
    if(det<0 || det>3){
      std::cout << "\n******************     det" << det<< "->0  ***********************"<<std::endl;
      det=0;
    }

    if(ch<0 || ch>63){
      std::cout << "\n******************     ch" << ch<< "->0  ***********************"<<std::endl;

      ch=0;
    }

  if(mod<0 || mod>32){
    std::cout << "\n******************     mod" << mod<< "->0  ***********************"<<std::endl;
    mod=0;
  }

  if(side<0 || side>2){
    std::cout << "\n******************     side" << side<< "->0  ***********************"<<std::endl;
    side=0;
  }

    //DECAY DECAY DECAY
    if(my_cal_data.GetAdcRange()==0){
      hADClowCh[mod]->Fill(ch,my_cal_data.GetAdcData());
      hCh_ADClow[mod]->Fill(ch);

      hElow[mod]->Fill(my_cal_data.GetAdcData());
      if(my_cal_data.GetDiscFlag()) hEdisc[mod]->Fill(my_cal_data.GetAdcData());

      if(my_cal_data.GetDiscFlag()){
	hADCdiscCh[mod]->Fill(ch,my_cal_data.GetAdcData());
	hCh_ADCdisc[mod]->Fill(ch);
      }

      hTimeADClow[0]->Fill(my_cal_data.GetTimeAida()-t_low_prev);
      t_low_prev= my_cal_data.GetTimeAida();

      if(my_cal_data.GetDiscFlag()){
	hTimeADCdisc[0]->Fill(my_cal_data.GetTimeDisc()-t_disc_prev);
	t_disc_prev= my_cal_data.GetTimeDisc();
      }
    }
    //IMPLANT IMPLANT IMPLANT
    else if(my_cal_data.GetAdcRange()==1){

      hADChighCh[mod]->Fill(ch,my_cal_data.GetAdcData());
      hCh_ADChigh[mod]->Fill(ch);

      //  hEhigh[mod]->Fill(my_cal_data.GetAdcEnergy());
      hEhigh[mod]->Fill(my_cal_data.GetAdcData());

      //TS dist
      hTimeADChigh[0]->Fill(my_cal_data.GetTimeAida()-t_high_prev);
      t_high_prev= my_cal_data.GetTimeAida();
    }

    if(0) std::cout << " TS(aida), TS(ext): " << my_cal_data.GetTimeAida()<<",  "<< my_cal_data.GetCorrFlag() << ":  "<< my_cal_data.GetTimeExternal()<<std::endl;

    hTimeStamp->Fill( my_cal_data.GetTimeAida() );

    if(my_cal_data.GetCorrFlag()){
      hTimeStampExt->Fill( my_cal_data.GetTimeExternal() );

      hTimeStampFlag->Fill( 1 );
    }
    else  hTimeStampFlag->Fill( 0 );
  }
  ******************/
}

void Analysis::FillHistogramsEvent(){


  /**********************
  double e_det[common::N_DSSD]={0};
  double e_aida=0;
  bool b_gE= true;
  bool b_gX= true;

  int multi_d=0;
  int multi_i=0;


  if(b_debug) std::cout<<" ...evt histogrms..." << std::endl;


  for(int i=0;i<common::N_DSSD; i++){

    if(i>0)  multi_i += evt_data.n_det_i[i];
    if(i>0)  multi_d += evt_data.n_det_d[i];

    if(i==0) e_det[i]=evt_data.e_i[i][0];
    else if(i>0){
      if(evt_data.e_i[i][0]>evt_data.e_i[i][1]) e_det[i]=evt_data.e_i[i][0];
      else e_det[i]=evt_data.e_i[i][1];
      
      if(evt_data.n_det_i[i]>0) e_aida += e_det[i];
    }
  }

  hEvt_MultiID->Fill(multi_i,multi_d);
  if(evt_data.implant_flag>0) hEvt_HitsFlag->Fill(0);
  if(evt_data.decay_flag>0) hEvt_HitsFlag->Fill(1);

  if(b_debug){
    printf("        eaida %f,    edet[i]  %f  %f  %f  %f \n", e_aida, e_det[0], e_det[1], e_det[2], e_det[3]);
  }


  if(multi_i>0){

    for(int i=0;i<common::N_DSSD; i++){
      
      for(int j=0;j<2;j++){
	if(evt_data.n_side_i[i][j]>0){
	  hEvt_Eside[i][j]->Fill( evt_data.e_i[i][j] );
	  hEvt_Multi[i][j]->Fill(evt_data.n_side_i[i][j]);

	  if( (evt_data.implant_flag%10)==i && evt_data.implant_flag>0) hEvt_Eside_if[i][j]->Fill( evt_data.e_i[i][j] );

	}
      }
      
      if(evt_data.n_det_i[i]>0 && i>0){
	
	hEvt_ExEy[i]->Fill(evt_data.e_i[i][1],evt_data.e_i[i][0]);
	
	hEvt_X[i]->Fill(evt_data.x_i[i]);
	hEvt_Y[i]->Fill(evt_data.y_i[i]);
	hEvt_XY[i]->Fill(evt_data.x_i[i],evt_data.y_i[i]);

	if((evt_data.implant_flag%10)==i && evt_data.implant_flag>0){
	  hEvt_ExEy_if[i]->Fill(evt_data.e_i[i][1],evt_data.e_i[i][0]);
	  hEvt_XY_if[i]->Fill(evt_data.x_i[i],evt_data.y_i[i]);
	}
      }

      if(evt_data.n_det_i[i]>0){
	hEvt_HitsDet->Fill(i,1);
      }
      
    }
        
    hEvt_Eaida->Fill(e_aida);
    hEvt_EdE->Fill(e_det[0],e_aida);

    
    //det1, det2 hits
    if(evt_data.n_det_i[1]>0 && evt_data.n_det_i[2]>0){
      hEvt_dX[0]->Fill(evt_data.x_i[2]-evt_data.x_i[1]);
      hEvt_dY[0]->Fill(evt_data.y_i[2]-evt_data.y_i[1]);
    }
    
    if(evt_data.n_det_i[1]>0 && evt_data.n_det_i[2]>0){
      hEvt_dX[1]->Fill(evt_data.x_i[3]-evt_data.x_i[1]);
      hEvt_dY[1]->Fill(evt_data.y_i[3]-evt_data.y_i[1]);
    }
    
    if(evt_data.n_det_i[1]>0 && evt_data.n_det_i[2]>0){
      hEvt_dX[2]->Fill(evt_data.x_i[3]-evt_data.x_i[2]);
      hEvt_dY[2]->Fill(evt_data.y_i[3]-evt_data.y_i[2]);
    }
    
    if(evt_data.n_det_i[1]>0 && evt_data.n_det_i[2]>0 && evt_data.n_det_i[3]>0) {
      hEvt_dXdX->Fill(evt_data.x_i[2]-evt_data.x_i[1],evt_data.x_i[3]-evt_data.x_i[2]);
      hEvt_dYdY->Fill(evt_data.y_i[2]-evt_data.y_i[1],evt_data.y_i[3]-evt_data.y_i[2]);
    }
    
    
    for(int i=0;i<common::N_DSSD;i++){
      if(evt_data.n_side_i[i][0]>0) hEvt_HitsSide->Fill(i*2,1); 
      if(evt_data.n_side_i[i][1]>0) hEvt_HitsSide->Fill(i*2+1,1); 
    }
    

    hEvt_TmStpDist[1]->Fill( evt_data.dt );

    if(b_debug) std::cout << "db     Analysis::FillHistrgramsEvent():   done with evt_implant (multi="<<multi_i<<")"<< std::endl;

  }
    
  if(b_pulser){
    hEvt_EPulser_d->Fill( (evt_data.e_d[1][0]+evt_data.e_d[2][0]+evt_data.e_d[3][0]/3.),(evt_data.e_d[1][1]+evt_data.e_d[2][1]+evt_data.e_d[3][1]/3.) );
  }
  else if(multi_d>0){

    /// EVENT : DECAY : HISTOGRAMS
    int multi_det_d=0;
    
    for(int i=0;i<common::N_DSSD; i++){
    
      if(i>0 && evt_data.n_det_d[i]>0) multi_det_d++;
    }

    for(int i=0;i<common::N_DSSD; i++){
  
      for(int j=0;j<2;j++){
	if(evt_data.n_side_d[i][j]>0){

	  hEvt_Eside_d[i][j]->Fill( evt_data.e_d[i][j] );

	  if(evt_data.decay_flag>0 && (evt_data.decay_flag%10)==i){
	    hEvt_Eside_df[i][j]->Fill( evt_data.e_d[i][j] );

	    if(evt_data.decay_flag>10) hEvt_Eside_df2[i][j]->Fill( evt_data.e_d[i][j] );
	  }

	}
      }
      
      if(evt_data.n_det_d[i]>0 && i>0){
	
	hEvt_ExEy_d[i]->Fill(evt_data.e_d[i][1],evt_data.e_d[i][0]);
	hEvt_XY_d[i]->Fill(evt_data.x_d[i],evt_data.y_d[i]);


	if(evt_data.decay_flag>0 && (evt_data.decay_flag%10)==i){
	  hEvt_ExEy_df[i]->Fill(evt_data.e_d[i][1],evt_data.e_d[i][0]);
	  hEvt_XY_df[i]->Fill(evt_data.x_d[i],evt_data.y_d[i]);

	  if(evt_data.decay_flag>10){
	    hEvt_ExEy_df2[i]->Fill(evt_data.e_d[i][1],evt_data.e_d[i][0]);
	    hEvt_XY_df2[i]->Fill(evt_data.x_d[i],evt_data.y_d[i]);
	  
	  }
	}
	
      }

      hEvt_MultiDet_d->Fill(multi_det_d);
      if(evt_data.n_side_d[i][0]>0 && evt_data.n_side_d[i][1]>0)  hEvt_MultiSide_d[0]->Fill(i,2);
      else if(evt_data.n_side_d[i][0]>0 || evt_data.n_side_d[i][1]>0) hEvt_MultiSide_d[0]->Fill(i,1);
      
      if(i>0){
	if(evt_data.n_side_d[i][0]>0){
	  hEvt_MultiStrip_d[i][0]->Fill(evt_data.n_side_d[i][0]);
	  hEvt_MultidX_d[i][0]->Fill(evt_data.n_side_d[i][0],strip_max_d[i][0]-strip_min_d[i][0]);
	}

	if(evt_data.n_side_d[i][1]>0){
	  hEvt_MultiStrip_d[i][1]->Fill(evt_data.n_side_d[i][0]);
	  hEvt_MultidX_d[i][1]->Fill(evt_data.n_side_d[i][1],strip_max_d[i][1]-strip_min_d[i][1]);
	}
      }
    }

    if(b_debug) std::cout << "db     Analysis::FillHistrgramsEvent():   done with evt_decay (multi="<<multi_d<<")"<< std::endl;
  }

  *************************/
}


void Analysis::UpdateHistograms(){

  /****************
  if(GetBHistograms()){
    std::cout << "  Analysis::UpdateHistograms()... updating"<<std::endl;

    for(int i=0;i<2;i++){
      for(int j=0;j<16;j++){
	if(i==0){
	  cADClow[i]->cd(j+1)->Modified();
	  cADCdisc[i]->cd(j+1)->Modified();
	  cADChigh[i]->cd(j+1)->Modified();
	}
	cEall[i]->cd(j+1)->Modified();
      }

      for(int j=0;j<(8*4);j++){
	cADClow[1]->cd(j+1)->Modified();
      }
          
      cEall[i]->Update();
    }

      cADClow[0]->Update();
      cADCdisc[0]->Update();
      cADChigh[0]->Update();
      cADClow[1]->Update();


    
    for(int i=1;i<8;i++){
      cTimeDist[0]->cd(i)->Modified();
    }
  cTimeDist[0]->Update();
  
  
  for(int i=0;i<20;i++){
    if(i<16) cEvtE1->cd(i+1)->Modified();
    if(i<12) cEvtXY->cd(i+1)->Modified();
    if(i<9) cEvtdXdY->cd(i+1)->Modified();
    if(i<12) cEvtMulti->cd(i+1)->Modified();
    if(i<20) cEvtE_d->cd(i+1)->Modified();
    if(i<12) cEvtXY_d->cd(i+1)->Modified();
    if(i<6) cEvtXY2_d->cd(i+1)->Modified();
    if(i<12) cEvtMulti_d->cd(i+1)->Modified();
  }
  cEvtE1->Update();
  cEvtXY->Update();
  cEvtdXdY->Update();
  cEvtMulti->Update();
  cEvtE_d->Update();
  cEvtXY_d->Update();
  cEvtXY2_d->Update();
  cEvtMulti_d->Update();
  }
  ******************/
}




void Analysis::InitAnalysis(int opt){


  std::cout << "\n\nINIT ANALYSIS: queue size: " << q.size() << std::endl;
  evt_data.t = 0;

  event_count= 0;
  t_low_prev= 0;
  t_high_prev= 0;
  t_disc_prev= 0;
  b_pulser= false;

  ResetEvent();

  //file name-> to load parameters
  b_mod_enabled[0]= false;
  b_mod_enabled[1]= true;
  b_mod_enabled[2]= true;
  b_mod_enabled[3]= true;
  b_mod_enabled[4]= true;
  b_mod_enabled[5]= false;
  b_mod_enabled[6]= true;
  b_mod_enabled[7]= true;
  b_mod_enabled[8]= true;
  b_mod_enabled[9]= false;
  b_mod_enabled[10]= true;
  b_mod_enabled[11]= true;
  b_mod_enabled[12]= false;
  b_mod_enabled[13]= true;
  b_mod_enabled[14]= true;
  b_mod_enabled[15]= false;
  b_mod_enabled[16]= true;
  b_mod_enabled[17]= false;
  b_mod_enabled[18]= false;
  b_mod_enabled[19]= false;
  b_mod_enabled[20]= false;
  b_mod_enabled[21]= false;
  b_mod_enabled[22]= false;
  b_mod_enabled[23]= false;
  b_mod_enabled[24]= false;
  b_mod_enabled[25]= false;
  b_mod_enabled[26]= false;
  b_mod_enabled[27]= false;
  b_mod_enabled[28]= false;
  b_mod_enabled[29]= false;
  b_mod_enabled[30]= true;
  b_mod_enabled[31]= false;
  b_mod_enabled[32]= false;
  //b_mod_enabled[33]= false;

  /******/
  std::string nombre[33];
  nombre[0]="ZEROZEROZERO";
  nombre[1]="NNAIDA1 (Det3, Pside)";
  nombre[2]="NNAIDA2 (Det2, Pside)";
  nombre[3]="NNAIDA3 (Det3, Nside)";
  nombre[4]="NNAIDA4 (Det2, Nside)";
  nombre[5]="NNAIDA5 (DetX, Xside)";
  nombre[6]="NNAIDA6 (Det1, Pside)";
  nombre[7]="NNAIDA7 (Det2, Nside)";
  nombre[8]="NNAIDA8 (Det3, Nside";
  nombre[9]="NNAIDA9 (DetX, Xside)";
  nombre[10]="NNAIDA10 (Det1, Pside)";
  nombre[11]="NNAIDA11 (Det1, Nside)";
  nombre[12]="NNAIDA12 (DetX, Xside)";
  nombre[13]="NNAIDA13 (Det3, Pside)";
  nombre[14]="NNAIDA14 (Det2, Pside)";
  nombre[15]="NNAIDA15 (DetX, Xside)";
  nombre[16]="NNAIDA16 (Det1, Nside)";
  nombre[17]="NNAIDA17 (DetX, Xside)";
  nombre[18]="NNAIDA18 (DetX, Xside)";
  nombre[19]="NNAIDA19 (DetX, Xside)";
  nombre[20]="NNAIDA20 (DetX, Xside)";
  nombre[21]="NNAIDA21 (DetX, Xside)";
  nombre[22]="NNAIDA22 (DetX, Xside)";
  nombre[23]="NNAIDA23 (DetX, Xside)";
  nombre[24]="NNAIDA24 (DetX, Xside)";
  nombre[25]="NNAIDA25 (DetX, Xside)";
  nombre[26]="NNAIDA26 (DetX, Xside)";
  nombre[27]="NNAIDA27 (DetX, Xside)";
  nombre[28]="NNAIDA28 (DetX, Xside)";
  nombre[29]="NNAIDA29 (DetX, Xside)";
  nombre[30]="NNAIDA30 (DetX, Xside)";
  nombre[31]="NNAIDA31 (DetX, Xside)";
  nombre[32]="NNAIDA32 (Det0, IC)";
  //nombre[33]="NNAIDA33 (DetX, XsideY)";
  /***********/

  int m_side[33]= {-1,
		   1,1,0,0, /* 1:4 */
		   -1,1,0,0, /* 5:8 */
		   -1,1,0,-1, /* 9:12 */
		   1,1,-1,0, /* 13:16 */
		   -1,-1,-1,-1, /* 17:20 */
		   -1,-1,-1,-1, /* 21:24 */		   
		   -1,-1,-1,-1, /* 25:28 */
		   -1,1,-1,-1 /* 29:32 */};

  int m_dssd[33]= {-1,
		   3,2,3,2, /* 1:4 */
		   -1,1,2,3, /* 5:8 */
		   -1,1,1,-1, /* 9:12 */
		   3,2,-1,1, /* 13:16 */
		   -1,-1,-1,-1, /* 17:20 */
		   -1,-1,-1,-1, /* 21:24 */		   
		   -1,-1,-1,-1, /* 25:28 */
		   -1,0,-1,-1 /* 29:32 */};

 int m_strip[33]= {-1,
		   2,2,1,1, /* 1:4 */
		   -1,1,2,2, /* 5:8 */
		   -1,2,1,-1, /* 9:12 */
		   1,1,-1,2, /* 13:16 */
		   -1,-1,-1,-1, /* 17:20 */
		   -1,-1,-1,-1, /* 21:24 */		   
		   -1,-1,-1,-1, /* 25:28 */
		   -1,0,-1,-1 /* 29:32 */};

  for(int i=0;i<33;i++){
    geo_side[i]= m_side[i];
    geo_detector[i]= m_dssd[i];
    geo_strip[i]= m_strip[i];
    
  }

  // if first of 'opt' is 1, enable histogramming
  if( (opt & 0x01) == 1){

    std::cout << "\n *** Analysis:InitAnalysis(): NO HISTOGRAMS IMPLEMENTED IN THIS VERSION (FastSort) ***\n\n";

    /**************** FS FS ******
    char hname[256];
    char htitle[256];
    std::string stitle;
    std::string full_title;

    SetBHistograms(true);
    
      sprintf(hname,"cADClow_0");
      cADClow[0]= new TCanvas(hname, hname, 10,10,1200,900); cADClow[0]->Divide(5,3);

      sprintf(hname,"cADChigh_0");
      cADChigh[0]= new TCanvas(hname, hname, 20,20,1200,900); cADChigh[0]->Divide(5,3);

      sprintf(hname,"cADCdisc_0");
      cADCdisc[0]= new TCanvas(hname, hname, 30,30,1200,900); cADCdisc[0]->Divide(5,3);

      sprintf(hname,"cEall_0");
      cEall[0]= new TCanvas(hname, hname, 40,40,1200,900); cEall[0]->Divide(5,3);

      sprintf(hname,"cEall_1");
      cEall[1]= new TCanvas(hname, hname, 40,40,1200,900); cEall[1]->Divide(5,3);


      sprintf(hname,"cADClow_1");
      cADClow[1]= new TCanvas(hname, hname, 10,10,1800,1000); cADClow[1]->Divide(8,4);


    for(int i=0;i<common::N_FEE64;i++){
      if(b_mod_enabled[i]){


	int k;
	if(geo_detector[i]==0) k=5;
	else k= 5*(geo_detector[i]-1)+geo_side[i]*2+geo_strip[i];

	if(k<1 || k>15) {
	  k=15;
	  std::cout << " **** Analysis::InitAnalysis(): ERROR IN GEOMETRY"<<
	    "\n        mod, det, side, strip: " << i << " " << geo_detector[i]<< " "<< geo_side[i]<< " "<< "  "<<geo_strip[i]<< std::endl;
	}


	// ADC(decay range)
	sprintf(hname,"hADClowCh%i",i);
	sprintf(htitle," ADC(low);ch;ADC data");
	stitle= htitle;
	full_title= nombre[i]+stitle;
	hADClowCh[i]= new TH2I(hname, full_title.data(), 64, 0, 64, 1024, 0, 65536);


	cADClow[0]->cd(k); hADClowCh[i]->Draw("colz"); gPad->SetLogz(1);


	sprintf(hname,"hCh_AClow%i",i);
	sprintf(htitle," ADC(low);ch");
	stitle= htitle;
	full_title= nombre[i]+stitle;
	hCh_ADClow[i]= new TH1I(hname, full_title.data(), 64, 0, 64);

	cADClow[1]->cd(geo_detector[i]*8+geo_side[i]*2+geo_strip[i]); hCh_ADClow[i]->Draw("");

	// ADC(decay range) ! diacriminator hit
	sprintf(hname,"hADCdiscCh%i",i);
	sprintf(htitle," ADC(low) !DISC hits;ch;ADC data");
	stitle= htitle;
	full_title= nombre[i]+stitle;
	hADCdiscCh[i]= new TH2I(hname, full_title.data(), 64, 0, 64, 1024, 0, 65536);

	cADCdisc[0]->cd(k); hADCdiscCh[i]->Draw("colz"); gPad->SetLogz(1);

	sprintf(hname,"hCh_ACdisc%i",i);
	sprintf(htitle," ADC(low) !DISC hit;ch");
	stitle= htitle;
	full_title= nombre[i]+stitle;
	hCh_ADCdisc[i]= new TH1I(hname, full_title.data(), 64, 0, 64);


	// ADC(implant range)
	sprintf(hname,"hADChighCh%i",i);
	sprintf(htitle," ADC(high);ch;ADC data");
	stitle= htitle;
	full_title= nombre[i]+stitle;
	hADChighCh[i]= new TH2I(hname, full_title.data(), 64, 0, 64, 1024, 0, 65536);

	cADChigh[0]->cd(k); hADChighCh[i]->Draw("colz"); gPad->SetLogz(1);

	sprintf(hname,"hCh_AChigh%i",i);
	sprintf(htitle," ADC(high);ch");
	stitle= htitle;
	full_title= nombre[i]+stitle;
	hCh_ADChigh[i]= new TH1I(hname, full_title.data(), 64, 0, 64);
	cADClow[1]->cd(4+geo_detector[i]*8+geo_side[i]*2+geo_strip[i]); hCh_ADChigh[i]->Draw("");

	sprintf(hname,"hElow%i",i);
	sprintf(htitle," E(low); E [MeV]");
	stitle= htitle;
	full_title= nombre[i]+stitle;
	if(geo_side[i]==0) hElow[i]= new TH1I(hname, full_title.data(), 1000, 30000, 65000);
	else  hElow[i]= new TH1I(hname, full_title.data(), 1000, 0, 34000);

	cEall[0]->cd(k); hElow[i]->Draw(""); gPad->SetLogy(1);


	sprintf(hname,"hEdisc%i",i);
	sprintf(htitle," E(disc); E [MeV]");
	stitle= htitle;
	full_title= nombre[i]+stitle;
	if(geo_side[i]==0) hEdisc[i]= new TH1I(hname, full_title.data(), 1000, 30000, 65000);
	else  hEdisc[i]= new TH1I(hname, full_title.data(), 1000, 0, 34000);
	hEdisc[i]->SetLineColor(kRed);
	cEall[0]->cd(k); hEdisc[i]->Draw("same"); gPad->SetLogy(1);

	sprintf(hname,"hEhigh%i",i);
	sprintf(htitle," E(high); E [GeV]");
	stitle= htitle;
	full_title= nombre[i]+stitle;
	if(geo_detector[i]==0) hEhigh[i]= new TH1I(hname, full_title.data(), 1000, 30000, 65000);
	else if(geo_side[i]==0) hEhigh[i]= new TH1I(hname, full_title.data(), 1000, 30000, 50000);
	else  hEhigh[i]= new TH1I(hname, full_title.data(), 1000, 15000, 34000);

	cEall[1]->cd(k); hEhigh[i]->Draw(""); gPad->SetLogy(1);
      }
    }
    


      //only for detectors with data...for now
    sprintf(hname,"cTimeDist");
    cTimeDist[0]= new TCanvas(hname, hname, 40,40,1100,700); cTimeDist[0]->Divide(4,2);

     
    //time stamp distribution for ADClow
    sprintf(hname,"hTimeADClow");
    sprintf(htitle,"#Deltat ts !ADC(low);time stamp difference");
    hTimeADClow[0]= new TH1I(hname, htitle, 1000, -500, 9500);
    cTimeDist[0]->cd(1); hTimeADClow[0]->Draw(""); gPad->SetLogy(1);

    //time stamp distribution for ADCdisc
    sprintf(hname,"hTimeADCdisc");
    sprintf(htitle,"#Deltat ts !ADC(disc);time stamp difference");
    hTimeADCdisc[0]= new TH1I(hname, htitle, 1000, -500, 9500);
    cTimeDist[0]->cd(2); hTimeADCdisc[0]->Draw(""); gPad->SetLogy(1);
      
    //time stamp distribution for ADChigh
    sprintf(hname,"hTimeADChigh");
    sprintf(htitle,"#Deltat ts !ADC(high);time stamp difference");
    hTimeADChigh[0]= new TH1I(hname, htitle, 1000, -500, 9500);
    cTimeDist[0]->cd(3); hTimeADChigh[0]->Draw(""); gPad->SetLogy(1);


    hTimeStamp= new TH1I("hTimeStamp","Time stamp;tmstp [1/1e6]",1000,0,1e5);

    hTimeStampExt= new TH1I("hTimeStampExt","Time stamp External;tmstp [1/1e6]",1000,0,1e5);

    hTimeStampFlag= new TH1I("hTimeStampFlag","Time stamp Ext Flag;corr_flag",2,0,2);
    cTimeDist[0]->cd(4);  hTimeStampFlag->Draw(""); gPad->SetLogy(1);

    //
    //  Histograms to monitor event reconstruction
    //

    std::cout << "     InitAnalysis(): initialize histograms for evend clustering"<<std::endl;



    cEvtE1= new TCanvas("cEvtE1","cEvt Energy1", 100,100,1200,950); cEvtE1->Divide(4,4);
    cEvtXY= new TCanvas("cEvtXY","cEvt XY", 140,140,900,1000); cEvtXY->Divide(3,4);
    cEvtdXdY= new TCanvas("cEvtdXdY","cEvt dX:dY", 140,140,1200,800); cEvtdXdY->Divide(3,3);
    cEvtMulti= new TCanvas("cEvtMulti","cEvt Multiplicity", 140,140,1200,1000); cEvtMulti->Divide(4,3);

    for(int i=0;i<common::N_DSSD;i++){

      if(i>0){
	sprintf(hname,"hEvt_Eside%i_0",i);
	sprintf(htitle,"Energy (DSSD%i n-side);Energy [ch]",i);
	hEvt_Eside[i][0]= new TH1I(hname, htitle, 512, 0, 32768);
	cEvtE1->cd(i+1); hEvt_Eside[i][0]->Draw(""); gPad->SetLogy(1);

	sprintf(hname,"hEvt_Eside%i_1",i);
	sprintf(htitle,"Energy (DSSD%i p-side);Energy [ch]",i);
	hEvt_Eside[i][1]= new TH1I(hname, htitle, 512, 0, 32768);
	cEvtE1->cd(i+5); hEvt_Eside[i][1]->Draw(""); gPad->SetLogy(1);

	sprintf(hname,"hEvt_ExEy%i",i);
	sprintf(htitle,"Energy N vs P side (DSSD%i);Energy n-side [ch];Energy p-side [ch]",i);
	hEvt_ExEy[i]= new TH2I(hname, htitle, 256, 0, 16384, 256, 0, 16384);
	cEvtE1->cd(i+9); hEvt_ExEy[i]->Draw("colz"); gPad->SetLogz(0);



      }
      else if(i==0){
	sprintf(hname,"hEvt_Eside%i_0",i);
	sprintf(htitle,"Energy (Ionization Chamber A);Energy [ch]");
	hEvt_Eside[i][0]= new TH1I(hname, htitle, 1024, 0, 32768);
	cEvtE1->cd(i+1); hEvt_Eside[i][0]->Draw(""); gPad->SetLogy(1);

	sprintf(hname,"hEvt_Eside%i_1",i);
	sprintf(htitle,"Energy (Ionization Chamber B);Energy [ch]");
	hEvt_Eside[i][1]= new TH1I(hname, htitle, 1024, 0, 32768);
	cEvtE1->cd(i+5); hEvt_Eside[i][1]->Draw(""); gPad->SetLogy(1);

      }
    


      if(i==0){
	sprintf(hname,"hEvt_Eaida");
	sprintf(htitle,"Energy AIDA (DSSD sum);Energy [ch]");
	hEvt_Eaida= new TH1I(hname, htitle, 1024, 0, 49152);

	sprintf(hname,"hEvt_Eaida_gE");
	sprintf(htitle,"Energy AIDA (DSSD sum) !E cut;Energy [ch]");
	hEvt_Eaida_gE= new TH1I(hname, htitle, 1024, 0, 49152);

	sprintf(hname,"hEvt_Eaida_gX");
	sprintf(htitle,"Energy AIDA (DSSD sum) !X cut;Energy [ch]");
	hEvt_Eaida_gX= new TH1I(hname, htitle, 1024, 0, 49152);

	sprintf(hname,"hEvt_EdE");
	sprintf(htitle,"Energy AIDA vs IC;Energy IC [ch];Energy AIDA [ch]");
	hEvt_EdE= new TH2I(hname, htitle, 256, 0, 32768, 256, 0, 49152);
      }

      if(i>0){
	sprintf(hname,"hEvt_X%i",i);
	sprintf(htitle,"X DSSD%i;X [ch]",i);
	hEvt_X[i]= new TH1I(hname, htitle, 128, 0, 128);
	cEvtXY->cd(i); hEvt_X[i]->Draw(""); gPad->SetLogy(1);

	sprintf(hname,"hEvt_Y%i",i);
	sprintf(htitle,"Y DSSD%i;Y [ch]",i);
	hEvt_Y[i]= new TH1I(hname, htitle, 128, 0, 128);
	cEvtXY->cd(i+3); hEvt_Y[i]->Draw(""); gPad->SetLogy(1);

	sprintf(hname,"hEvt_XY%i",i);
	sprintf(htitle,"X vs Y DSSD%i;X [ch];Y [ch]",i);
	hEvt_XY[i]= new TH2I(hname, htitle, 32, 0, 128, 32, 0, 128);
	cEvtXY->cd(i+6); hEvt_XY[i]->Draw("colz"); gPad->SetLogz(0);
      
      }

      if(i==0){  
	sprintf(hname,"hEvt_dX1");
	sprintf(htitle,"dX (DSSD1 vs DSSD2);X2 - X1 [ch]");
	hEvt_dX[0]= new TH1I(hname, htitle, 64, -128, 128);
	cEvtdXdY->cd(1); hEvt_dX[0]->Draw(""); gPad->SetLogy(1);

	sprintf(hname,"hEvt_dX2");
	sprintf(htitle,"dX (DSSD1 vs DSSD3);X3 - X1 [ch]");
	hEvt_dX[1]= new TH1I(hname, htitle, 64, -128, 128);
	cEvtdXdY->cd(2); hEvt_dX[1]->Draw(""); gPad->SetLogy(1);

	sprintf(hname,"hEvt_dX3");
	sprintf(htitle,"dX (DSSD2 vs DSSD3);X3 - X2 [ch]");
	hEvt_dX[2]= new TH1I(hname, htitle, 64, -128, 128);
	cEvtdXdY->cd(3); hEvt_dX[2]->Draw(""); gPad->SetLogy(1);


	sprintf(hname,"hEvt_dY1");
	sprintf(htitle,"dY (DSSD1 vs DSSD2);Y2 - Y1 [ch]");
	hEvt_dY[0]= new TH1I(hname, htitle, 64, -128, 128);
	cEvtdXdY->cd(4); hEvt_dY[0]->Draw(""); gPad->SetLogy(1);

	sprintf(hname,"hEvt_dY2");
	sprintf(htitle,"dY (DSSD1 vs DSSD3);Y3 - Y1 [ch]");
	hEvt_dY[1]= new TH1I(hname, htitle, 64, -128, 128);
	cEvtdXdY->cd(5); hEvt_dY[1]->Draw(""); gPad->SetLogy(1);

	sprintf(hname,"hEvt_dY3");
	sprintf(htitle,"dY (DSSD2 vs DSSD3);Y3 - Y2 [ch]");
	hEvt_dY[2]= new TH1I(hname, htitle, 64, -128, 128);
	cEvtdXdY->cd(6); hEvt_dY[2]->Draw(""); gPad->SetLogy(1);
      

	sprintf(hname,"hEvt_dXdX");
	sprintf(htitle,"#Delta X1 vs #Delta X3;X2 - X1 [ch];X3 - X2 [ch]");
	hEvt_dXdX= new TH2I(hname, htitle, 32, -128, 128, 32, -128, 128);
	cEvtdXdY->cd(7); hEvt_dXdX->Draw("colz"); gPad->SetLogz(1);


	sprintf(hname,"hEvt_dYdY");
	sprintf(htitle,"#Delta Y1 vs #Delta Y3;Y2 - Y1 [ch];Y3 - Y2 [ch]");
	hEvt_dYdY= new TH2I(hname, htitle, 32, -128, 128, 32, -128, 128);
	cEvtdXdY->cd(8); hEvt_dYdY->Draw("colz"); gPad->SetLogz(1);
      }

      if(i>0){

	sprintf(hname,"hEvt_Multi%i_0",i);
	sprintf(htitle,"Multiplicity (DSSD%i n-side);N hits (n-side)",i);
	hEvt_Multi[i][0]= new TH1I(hname, htitle, 15, 0, 15);
	cEvtMulti->cd(i+1); hEvt_Multi[i][0]->Draw(""); gPad->SetLogy(1);


	sprintf(hname,"hEvt_Multi%i_1",i);
	sprintf(htitle,"Multiplicity (DSSD%i p-side);N hits (p-side)",i);
	hEvt_Multi[i][1]= new TH1I(hname, htitle, 15, 0, 15);
	cEvtMulti->cd(i+5); hEvt_Multi[i][1]->Draw(""); gPad->SetLogy(1);
      }
      else if(i==0){
	sprintf(hname,"hEvt_Multi%i_0",i);
	sprintf(htitle,"Multiplicity (Ion Chamber);N hits (IC)");
	hEvt_Multi[i][0]= new TH1I(hname, htitle, 15, 0, 15);
	cEvtMulti->cd(i+1); hEvt_Multi[i][0]->Draw(""); gPad->SetLogy(1);
      }

      if(i==0){
	hEvt_HitsSide= new TH1I("hEvt_HitsSide","Detector Hits (IC->Det3: each side);N hits (side)",8,0,8);
	cEvtMulti->cd(9); hEvt_HitsSide->Draw(""); gPad->SetLogy(1);

	hEvt_HitsDet= new TH1I("hEvt_HitsDet","Detector Hits (IC->Det3);N hits (Det)",4,0,4);
	cEvtMulti->cd(10); hEvt_HitsDet->Draw(""); gPad->SetLogy(1);

	hEvt_HitsDet_gE= new TH1I("hEvt_HitsDet_gE","Detector Hits (IC->Det3) !E cut;N hits (Det)",4,0,4);
	cEvtMulti->cd(11); hEvt_HitsDet_gE->Draw(""); gPad->SetLogy(1);

	hEvt_HitsDet_gX= new TH1I("hEvt_HitsDet_gX","Detector Hits (IC->Det3) !X cut;N hits (Det)",4,0,4);
	cEvtMulti->cd(12); hEvt_HitsDet_gX->Draw(""); gPad->SetLogy(1);

      }
    }

    hEvt_TmStpDist[0]= new TH1I("hEvt_TmStpDist0","Time Stamp Dist (within event);ts - ts_{0} [arb. units]",500,-2500,4500); 
    cTimeDist[0]->cd(5);
    hEvt_TmStpDist[0]->Draw(); gPad->SetLogy(1);

    hEvt_TmStpDist[1]= new TH1I("hEvt_TmStpDist1","Time Stamp Dist (last hit);#Delta ts [arb. units]",500,-2500,4500); 
    cTimeDist[0]->cd(6);
    hEvt_TmStpDist[1]->Draw(); gPad->SetLogy(1);

    hEvt_TmStpDist[2]= new TH1I("hEvt_TmStpDist2","Time Stamp Dist (within event !DISC);#Delta ts [arb. units]",500,-2500,4500); 
    cTimeDist[0]->cd(7);
    hEvt_TmStpDist[2]->Draw(); gPad->SetLogy(1);

    hEvt_TmStpDist[3]= new TH1I("hEvt_TmStpDist3","Time Stamp Dist (within event !ADC);#Delta ts [arb. units]",500,-2500,4500); 
    cTimeDist[0]->cd(8);
    hEvt_TmStpDist[3]->Draw(); gPad->SetLogy(1);




    cEvtE_d= new TCanvas("cEvtE_d","cEvt Energy1 (DECAY)", 100,100,1200,1000); cEvtE_d->Divide(5,4);
    cEvtXY_d= new TCanvas("cEvtXY_d","cEvt XY ( DECAY)", 140,140,1200,900); cEvtXY_d->Divide(4,3);
    cEvtXY2_d= new TCanvas("cEvtXY2_d","Cluster Size (DECAY)", 140,140,1000,800); cEvtXY2_d->Divide(3,2);
    cEvtMulti_d= new TCanvas("cEvtMulti_d","cEvt Multiplicity (DECAY)", 140,140,1200,800); cEvtMulti_d->Divide(4,3);


    hEvt_EPulser_d= new TH2I("hEvt_EPulser_E","Pulser spectra;<E n-side> [ch];<E p-side> [ch]",100,0,32000, 100, 0, 32000); 
    cEvtE_d->cd(3);  hEvt_EPulser_d->Draw("colz"); gPad->SetLogz(0);

    for(int i=0;i<common::N_DSSD;i++){

      // E(decay range)
      sprintf(hname,"hEvt_Eside_d%i",i);
      sprintf(htitle,"E(decay) Det%i n-side;E [***]",i);
      hEvt_Eside_d[i][0]= new TH1I(hname, htitle, 512, 0, 32768);

      sprintf(hname,"hEvt_Eside_d%i",i+4);
      sprintf(htitle,"E(decay) Det%i p-side;E [***]",i);
      hEvt_Eside_d[i][1]= new TH1I(hname, htitle, 512, 0, 32768);
      if(i==0){
	cEvtE_d->cd(1);  hEvt_Eside_d[0][0]->Draw(); gPad->SetLogy(1);
	cEvtE_d->cd(2);  hEvt_Eside_d[0][1]->Draw(); gPad->SetLogy(1);
      }
      else{
	cEvtE_d->cd(1+i*5);  hEvt_Eside_d[i][0]->Draw(); gPad->SetLogy(1);
	cEvtE_d->cd(2+i*5);  hEvt_Eside_d[i][1]->Draw(); gPad->SetLogy(1);
      }

      sprintf(hname,"hEvt_ExEy_d%i",i);
      sprintf(htitle,"Ex vs Ey (decay) Det%i;Ex [***];Ey [***]",i);
      hEvt_ExEy_d[i]= new TH2I(hname, htitle, 256, 0, 16384, 256, 0, 16384);
      if(i>0){
	cEvtE_d->cd(3+i*5);  hEvt_ExEy_d[i]->Draw("colz"); gPad->SetLogz(1);
      }


      sprintf(hname,"hEvt_XY_d%i",i);
      sprintf(htitle,"X vs Y (decay) Det%i;Ex [***];Ey [***]",i);
      hEvt_XY_d[i]= new TH2I(hname, htitle, 128, 0, 128, 128, 0, 128);
      if(i>0){
	cEvtXY_d->cd(i+1);  hEvt_XY_d[i]->Draw("colz"); gPad->SetLogz(1);
      }

      sprintf(hname,"hEvt_MultiStrip_d%i_0",i);
      sprintf(htitle,"Multiplicity Strips Det%i n-side;N strips",i);
      hEvt_MultiStrip_d[i][0] = new TH1I(hname,htitle,40,0,40);
      cEvtMulti_d->cd(i+5);  hEvt_MultiStrip_d[i][0]->Draw(""); gPad->SetLogy(1);

      sprintf(hname,"hEvt_MultiStrip_d%i_1",i);
      sprintf(htitle,"Multiplicity Strips Det%i p-side;N strips",i);
      hEvt_MultiStrip_d[i][1] = new TH1I(hname,htitle,40,0,40);
      cEvtMulti_d->cd(i+9);  hEvt_MultiStrip_d[i][1]->Draw(""); gPad->SetLogy(1);


      ////////////////NNNNNNNEWWWWWWWWWWWWWW//////////////////
      ////////////////////volver/////////////////////////////
      sprintf(hname,"hEvt_Eside_df%i",i);
      sprintf(htitle,"E(decay) Det%i n-side !decay;E [***]",i);
      hEvt_Eside_df[i][0]= new TH1I(hname, htitle, 512, 0, 16384);
      hEvt_Eside_df[i][0]->SetLineColor(kRed);

      sprintf(hname,"hEvt_Eside_df%i",i+4);
      sprintf(htitle,"E(decay) Det%i p-side !decay;E [***]",i);
      hEvt_Eside_df[i][1]= new TH1I(hname, htitle, 512, 0, 16384);
      hEvt_Eside_df[i][1]->SetLineColor(kRed);


      sprintf(hname,"hEvt_Eside_df2%i",i);
      sprintf(htitle,"E(decay) Det%i n-side !decay2;E [***]",i);
      hEvt_Eside_df2[i][0]= new TH1I(hname, htitle, 512, 0, 16384);
      hEvt_Eside_df2[i][0]->SetLineColor(kGreen);

      sprintf(hname,"hEvt_Eside_df2%i",i+4);
      sprintf(htitle,"E(decay) Det%i p-side !decay2;E [***]",i);
      hEvt_Eside_df2[i][1]= new TH1I(hname, htitle, 512, 0, 16384);
      hEvt_Eside_df2[i][1]->SetLineColor(kGreen);


      sprintf(hname,"hEvt_ExEy_df%i",i);
      sprintf(htitle,"Ex vs Ey (decay) Det%i !decay;Ex [***];Ey [***]",i);
      hEvt_ExEy_df[i]= new TH2I(hname, htitle, 256, 0, 16384, 256, 0, 16384);

      sprintf(hname,"hEvt_ExEy_df2%i",i);
      sprintf(htitle,"Ex vs Ey (decay) Det%i !decay2;Ex [***];Ey [***]",i);
      hEvt_ExEy_df2[i]= new TH2I(hname, htitle, 256, 0, 16384, 256, 0, 16384);

      if(i>0){
	cEvtE_d->cd(1+i*5);  hEvt_Eside_df[i][0]->Draw("same");// gPad->SetLogy(1);
	cEvtE_d->cd(2+i*5);  hEvt_Eside_df[i][1]->Draw("same");// gPad->SetLogy(1);

	cEvtE_d->cd(1+i*5);  hEvt_Eside_df2[i][0]->Draw("same");// gPad->SetLogy(1);
	cEvtE_d->cd(2+i*5);  hEvt_Eside_df2[i][1]->Draw("same");// gPad->SetLogy(1);

	cEvtE_d->cd(4+i*5);  hEvt_ExEy_df[i]->Draw("colz"); gPad->SetLogz(1);
	cEvtE_d->cd(5+i*5);  hEvt_ExEy_df2[i]->Draw("colz"); gPad->SetLogz(1);

      }

      sprintf(hname,"hEvt_XY_df%i",i);
      sprintf(htitle,"X vs Y (decay) Det%i !decay;Ex [***];Ey [***]",i);
      hEvt_XY_df[i]= new TH2I(hname, htitle, 128, 0, 128, 128, 0, 128);

      sprintf(hname,"hEvt_XY_df2%i",i);
      sprintf(htitle,"X vs Y (decay) Det%i !decay2;Ex [***];Ey [***]",i);
      hEvt_XY_df2[i]= new TH2I(hname, htitle, 128, 0, 128, 128, 0, 128);

      if(i>0){
	cEvtXY_d->cd(i+5);  hEvt_XY_df[i]->Draw("colz"); gPad->SetLogz(0);
	cEvtXY_d->cd(i+9);  hEvt_XY_df2[i]->Draw("colz"); gPad->SetLogz(0);
      }


      sprintf(hname,"hEvt_Eside_if%i_0",i);
      sprintf(htitle,"Energy (DSSD%i n-side) !implant;Energy [ch]",i);
      hEvt_Eside_if[i][0]= new TH1I(hname, htitle, 512, 0, 16384);
      hEvt_Eside_if[i][0]->SetLineColor(kRed);
      cEvtE1->cd(i+1); hEvt_Eside_if[i][0]->Draw("same"); gPad->SetLogy(1);
      
      sprintf(hname,"hEvt_Eside_if%i_1",i);
      sprintf(htitle,"Energy (DSSD%i p-side) !implant;Energy [ch]",i);
      hEvt_Eside_if[i][1]= new TH1I(hname, htitle, 512, 0, 16384);
      hEvt_Eside_if[i][1]->SetLineColor(kRed);
      cEvtE1->cd(i+5); hEvt_Eside_if[i][1]->Draw("same"); gPad->SetLogy(1);
      
      sprintf(hname,"hEvt_ExEy_if%i",i);
      sprintf(htitle,"Energy N vs P side (DSSD%i) !implant;Energy n-side [ch];Energy p-side [ch]",i);
      hEvt_ExEy_if[i]= new TH2I(hname, htitle, 256, 0, 16384, 256, 0, 16384);
      cEvtE1->cd(i+13); hEvt_ExEy_if[i]->Draw("colz"); gPad->SetLogz(0);


      sprintf(hname,"hEvt_XY_if%i",i);
      sprintf(htitle,"X vs Y (implant) Det%i !implant;Ex [***];Ey [***]",i);
      hEvt_XY_if[i]= new TH2I(hname, htitle, 128, 0, 128, 128, 0, 128);
      if(i>0) cEvtXY->cd(i+9);  hEvt_XY_if[i]->Draw("colz"); gPad->SetLogz(0);


      sprintf(hname,"hEvt_MultidX_d%i_0",i);
      sprintf(htitle,"#DeltaX vs multiplicity Det%i, n-side;hit multiplicity;x_max - x_min [ch]",i);
      hEvt_MultidX_d[i][0]= new TH2I(hname, htitle, 32, 0, 32, 64, 0, 64);
      if(i>0) cEvtXY2_d->cd(i);  hEvt_MultidX_d[i][0]->Draw("colz"); gPad->SetLogz(1);

      sprintf(hname,"hEvt_MultidX_d%i_1",i);
      sprintf(htitle,"#DeltaX vs multiplicity Det%i, p-side;hit multiplicity;x_max - x_min [ch]",i);
      hEvt_MultidX_d[i][1]= new TH2I(hname, htitle, 32, 0, 32, 64, 0, 64);
      if(i>0) cEvtXY2_d->cd(i+3);  hEvt_MultidX_d[i][1]->Draw("colz"); gPad->SetLogz(1);


    }

    hEvt_MultiDet_d = new TH1I("hEvt_MultiDet_d","Detector Multiplicity (Decay);N detectors",4,0,4);
    hEvt_MultiSide_d[0] = new TH2I("hEvt_MultiSide_d0","Sides Multiplicity (Decay);N sides;Detector",2,1,3,4,0,4);

    cEvtMulti_d->cd(2);  hEvt_MultiDet_d->Draw(); gPad->SetLogy(1);
    cEvtMulti_d->cd(3);  hEvt_MultiSide_d[0]->Draw("colz"); gPad->SetLogz(1);

    hEvt_MultiID= new TH2I("hEvt_MultiID","Multiplicity Implant:Decay;N implant hits;N decay hits",40,0,40,40,0,40);
    cEvtMulti_d->cd(1);  hEvt_MultiID->Draw("colz"); gPad->SetLogz(1);

    hEvt_HitsFlag= new TH1I("hEvt_HitsFlag","Implant & Decay Flags;Flag (0: implant, 1: decay)",2,0,2);
    cEvtMulti_d->cd(4);  hEvt_HitsFlag->Draw(""); gPad->SetLogy(1);
  



    if(b_debug) std::cout << "db    Analysis.cpp: Initialized histograms and canvas for this step"<<std::endl;
********************************/

    
  }
  //if second bit of 'opt' is 1, enable TTree for output
  if( ( (opt & 0x02) >> 1) == 1){
  
    std::cout << " ***     Analysis::InitAnalysis(): initializing TTree" << std::endl;
    //Initialize TTree
    out_root_tree = new TTree("AIDA_hits","AIDA_hits");
    out_root_tree->Branch("aida_hit",&hit,"t/l:t_fast/l:e/D:e_x/D:e_y/D:x/D:y/D:z/D:n/I:n_x/I:n_y/I:n_z/I:type/b");
    //x/I:y/I:z/I:ex/I:ey/I:flag/I");
    SetBRootTree(true);
  }

}


void Analysis::ResetEvent(){


  evt_data.t= 0;
  evt_data.t_fast= 0;
  evt_data.e= -1;
  evt_data.e_x= -1;
  evt_data.e_y= -1;
  evt_data.x= -1;
  evt_data.y= -1;
  evt_data.z= -1;
  evt_data.type= 0;

  // these are all assigned a value in the CloseEvent() function
  // there should be not case when they are saved with garbage values
  /**********************
  hit.t= -1;
  hit.t_ext= -1;
  hit.x= -1;
  hit.y= -1;
  hit.z= -1;
  hit.ex= -1;
  hit.ey= -1;
  hit.flag= -1;
  *******************/

  //FS: all zero!!!
  /***********************
  b_pulser= false;

  evt_data.multiplicity= 0;
  for(int i=0;i<common::N_DSSD;i++){
    //
    e_sum_d[i][0]=0;
    e_sum_d[i][1]=0;

    strip_max_d[i][0]=0;
    strip_max_d[i][1]=0;
    strip_min_d[i][0]=128;
    strip_min_d[i][1]=128;



    evt_data.n_det_i[i]=0;  
    evt_data.n_side_i[i][0]=0;  
    evt_data.n_side_i[i][1]=0;  
    evt_data.x_i[i]=-999;  
    evt_data.y_i[i]=-999;  
    evt_data.e_i[i][0]=-99999;  
    evt_data.e_i[i][1]=-99999;  

    evt_data.n_det_d[i]=0;  
    evt_data.n_side_d[i][0]=0;  
    evt_data.n_side_d[i][1]=0;  
    evt_data.x_d[i]=-999;  
    evt_data.y_d[i]=-999;  
    evt_data.e_d[i][0]=-99999;  
    evt_data.e_d[i][1]=-99999;  

    evt_data.dx_d[i]=-1;  
    evt_data.dy_d[i]=-1;  


    evt_data.t0= -99999;
    evt_data.t0_ext= -99999;
    evt_data.dt= 0; 
  }
  evt_data.decay_flag= 0;
  evt_data.implant_flag= 0;

  hit.t= -1;
  hit.t_ext= -1;
  hit.x= -1;
  hit.y= -1;
  hit.z= -1;
  hit.ex= -1;
  hit.ey= -1;
  hit.flag= -1;
  **************************/
}


void Analysis::WriteHistograms(){

  /*********************************
  for(int i=0;i<common::N_FEE64;i++){
    if(b_mod_enabled[i]){
      hADClowCh[i]->Write();
      hADCdiscCh[i]->Write();
      hADChighCh[i]->Write();
      hCh_ADClow[i]->Write();
      hCh_ADChigh[i]->Write();
      hCh_ADCdisc[i]->Write();
      
      hElow[i]->Write();
      hEhigh[i]->Write();
      hEdisc[i]->Write();
      
    }
  }



  //for(int i=1;i<4;i++){
    hTimeADClow[0]->Write();
    hTimeADCdisc[0]->Write();
    hTimeADChigh[0]->Write();

    hTimeStamp->Write();
    hTimeStampExt->Write();
    hTimeStampFlag->Write();


  for(int i=0; i<4;i++){
    for(int j=0;j<2;j++){
      // if(i!=0 || j!=1) hEvt_Eside[i][j]->Write();
      hEvt_Eside[i][j]->Write();

      if(i>0){
	hEvt_Eside_d[i][j]->Write();
	hEvt_Eside_df[i][j]->Write();
	hEvt_Eside_df2[i][j]->Write();

	hEvt_Eside_if[i][j]->Write();
      }
      if(i!=0){
	hEvt_Multi[i][j]->Write();
	hEvt_MultidX_d[i][j]->Write();

      }


    }
    
    if(i>0){
      hEvt_ExEy[i]->Write();
      hEvt_X[i]->Write();
      hEvt_Y[i]->Write();
      hEvt_XY[i]->Write();

      hEvt_ExEy_d[i]->Write();
      hEvt_XY_d[i]->Write();


      hEvt_ExEy_df[i]->Write();
      hEvt_XY_df[i]->Write();
      hEvt_ExEy_df2[i]->Write();
      hEvt_XY_df2[i]->Write();

      hEvt_ExEy_if[i]->Write();
      hEvt_XY_if[i]->Write();

    }
    //printf("wrote two..... \n");
    
    if(i<3){
      hEvt_dX[i]->Write(); // 1:2, 1:3, 2:3
      hEvt_dY[i]->Write();
    }
    //    printf("wrote four..... %i \n", i);


  }

  hEvt_TmStpDist[1]->Write();
  hEvt_TmStpDist[0]->Write();
  hEvt_TmStpDist[2]->Write();
  hEvt_TmStpDist[3]->Write();


  hEvt_EdE->Write();
  
  hEvt_Eaida->Write();
  hEvt_Eaida_gE->Write();
  hEvt_Eaida_gX->Write();
  
  
  hEvt_dXdX->Write();
  hEvt_dYdY->Write();
  
  
  hEvt_HitsSide->Write();
  hEvt_HitsDet->Write();
  hEvt_HitsDet_gE->Write();
  hEvt_HitsDet_gX->Write();

  hEvt_MultiID->Write();
  hEvt_HitsFlag->Write();

  hEvt_EPulser_d->Write();



  cADClow[0]->Write();
  cADChigh[0]->Write();
  cADCdisc[0]->Write();
  cEall[0]->Write();
  cADClow[1]->Write();
  cEall[1]->Write();
  cTimeDist[0]->Write();

  cEvtE1->Write(); 
  cEvtXY->Write();
  cEvtdXdY->Write();
  cEvtMulti->Write();

  cEvtE_d->Write();
  cEvtXY_d->Write();
  cEvtXY2_d->Write();
  cEvtMulti_d->Write();

  std::cout << "\n done writing Analysis histograms to file..." << std::endl;

  *************************/
}




void Analysis::ResetHistograms(){


  std::cout << " *** Analysis::ResetHistograms() - NO HISTOGRAMS IMPLEMENTED in FastSort, WE SHOULD NOT CALL THIS FUNCTION *** \n" << std::endl;
  /****************** FS FS ***********8
  if(GetBHistograms()){

    for(int i=0;i<common::N_FEE64;i++){
      if(b_mod_enabled[i]){
	hADClowCh[i]->Reset();
	hADCdiscCh[i]->Reset();
	hADChighCh[i]->Reset();
	hCh_ADClow[i]->Reset();
	hCh_ADChigh[i]->Reset();
	hCh_ADCdisc[i]->Reset();
	
	hElow[i]->Reset();
	hEhigh[i]->Reset();
	hEdisc[i]->Reset();
	
      }
    }
    
    hTimeADClow[0]->Reset();
    hTimeADCdisc[0]->Reset();
    hTimeADChigh[0]->Reset();
    
    hTimeStamp->Reset();
    hTimeStampExt->Reset();
    hTimeStampFlag->Reset();
    
    for(int i=0; i<4;i++){
      for(int j=0;j<2;j++){
	hEvt_Eside[i][j]->Reset();
	hEvt_Eside_if[i][j]->Reset();
	hEvt_Eside_df[i][j]->Reset();
	hEvt_Eside_df2[i][j]->Reset();
	
	if(i!=0){
	  hEvt_Multi[i][j]->Reset();
	  hEvt_Eside_d[i][j]->Reset();
	  hEvt_MultidX_d[i][j]->Reset();
	}
      }
      
      if(i>0){
	hEvt_ExEy[i]->Reset();
	hEvt_X[i]->Reset();
	hEvt_Y[i]->Reset();
	
	hEvt_ExEy_d[i]->Reset();
      }
      
      hEvt_ExEy_if[i]->Reset();
      hEvt_XY_if[i]->Reset();
      hEvt_ExEy_df[i]->Reset();
      hEvt_XY_df[i]->Reset();
      hEvt_ExEy_df2[i]->Reset();
      hEvt_XY_df2[i]->Reset();
      
      if(i<3){
	hEvt_dX[i]->Reset(); // 1:2, 1:3, 2:3
	hEvt_dY[i]->Reset();
      }
      if(i>0){
	hEvt_XY[i]->Reset();
	hEvt_XY_d[i]->Reset();
	
      }
    }
    
    
    hEvt_TmStpDist[1]->Reset();
    hEvt_TmStpDist[0]->Reset();
    hEvt_TmStpDist[2]->Reset();
    hEvt_TmStpDist[3]->Reset();
    
    
    hEvt_EdE->Reset();
    
    hEvt_Eaida->Reset();
    hEvt_Eaida_gE->Reset();
    hEvt_Eaida_gX->Reset();
    
    
    hEvt_dXdX->Reset();
    hEvt_dYdY->Reset();
    
    
    hEvt_HitsSide->Reset();
    hEvt_HitsDet->Reset();
    hEvt_HitsDet_gE->Reset();
    hEvt_HitsDet_gX->Reset();
    
    hEvt_MultiID->Reset();
    hEvt_HitsFlag->Reset();
    
    hEvt_EPulser_d->Reset();
    

    std::cout << "        Analysis::ResetHistograms(): all histograms have been reset..." << std::endl;
  }
  **************************/

}



bool Analysis::IsChEnabled(Calibrator & my_cal_data){

  if(b_mod_enabled[my_cal_data.GetModule()]){

    //can also skip individual channels here...

    if(my_cal_data.GetModule()==30){
      //NNAIDA#11:1.10      if( my_cal_data.GetChannel() == 10) return true;
      if( my_cal_data.GetChannel() == 24) return true; //NNAIAD#11:2.8
      else return false;
    }

    return true;
  }

  else return false;
}







bool Analysis::SetEventTimeWindow(double value){


  if(value>0){
    event_time_window= value;
    return true;
  }
  else{
    event_time_window =0;
    return false;
  }
}


void Analysis::PrintEvent(){

  /*************
  printf("\n  *EVT*   Multiplicity : ");
  printf("\n  *EVT*        N total= %i",evt_data.multiplicity);
  printf("\n  *EVT*i       N det0=  %i  (%i, %i)",evt_data.n_det_i[0], evt_data.n_side_i[0][0], evt_data.n_side_i[0][1]);
  printf("\n  *EVT*i       N det1=  %i  (%i, %i)",evt_data.n_det_i[1], evt_data.n_side_i[1][0], evt_data.n_side_i[1][1]);
  printf("\n  *EVT*i       N det2=  %i  (%i, %i)",evt_data.n_det_i[2], evt_data.n_side_i[2][0], evt_data.n_side_i[2][1]);
  printf("\n  *EVT*i       N det3=  %i  (%i, %i)",evt_data.n_det_i[3], evt_data.n_side_i[3][0], evt_data.n_side_i[3][1]);
  printf("\n  *EVT*i  Position     : ");
  printf("\n  *EVT*i       X det0=  %i   Y det0= %i",evt_data.x_i[0], evt_data.y_i[0]);
  printf("\n  *EVT*i       X det1=  %i   Y det1= %i",evt_data.x_i[1], evt_data.y_i[1]);
  printf("\n  *EVT*i       X det2=  %i   Y det2= %i",evt_data.x_i[2], evt_data.y_i[2]);
  printf("\n  *EVT*i       X det3=  %i   Y det3= %i",evt_data.x_i[3], evt_data.y_i[3]);
  printf("\n  *EVT*i  Energy       : ");
  printf("\n  *EVT*i       En det0=  %li   Ep det0= %li",evt_data.e_i[0][0], evt_data.e_i[0][1]);
  printf("\n  *EVT*i       En det1=  %li   Ep det1= %li",evt_data.e_i[1][0], evt_data.e_i[1][1]);
  printf("\n  *EVT*i       En det2=  %li   Ep det2= %li",evt_data.e_i[2][0], evt_data.e_i[2][1]);
  printf("\n  *EVT*i       En det3=  %li   Ep det3= %li",evt_data.e_i[3][0], evt_data.e_i[3][1]);

  printf("\n  *EVT*d       N det0=  %i  (%i, %i)",evt_data.n_det_d[0], evt_data.n_side_d[0][0], evt_data.n_side_d[0][1]);
  printf("\n  *EVT*d       N det1=  %i  (%i, %i)",evt_data.n_det_d[1], evt_data.n_side_d[1][0], evt_data.n_side_d[1][1]);
  printf("\n  *EVT*d       N det2=  %i  (%i, %i)",evt_data.n_det_d[2], evt_data.n_side_d[2][0], evt_data.n_side_d[2][1]);
  printf("\n  *EVT*d       N det3=  %i  (%i, %i)",evt_data.n_det_d[3], evt_data.n_side_d[3][0], evt_data.n_side_d[3][1]);
  printf("\n  *EVT*d  Position     : ");
  printf("\n  *EVT*d       X det0=  %i   Y det0= %i",evt_data.x_d[0], evt_data.y_d[0]);
  printf("\n  *EVT*d       X det1=  %i   Y det1= %i",evt_data.x_d[1], evt_data.y_d[1]);
  printf("\n  *EVT*d       X det2=  %i   Y det2= %i",evt_data.x_d[2], evt_data.y_d[2]);
  printf("\n  *EVT*d       X det3=  %i   Y det3= %i",evt_data.x_d[3], evt_data.y_d[3]);
  printf("\n  *EVT*d  Energy       : ");
  printf("\n  *EVT*d       En det0=  %li   Ep det0= %li",evt_data.e_d[0][0], evt_data.e_d[0][1]);
  printf("\n  *EVT*d       En det1=  %li   Ep det1= %li",evt_data.e_d[1][0], evt_data.e_d[1][1]);
  printf("\n  *EVT*d       En det2=  %li   Ep det2= %li",evt_data.e_d[2][0], evt_data.e_d[2][1]);
  printf("\n  *EVT*d       En det3=  %li   Ep det3= %li",evt_data.e_d[3][0], evt_data.e_d[3][1]);


  printf("\n  *EVT*   Time         : ");
  printf("\n  *EVT*        t0=  %li      dt=  %li",evt_data.t0, evt_data.dt);
  printf("\n  *EVT*    t0_ext=  %lX",evt_data.t0_ext);

  if(evt_data.decay_flag)  printf("\n  *EVT*   Type:  decay \n");
  else printf("\n  *EVT*   Type:  implant \n");
  **************/
}


// called from main.cpp to write objects to Root file
void Analysis::Close(){
  if(GetBHistograms()) WriteHistograms();

  if(GetBRootTree()) out_root_tree->Write(); 

}


void Analysis::SetBDebug(bool flag){
  b_debug= flag;
}
void Analysis::SetBHistograms(bool flag){
  b_histograms= flag;
}
//void Analysis::SetBPushData(bool flag){
//  b_push_data= flag;
//}

void Analysis::SetBRootTree(bool flag){
  b_root_tree= flag;
}










  //Getters...
double Analysis::GetEventTimeWindow(){ return event_time_window; }

//bool Analysis::GetBDebug(){ return b_debug; }
bool Analysis::GetBHistograms(){ return b_histograms; }
//bool Analysis::GetBPushData(){ return b_push_data; }
bool Analysis::GetBRootTree(){ return b_root_tree; }

//fS
int Analysis::GetMultiplicity(){ return 1; /*evt_data.multiplicity;*/ }





Analysis::Analysis(){


  b_debug = false;
  b_histograms= false;

  b_root_tree= false;

  b_pulser= false;

  event_count= 0;
  t_low_prev= 0;
  t_high_prev= 0;
  t_disc_prev= 0;

  event_time_window = 3200;

  p_opt_tm_stp= 0;

  p_N_MAX[0]=6;
  p_N_MAX[1]=12;
  p_DELTA_MAX=3;
  p_N_DET_MAX=3;

  /********8
  hit.t= -1;
  hit.t_ext= -1;
  hit.x= -1;
  hit.y= -1;
  hit.z= -1;
  hit.ex= -1;
  hit.ey= -1;
  hit.flag= -1;
  **********/

  //dE_i_lim= 2000;
  //dX_i_lim= 15;
  //E_i_min= 300;

  //dE_d_lim= 3000;
  //dX_d_lim= 5;
  //E_d_min= 150;
  //E_d_max= 3000;

  //FS:   evt_data.multiplicity= 0;
  //for(int i=0;i<common::N_DSSD;i++){
    //
    //    e_sum_d[i][0]=0;
    //e_sum_d[i][1]=0;
    //

    /*******
    evt_data.n_det_i[i]=0;  
    evt_data.n_side_i[i][0]=0;  
    evt_data.n_side_i[i][1]=0;  
    evt_data.x_i[i]=-999;  
    evt_data.y_i[i]=-999;  
    evt_data.e_i[i][0]=-99999;  
    evt_data.e_i[i][1]=-99999;  

    evt_data.n_det_d[i]=0;  
    evt_data.n_side_d[i][0]=0;  
    evt_data.n_side_d[i][1]=0;  
    evt_data.x_d[i]=-999;  
    evt_data.y_d[i]=-999;  
    evt_data.e_d[i][0]=-99999;  
    evt_data.e_d[i][1]=-99999;  

    evt_data.dx_d[i]=-1;  
    evt_data.dy_d[i]=-1;  


    evt_data.t0= -99999;
    evt_data.t0_ext= -99999;
    evt_data.dt= 0;
    *********/ 
    //}
  //  evt_data.decay_flag= 0;
  //evt_data.implant_flag= 0;



}


