
//<File !Start!>
// FILE: [brygg.ino]
// Created by GUIslice Builder version: [0.13.b014]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
//

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include <GUIslice.h>
#include <GUIslice_drv.h>

// Include any extended elements
//<Includes !Start!>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "WiFiManager.h"
#include <ESP8266mDNS.h>
#include "ESP8266HTTPUpdateServer_edited.h"
#include "CountUpDownTimer.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <math.h>
#include "ESP8266FtpServer.h"
#include <ArduinoJson.h>

//include webpages
//#include "css.h"
//<Includes !End!>

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
//<Fonts !Start!>
#if defined(DRV_DISP_TFT_ESPI)
  #error Builder config "Edit->Options->General->Target Platform" should be "arduino TFT_eSPI"
#endif 
#include <Adafruit_GFX.h>
// Note that these files are located within the Adafruit-GFX library folder:
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSansBold18pt7b.h"
#include "Fonts/FreeSerifBold12pt7b.h"
#include "Fonts/FreeSerifBold18pt7b.h"
#include "Fonts/FreeSerifBold24pt7b.h"
//<Fonts !End!>

// ------------------------------------------------
// Defines for resources
// ------------------------------------------------
//<Resources !Start!>
int debug = 0; //if 1 debug is enabled which means buzzer is disabled and tx port i free to send over serial
void playButtonPressed(int fromButton);
void stepConfigButtonPressed();
void handleHeaters();
void handlePump();
void handleElapsedTimer();
void setTargetTime();
void setTargetTempFunc();
void plusButtonPressed();
void minusButtonPressed();
void tempButtonPressed();
void autoButtonPressed();
void playBuzzer();
void changeSetTempDisplay();
void changeSetTimeDisplay();
void handleTimerFinished();
void configModeCallback();
void saveConfigFile();

//set variables for brewer
int autoOrMan = 0; //0 = man 1 = auto
int selectedStep = 1; //0 = no step selected steps avaliable 1 - 6
int runningStep = 1; //0 = no step is running steps avaliable 1 - 6
int actTemp = 0; //Actual temp
String actTempDisplay = "000"; //actual temp on display
int targetTemp = 0; //Target temp
int manTargetTemp = 0; //Manual tearget temp
int setTargetTemp = 0;
int tempTargetTemp = 0;
int actTimerPaused = 0; //0 = timer not paused 1 = timer paused
String actTimeMin = "00"; //Time elapsed minutes
String actTimeSec = "00"; //Time elapse hours
int targetTimeMin = 0;
int targetTimeSec = 0;
int manTargetTimeMin = 0;
int tempTargetTimeMin = 0;
int manTargetTimeSec = 0;
int setTimeMin = 0; //if 1 set target min active
int manPump = 0;
int manNext = 0;
int playPauseStop = 0; //0 = stopped 1 = started 2 = paused
int stepConfigMode = 0; //0 = configmode deactivated 1 = configmode activated
int stepConfigMinusPump = 0; //0 = show pump text 1 = -
int stepConfigPlusNext = 0; //0 = show next text 1 = + 
String valueTimer = "null"; //Timer check if start stop or pause


//step 1
int s1TargetTimeMin = 0;
int s1TargetTimeSec = 0;
int s1TargetTemp = 10;
int s1Pump = 0;
int s1Next = 0;

//step 2
int s2TargetTimeMin = 0;
int s2TargetTimeSec = 0;
int s2TargetTemp = 13;
int s2Pump = 0;
int s2Next = 0;

//step 3
int s3TargetTimeMin = 0;
int s3TargetTimeSec = 0;
int s3TargetTemp = 00;
int s3Pump = 0;
int s3Next = 0;

//step 4
int s4TargetTimeMin = 0;
int s4TargetTimeSec = 0;
int s4TargetTemp = 00;
int s4Pump = 0;
int s4Next = 0;

//step 5
int s5TargetTimeMin = 0;
int s5TargetTimeSec = 0;
int s5TargetTemp = 00;
int s5Pump = 0;
int s5Next = 0;

//step 6
int s6TargetTimeMin = 0;
int s6TargetTimeSec = 0;
int s6TargetTemp = 00;
int s6Pump = 0;
int s6Next = 0;

//setup tempsensor ds18b20
const int oneWireBus = 3; 
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

//setup buzzer
int buzzer = D1; //Buzzer control port 

//setup relays for heaters
int relay1 = D4; //pin for relay1 500w D1
int relay2 = D2; //pin for relay2 1900w D2
int relay3 = 1; //pin for relay3 Pump 

//Timers
CountUpDownTimer countElapsedTimer(UP, HIGH);

//Delay
unsigned long previousMillis = 0;
const long interval = 5000;

//define settings file stored in flash
#define BREWER_CONFIG  "/brewerConfig.json"

//Webserver
const char* host = "BeerBrewer";
//void handleRoot();
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;
WiFiManager wifiManager;
//<Resources !End!>

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
//<Enum !Start!>
enum {E_PG_MAIN,E_PG_AP};
enum {E_ELEM_ActTempTxt,E_ELEM_ActTimeTxt,E_ELEM_AutoBtn
      ,E_ELEM_MinusBtn,E_ELEM_PlayBtn,E_ELEM_PlusBtn,E_ELEM_StepBtn
      ,E_ELEM_S6,E_ELEM_S1,E_ELEM_S2,E_ELEM_S3
      ,E_ELEM_S4,E_ELEM_S5,E_ELEM_TargetTempTxt
      ,E_ELEM_TargetTimeTxt,E_ELEM_TempBtn,E_ELEM_TimeBtn,E_ELEM_APMode,E_ELEM_Header,E_ELEM_IPTxt};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_FONT_SANS9,E_FONT_SANSB18,E_FONT_SERIFB12,E_FONT_SERIFB18,E_FONT_SERIFB24
      ,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                2

#define MAX_ELEM_PG_MAIN 17 // # Elems total on page
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN // # Elems in RAM
#define MAX_ELEM_PG_AP 3 // # Elems total on page
#define MAX_ELEM_PG_AP_RAM MAX_ELEM_PG_AP // # Elems in RAM
//<ElementDefines !End!>

// ------------------------------------------------
// Create element storage
// ------------------------------------------------
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

