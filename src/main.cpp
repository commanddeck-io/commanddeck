#include <Arduino.h>
#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "secrets.h"
#include "modes.h"

/*
  Blynk
  Define your secrets in secrets.h
*/
char auth[] = BLYNK_TOKEN;

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

char otaHostname[] = OTA_HOSTNAME;
char otaPass[] = OTA_PASSWORD;

BlynkTimer timer;

WidgetLCD lcd(V0);
WidgetLED led(V4);

// Report toggles

bool offlineMode = true; // set to true to not connect to Blynk

bool reportLed = true; // whether to report LED status to Blynk, ignored if in offline mode
bool reportLabels = false; // whether to report key labels to Blynk, ignored if in offline mode


// Modes
// Define your keybindings in modes.h
int curMode = 0; // select your default mode here
int tmpMode = -1;

/*
  External LED
*/

#define pinLED D4   // Connected to external LED

void ledOn() {
  digitalWrite(pinLED, HIGH);

  if (reportLed) {
    led.on();
  }
}

void ledOff() {
  digitalWrite(pinLED, LOW);

  if (reportLed) {
    led.off();
  }
}

/*
  Display
*/
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R2, /* clock=*/ D6, /* data=*/ D7, /* reset=*/ U8X8_PIN_NONE); // 128x32
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ D6, /* data=*/ D7, /* reset=*/ U8X8_PIN_NONE); // 128x64

// Alignment
#define LCDWidth                        u8g2.getDisplayWidth()
#define ALIGN_CENTER(t)                 ((LCDWidth - (u8g2.getUTF8Width(t))) / 2)
#define ALIGN_RIGHT(t)                  (LCDWidth -  u8g2.getUTF8Width(t))
#define ALIGN_LEFT                      0

// Display Functions
void printTwo(char *msg1, char *msg2) {
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
  //u8g2.setFont(u8g2_font_crox4hb_tr);

  u8g2.drawStr(ALIGN_CENTER(msg1),12, msg1);
  u8g2.drawStr(ALIGN_CENTER(msg2),22, msg2);

  u8g2.sendBuffer();					// transfer internal memory to the display

  lcd.clear();
  lcd.print(0, 0, msg1);
  lcd.print(0, 1, msg2);
}

void printLine(char *msg) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  //u8g2.setFont(u8g2_font_crox4hb_tr);

  u8g2.drawStr(ALIGN_CENTER(msg),16, msg);

  u8g2.sendBuffer();

  lcd.clear();
  lcd.print(0, 0, msg);
}

void printLine(const char *msg) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  u8g2.drawStr(ALIGN_CENTER(msg),16, msg);

  u8g2.sendBuffer();

  lcd.clear();
  lcd.print(0, 0, msg);
}


/*
  Rotary
*/
#define pinSW D3    // Connected to SW  on KY-040
#define pinCLK  D1  // Connected to CLK on KY-040
#define pinDT  D2   // Connected to DT  on KY-040

static uint8_t prevNextCode = 0;
static uint16_t store=0;

// A valid CW or CCW move returns 1, invalid returns 0.
int8_t read_rotary() {
  static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};

  prevNextCode <<= 2;
  if (digitalRead(pinDT)) prevNextCode |= 0x02;
  if (digitalRead(pinCLK)) prevNextCode |= 0x01;
  prevNextCode &= 0x0f;

  // If valid then store as 16 bit data.
  if  (rot_enc_table[prevNextCode] ) {
    store <<= 4;
    store |= prevNextCode;
    //if (store==0xd42b) return 1;
    //if (store==0xe817) return -1;
    if ((store&0xff)==0x2b) return -1;
    if ((store&0xff)==0x17) return 1;
  }
  return 0;
}

// Rotary functions
void moveRight() {
  ledOn();

  if (tmpMode == -1) {
    tmpMode = curMode;
  }

  tmpMode++;

  if (tmpMode >= numModes) {
    tmpMode = 0;
  }

  printTwo("Press to apply", modes[tmpMode]);
  ledOff();
}

void moveLeft() {
  ledOn();

  if (tmpMode == -1) {
    tmpMode = curMode;
  }

  tmpMode--;

  if (tmpMode < 0) {
    tmpMode = numModes - 1;
  }

  printTwo("Press to apply", modes[tmpMode]);
  ledOff();
}


/*
  Handle keys
*/

void pressKey(char key) {
  String str = "KEY key=";
  str.concat(key);
  str.concat("|");
  Serial.println(str);
  str.remove(0);
}

