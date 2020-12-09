//#include <Encoder.h>
#include "EncoderTool.h"
using namespace EncoderTool;

#include <FlightSimSwitches.h>
#include <math.h>  

//switches
FlightSimSwitches switches;
FlightSimOnOffCommandSwitch taxiLightSwitch(2);
FlightSimOnOffCommandSwitch landingLightSwitch(5);

// Push buttons
FlightSimPushbutton pb1(12); // Pushbutton between pin 12 and GND

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

   // delay(1000);
  // set pushbutton 1 command
 pb1 = XPlaneRef("RXP/GNS/PROC_1");

//encoders

  //heading bug
  headingBugEnc.begin(13,14,CountMode::half);
  headingBug = XPlaneRef("sim/cockpit2/autopilot/heading_dial_deg_mag_pilot");

  //altimeter
  baroSettingEnc.begin(10,11,CountMode::half);
  baroSetting= XPlaneRef("sim/cockpit/misc/barometer_setting");
  baroSettingG5= XPlaneRef("sim/cockpit2/gauges/actuators/barometer_setting_in_hg_stby");

//end encoders


}

void loop() {
  FlightSim.update();

//encoder stuff

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
  
 //end encoder
 
  switches.loop();
}