//<GUI_Extra_Elements !Start!>
gslc_tsElem                     m_asPage1Elem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_PG_MAIN];
gslc_tsElem                     m_asPage2Elem[MAX_ELEM_PG_AP_RAM];
gslc_tsElemRef                  m_asPage2ElemRef[MAX_ELEM_PG_AP];

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef*  m_pElemActTempTxt = NULL;
gslc_tsElemRef*  m_pElemActTimeTxt = NULL;
gslc_tsElemRef*  m_pElemAutoBtn    = NULL;
gslc_tsElemRef*  m_pElemPlayBtn    = NULL;
gslc_tsElemRef*  m_pElemTargetTempTxt= NULL;
gslc_tsElemRef*  m_pElemTargetTimeTxt= NULL;
gslc_tsElemRef*  m_pElemTempBtn    = NULL;
gslc_tsElemRef*  m_pElemTimeBtn    = NULL;
gslc_tsElemRef*  m_pElemMinusBtn = NULL;
gslc_tsElemRef*  m_pElemPlusBtn = NULL;
gslc_tsElemRef*  m_pElemS1 = NULL;
gslc_tsElemRef*  m_pElemS2 = NULL;
gslc_tsElemRef*  m_pElemS3 = NULL;
gslc_tsElemRef*  m_pElemS4 = NULL;
gslc_tsElemRef*  m_pElemS5 = NULL;
gslc_tsElemRef*  m_pElemS6 = NULL;
//<Save_References !End!>

// Define debug message function
static int16_t DebugOut(char ch) { if (ch == (char)'\n') Serial.println(""); else Serial.write(ch); return 0; }
int m_nTimePressStart = 0;

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
// Common Button callback

bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = pElemRef->pElem;
  if (eTouch == GSLC_TOUCH_DOWN_IN) {
    m_nTimePressStart = millis();
  } else if ( eTouch == GSLC_TOUCH_UP_IN ) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
      //<Button Enums !Start!>
      case E_ELEM_TempBtn:
        tempButtonPressed();
        break;
      case E_ELEM_TimeBtn:
        if (setTimeMin == 0){
          setTimeMin = 1;
          gslc_ElemSetTxtStr(&m_gui, m_pElemMinusBtn, "-");
          gslc_ElemSetTxtStr(&m_gui, m_pElemPlusBtn, "+");
          gslc_ElemSetFillEn(&m_gui, m_pElemTimeBtn,true);
          gslc_ElemSetFillEn(&m_gui, m_pElemTempBtn,false);
          stepConfigMinusPump = 1;
          stepConfigPlusNext = 1;
        } else if (setTimeMin == 1){
          setTimeMin = 0;
          gslc_ElemSetTxtStr(&m_gui, m_pElemMinusBtn, "Pump");
          gslc_ElemSetTxtStr(&m_gui, m_pElemPlusBtn, "Next");
          gslc_ElemSetFillEn(&m_gui, m_pElemTimeBtn,false);
          stepConfigMinusPump = 0;
          stepConfigPlusNext = 0;
        }
        break;
      case E_ELEM_AutoBtn:
        autoButtonPressed();        
        break;
      case E_ELEM_MinusBtn:
        minusButtonPressed();
        break;
      case E_ELEM_PlusBtn:
        plusButtonPressed();
        break;
      case E_ELEM_PlayBtn:
        playButtonPressed(1);
        break;
      case E_ELEM_StepBtn:
        stepConfigButtonPressed();
        break;
      //<Button Enums !End!>
      default:
        break;
    }
  }
  return true;
}
// This is called if the WifiManager is in config mode (AP open)
void configModeCallback (WiFiManager *myWiFiManager) {  
  gslc_SetPageCur(&m_gui,E_PG_AP);
  
}
//button functions
void autoButtonPressed() {
  if (autoOrMan == 0){
    gslc_ElemSetTxtStr(&m_gui, m_pElemAutoBtn, "AUTO");
    if (selectedStep == 1){
      if (s1Pump == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
      }
      if (s1Next == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,true);
      }
      gslc_ElemSetTxtStr(&m_gui, m_pElemS1, "S1");
      changeSetTempDisplay(s1TargetTemp);
      changeSetTimeDisplay(s1TargetTimeMin);
    } else if (selectedStep == 2){
      if (s2Pump == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
      }
      if (s2Next == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,true);
      }
      gslc_ElemSetTxtStr(&m_gui, m_pElemS2, "S2");
      changeSetTempDisplay(s2TargetTemp);
      changeSetTimeDisplay(s2TargetTimeMin);
    } else if (selectedStep == 3){
      if (s3Pump == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
      }
      if (s3Next == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,true);
      }
      gslc_ElemSetTxtStr(&m_gui, m_pElemS3, "S3");
      changeSetTempDisplay(s3TargetTemp);
      changeSetTimeDisplay(s3TargetTimeMin);
    } else if (selectedStep == 4){
      if (s4Pump == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
      }
      if (s4Next == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,true);
      }
      gslc_ElemSetTxtStr(&m_gui, m_pElemS4, "S4");
      changeSetTempDisplay(s4TargetTemp);
      changeSetTimeDisplay(s4TargetTimeMin);
    } else if (selectedStep == 5){
      if (s5Pump == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
      }
      if (s5Next == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,true);
      }
      gslc_ElemSetTxtStr(&m_gui, m_pElemS5, "S5");
      changeSetTempDisplay(s5TargetTemp);
      changeSetTimeDisplay(s5TargetTimeMin);
    } else if (selectedStep == 6){
      if (s6Pump == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
      }
      if (s6Next == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,true);
      }
      gslc_ElemSetTxtStr(&m_gui, m_pElemS6, "S6");
      changeSetTempDisplay(s6TargetTemp);
      changeSetTimeDisplay(s6TargetTimeMin);
      }         
      autoOrMan = 1;
  } else if (autoOrMan == 1) {
    if (manPump == 0){
      gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
    } else {
      gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
    }
    gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
    gslc_ElemSetTxtStr(&m_gui, m_pElemAutoBtn, "MAN");
    gslc_ElemSetTxtStr(&m_gui, m_pElemS1, "");
    gslc_ElemSetTxtStr(&m_gui, m_pElemS2, "");
    gslc_ElemSetTxtStr(&m_gui, m_pElemS3, "");
    gslc_ElemSetTxtStr(&m_gui, m_pElemS4, "");
    gslc_ElemSetTxtStr(&m_gui, m_pElemS5, "");
    gslc_ElemSetTxtStr(&m_gui, m_pElemS6, "");
    changeSetTempDisplay(manTargetTemp);
    changeSetTimeDisplay(manTargetTimeMin);
    autoOrMan = 0;
  }
}
void tempButtonPressed() {
  int test = millis() - m_nTimePressStart;
    if (test >= 10000){
      Serial.println("Very loong");
      wifiManager.resetSettings();
      delay(3000);
      ESP.reset();
      delay(5000);
    } else {
      Serial.println("short");
      if (setTargetTemp == 0){
        setTargetTemp = 1;
        gslc_ElemSetFillEn(&m_gui, m_pElemTempBtn,true);
        gslc_ElemSetFillEn(&m_gui, m_pElemTimeBtn,false);
        gslc_ElemSetTxtStr(&m_gui, m_pElemMinusBtn, "-");
        gslc_ElemSetTxtStr(&m_gui, m_pElemPlusBtn, "+");
        stepConfigMinusPump = 1;
        stepConfigPlusNext = 1;
      } else if (setTargetTemp == 1){
        setTargetTemp = 0;
        gslc_ElemSetFillEn(&m_gui, m_pElemTempBtn,false);
        gslc_ElemSetTxtStr(&m_gui, m_pElemMinusBtn, "Pump");
        gslc_ElemSetTxtStr(&m_gui, m_pElemPlusBtn, "Next");
        stepConfigMinusPump = 0;
        stepConfigPlusNext = 0;
      }
    m_nTimePressStart = 0;
    }    
}
void plusButtonPressed(){
  if (stepConfigPlusNext == 0){ //handle next button
    if (autoOrMan == 0){
      //do nothing
    } else {
      if (selectedStep == 1){
        if (s1Next == 0) {
          s1Next = 1;
          gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,true);
        } else {
          s1Next = 0;
          gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
        }
      } else if (selectedStep == 2){
        if (s2Next == 0) {
          s2Next = 1;
          gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,true);
        } else {
          s2Next = 0;
          gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
        }
      } else if (selectedStep == 3){
        if (s3Next == 0) {
          s3Next = 1;
          gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,true);
        } else {
          s3Next = 0;
          gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
        }
      } else if (selectedStep == 4){
        if (s4Next == 0) {
          s4Next = 1;
          gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,true);
        } else {
          s4Next = 0;
          gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
        }
      } else if (selectedStep == 5){
        if (s5Next == 0) {
          s5Next = 1;
          gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,true);
        } else {
          s5Next = 0;
          gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
        }
      } else if (selectedStep == 6){
        if (s6Next == 0) {
          s6Next = 1;
          gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,true);
        } else {
          s6Next = 0;
          gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
        }
      }
    }
    saveConfigFile();
  } else {
    int test = millis() - m_nTimePressStart;
    if (test >= 500){
      // Long press
      Serial.println("long");
      if (setTimeMin == 1){        
        tempTargetTimeMin = tempTargetTimeMin + 10;
        setTargetTime();       
      } else if (setTargetTemp == 1){
          tempTargetTemp = tempTargetTemp + 10;
          setTargetTempFunc();
      }
    } else {
      Serial.println("short");
      if (setTimeMin == 1){
          tempTargetTimeMin = tempTargetTimeMin + 1;
          setTargetTime();
       }else if (setTargetTemp == 1){
          tempTargetTemp = tempTargetTemp + 1;
          setTargetTempFunc();
      }
      m_nTimePressStart = 0;
    }
    saveConfigFile();
  }  
}