void reportKeys() {
  for(int row = 0; row < n_rows; row++) {
    for (int col = 0; col < n_cols; col++) {
      String str = "CMD row=";
      str.concat(row);
      str.concat(" col=");
      str.concat(col);
      str.concat(" cmd=");
      str.concat(keys[curMode][row][col]);
      str.concat("|");
      Serial.println(str);
      str.remove(0);

      if (reportLabels) {
        switch (row) {
        case 0:
          switch (col) {
          case 0: Blynk.setProperty(V10, "label", keys[curMode][row][col]);
          case 1: Blynk.setProperty(V11, "label", keys[curMode][row][col]);
          case 2: Blynk.setProperty(V12, "label", keys[curMode][row][col]);
          case 3: Blynk.setProperty(V13, "label", keys[curMode][row][col]);
          }
          break;
        case 1:
          switch (col) {
          case 0: Blynk.setProperty(V14, "label", keys[curMode][row][col]);
          case 1: Blynk.setProperty(V15, "label", keys[curMode][row][col]);
          case 2: Blynk.setProperty(V16, "label", keys[curMode][row][col]);
          case 3: Blynk.setProperty(V17, "label", keys[curMode][row][col]);
          }
          break;
        case 2:
          switch (col) {
          case 0: Blynk.setProperty(V18, "label", keys[curMode][row][col]);
          case 1: Blynk.setProperty(V19, "label", keys[curMode][row][col]);
          case 2: Blynk.setProperty(V20, "label", keys[curMode][row][col]);
          case 3: Blynk.setProperty(V21, "label", keys[curMode][row][col]);
          }
          break;
        case 3:
          switch (col) {
          case 0: Blynk.setProperty(V22, "label", keys[curMode][row][col]);
          case 1: Blynk.setProperty(V23, "label", keys[curMode][row][col]);
          case 2: Blynk.setProperty(V24, "label", keys[curMode][row][col]);
          case 3: Blynk.setProperty(V25, "label", keys[curMode][row][col]);
          }
          break;
        }
      }
    }
  }
}


/*
  Apply Mode
*/
void applyMode() {
  if (tmpMode < 0 || tmpMode >= numModes) {
    tmpMode = curMode;
  }

  printTwo("Applying mode", modes[tmpMode]);
  curMode = tmpMode;
  reportKeys();
  tmpMode = -1;

  printTwo("Current mode", modes[curMode]);
}

/*
  Loop
*/
void loop() { 
  timer.run();
  ArduinoOTA.handle();
  if (!offlineMode) {
    Blynk.run();
  }
}

void runLoop() {
  static int8_t c,val;

  if (!(digitalRead(pinSW))) {
    ledOn();
    while (!digitalRead(pinSW)) {}
    applyMode();
    ledOff();
  }

  if( val=read_rotary() ) {

    if ( prevNextCode==0x0b) {
      moveLeft();
    }

    if ( prevNextCode==0x07) {
      moveRight();
    }
  }

  if (!Serial) {
    while (!Serial) {};
    applyMode();
  }

  while (Serial.available()) {
    if (Serial.available() > 0) {
      String str = Serial.readStringUntil('|');
      str.trim();

      if (str == "BOOTUP") {
        applyMode();
      } else if (str.startsWith("KEY")) {
        ledOn();
        delay(100);
        ledOff();
      } else if (str.startsWith("LOVE")) {
      }
    }
  }
}

