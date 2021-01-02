#include "EncoderTool.h"
using namespace EncoderTool;

//EncoderTool eliminates bounce in the encoder.
//Use instead of built in Encoder library.
//Requires updated Bounce2 library for Teensy.
//https://github.com/luni64/EncoderTool

#include <FlightSimSwitches.h>
#include <math.h>  
#include <Keypad.h>

//switches
FlightSimSwitches switches;

// Push buttons
//FlightSimPushbutton pb1(13); // Pushbutton between pin 13 and GND


//encoders
//heading bug
Encoder headingBugEnc;
short headingBugPrev = 0;
FlightSimFloat headingBug;
elapsedMillis headingBugClickInterval = 0;

//altimeter adjustment
Encoder baroSettingEnc;
short baroSettingPrev=0;
FlightSimFloat baroSetting;
FlightSimFloat baroSettingG5;
elapsedMillis baroSettingClickInterval = 0;

//G530 comOuter
Encoder comOuterEnc;
short comOuterPrev=0;
FlightSimCommand comOuterCW;
FlightSimCommand comOuterCCW;

//G530 comInner
Encoder comInnerEnc;
short comInnerPrev=0;
FlightSimCommand comInnerCW;
FlightSimCommand comInnerCCW;

//G530 gpsOuter
Encoder gpsOuterEnc;
short gpsOuterPrev=0;
FlightSimCommand gpsOuterCW;
FlightSimCommand gpsOuterCCW;

//G530 gpsInner
Encoder gpsInnerEnc;
short gpsInnerPrev=0;
FlightSimCommand gpsInnerCW;
FlightSimCommand gpsInnerCCW;

//GPS Pushbuttons
FlightSimCommand comFlip;
FlightSimCommand navFlip;
FlightSimCommand comPush;

FlightSimCommand pbCDI;
FlightSimCommand pbOBS;
FlightSimCommand pbMSG;
FlightSimCommand pbFPL;
FlightSimCommand pbVNAV;
FlightSimCommand pbPROC;

FlightSimCommand pbRUP;
FlightSimCommand pbRDN;
FlightSimCommand pbDTO;
FlightSimCommand pbMNU;
FlightSimCommand pbCLR;
FlightSimCommand pbENT;
FlightSimCommand gpsPush;

//keypad
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','4'},
  {'5','6','7','8'},
  {'9','a','b','c'},
  {'d','e','f','g'}
};
byte rowPins[ROWS] = {10,11,12,18}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {14,15,16,17}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


void setup() {
  Serial.begin(57600);


  switches.setDebug(DEBUG_SWITCHES);
  switches.begin();

    delay(1000);
  // set pushbutton 1 command
 //pb1 = XPlaneRef("RXP/GNS/ENT_1");

  //GPS pushbuttons
  comFlip=XPlaneRef("RXP/GNS/CFLP_1");
  navFlip=XPlaneRef("RXP/GNS/VFLP_1");
  comPush= XPlaneRef("RXP/GNS/COM_PUSH_1");

  pbCDI=XPlaneRef("RXP/GNS/CDI_SOURCE_1");
  pbOBS=XPlaneRef("RXP/GNS/OBS_MODE_1");
  pbMSG=XPlaneRef("RXP/GNS/MSG_1");
  pbFPL=XPlaneRef("RXP/GNS/FPL_1");
  pbVNAV=XPlaneRef("RXP/GNS/VNAV_1");
  pbPROC=XPlaneRef("RXP/GNS/PROC_1");
  pbRUP=XPlaneRef("RXP/GNS/RUP_1");
  pbRDN=XPlaneRef("RXP/GNS/RDN_1");
  pbDTO=XPlaneRef("RXP/GNS/DTO_1");
  pbMNU=XPlaneRef("RXP/GNS/MNU_1");
  pbCLR= XPlaneRef("RXP/GNS/CLR_1");
  pbENT=XPlaneRef("RXP/GNS/ENT_1");
  gpsPush=XPlaneRef("RXP/GNS/GPS_PUSH_1");

//---------------------------------------------------------------------
//encoders

  //heading bug
  headingBugEnc.begin(20,21,CountMode::half);
  headingBug = XPlaneRef("sim/cockpit2/autopilot/heading_dial_deg_mag_pilot");

  //altimeter
  baroSettingEnc.begin(22,23,CountMode::half);
  baroSetting= XPlaneRef("sim/cockpit/misc/barometer_setting");
  baroSettingG5= XPlaneRef("sim/cockpit2/gauges/actuators/barometer_setting_in_hg_stby");

  //com outer
  comOuterEnc.begin(9,8,CountMode::half);
  comOuterCW=XPlaneRef("RXP/GNS/COM_OUTR_CW_1");
  comOuterCCW=XPlaneRef("RXP/GNS/COM_OUTR_CCW_1");

  //com inner
  comInnerEnc.begin(7,6,CountMode::half);
  comInnerCW=XPlaneRef("RXP/GNS/COM_INNR_CW_1");
  comInnerCCW=XPlaneRef("RXP/GNS/COM_INNR_CCW_1");

  //gps outer
  gpsOuterEnc.begin(2,3,CountMode::half);
  gpsOuterCW=XPlaneRef("RXP/GNS/GPS_OUTR_CW_1");
  gpsOuterCCW=XPlaneRef("RXP/GNS/GPS_OUTR_CCW_1");

  //gps inner
  gpsInnerEnc.begin(5,4  ,CountMode::half);
  gpsInnerCW=XPlaneRef("RXP/GNS/GPS_INNR_CW_1");
  gpsInnerCCW=XPlaneRef("RXP/GNS/GPS_INNR_CCW_1");

//end encoders


}