void minusButtonPressed(){
  if (stepConfigMinusPump == 0){
    if (autoOrMan == 0){
      if (manPump == 0){
        manPump = 1;
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
      } else {
        manPump = 0;
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
      }
    } else {
      if (selectedStep == 1){
        if (s1Pump == 0) {
          s1Pump = 1;
          gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
        } else {
          s1Pump = 0;
          gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
        }
      } else if (selectedStep == 2){
        if (s2Pump == 0) {
          s2Pump = 1;
          gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
        } else {
          s2Pump = 0;
          gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
        }
      } else if (selectedStep == 3){
        if (s3Pump == 0) {
          s3Pump = 1;
          gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
        } else {
          s3Pump = 0;
          gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
        }
      } else if (selectedStep == 4){
        if (s4Pump == 0) {
          s4Pump = 1;
          gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
        } else {
          s4Pump = 0;
          gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
        }
      } else if (selectedStep == 5){
        if (s5Pump == 0) {
          s5Pump = 1;
          gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
        } else {
          s5Pump = 0;
          gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
        }
      } else if (selectedStep == 6){
        if (s6Pump == 0) {
          s6Pump = 1;
          gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
        } else {
          s6Pump = 0;
          gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
        }
      }
    }
    saveConfigFile();
  } else {
    gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
    int test = millis() - m_nTimePressStart;
    if (test >= 500){
      // Long press
      Serial.println("long");
      if (setTimeMin == 1){
        if (tempTargetTimeMin > 10){
          tempTargetTimeMin = tempTargetTimeMin - 10;
          setTargetTime();
        }        
      } else if (setTargetTemp == 1){        
        if (tempTargetTemp > 10){
          tempTargetTemp = tempTargetTemp - 10;
          setTargetTempFunc();
        }
      }
    } else {
      Serial.println("short");
      if (setTimeMin == 1){
        if (tempTargetTimeMin > 0) {
          tempTargetTimeMin = tempTargetTimeMin - 1;
          setTargetTime();
        }
      } else if (setTargetTemp == 1){       
        if (tempTargetTemp > 0) {
          tempTargetTemp = tempTargetTemp - 1;
          setTargetTempFunc();
        }       
      }
      m_nTimePressStart = 0;
    }
    saveConfigFile(); 
  } 
}

