
# Tiny LOGO

## Context

Read: https://github.com/visualopsholdings/ringbuffer

Read: https://github.com/visualopsholdings/cmd

Ok. So you've written all this code, you can send various commands to your Arduino to
do interesting things, but how nice would it be to be able to SEND AND RUN ACTUAL CODE
to the Arduino AFTER you've left it on the Moon!

That's where the whole "TinyLogo" project that is a part of this is for. It's only in
early days with very simple syntax and capabilities, but just for a teaser take
a look at the source for led/led.ino and you
can see some cool code that takes this LOGO program:

```
to ON
  dhigh 13
end
to OFF
  dlow 13
end
to FLASH
  ON WAIT 100 OFF WAIT 1000
end
to GO
  FOREVER FLASH
end
to STOP
  OFF
end
```

And then when you get an I2C string or serial command that says "GO;" (or "GOT\n" it will flash the LEDs on and off.

You can send it any of that code ON THE FLY, so even word definitions.

So You could send it:

```
FLASH
```

And it would flash and

```
STOP
```

to stop it.

And that code above fits happily on a Leonardo with 32k of memory.

After you compile an example with LOGO in it, make sure you have at least 100 bytes for local
variables or it just won't work!!

To free more space, go into the top of logo.hpp and change the sizes of various data structures
until you free more up.

Open "led/led.ino" and you can flash that example right onto your Arduino and try it out.

There are about 5 different examples in there of various other things to get you started using
the various ways you might use this stuff.

One nice feature of the LOGO code is that implements WAIT the correct way without using delay().

You can read more about logo here https://en.wikipedia.org/wiki/Logo_(programming_language)

### Using in your sketch

Declare Logo like this:

Now you can declare the logo runtime and compiler ready to use.

```
ArduinoTimeProvider time;
Logo logo(&time);
LogoCompiler compiler(&logo);
```

Now in your "setup()", compile your LOGO program:

```
compiler.compiler("to ON; dhigh 13; end;");
compiler.compiler("to OFF; dlow 13; end;");
compiler.compiler("to FLASH; ON WAIT 100 OFF WAIT 100; end;");
compiler.compiler("to GO; FOREVER FLASH; end;");
compiler.compiler("to STOP; OFF; end;");
int err = logo.geterr();
if (err) {
  ... do something with the error.
}
```

And then your loop would get the string "FLASH", "GO" or "STOP" and just feed it
to logo someline like this:

```
  if (you have a command) {
    logo.resetcode();
    compiler.compile(command);
    int err = logo.geterr();
    if (err) {
      ... do something with the error.
    }
  }
```

And near the bottom of your loop just "step" logo and it will "do" the code :-)

```
  int err = logo.step();
  if (err && err != LG_STOP) {
      ... do something with the error.
  }
```

### LOGO syntax

#### word definitions

Words need to be defined on 3 lines like this:

```
to wordname; BLOCK; end
```

OR
```
to wordname
  BLOCK
end
```

A semicolon is just a replacement for new line. White space is ignored, but you MUST
have spaces between words

#### Arguments

Argumemts to functions work BUT it is just a lexical trick, not scoped at all, 
so recursion won't work.

```
to MULT :A :B
  A * :B
end
MULT 10 20
```

Is functionally equivalent to:

```
to MULT
  :A * :B
end
make \"A 10 
make \"B 20 
MULT
```

