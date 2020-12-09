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
//heding bug
//Encoder headingBugEnc(10, 11);+
Encoder headingBugEnc;
short headingBugPrev = 0;
FlightSimFloat headingBug;
elapsedMillis headingBugClickInterval = 0;

void setup() {
  Serial.begin(57600);

  taxiLightSwitch.setOnOffCommands(
    XPlaneRef("sim/lights/taxi_lights_on"),
    XPlaneRef("sim/lights/taxi_lights_off"));

  landingLightSwitch.setOnOffCommands(
    XPlaneRef("sim/lights/landing_lights_on"),
    XPlaneRef("sim/lights/landing_lights_off"));



   // delay(1000);
  // set pushbutton 1 command
 pb1 = XPlaneRef("RXP/GNS/PROC_1");

//encoder
headingBugEnc.begin(10,11,CountMode::half);

 headingBug = XPlaneRef("sim/cockpit2/autopilot/heading_dial_deg_mag_pilot");

  switches.setDebug(DEBUG_SWITCHES);
  switches.begin();
}

void loop() {
  FlightSim.update();

//encoder stuff

  // compare current position to previous position

//current position
//short headingBugPos=headingBugEnc.read();
short headingBugPos=headingBugEnc.getValue();

//only activate when bug is divisible by 2 (one full click)

 short headingBugDiff;

// if(headingBugPos%2==0)
if(headingBugPos)
 {
  headingBugDiff = (headingBugPos - headingBugPrev);

    if(headingBugDiff <0)
      headingBugDiff=-1;

    if(headingBugDiff >0)
      headingBugDiff=1;  
//
      if(headingBugClickInterval < 30)
       headingBugDiff=headingBugDiff*5;
 }
 else
 {
  headingBugDiff=0;
 }

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

    headingBugDiff=0;

  Serial.println((String) "headingBug: " + headingBug);
   Serial.println((String) "-------------------------");

   //reset positions
   headingBugPrev=0;
//   headingBugEnc.write(0);
headingBugEnc.setValue(0);
   headingBugClickInterval=0;
    }
 //end encoder
 
  switches.loop();
}