void playButtonPressed(int fromButton){
  int test = millis() - m_nTimePressStart;
  if (test >= 500 && fromButton == 1){
    // Long press
    Serial.println("long");      
    if (playPauseStop == 1 || playPauseStop == 2){
      //stop
      playPauseStop = 0;
      runningStep = 0;
      gslc_ElemSetTxtStr(&m_gui, m_pElemPlayBtn, "Start");
      valueTimer = "Stop";
    }          
    m_nTimePressStart = 0;
  } else {
    Serial.println("short");
    if (playPauseStop == 0 || playPauseStop == 2){
      //start
      gslc_ElemSetTxtStr(&m_gui, m_pElemPlayBtn, "Pause");
      playPauseStop = 1;
      runningStep = selectedStep;
      valueTimer = "Start";
    } else if (playPauseStop == 1){
      //pause
      gslc_ElemSetTxtStr(&m_gui, m_pElemPlayBtn, "Start");
      playPauseStop = 2;            
      valueTimer = "Pause";
    }                
    m_nTimePressStart = 0;
  }
}
void stepConfigButtonPressed(){
  if (autoOrMan == 0){
  //Do nothing   
  } else {
    if (selectedStep == 6){
      if (s1Pump == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
      }
      if (s1Next == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,true);
      }
      tempTargetTimeMin = s1TargetTimeMin;
      gslc_ElemSetTxtStr(&m_gui, m_pElemS1, "S1");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS2, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS3, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS4, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS5, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS6, "");
      changeSetTempDisplay(s1TargetTemp);
      changeSetTimeDisplay(s1TargetTimeMin);
      selectedStep = 1;
    } else if (selectedStep == 1){
      if (s2Pump == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
      }
      if (s2Next == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,true);
      }
      tempTargetTimeMin = s2TargetTimeMin;      
      gslc_ElemSetTxtStr(&m_gui, m_pElemS1, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS2, "S2");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS3, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS4, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS5, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS6, "");
      changeSetTempDisplay(s2TargetTemp);
      changeSetTimeDisplay(s2TargetTimeMin);
      selectedStep = 2;
    } else if (selectedStep == 2){
      if (s3Pump == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
      }
      if (s3Next == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,true);
      }
      tempTargetTimeMin = s3TargetTimeMin;
      gslc_ElemSetTxtStr(&m_gui, m_pElemS1, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS2, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS3, "S3");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS4, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS5, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS6, "");
      changeSetTempDisplay(s3TargetTemp);
      changeSetTimeDisplay(s3TargetTimeMin);
      selectedStep = 3;
    } else if (selectedStep == 3){
      if (s4Pump == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
      }
      if (s4Next == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,true);
      }
      tempTargetTimeMin = s4TargetTimeMin;
      gslc_ElemSetTxtStr(&m_gui, m_pElemS1, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS2, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS3, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS4, "S4");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS5, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS6, "");
      changeSetTempDisplay(s4TargetTemp);
      changeSetTimeDisplay(s4TargetTimeMin);
      selectedStep = 4;
    } else if (selectedStep == 4){
      if (s5Pump == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
      }
      if (s5Next == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,true);
      }
      tempTargetTimeMin = s5TargetTimeMin;
      gslc_ElemSetTxtStr(&m_gui, m_pElemS1, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS2, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS3, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS4, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS5, "S5");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS6, "");
      changeSetTempDisplay(s5TargetTemp);
      changeSetTimeDisplay(s5TargetTimeMin);
      selectedStep = 5;
    } else if (selectedStep == 5){
      if (s6Pump == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemMinusBtn,true);
      }
      if (s6Next == 0){
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,false);
      } else {
        gslc_ElemSetFillEn(&m_gui,m_pElemPlusBtn,true);
      }
      tempTargetTimeMin = s6TargetTimeMin;
      gslc_ElemSetTxtStr(&m_gui, m_pElemS1, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS2, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS3, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS4, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS5, "");
      gslc_ElemSetTxtStr(&m_gui, m_pElemS6, "S6");
      changeSetTempDisplay(s6TargetTemp);
      changeSetTimeDisplay(s6TargetTimeMin);
      selectedStep = 6;
    }
    runningStep = selectedStep; 
  }
}
void handleHeaters(){
  int tempTemp = 0;
  Serial.println(targetTemp);
  //check if heater should turn on or off
  if (playPauseStop == 1) {
    if (autoOrMan == 0) {
      //manual mode selected man target temp
      targetTemp = manTargetTemp;
    } else if (autoOrMan == 1) {
      //auto mode selected check if timer and temp is present. and also check which step is active     
      if (runningStep == 1 || runningStep == 2 || runningStep == 3 || runningStep == 4 || runningStep == 5 || runningStep == 6) {
        if  (runningStep == 1) {
          targetTemp = s1TargetTemp;
        } else if  (runningStep == 2) {
          targetTemp = s2TargetTemp;
        } else if  (runningStep == 3) {
          targetTemp = s3TargetTemp;
        } else if  (runningStep == 4) {
          targetTemp = s4TargetTemp;
        } else if  (runningStep == 5) {
          targetTemp = s5TargetTemp;
        } else if  (runningStep == 6) {
          targetTemp = s6TargetTemp;
        }
      }      
    }
    tempTemp = targetTemp - 2; //if temperature is 2 below use both heaters else only use the lower one 
    if (tempTemp > actTemp) {
      //Use both heaters
      Serial.println("Both heaters active");
      digitalWrite(relay1, LOW);
      digitalWrite(relay2, LOW);
    } else if (actTemp >= tempTemp && targetTemp > actTemp) {
      //Use only one heater
      Serial.println("500w heater active");
      digitalWrite(relay1, LOW);
      digitalWrite(relay2, HIGH);   
    } else {
      //Both heaters off
      Serial.println("Both heaters off");
      digitalWrite(relay1, HIGH);
      digitalWrite(relay2, HIGH);
    }
  } else {
    Serial.println("Both heaters off");
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
  }
}
void handlePump(){
  int tempPump = 0;
  if (autoOrMan == 0) {
    tempPump = manPump;
  } else {
    if (playPauseStop == 1) {
      if (runningStep == 1 || runningStep == 2 || runningStep == 3 || runningStep == 4 || runningStep == 5 || runningStep == 6) {
        if  (runningStep == 1) {
          tempPump = s1Pump;
        } else if  (runningStep == 2) {
          tempPump = s2Pump;
        } else if  (runningStep == 3) {
          tempPump = s3Pump;
        } else if  (runningStep == 4) {
          tempPump = s4Pump;
        } else if  (runningStep == 5) {
          tempPump = s5Pump;
        } else if  (runningStep == 6) {
          tempPump = s6Pump;
        }
      }  
    }
  }
  if (tempPump == 0){
      digitalWrite(relay3, HIGH);
  } else {
      digitalWrite(relay3, LOW);
  }
}
void readTemp(){  
  sensors.requestTemperatures(); 
  float Tc = sensors.getTempCByIndex(0);
  if (Tc >= 10 && Tc < 100) {
    actTempDisplay = "0" + String(Tc, 0);
  } else if (Tc >= 100) {
    actTempDisplay = String(Tc, 0);
  } else {
    actTempDisplay = "00" + String(Tc, 0);
  }
  actTemp = Tc;
  String tempTempTxt = actTempDisplay + " C";
  const char * c = tempTempTxt.c_str();
  gslc_ElemSetTxtStr(&m_gui, m_pElemActTempTxt, c);
  
}

void handleElapsedTimer(){
  if (valueTimer == "Start"){
    if (autoOrMan == 0) {
      targetTimeMin = manTargetTimeMin;
      targetTimeSec = manTargetTimeSec;
      targetTemp = manTargetTemp;      
    } else {
      if (runningStep == 1 || runningStep == 2 || runningStep == 3 || runningStep == 4 || runningStep == 5 || runningStep == 6) {
        if  (runningStep == 1) {
          targetTimeSec = s1TargetTimeSec;
          targetTimeMin = s1TargetTimeMin;
          targetTemp = s1TargetTemp;
        } else if  (runningStep == 2) {
          targetTimeSec = s2TargetTimeSec;
          targetTimeMin = s2TargetTimeMin;
          targetTemp = s2TargetTemp;
        } else if  (runningStep == 3) {
          targetTimeSec = s3TargetTimeSec;
          targetTimeMin = s3TargetTimeMin;
          targetTemp = s3TargetTemp;
        } else if  (runningStep == 4) {
          targetTimeSec = s4TargetTimeSec;
          targetTimeMin = s4TargetTimeMin;
          targetTemp = s4TargetTemp;
        } else if  (runningStep == 5) {
          targetTimeSec = s5TargetTimeSec;
          targetTimeMin = s5TargetTimeMin;
          targetTemp = s5TargetTemp;
        } else if  (runningStep == 6) {
          targetTimeSec = s6TargetTimeSec;
          targetTimeMin = s6TargetTimeMin;
          targetTemp = s6TargetTemp;
        }
      }   
    }
    if ((targetTimeMin > 0 && targetTemp == 00) || (targetTimeMin > 0 && targetTemp >= actTemp)) {
      if (actTimerPaused == 0) {
          countElapsedTimer.SetStopTime(0,targetTimeMin,targetTimeSec);
          countElapsedTimer.StartTimer();
      } else {
          countElapsedTimer.ResumeTimer();
          actTimerPaused = 0; 
      }
      valueTimer = "null";
    }
    
  } else if (valueTimer == "Pause"){
    if (targetTimeMin > 0){
      countElapsedTimer.PauseTimer();
      actTimerPaused = 1;
    }
    valueTimer = "null"; 
  } else if (valueTimer == "Stop") {
    countElapsedTimer.StopTimer();
    gslc_ElemSetTxtStr(&m_gui, m_pElemActTimeTxt, "00:00");
    actTimeSec = "00";
    actTimeMin = "00";
    actTimerPaused = 0;
    valueTimer = "null";
  } else if (valueTimer == "null") {
    //do nothing
  }
}