Note: There is a bg where the arguments are pushed in reverse order of the call :-( so the 
code above is really:

```
to MULT :B :A
  :A * :B
end
MULT 10 20
```

### LOGO words

Builtin words are not case sensitive. So you could write make, Or MAKE or even Make.

Your own words Are case sensitive.

#### make 

Make a variable. The syntax is:

```
make "VARNAME VALUE
VALUE := [number | string] 
```

#### thing 

Push the value of a variable onto the stack.

```
thing "VARNAME
```

This is the same as :VARNAME.

#### forever

Repeat the next word forever. The syntax is:

```
forever wordname
```

#### repeat

Repeat the next word a number of times. The syntax is:

```
repeat num word
num := [number | varname]
word := [wordname | literal]
literal := [number | string | var] 
var := :varname
```

#### ifelse

Test an expression, it true put the first word on the stack else the second. The syntax is:

```
ifelse expr then else
expr := [wordname | literal]
literal := [number | string | var] 
var := :varname
then := [wordname | literal]
else := [wordname | literal]
```

#### if

Test an expression, it true execute the next word. The syntax is:

```
if expr then
expr := [wordname | literal]
literal := [number | string | var] 
var := :varname
then := [wordname | literal]
```

#### =

Push a 1 on the stack if the operands are the same. The syntax is:

```
word '=' word
word := [wordname | literal]
literal := [number | string | var] 
```

#### !=

Opposite of =

#### WAIT (not working yet so not in current build)

Wait for milliseconds before executing the next word. The syntax is:

```
WAIT num wordname
num := [number | var]
var := :varname
```

#### Arithmetic

+, -, * and / all work but the order of evaluation is not what you would expect. It's
right to left, not left to right.

So the expression:

```
3 / 4 + 4
```

To yield 4.75 (it will give you 0.375), Just write:

```
(3 / 4) + 4
```

#### Lists

Lists are represented as strings or numbers between []

So the expression:

```
[ 1 2 3 ]
```

Would have a list of 3 elements on the stack. The elements can be mixed types:

```
[ a word with 1.2 and 3 ]
```

Which would have a list with 3 strings, a double another string and an integer.

Note: The only thing supported is to print a list. Printing doesn't support nested
lists.

#### Logic

>, >=, <, <=  and ! all work like you wold think.

#### print

Print the literal out to the serial port

```
print word
word := [wordname | number | string | list | var]
var := :varname
```

#### dread

Read from a digital pin. Takes the pin number off the stack and pushes the value at that
pin onto the stack.

```
dread pin
pin := [wordname | number | var]
var := :varname
```

#### dhigh

Output high to a digital pin. Takes the pin number off the stack.

```
dhigh pin
pin := [wordname | number | var]
var := :varname
```

#### dlow

Output low to a digital pin. Takes the pin number off the stack.

```
dlow pin
pin := [wordname | number | var]
var := :varname
```

#### pinout

Set's a PIN up to be an output. Takes the pin number off the stack.

```
pinout pin
pin := [wordname | number | var]
var := :varname
```

#### pinin

Set's a PIN up to be an input. Takes the pin number off the stack.

```
pinin pin
pin := [wordname | number | var]
var := :varname
```

#### pininup

Set's a PIN up to be an input and use it's internal pull up resistor. Takes the pin number off the stack.

```
pininup pin
pin := [wordname | number | var]
var := :varname
```

#### pinrgb

Set's a PIN up to be an rgb (analog) output and assigns a channel. Takes the pin number off the stack, and
the channel number off the stack.

This handles PINs on all arduinos. There can be up to 8 channels.

```
pinrgb pin channel
pin := [wordname | number | var]
channel := [wordname | number | var]
var := :varname
```

#### rgbout

Output's a value to an analog output that is a channel of an RGB LED. Takes the channel number and value off the stack.

The channel had to be assigned by "pinrgb" above.

On an AVR arduino, we reverse the values to be more convenient. So 0 is off an 255 is full.
```
rgbout channel value
channel := [wordname | number | var]
value := [wordname | number | var]
var := :varname
```

#### aout

Output's a value to an analog output. Takes the pin number and value off the stack.

This only works on those Arduinos that have hardware PWM. For others, or if your not
sure, use pinrgb/rgbout instead of pinout/aout.

```
aout pin value
pin := [wordname | number | var]
value := [wordname | number | var]
var := :varname
```

#### type

Output the type of the value on the stack.

```
type value
value := [number | string | list | var]
var := :varname
```

#### machineinfo

Output the sizes used for TinyLogo and the version.

```
machineinfo
```

#### wifistation

Set the arduino as up as a wifi station if Wifi is supported

```
wifistation
```

#### wifiscan

Push a list of wifi access points available onto the stack.

```
print wifiscan
```

#### wificonnect

Pop the access point name and password off the stack and connect, if successful
push the IP address on the stack.

```
print wificonnect "MyAccessPoint" "password"
```

#### wifiget

Send an HTTP "GET" to the server.

This relies on the server returning JSON and then the field is selected
```
print wifiget "192.168.0.1" 8081 "/somejson" "fieldx"
```

#### wifilogin

Send an HTTP "POST" with this json to the server.

```
{ "name":  "fred", "password": "password", "insecure": false }
```

When the server replies with "Set-Cookie", returns (and remembers) the cookie.

```
print wifilogin "remoteurl" 443 "fred" "password"
```

#### wifilogintest

Send an HTTP "GET" like this to the server.

```
/login/?username=fred
```

When the server replies with "Set-Cookie", returns (and remembers) the cookie.

```
print wifilogintest "192.168.0.1" 8081 "fred"
```

#### wifisockets

Setup Socket.IO with a visual ops server.

The connect MSG is sent to the server upon connection
```
wifisockets "remoteurl" 443 connectmsg
```

#### vopsopenmsg

Creates a special piece of JSON that describes an "open stream" message for it's sockets.

```
  [ "openDocuments", { "docs": [ { "id": "streamid", "type": "stream" } ], "userid": "userid" } ]
```

```
print vopsopenmsg "vopsdocid" "vopsuserid"
```

#### btstart

Starts up BLE (Bluetooth Low Energy) with the name and service ID specified.

This creates 2 characteristics:

Compile: c807fa87-21a3-47b2-a0f8-9cd1da3407b7

Send this lines of Logo code and it will compile them into the words.

Run: 9de01783-a173-4d85-a7f6-48c7a82c6a63

Send this:

RESETCODE

reset the code, but NOT the words. Then when you compile, the actual code specified
will be rady to run. Otherwise your compiled code will APPEND to the code in memory
and most likely you will get unusual results.

RESTART

Run the program from the start


So to send a program you would most likely:

Run: RESETCODE
For each line:
  Compile:
Run: RESTART

```
btstart "name" "ServiceUID"
```

### Excpetions

Exceptions are builtin to the engine and use operators.

Remember that all whitespace is ignored, so you can make your exception blocks look
nice like this:

{
  GOOD1
  GOOD2
  BAD
  GOOD3
} HANDLEIT

So in this case, GOOD1, GOOD2 would do something and then whatever succeeds in BAD and
then we jump down to HANDLEIT and do that.

To fail, simply use a period and why you failed

. "Something bad happened"

Then later on you can get that message with & like this

print &

```
to ONE
  print 1
end

to TWO
  print 2
end

to THREE
  print 3
end

to FOUR
  . \"failed
end

to FIVE
  print 5
end

to SIX
  print 6
end

to SHOWERR
  print &
end

ONE 
TWO 
{ 
  THREE FOUR FIVE 
} SHOWERR 
SIX
```

This code would print 1 2 3 failed 6

If you are familiar with other languages, ? is like "try" and # is like "catch"

### UCBLogo differences

- On page 4, we don't implement the setter notation, SETFOO is just the name of a defined
word and the variable AllowGetSet is as if it's TRUE.
- Most of section 1.4 might be different, especially comments which can just be a # and then
anything else.
- Only a few of the words are defined (see above) there are no lists, just strings and numbers.

## Static code

Using the compiler on the arduino takes a lot of program space up. There is a tool called "flashcode" that will
take a logo program and outpu a compiled version of that program that can exist entirely in the 
flash memory of the Arduino.

The simplest possible way to use it is to create a .ino file that looks like this:

```
#include "logosketch.hpp"

//#LOGO FILE=../logo/btnledflash.lgo NAME=sketch
//#LOGO ENDFILE

LogoSketch sketch(strings_sketch, (const PROGMEM short *)code_sketch);
void setup() {
  sketch.setup();
}
void loop() {
  sketch.loop();
}
```

$ tools/build/flashcode sketch/sketch.ino

Which will find the .lgo file, compile it and outut the compiled code into that section. Then
just open up your .ino sketch and upload it to your arduino and run it.

There are examples in the "logo" folder.

You can do a setup by simply declaring a "SETUP" word like this:

```
to LEDPIN
  13
end
to BTNPIN
  7
end
to SETUP
  pinout LEDPIN
  dlow LEDPIN
  pinin BTNPIN
end
```

This will be run automatically at the start in the sketch.setup() line.

To build the tool see below in development.

To use it:

$ tools/build/flashcode logo/rgb.lgo --name rgb

Which will output the runtime code and strings list as variables you simply include in your sketch. Then
you can dispense with the compiler completely and use your code like this:

```
ArduinoFlashString strings(strings_rgb);
ArduinoFlashCode code((const PROGMEM short *)code_rgb);
ArduinoTimeProvider time;
Logo logo(&time, &strings, &code);
```

And now instead of COMPILING your commands into the runtime in your loop, you can call any of the
words your have defined or primitives directly with

```
    int err = logo.callword(cmdbuf);
    if (err) {
      ... do something with the error.
    }
```

This will allow your to fit quite large programs into yoru arduino, and also allow the programs
to come from an actual text file with comments etc.

The tool can also modify your .ino file directly with this:

$ tools/build/flashcode rgb/rgb.ino

This searches for 2 lines in the file:

```
//#LOGO FILE=../logo/rgb.lgo NAME=rgb BTNPIN=9
//#LOGO ENDFILE
```

And modifies whatever is between 2 lines to output the actual compiled runtime for that logo file.
This allows development to be very quick. You can modify and run your .lgo file in a dev
environment and then run flashcode and then in the IDE upload directly to your Arduino
to try the code out.

If you had "$BTNPIN" inside your .lgo file, it would be replaced by "9". Have as many
directives as you want.

## Running a .lgo file and seeing what it does.

You can run one of the LOGO source files directly with the "run" tool provided.

To build the tool see below in development.

To use it:

$ tools/build/run logo/rgb.lgo

## Development

The development process for all of this code used a normal Linux environment with the BOOST
libraries and a C++ compiler to write and run unit tests to test all of the various
things and then use that code in the Arduino IDE to flash the Arduino.

It's written in a subset of C++ that happily builds on an Arduino:

  - No dynamic memory (no new, malloc etc. Everything just static)
  - No inheritence (see above)
  - No STL (see above)

BUT for the actual development, including a fairly complex debugger for the LOGO stuff,
we use all of these things and just don't build that bit for the Arduino.

Firstly, checkout the repos:

https://github.com/visualopsholdings/ringbuffer
https://github.com/visualopsholdings/cmd

To be a sibling of this repo. So they need to be in the same folder like this:

- folder
  - cmd
  - ringbuffer
  - tinylogo

So on your Linux (or mac using Homebrew etc), get all you need:

```
$ sudo apt-get -y install g++ gcc make cmake boost
```

And then inside the "test" folder on your machine, run:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ make test
```

And then you can go in and fiddle and change the code in lgtestsketch.cpp to change what
your sketch might look like on in your .ino and then do

```
$ make && ./LGTestSketch
```

And it will just run your test in your Dev environment! Just stub out all your builtin words
like "ledOn()" etc to output a string.

For an M1 or M2 mac, use this command for cmake

```
$ cmake -DCMAKE_OSX_ARCHITECTURES="arm64" ..
```

To turn on all the debugging for the various things, each header has something like:

```
//#define LOGO_DEBUG
```

That you can comment out BUT while it's commented out the code won't build on the Arduino.

To build the tools, rather than the tests, just:

```
$ cd tools
$ mkdir build
$ cd build
$ cmake ..
$ make
$ make test
```

This stuff is just for the development environment.
  
## Related discussions

https://forum.arduino.cc/t/how-to-properly-use-wire-onreceive/891195/12

## Current development focus

### Implement exception handling in BLE and WIFI work
  
## Change Log

### 24 May 2023
- Moved tiny logo, cmd and ringbuffer to their repos.

### 25 May 2023
- Broke compiler out into seperate class hopefully not needed. Also added code
to allow strings and code to be in FLASH memory.

### 26 May 2023
- Eliminate a whole lot of the buffers to free memory up.

### 30 May 2023
- Rewrite scanning to not need buffers in simple cases (still needed for sentences)

### 3 Jun 2023
- Major rewrite to allow compilation and storage of the program in the flash memory of the arduino.

### 6 Jun 2023
- Rewrote the way primitive code works and a new tool which can run an .lgo file.
- Removed sentence support until it's far more robust.

### 7 Jun 2023
- Rewrote the ifelse works to be simpler.

### 10 Jun 2023
- Add code to flashcode to allow it to inline process an LOGO file.

### 13 Jun 2023
- Finally created a sketch with decent amount of logic that allows 2 buttons to be
pressed to drive 2 colored LEDs called trafficlights.lgo

### 14 Jun 2023
- Logo sketches are a thing now! The .lgo file can contain the pin definitions and all the 
writes and reads. Use the flashcode tool to write new LOGO compiled code right into 
your sketch.

### 15 Jun 2023
- Remove case sensitivity and whitespace sensitivity. You can write 3+3/4 now.

### 17 Jun 2023
- Get arithmetic grouping working. So you can write (3+3)/4 now.

### 18 Jun 2023
- Arguments don't need : in TO X AA now. More work following along with the UCB manual.

### 26 Jun 2023
- Get working for non AVR arduinos like the esp32 modules.

### 29 Jun 2023
- Rudimentary support for being able to PRINT a list. This is actually full support for
storing lists but only "PRINT" knows how to use them.

### 30 Jun 2023
- Track down bug that stopped sketches working on the esp32
- add in ability use directives to replace values in .lgo files

### 31 Jun 2023
- Start work for allowing sketches to be inlined for large hardware

### 2 Jul 2023
- Get ESP32 RGB LED code working.

### 3 Jul 2023
- Get Wifi (rudimentary) working. But it uses HTTPS :-)

### 9 Jul 2023
- Get sockets, wifi access points, https requests working.
- make can now create string, list, double and int variables.
- remove the SSL code. Not needed now.

### 14 Jul 2023
- BLE support

### 15 Jul 2023
- BLE load utility for loading code into a BLE ready ESP32

### 16 Jul 2023
- Implement exception handling