/*
  Setup
*/
void setup() { 
  /*
    Set pin modes
  */
  pinMode (pinSW, INPUT);
  pinMode (pinSW, INPUT_PULLUP);
  pinMode (pinCLK, INPUT);
  pinMode (pinCLK, INPUT_PULLUP);
  pinMode (pinDT, INPUT);
  pinMode (pinDT, INPUT_PULLUP);
  pinMode (pinLED, OUTPUT);

  /*
    Init display
  */
  u8g2.begin();
  printTwo("Command", "Deck");
  delay(500);

  /*
    Loop Timer
  */
  timer.setInterval(1L, runLoop);

  /*
    Connect to Blynk
  */
  if (!offlineMode) {
    printTwo("Connecting to", "Blynk server..");
    Blynk.begin(auth, ssid, pass);
    printTwo("Blynk", "Connected!");
    delay(1000);
  } else {
    reportLed = false;
    reportLabels = false;

    printTwo("Connecting to", "WiFi..");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    };
    printTwo("WiFi", "Connected!");
    delay(1000);
  }

  /*
    OTA
  */
  ArduinoOTA.setHostname(otaHostname);
  ArduinoOTA.setPasswordHash(otaPass);

  ArduinoOTA.onStart([]() {
                       printTwo("OTA", "Updating...");
                     });
  ArduinoOTA.onEnd([]() {
                     printTwo("OTA", "Finished!");
                     delay(1000);
                     printTwo("OTA", "Rebooting...");
                   });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
                          char text[16];
                          sprintf(text, "Progress: %u%%", (progress / (total / 100)));
                          printTwo("OTA", text);
                        });
  ArduinoOTA.onError([](ota_error_t error) {
                       if (error == OTA_AUTH_ERROR) printTwo("OTA", "Auth Failed");
                       else if (error == OTA_BEGIN_ERROR) printTwo("OTA", "Begin Failed");
                       else if (error == OTA_CONNECT_ERROR) printTwo("OTA", "Connect Failed");
                       else if (error == OTA_RECEIVE_ERROR) printTwo("OTA", "Receive Failed");
                       else if (error == OTA_END_ERROR) printTwo("OTA", "End Failed");
                       delay(5000);
                       applyMode();
                     });

  ArduinoOTA.begin();

  char ip[16];
  sprintf(ip, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );

  printTwo("OTA Ready", ip);

  delay(1000);

  /*
    Connect to Leonardo
  */
  printTwo("Waiting for", "Arduino...");

  Serial.begin (9600);
  while (!Serial) {};

  applyMode();
}

/*
  Handle Blynk commands
*/
BLYNK_WRITE(V1) { // left
  int pinValue = param.asInt();
  if (pinValue == 1) {
    moveLeft();
  }
}

BLYNK_WRITE(V2) { // right
  int pinValue = param.asInt();
  if (pinValue == 1) {
    moveRight();
  }
}

BLYNK_WRITE(V3) { // ok
  int pinValue = param.asInt();
  if (pinValue == 1) {
    ledOn();
    applyMode();
    ledOff();
  }
}

BLYNK_WRITE(V10) { // 1
  int pinValue = param.asInt();
  if (pinValue == 1) {
    pressKey('1');
  }
}

BLYNK_WRITE(V11) { // 2
  int pinValue = param.asInt();
  if (pinValue == 1) {
    pressKey('2');
  }
}

BLYNK_WRITE(V12) { // 3
  int pinValue = param.asInt();
  if (pinValue == 1) {
    pressKey('3');
  }
}

BLYNK_WRITE(V13) { // A
  int pinValue = param.asInt();
  if (pinValue == 1) {
    pressKey('A');
  }
}

BLYNK_WRITE(V14) { // 4
  int pinValue = param.asInt();
  if (pinValue == 1) {
    pressKey('4');
  }
}

BLYNK_WRITE(V15) { // 5
  int pinValue = param.asInt();
  if (pinValue == 1) {
    pressKey('5');
  }
}

BLYNK_WRITE(V16) { // 6
  int pinValue = param.asInt();
  if (pinValue == 1) {
    pressKey('6');
  }
}

BLYNK_WRITE(V17) { // B
  int pinValue = param.asInt();
  if (pinValue == 1) {
    pressKey('B');
  }
}

BLYNK_WRITE(V18) { // 7
  int pinValue = param.asInt();
  if (pinValue == 1) {
    pressKey('7');
  }
}

BLYNK_WRITE(V19) { // 8
  int pinValue = param.asInt();
  if (pinValue == 1) {
    pressKey('8');
  }
}

BLYNK_WRITE(V20) { // 9
  int pinValue = param.asInt();
  if (pinValue == 1) {
    pressKey('9');
  }
}

BLYNK_WRITE(V21) { // C
  int pinValue = param.asInt();
  if (pinValue == 1) {
    pressKey('C');
  }
}

BLYNK_WRITE(V22) { // *
  int pinValue = param.asInt();
  if (pinValue == 1) {
    pressKey('*');
  }
}

BLYNK_WRITE(V23) { // 0
  int pinValue = param.asInt();
  if (pinValue == 1) {
    pressKey('0');
  }
}

BLYNK_WRITE(V24) { // #
  int pinValue = param.asInt();
  if (pinValue == 1) {
    pressKey('#');
  }
}

BLYNK_WRITE(V25) { // D
  int pinValue = param.asInt();
  if (pinValue == 1) {
    pressKey('D');
  }
}