void setTargetTime(){
  if (autoOrMan == 0){ 
    manTargetTimeMin = tempTargetTimeMin;  
  } else {
    if (selectedStep == 1 || selectedStep == 2 || selectedStep == 3 || selectedStep == 4 || selectedStep == 5 || selectedStep == 6) {
      if  (selectedStep == 1) {
        s1TargetTimeMin = tempTargetTimeMin; 
      } else if  (selectedStep == 2) {
        s2TargetTimeMin = tempTargetTimeMin;
      } else if  (selectedStep == 3) {
        s3TargetTimeMin = tempTargetTimeMin;
      } else if  (selectedStep == 4) {
        s4TargetTimeMin = tempTargetTimeMin;
      } else if  (selectedStep == 5) {
        s5TargetTimeMin = tempTargetTimeMin;
      } else if  (selectedStep == 6) {
        s6TargetTimeMin = tempTargetTimeMin;
      }
    }
  }
  changeSetTimeDisplay(tempTargetTimeMin);    
}

void setTargetTempFunc(){
  String tempTargetTempDisplay;     
  if (autoOrMan == 0){
    manTargetTemp = tempTargetTemp;        
  } else {
    if (selectedStep == 1 || selectedStep == 2 || selectedStep == 3 || selectedStep == 4 || selectedStep == 5 || selectedStep == 6) {
      if  (selectedStep == 1) {
        s1TargetTemp = tempTargetTemp;
      } else if  (selectedStep == 2) {
        s2TargetTemp = tempTargetTemp;
      } else if  (selectedStep == 3) {
        s3TargetTemp = tempTargetTemp;
      } else if  (selectedStep == 4) {
        s4TargetTemp = tempTargetTemp;
      } else if  (selectedStep == 5) {
        s5TargetTemp = tempTargetTemp;
      } else if  (selectedStep == 6) {
        s6TargetTemp = tempTargetTemp;
      }
    }
  }
  changeSetTempDisplay(tempTargetTemp);
}
//Update display whit acutal target temp
void changeSetTempDisplay(int incTemp) {
  String tempDisplayText;
  if (incTemp < 10){
    tempDisplayText = "00" + String(incTemp) + " C";
  } else if (incTemp < 100 && incTemp > 10) {
    tempDisplayText = "0" + String(incTemp) + " C";
  } else if (incTemp > 100){
    tempDisplayText = String(incTemp) + " C";
  }
        
  const char * ctempTargetTemp = tempDisplayText.c_str();
  gslc_ElemSetTxtStr(&m_gui, m_pElemTargetTempTxt, ctempTargetTemp);
  
}
//update display whit actual time
void changeSetTimeDisplay(int incTime){
  String timeDisplayText;
  if (incTime < 10) {
    timeDisplayText  = "0" + String(incTime);
  } else {
    timeDisplayText  = String(incTime);
  }
  timeDisplayText = timeDisplayText + ":00";
  const char * ctempTargetTime = timeDisplayText.c_str();
  gslc_ElemSetTxtStr(&m_gui, m_pElemTargetTimeTxt, ctempTargetTime);
}
void handleTimerFinished() {
  Serial.println("Timer Finnished");
  playBuzzer();
  int tempNext = 0;
  int tempNextStep = 0;
  if (autoOrMan == 0) {
      tempNext = 0;
  } else if (autoOrMan == 1) {
    if (runningStep == 1 || runningStep == 2 || runningStep == 3 || runningStep == 4 || runningStep == 5 || runningStep == 6) {
      if  (runningStep == 1) {
        if (s1Next == 1) {
          tempNext = 1;
        } else {
          tempNext = 0;
        }
        tempNextStep = 2;
      } else if  (runningStep == 2) {
        if (s2Next == 1) {
          tempNext = 1;
        } else {
          tempNext = 0;
        }
        tempNextStep = 3;
      } else if  (runningStep == 3) {
        if (s3Next == 1) {
          tempNext = 1;
        } else {
          tempNext = 0;
        }
      } else if  (runningStep == 4) {
        if (s4Next == 1) {
          tempNext = 1;
        } else {
          tempNext = 0;
        }
        tempNextStep = 4;
      } else if  (runningStep == 5) {
        if (s5Next == 1) {
          tempNext = 1;
        } else {
          tempNext = 0;
        }
        tempNextStep = 6;
      } else if  (runningStep == 6) {
          tempNext = 0;
          tempNextStep = 1;
      }
    }      
  }
  if (tempNext == 0){
    valueTimer = "null";
    gslc_ElemSetTxtStr(&m_gui, m_pElemPlayBtn, "Start");
    playPauseStop = 0;
  } else {
    gslc_ElemSetTxtStr(&m_gui, m_pElemPlayBtn, "Pause");
    playPauseStop = 1;
    runningStep = tempNextStep;
    stepConfigButtonPressed();
    valueTimer = "Start";
  }
  
}
//buzzer
void playBuzzer() {
  analogWrite(buzzer, 512);
  delay(1000);
  analogWrite(buzzer, 0);
  digitalWrite(buzzer, LOW);
}