void loop() {
  FlightSim.update();



//encoder stuff
#pragma region GPS
  //-------------------------------------------
  //   GPS INNER
  //-------------------------------------------
  // compare current position to previous position

  //current position
  short gpsInnerPos=gpsInnerEnc.getValue();
  short gpsInnerDiff;
  
  if(gpsInnerPos)
   {
      gpsInnerDiff = (gpsInnerPos - gpsInnerPrev);
  
      if(gpsInnerDiff <0)
        gpsInnerDiff=-1;
  
      if(gpsInnerDiff >0)
        gpsInnerDiff=1;  
        
  
      //if you spin faster it moves faster
//      if(baroSettingClickInterval < 30)
//       baroSettingDiff=baroSettingDiff*2;
   }
   else
   {
    gpsInnerDiff=0;
   }
  
  //print output for debugging
  if(gpsInnerPos!=gpsInnerPrev)
  { 
   Serial.println((String) "gpsInnerDiff: " + gpsInnerDiff);
   Serial.println((String) "gpsInnerPos: " + gpsInnerPos);
   Serial.println((String) "gpsInnerDiff: " + gpsInnerDiff);
  }

 
   // if there was movement, change the dataref
   if (gpsInnerDiff!=0) {
      if(gpsInnerDiff==-1){
      gpsInnerCCW.once();
      Serial.println((String) "direction: CCW");
      }

    if(gpsInnerDiff==1){
      gpsInnerCW.once();
      Serial.println((String) "direction: CW");
          }

    //reset
    gpsInnerDiff=0;

    //debug output what we set it to
    Serial.println((String) "gpsInner: " + gpsInnerDiff);
    Serial.println((String) "-------------------------");

    //reset positions
    gpsInnerPrev=0;
    gpsInnerEnc.setValue(0);

    
    

    //reset clock to 0
//    baroSettingClickInterval=0;
    }

  //-------------------------------------------
  //   END GPS INNER
  //-------------------------------------------
  //-------------------------------------------
  //   GPS OUTER
  //-------------------------------------------
  // compare current position to previous position

  //current position
  short gpsOuterPos=gpsOuterEnc.getValue();
  short gpsOuterDiff;
  
  if(gpsOuterPos)
   {
      gpsOuterDiff = (gpsOuterPos - gpsOuterPrev);
  
      if(gpsOuterDiff <0)
        gpsOuterDiff=-1;
  
      if(gpsOuterDiff >0)
        gpsOuterDiff=1;  
        
  
      //if you spin faster it moves faster
//      if(baroSettingClickInterval < 30)
//       baroSettingDiff=baroSettingDiff*2;
   }
   else
   {
    gpsOuterDiff=0;
   }
  
  //print output for debugging
  if(gpsOuterPos!=gpsOuterPrev)
  { 
   Serial.println((String) "gpsOuterDiff: " + gpsOuterDiff);
   Serial.println((String) "gpsOuterPos: " + gpsOuterPos);
   Serial.println((String) "gpsOuterDiff: " + gpsOuterDiff);
  }

 
   // if there was movement, change the dataref
   if (gpsOuterDiff!=0) {
      if(gpsOuterDiff==-1){
      gpsOuterCCW.once();
      Serial.println((String) "direction: CCW");
      }

    if(gpsOuterDiff==1){
      gpsOuterCW.once();
      Serial.println((String) "direction: CW");
          }

    //reset
    gpsOuterDiff=0;

    //debug output what we set it to
    Serial.println((String) "gpsOuter: " + gpsOuterDiff);
    Serial.println((String) "-------------------------");

    //reset positions
    gpsOuterPrev=0;
    gpsOuterEnc.setValue(0);

    
    

    //reset clock to 0
//    baroSettingClickInterval=0;
    }

  //-------------------------------------------
  //   END GPS OUTER
  //-------------------------------------------  
#pragma endregion GPS
#pragma region COM
  //-------------------------------------------
  //   COM INNER
  //-------------------------------------------
  // compare current position to previous position

  //current position
  short comInnerPos=comInnerEnc.getValue();
  short comInnerDiff;
  
  if(comInnerPos)
   {
      comInnerDiff = (comInnerPos - comInnerPrev);
  
      if(comInnerDiff <0)
        comInnerDiff=-1;
  
      if(comInnerDiff >0)
        comInnerDiff=1;  
        
  
      //if you spin faster it moves faster
//      if(baroSettingClickInterval < 30)
//       baroSettingDiff=baroSettingDiff*2;
   }
   else
   {
    comInnerDiff=0;
   }
  
  //print output for debugging
  if(comInnerPos!=comInnerPrev)
  { 
   Serial.println((String) "comInnerDiff: " + comInnerDiff);
   Serial.println((String) "comInnerPos: " + comInnerPos);
   Serial.println((String) "comInnerDiff: " + comInnerDiff);
  }

 
   // if there was movement, change the dataref
   if (comInnerDiff!=0) {
      if(comInnerDiff==-1){
      comInnerCCW.once();
      Serial.println((String) "direction: CCW");
      }

    if(comInnerDiff==1){
      comInnerCW.once();
      Serial.println((String) "direction: CW");
          }

    //reset
    comInnerDiff=0;

    //debug output what we set it to
    Serial.println((String) "comInner: " + comInnerDiff);
    Serial.println((String) "-------------------------");

    //reset positions
    comInnerPrev=0;
    comInnerEnc.setValue(0);

    
    

    //reset clock to 0
//    baroSettingClickInterval=0;
    }

  //-------------------------------------------
  //   END COM INNER
  //-------------------------------------------
  //-------------------------------------------
  //   COM OUTER
  //-------------------------------------------
  // compare current position to previous position

  //current position
  short comOuterPos=comOuterEnc.getValue();
  short comOuterDiff;
  
  if(comOuterPos)
   {
      comOuterDiff = (comOuterPos - comOuterPrev);
  
      if(comOuterDiff <0)
        comOuterDiff=-1;
  
      if(comOuterDiff >0)
        comOuterDiff=1;  
        
  
      //if you spin faster it moves faster
//      if(baroSettingClickInterval < 30)
//       baroSettingDiff=baroSettingDiff*2;
   }
   else
   {
    comOuterDiff=0;
   }
  
  //print output for debugging
  if(comOuterPos!=comOuterPrev)
  { 
   Serial.println((String) "comOuterDiff: " + comOuterDiff);
   Serial.println((String) "comOuterPos: " + comOuterPos);
   Serial.println((String) "comOuterDiff: " + comOuterDiff);
  }

 
   // if there was movement, change the dataref
   if (comOuterDiff!=0) {
      if(comOuterDiff==-1){
      comOuterCCW.once();
      Serial.println((String) "direction: CCW");
      }

    if(comOuterDiff==1){
      comOuterCW.once();
      Serial.println((String) "direction: CW");
          }

    //reset
    comOuterDiff=0;

    //debug output what we set it to
    Serial.println((String) "comOuter: " + comOuterDiff);
    Serial.println((String) "-------------------------");

    //reset positions
    comOuterPrev=0;
    comOuterEnc.setValue(0);

    
    

    //reset clock to 0
//    baroSettingClickInterval=0;
    }

  //-------------------------------------------
  //   END COM OUTER
  //-------------------------------------------  
#pragma endregion COM
  
#pragma region G5
  //-------------------------------------------
  //   HEADING BUG
  //-------------------------------------------
  // compare current position to previous position

  //current position
  short headingBugPos=headingBugEnc.getValue();
  short headingBugDiff;
  
  if(headingBugPos)
   {
      headingBugDiff = (headingBugPos - headingBugPrev);
  
      if(headingBugDiff <0)
        headingBugDiff=-1;
  
      if(headingBugDiff >0)
        headingBugDiff=1;  
  
      //if you spin faster it moves faster
      if(headingBugClickInterval < 250)
       headingBugDiff=headingBugDiff*10;
   }
   else
   {
    headingBugDiff=0;
   }
  
  //print output for debugging
  if(headingBugPos!=headingBugPrev)
  { 
   
   Serial.println((String) "headingBugPos: " + headingBugPos);
   Serial.println((String) "headingBugDiff: " + headingBugDiff);
  }

   // update previous position
   headingBugPrev = headingBugPos;
 
   // if there was movement, change the dataref
   if (headingBugDiff!=0) {
      headingBug = headingBug + headingBugDiff;

    //reset
    headingBugDiff=0;

    //debug output what we set it to
    Serial.println((String) "headingBug: " + headingBug);
    Serial.println((String) "-------------------------");

    //reset positions
    headingBugPrev=0;

    headingBugEnc.setValue(0);

    //reset clock to 0
    headingBugClickInterval=0;
    }

  //-------------------------------------------
  //   END HEADING BUG
  //-------------------------------------------    


  //-------------------------------------------
  //   ALTIMETER
  //-------------------------------------------
  // compare current position to previous position

  //current position
  float baroSettingPos=baroSettingEnc.getValue();
  float baroSettingDiff;
  
  if(baroSettingPos)
   {
      baroSettingDiff = (baroSettingPos - baroSettingPrev);
  
      if(baroSettingDiff <0)
        baroSettingDiff=-0.01;
  
      if(baroSettingDiff >0)
        baroSettingDiff=0.01;  
  
      //if you spin faster it moves faster
      if(baroSettingClickInterval < 30)
       baroSettingDiff=baroSettingDiff*2;
   }
   else
   {
    baroSettingDiff=0;
   }
  
  //print output for debugging
  if(baroSettingPos!=baroSettingPrev)
  { 
   
   Serial.println((String) "baroSettingPos: " + baroSettingPos);
   Serial.println((String) "baroSettingDiff: " + baroSettingDiff);
  }

   // update previous position
   baroSettingPrev = baroSettingPos;
 
   // if there was movement, change the dataref
   if (baroSettingDiff!=0) {
      baroSetting = baroSetting + baroSettingDiff;
      baroSettingG5=baroSetting+0;

    //reset
    baroSettingDiff=0;

    //debug output what we set it to
    Serial.println((String) "baroSetting: " + baroSetting);
    Serial.println((String) "-------------------------");

    //reset positions
    baroSettingPrev=0;
    baroSettingEnc.setValue(0);

    //reset clock to 0
    baroSettingClickInterval=0;
    }

  //-------------------------------------------
  //   END ALTIMETER
  //-------------------------------------------  
#pragma endregion G5
 //end encoder
 
  //switches.loop();
delay(100);
  char key = keypad.getKey();
  
  if (key != NO_KEY){
    Serial.println(key);
    Serial.println(stricmp('2',key));

      if(key=='1'){
      pbMSG.once();
      Serial.println((String) "button: MSG");
      }  
    
      if(key=='2'){
      pbFPL.once();
      Serial.println((String) "button: FPL");
      }  
      if(key=='3'){
      pbCDI.once();
      Serial.println((String) "button: CDI");
      }  
      if(key=='4'){
      pbOBS.once();
      Serial.println((String) "button: OBS");
      }  
      if(key=='5'){
      pbMNU.once();
      Serial.println((String) "button: MNU");
      }  
      if(key=='6'){
      pbDTO.once();
      Serial.println((String) "button: DTO");
      }  
      if(key=='7'){
      pbPROC.once();
      Serial.println((String) "button: PROC");
      }  
      if(key=='8'){
      pbVNAV.once();
      Serial.println((String) "button: VNAV");
      }  
      if(key=='9'){
      gpsPush.once();
      Serial.println((String) "button: gpsPush");
      }  
      if(key=='a'){
      comPush.once();
      Serial.println((String) "button: comPush");
      }  
      if(key=='b'){
      pbENT.once();
      Serial.println((String) "button: ENT");
      }  
      if(key=='c'){
      pbCLR.once();
      Serial.println((String) "button: CLR");
      }  
      if(key=='d'){
      pbRDN.once();
      Serial.println((String) "button: RUP");
      }  
      if(key=='e'){
      pbRUP.once();
      Serial.println((String) "button: RDN");
      }  
      if(key=='f'){
      comFlip.once();
      Serial.println((String) "button: comFlip");
      }  
      if(key=='g'){
      navFlip.once();
      Serial.println((String) "button: navFlip");
      }  
  }
  
}
