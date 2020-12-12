#include "EncoderTool.h"
using namespace EncoderTool;

//EncoderTool eliminates bounce in the encoder.
//Use instead of built in Encoder library.
//Requires updated Bounce2 library for Teensy.
//https://github.com/luni64/EncoderTool

#include <FlightSimSwitches.h>
#include <math.h>  

//switches
FlightSimSwitches switches;
FlightSimOnOffCommandSwitch taxiLightSwitch(2);
FlightSimOnOffCommandSwitch landingLightSwitch(5);

// Push buttons
FlightSimPushbutton pb1(13); // Pushbutton between pin 12 and GND

//GPS Pushbuttons
FlightSimPushbutton  pbComVol(23);

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

void setup() {
  Serial.begin(57600);

  taxiLightSwitch.setOnOffCommands(
    XPlaneRef("sim/lights/taxi_lights_on"),
    XPlaneRef("sim/lights/taxi_lights_off"));

  landingLightSwitch.setOnOffCommands(
    XPlaneRef("sim/lights/landing_lights_on"),
    XPlaneRef("sim/lights/landing_lights_off"));

  switches.setDebug(DEBUG_SWITCHES);
  switches.begin();

    delay(1000);
  // set pushbutton 1 command
 pb1 = XPlaneRef("RXP/GNS/ENT_1");

 //GPS pushbuttons
 pbComVol= XPlaneRef("RXP/GNS/COM_PUSH_1");

//---------------------------------------------------------------------
//encoders

  //heading bug
  headingBugEnc.begin(13,14,CountMode::half);
  headingBug = XPlaneRef("sim/cockpit2/autopilot/heading_dial_deg_mag_pilot");

  //altimeter
  baroSettingEnc.begin(15,16,CountMode::half);
  baroSetting= XPlaneRef("sim/cockpit/misc/barometer_setting");
  baroSettingG5= XPlaneRef("sim/cockpit2/gauges/actuators/barometer_setting_in_hg_stby");

  //com outer
  comOuterEnc.begin(21,22,CountMode::half);
  comOuterCW=XPlaneRef("RXP/GNS/COM_OUTR_CW_1");
  comOuterCCW=XPlaneRef("RXP/GNS/COM_OUTR_CCW_1");

  //com inner
  comInnerEnc.begin(19,20,CountMode::half);
  comInnerCW=XPlaneRef("RXP/GNS/COM_INNR_CW_1");
  comInnerCCW=XPlaneRef("RXP/GNS/COM_INNR_CCW_1");

//end encoders


}

void loop() {
  FlightSim.update();



//encoder stuff
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
      if(headingBugClickInterval < 30)
       headingBugDiff=headingBugDiff*5;
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
 
  switches.loop();
}