void saveConfigFile(){
  Serial.println("saving config");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["manTargetTimeMin"] = manTargetTimeMin;
  json["manTargetTemp"] = manTargetTemp;
  json["manPump"] = manPump;
  json["manNext"] = manNext;
  json["s1TargetTimeMin"] = s1TargetTimeMin;
  json["s1TargetTemp"] = s1TargetTemp;
  json["s1Pump"] = s1Pump;
  json["s1Next"] = s1Next;
  json["s2TargetTimeMin"] = s2TargetTimeMin;
  json["s2TargetTemp"] = s2TargetTemp;
  json["s2Pump"] = s2Pump;
  json["s2Next"] = s2Next;
  json["s3TargetTimeMin"] = s3TargetTimeMin;
  json["s3TargetTemp"] = s3TargetTemp;
  json["s3Pump"] = s3Pump;
  json["s3Next"] = s3Next;
  json["s4TargetTimeMin"] = s4TargetTimeMin;
  json["s4TargetTemp"] = s4TargetTemp;
  json["s4Pump"] = s4Pump;
  json["s4Next"] = s4Next;
  json["s5TargetTimeMin"] = s5TargetTimeMin;
  json["s5TargetTemp"] = s5TargetTemp;
  json["s5Pump"] = s5Pump;
  json["s5Next"] = s5Next;
  json["s6TargetTimeMin"] = s6TargetTimeMin;
  json["s6TargetTemp"] = s6TargetTemp;
  json["s6Pump"] = s6Pump;
  json["s6Next"] = s6Next;
  File configFile = SPIFFS.open(BREWER_CONFIG, "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }
  json.prettyPrintTo(Serial);
  json.printTo(configFile);
  configFile.close();
  //end save
}
// ------------------------------------------------
// Create page elements
// ------------------------------------------------
bool InitGUI()
{
  gslc_tsElemRef* pElemRef = NULL;

//<InitGUI !Start!>
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPage1Elem,MAX_ELEM_PG_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui,E_PG_AP,m_asPage2Elem,MAX_ELEM_PG_AP_RAM,m_asPage2ElemRef,MAX_ELEM_PG_AP);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_LT3);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  
  
  // Create E_ELEM_TempBtn button with modifiable text label
  static char m_strbtn1[5] = "TEMP";
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_TempBtn,E_PG_MAIN,
    (gslc_tsRect){10,10,75,50},
    (char*)m_strbtn1,5,E_FONT_SERIFB12,&CbBtnCommon);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_GRAY,GSLC_COL_GRAY);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  m_pElemTempBtn = pElemRef;
  
  // create E_ELEM_TimeBtn button with text label
  static char m_sDisplayTextTime[5] = "TIME";
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_TimeBtn,E_PG_MAIN,
    (gslc_tsRect){10,70,75,50},
    (char*)m_sDisplayTextTime,5,E_FONT_SERIFB12,&CbBtnCommon);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_GRAY,GSLC_COL_GRAY);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  m_pElemTimeBtn = pElemRef;

  // Create E_ELEM_AutoBtn button with modifiable text label
  static char m_strbtn3[5] = "MAN";
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_AutoBtn,E_PG_MAIN,
    (gslc_tsRect){10,130,75,50},
    (char*)m_strbtn3,5,E_FONT_SERIFB12,&CbBtnCommon);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK1);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  m_pElemAutoBtn = pElemRef;
  
  // create E_ELEM_MinusBtn button with text label
  static char m_sDisplayTextMinus1[5] = "Pump";
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_MinusBtn,E_PG_MAIN,
    (gslc_tsRect){10,190,145,40},(char*)m_sDisplayTextMinus1,5,E_FONT_SANSB18,&CbBtnCommon);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_GRAY,GSLC_COL_GRAY);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  m_pElemMinusBtn = pElemRef;
  
  // create E_ELEM_PlusBtn button with text label
  static char m_sDisplayTextPlus[5] = "Next";
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_PlusBtn,E_PG_MAIN,
    (gslc_tsRect){165,190,145,40},(char*)m_sDisplayTextPlus,5,E_FONT_SANSB18,&CbBtnCommon);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_GRAY,GSLC_COL_GRAY);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  m_pElemPlusBtn = pElemRef;
  
  // Create E_ELEM_ActTempTxt runtime modifiable text
  static char m_sDisplayText1[6] = "000 C";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_ActTempTxt,E_PG_MAIN,(gslc_tsRect){90,30,96,50},
    (char*)m_sDisplayText1,6,E_FONT_SERIFB18);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY,GSLC_COL_BLACK);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  m_pElemActTempTxt = pElemRef;
  
  // Create E_ELEM_TargetTempTxt runtime modifiable text
  String startTempDisplay = "";
  if (manTargetTemp <= 9){
    startTempDisplay = "00" + String(manTargetTemp) + " C";
  } else {
    startTempDisplay = "0" + String(manTargetTemp) + " C";
  }
  static char m_sDisplayText2[6];
  startTempDisplay.toCharArray(m_sDisplayText2, 6);
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TargetTempTxt,E_PG_MAIN,(gslc_tsRect){90,80,96,50},
    (char*)m_sDisplayText2,6,E_FONT_SERIFB18);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY,GSLC_COL_BLACK);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  m_pElemTargetTempTxt = pElemRef;
  
  // Create E_ELEM_ActTimeTxt runtime modifiable text
  static char m_sDisplayText3[6] = "00:00";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_ActTimeTxt,E_PG_MAIN,(gslc_tsRect){210,30,96,50},
    (char*)m_sDisplayText3,6,E_FONT_SERIFB18);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY,GSLC_COL_BLACK);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  m_pElemActTimeTxt = pElemRef;
  
  // Create E_ELEM_TargetTimeTxt runtime modifiable text
  static char m_sDisplayText4[6] = "00:00";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TargetTimeTxt,E_PG_MAIN,(gslc_tsRect){210,80,96,50},
    (char*)m_sDisplayText4,6,E_FONT_SERIFB18);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY,GSLC_COL_BLACK);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  m_pElemTargetTimeTxt = pElemRef;
  
  // Create E_ELEM_S1 text label
  static char m_sDisplayText5[3] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_S1,E_PG_MAIN,(gslc_tsRect){90,10,22,24},
    (char*)m_sDisplayText5,3,E_FONT_SANS9);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY,GSLC_COL_BLACK);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  m_pElemS1 = pElemRef;
  
  // Create E_ELEM_PlayBtn button with modifiable text label
  static char m_strbtn6[6] = "Start";
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_PlayBtn,E_PG_MAIN,
    (gslc_tsRect){180,130,130,50},
    (char*)m_strbtn6,6,E_FONT_SERIFB12,&CbBtnCommon);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK1);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  m_pElemPlayBtn = pElemRef;
  
  // create E_ELEM_StepBtn button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_StepBtn,E_PG_MAIN,
    (gslc_tsRect){90,130,85,50},(char*)"S1 - S6",0,E_FONT_SERIFB12,&CbBtnCommon);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK1);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  
  // Create E_ELEM_S2 text label
  static char m_sDisplayText6[3] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_S2,E_PG_MAIN,(gslc_tsRect){130,10,22,24},
    (char*)m_sDisplayText6,3,E_FONT_SANS9);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY,GSLC_COL_BLACK);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  m_pElemS2 = pElemRef;
  
  // Create E_ELEM_S3 text label
  static char m_sDisplayText7[3] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_S3,E_PG_MAIN,(gslc_tsRect){170,10,22,24},
    (char*)m_sDisplayText7,3,E_FONT_SANS9);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY,GSLC_COL_BLACK);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  m_pElemS3 = pElemRef;

  // Create E_ELEM_S4 text label
  static char m_sDisplayText8[3] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_S4,E_PG_MAIN,(gslc_tsRect){210,10,22,24},
    (char*)m_sDisplayText8,3,E_FONT_SANS9);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY,GSLC_COL_BLACK);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  m_pElemS4 = pElemRef;

  // Create E_ELEM_S5 text label S5
  static char m_sDisplayText9[3] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_S5,E_PG_MAIN,(gslc_tsRect){250,10,22,24},
    (char*)m_sDisplayText9,3,E_FONT_SANS9);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY,GSLC_COL_BLACK);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  m_pElemS5 = pElemRef;

  // Create E_ELEM_S6 text label
  static char m_sDisplayText10[3] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_S6,E_PG_MAIN,(gslc_tsRect){280,10,22,24},
    (char*)m_sDisplayText10,3,E_FONT_SANS9);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY,GSLC_COL_BLACK);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  m_pElemS6 = pElemRef;

  // PAGE: E_PG_AP 
  // Create E_ELEM_Header text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_Header,E_PG_AP,(gslc_tsRect){36,10,247,62},
    (char*)"BeerBrewer",0,E_FONT_SERIFB24);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY,GSLC_COL_BLACK);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  
  // Create E_ELEM_APMode text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_APMode,E_PG_AP,(gslc_tsRect){63,70,194,32},
    (char*)"Access Point Mode",0,E_FONT_SERIFB12);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY,GSLC_COL_BLACK);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  
  // Create E_ELEM_IPTxt text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_IPTxt,E_PG_AP,(gslc_tsRect){87,130,146,32},
    (char*)"IP:192.168.4.1",0,E_FONT_SERIFB12);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY,GSLC_COL_BLACK);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
