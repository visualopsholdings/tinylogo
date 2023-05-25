/*
  Control 2 RGB LEDs in LOGO over serial or i2c
  
  Author: Paul Hamilton
  Date: 22-May-2023
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "ringbuffer.hpp"
#include "cmd.hpp"
#include "logo.hpp"
#include "arduinotimeprovider.hpp"
#include "arduinoflashstring.hpp"

#include <Wire.h>

#define SERIAL

#define I2C_ADDRESS 8
#define LED_PIN     13
#define BLUE_PIN    2
#define RED_PIN     3
#define GREEN_PIN   4

void ledOn(Logo &logo) {
  digitalWrite(LED_PIN, HIGH);
}

void ledOff(Logo &logo) {
  digitalWrite(LED_PIN, LOW);
}

void redRaw(Logo &logo) {
  analogWrite(RED_PIN, logo.popint());
}

void greenRaw(Logo &logo) {
  analogWrite(GREEN_PIN, logo.popint());
}

void blueRaw(Logo &logo) {
  analogWrite(BLUE_PIN, logo.popint());
}

RingBuffer buffer;
Cmd cmd;
char cmdbuf[64];

// the strings for the program in flash
static const char strings_flash[] PROGMEM = 
  "SCLR\n"    // 1
  "C\n"       // 2
  "REDC\n"    // 3
  "N\n"       // 4
  "GREENC\n"  // 5
  "BLUEC\n"   // 6
  "AMBER\n"   // 7
  "RED\n"     // 8
  "GREEN\n"   // 9
  "BLUE\n"    // 10
  "REDR\n"    // 11
  "GREENR\n"  // 12
  "BLUER\n"   // 13
  "ON\n"      // 14
  "OFF\n"     // 15
  "SETALL\n"  // 16
  "R\n"       // 17
  "G\n"       // 18
  "B\n"       // 19
  ;

LogoBuiltinWord builtins[] = {
  { "ON", &ledOn },
  { "OFF", &ledOff },
  { "REDR", &redRaw, 1 },
  { "GREENR", &greenRaw, 1 },
  { "BLUER", &blueRaw, 1 },
//   { "$14", &ledOn },
//   { "$15", &ledOff },
//   { "$11", &redRaw, 1 },
//   { "$12", &greenRaw, 1 },
//   { "$13", &blueRaw, 1 },
};
ArduinoTimeProvider time;
ArduinoFlashString strings(strings_flash);
Logo logo(builtins, sizeof(builtins), &time, Logo::core, &strings);
LogoCompiler compiler(&logo);

// the initial program goes into FLASH
static const char program_flash[] PROGMEM = 
  "TO SCLR :C; 100 - :C / 100  * 255; END;"
  "TO REDC :N; REDR SCLR :N; END;"
  "TO GREENC :N; GREENR SCLR :N; END;"
  "TO BLUEC :N; BLUER SCLR :N; END;"
  // arguments are reversed!
  "TO SETALL :B :G :R; REDC :R GREENC :G BLUEC :B; END;"
  "TO AMBER; SETALL 100 75 0; END;"
  "TO RED; SETALL 100 0 0; END;"
  "TO GREEN; SETALL 0 100 0; END;"
  "TO BLUE; SETALL 0 0 100; END;"
//   "TO $1 :$2; 100 - :$2 / 100  * 255; END;"
//   "TO $3 :$4; $11 $1 :$4; END;"
//   "TO $5 :$4; $12 $1 :$4; END;"
//   "TO $6 :$4; $13 $1 :$4; END;"
// //   // arguments are reversed!
//   "TO $16 :$19 :$18 :$17; $3 :$17 $5 :$18 $6 :$19; END;"
//   "TO $7; $16 100 75 0; END;"
//   "TO $8; $16 100 0 0; END;"
//   "TO $9; $16 0 100 0; END;"
//   "TO $10; $16 0 0 100; END;"
  ;

void flashErr(int mode, int n) {

#ifdef SERIAL
  Serial.print("err ");
  Serial.print(mode);
  Serial.print(", ");
  Serial.println(n);
#endif

 // it's ok to tie up the device with delays here.
  for (int i=0; i<mode; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
  delay(500);
  for (int i=0; i<n; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

// At the start
void setup() {

  // Setup the LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // the RGB pins
  pinMode(RED_PIN, OUTPUT);
  analogWrite(RED_PIN, 255);
  pinMode(BLUE_PIN, OUTPUT);
  analogWrite(BLUE_PIN, 255);
  pinMode(GREEN_PIN, OUTPUT);
  analogWrite(GREEN_PIN, 255);
 
 #ifdef SERIAL
  Serial.begin(9600);
 #endif

  // Setup I2C bus address
  Wire.begin(I2C_ADDRESS);
  
  // Register handler for when data comes in
  Wire.onReceive(receiveEvent);

  // Compile a little program into the LOGO interpreter :-)
  ArduinoFlashString str(program_flash);
  compiler.compile(&str);
  int err = logo.geterr();
  if (err) {
    flashErr(1, err + 2);
  }
}

// recieve data on I2C and write it into the buffer
void receiveEvent(int howMany) {
  while (Wire.available()) {
    buffer.write(Wire.read());
  }
}

// Go around and around
void loop() {

  // consume the serial data into the buffer as it comes in.
#ifdef SERIAL
   while (Serial.available()) {
    buffer.write(Serial.read());
  }
#endif

  // The buffer is filled in an interrupt as it comes in

  // accept the buffer into the command parser
  cmd.accept(&buffer);
  
  // when there is a valid command
  if (cmd.ready()) {

    // read it in
    cmd.read(cmdbuf, sizeof(cmdbuf));
#ifdef SERIAL
    Serial.print("cmd ");
    Serial.println(cmdbuf);
#endif

    // reset the code but keep all our words we have defined.
    logo.resetcode();
    
    // compile whatever it is into the LOGO interpreter and if there's
    // a compile error flash the LED
    compiler.compile(cmdbuf);
    int err = logo.geterr();
    if (err) {
      flashErr(1, err);
    }
  }
  
  // just execute each LOGO word
  int err = logo.step();
  
  // And if there was an error doing that, apart from STOP (at the end)
  // flash the LED
  if (err && err != LG_STOP) {
    flashErr(2, err);
  }

}