//<InitGUI !End!>

  return true;
}

//ftp server
FtpServer ftpSrv;
// Variable to store the HTTP request
String header;
//Initilize webpage and updatepage
void getTemperature(){
  //return String(actTemp);
  Serial.println("get temp");
  Serial.println(String(actTemp));
 server.send(200, "text/html", String(actTemp));
}

void getStats(){
  String tempTemp = String(actTemp);
  int tempSendTargetTemp = 0;
  int tempSendActiveStep = 0;
  int tempSendPumpStatus = 0;
  int tempSendTargetTimeMin = 0;
  if (autoOrMan == 0) {
    tempSendActiveStep = 0;
    tempSendTargetTemp = manTargetTemp;
    tempSendPumpStatus = manPump;
    tempSendTargetTimeMin = manTargetTimeMin;
  } else if(selectedStep == 1){
    tempSendActiveStep = 1; 
    tempSendTargetTemp = s1TargetTemp;
    tempSendPumpStatus = s1Pump;
    tempSendTargetTimeMin = s1TargetTimeMin;
  } else if(selectedStep == 2){
    tempSendActiveStep = 2;
    tempSendTargetTemp = s2TargetTemp;
    tempSendPumpStatus = s2Pump;
    tempSendTargetTimeMin = s2TargetTimeMin;
  } else if(selectedStep == 3){
    tempSendActiveStep = 3;
    tempSendTargetTemp = s3TargetTemp;
    tempSendPumpStatus = s3Pump;
    tempSendTargetTimeMin = s3TargetTimeMin;
  } else if(selectedStep == 4){
    tempSendActiveStep = 4;
    tempSendTargetTemp = s4TargetTemp;
    tempSendPumpStatus = s4Pump;
    tempSendTargetTimeMin = s4TargetTimeMin;
  } else if(selectedStep == 5){
    tempSendActiveStep = 5;
    tempSendTargetTemp = s5TargetTemp;
    tempSendPumpStatus = s5Pump;
    tempSendTargetTimeMin = s5TargetTimeMin;
  } else if(selectedStep == 6){
    tempSendActiveStep = 6;
    tempSendTargetTemp = s6TargetTemp;
    tempSendPumpStatus = s6Pump;
    tempSendTargetTimeMin = s6TargetTimeMin;
  }
  String tempTempActiveStep = String(tempSendActiveStep);
  String sendStats = tempTemp + "," + tempTempActiveStep + "," + String(tempSendTargetTemp) + "," + String(tempSendPumpStatus) + "," + actTimeMin + "," + actTimeSec + "," + String(tempSendTargetTimeMin) + "," + String(playPauseStop); 
  server.send(200, "text/html", sendStats);
}
void getStart(){
  m_nTimePressStart = 0;
  playButtonPressed(0);
  server.send(200, "text/html", String(playPauseStop));
}
void getStop(){
  playButtonPressed(1);
  server.send(200, "text/html", String(playPauseStop));
}
void getActiveStep(){
  server.send(200, "text/html", String(selectedStep));
}
void postChangeStep(){
  if( !server.hasArg("step")|| server.arg("step") == NULL){
    server.send(400, "text/plain", "400: Invalid Request");
    return; 
  }
  if (server.arg("step") == "man"){
    autoOrMan = 1;
    autoButtonPressed();
  } else if (server.arg("step") == "1") {
    if (autoOrMan == 0){
      selectedStep = 1;
      autoButtonPressed();
    } else {
      selectedStep = 6;
      stepConfigButtonPressed();
    }
  } else if (server.arg("step") == "2") {
    if (autoOrMan == 0){
      selectedStep = 2;
      autoButtonPressed();
    } else {
      selectedStep = 1;
      stepConfigButtonPressed();
    }
  } else if (server.arg("step") == "3") {
    if (autoOrMan == 0){
      selectedStep = 3;
      autoButtonPressed();
    } else {
      selectedStep = 2;
      stepConfigButtonPressed();
    }
  } else if (server.arg("step") == "4") {
    if (autoOrMan == 0){
      selectedStep = 4;
      autoButtonPressed();
    } else {
      selectedStep = 3;
      stepConfigButtonPressed();
    }
  } else if (server.arg("step") == "5") {
    if (autoOrMan == 0){
      selectedStep = 5;
      autoButtonPressed();
    } else {
      selectedStep = 4;
      stepConfigButtonPressed();
    }
  } else if (server.arg("step") == "6") {
    if (autoOrMan == 0){
      selectedStep = 6;
      autoButtonPressed();
    } else {
      selectedStep = 5;
      stepConfigButtonPressed();
    }
  }
  server.send(200, "text/html", "ok");  
}
void getPump(){
  if(!stepConfigMinusPump == 1){
    minusButtonPressed();
  }
}
void getTempPlus(){
  tempTargetTemp = tempTargetTemp + 1;
  setTargetTempFunc();
  server.send(200, "text/html", String(tempTargetTemp));
}
void getTempMinus(){
  if (tempTargetTemp > 0) {
    tempTargetTemp = tempTargetTemp - 1;
    setTargetTempFunc();
    server.send(200, "text/html", String(tempTargetTemp));
  }  
}
void getTimeMinus(){  
  if (tempTargetTimeMin > 0) {
    tempTargetTimeMin = tempTargetTimeMin - 1;
    setTargetTime();
    server.send(200, "text/html", String(tempTargetTimeMin));
  }
}
void getTimePlus(){
  tempTargetTimeMin = tempTargetTimeMin + 1;
  setTargetTime();
  server.send(200, "text/html", String(tempTargetTimeMin));
}
void setup() {
  Serial.begin(115200);
  
  gslc_InitDebug(&DebugOut);

  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // Load Fonts
    if (!gslc_FontSet(&m_gui,E_FONT_SANS9,GSLC_FONTREF_PTR,&FreeSans9pt7b,1)) { return; }
    if (!gslc_FontSet(&m_gui,E_FONT_SANSB18,GSLC_FONTREF_PTR,&FreeSansBold18pt7b,1)) { return; }
    if (!gslc_FontSet(&m_gui,E_FONT_SERIFB12,GSLC_FONTREF_PTR,&FreeSerifBold12pt7b,1)) { return; }
    if (!gslc_FontSet(&m_gui,E_FONT_SERIFB18,GSLC_FONTREF_PTR,&FreeSerifBold18pt7b,1)) { return; }
    if (!gslc_FontSet(&m_gui,E_FONT_SERIFB24,GSLC_FONTREF_PTR,&FreeSerifBold24pt7b,1)) { return; }

  //WifiManager
  //wifiManager.setAPCallback(configModeCallback);
  wifiManager.setConfigPortalBlocking(false);
  wifiManager.autoConnect("BeerBrewer");
  delay(2000);

 
  //Load config file. if it not exist creates it first then loads it. and start ftp serv
  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    ftpSrv.begin("admin", "admin"); // username, password for ftp. Set ports in ESP8266FtpServer.h (default 21, 50009 for PASV)
    if (!SPIFFS.exists(BREWER_CONFIG)) {
      Serial.println("creating config file");
      saveConfigFile();
    }
    if (SPIFFS.exists(BREWER_CONFIG)) {
      // parse json config file
      File jsonFile = SPIFFS.open(BREWER_CONFIG, "r");
      if (jsonFile) {
        // Allocate a buffer to store contents of the file.
        size_t size = jsonFile.size();
        std::unique_ptr<char[]> jsonBuf(new char[size]);
        jsonFile.readBytes(jsonBuf.get(), size); 
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(jsonBuf.get());
        if (json.success()) {
          manTargetTimeMin = json["manTargetTimeMin"];
          manTargetTemp = json["manTargetTemp"];
          manPump = json["manPump"];
          manNext = json["manNext"];
          s1TargetTimeMin = json["s1TargetTimeMin"];
          s1TargetTemp = json["s1TargetTemp"];
          s1Pump = json["s1Pump"];
          s1Next = json["s1Next"];
          s2TargetTimeMin = json["s2TargetTimeMin"];
          s2TargetTemp = json["s2TargetTemp"];
          s2Pump = json["s2Pump"];
          s2Next = json["s2Next"];
          s3TargetTimeMin = json["s3TargetTimeMin"];
          s3TargetTemp = json["s3TargetTemp"];
          s3Pump = json["s3Pump"];
          s3Next = json["s3Next"];
          s4TargetTimeMin = json["s4TargetTimeMin"];
          s4TargetTemp = json["s4TargetTemp"];
          s4Pump = json["s4Pump"];
          s4Next = json["s4Next"];
          s5TargetTimeMin = json["s5TargetTimeMin"];
          s5TargetTemp = json["s5TargetTemp"];
          s5Pump = json["s5Pump"];
          s5Next = json["s5Next"];
          s6TargetTimeMin = json["s6TargetTimeMin"];
          s6TargetTemp = json["s6TargetTemp"];
          s6Pump = json["s6Pump"];
          s6Next = json["s6Next"];  
        } else {
          Serial.println("failed to load json config");
        }
        jsonFile.close();
      } else {
        Serial.println("failed to load json config");
      }
    } else {
      Serial.println("No file exist");       
    }
  } else {
    Serial.println("failed to mount FS");
  }
  // Create graphic elements
  InitGUI();
  //Webserver
  //include webpages
  MDNS.begin(host);
  httpUpdater.setup(&server);
  server.on("/temperature", HTTP_GET, getTemperature);
  server.on("/step", HTTP_GET, getActiveStep);
  server.on("/stats", HTTP_GET, getStats);
  server.on("/start", HTTP_GET, getStart);
  server.on("/stop", HTTP_GET, getStop);
  server.on("/pump", HTTP_GET, getPump);
  server.on("/tempplus", HTTP_GET, getTempPlus);
  server.on("/tempminus", HTTP_GET, getTempMinus);
  server.on("/timeplus", HTTP_GET, getTimePlus);
  server.on("/timeminus", HTTP_GET, getTimeMinus);
  server.on("/changestep", HTTP_POST, postChangeStep);
  server.serveStatic("/", SPIFFS, "/index.html");
  server.serveStatic("/jquery-3.4.1.min.js", SPIFFS, "/jquery-3.4.1.min.js"); 
  server.begin();
  MDNS.addService("http", "tcp", 80);

  //Relays
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW); 

  //pump
  if (debug == 0){
    //GPIO 1 (TX) swap the pin to a GPIO.
    pinMode(1, FUNCTION_3);
    pinMode(relay3, OUTPUT);
    digitalWrite(relay3, LOW);
  } else if (debug == 1){
    //GPIO 1 (TX) swap the pin to a TX.
    pinMode(1, FUNCTION_0); 
  }

  //tempsensor
  //GPIO 3 (RX) swap the pin to a GPIO.
  pinMode(3, FUNCTION_3); 
  sensors.begin(); 
}

// -----------------------------------
// Main event loop
// -----------------------------------
void loop()
{
  wifiManager.process();
  MDNS.update();
  server.handleClient();
  ftpSrv.handleFTP();
  unsigned long currentMillis = millis();

  //every 5000 millis
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    readTemp();
    handleHeaters();
    handlePump();
    handleElapsedTimer();
  }
  //Timer
  countElapsedTimer.Timer();
  if (countElapsedTimer.TimeHasChanged() ) // this prevents the time from being constantly shown.
  {
    int tempMin = countElapsedTimer.ShowHours() * 60 + countElapsedTimer.ShowMinutes();
    int tempSec = countElapsedTimer.ShowSeconds();
    if (tempMin < 10){
      actTimeMin = "0" + String(tempMin);
    } else {
      actTimeMin = String(tempMin);
    }
    if (tempSec < 10) {
      actTimeSec = "0" + String(tempSec);
    } else {
      actTimeSec = String(tempSec);
    }
    String tempTimeSet = actTimeMin + ":" + actTimeSec;
    const char * cTime = tempTimeSet.c_str();
    gslc_ElemSetTxtStr(&m_gui, m_pElemActTimeTxt, cTime);
    Serial.print("elapsedtimer");
    Serial.println(countElapsedTimer.TimeCheck());
    if (countElapsedTimer.TimeCheck() == 1){
      handleTimerFinished();
    }
  }
  
  // Periodically call GUIslice update function
  gslc_Update(&m_gui);   
}