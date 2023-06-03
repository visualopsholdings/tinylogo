
# Tiny LOGO

## Context

Read: https://github.com/visualopsholdings/ringbuffer

Read: https://github.com/visualopsholdings/cmd

Ok. So you've written all this code, you can send various commands to your Arduino to
do interesting things, but how nice would it be to be able to SEND AND RUN ACTUAL CODE
to the Arduino AFTER you've left it on the Moon!

That's where the whole "TinyLogo" project that is a part of this is for. It's only in
early days with very simple syntax and capabilities, but just for a teaser take
a look at the source for i2c.ino and search down to "#idef I2C_LOGO" and you
can see some cool code that takes this LOGO program:

```
TO FLASH
  ON WAIT 100 OFF WAIT 1000
END
TO GO
  FOREVER FLASH
END
TO STOP
  OFF
END
```

And then when you get an I2C string that says "GO;" it will flash the LEDs on and off.

You can send it any of that code ON THE FLY, so even word definitions.

So You could send it:

```
TO FLASH; ON WAIT 20 OFF WAIT 200; END;
```

To make it flash faster, or even completely write a new thing like a morse code
tapper etc without reflashing your Arduino :-)

And that code above fits happily on a Leonardo with 32k of memory (it about fills it though).

After you compile an example with LOGO in it, make sure you have at least 100 bytes for local
variables or it just won't work!!

To free more space, go into the top of logo.hpp and change the sizes of various data structures
until you free more up.

Open "i2c.ino" and you can flash that example right onto your Arduino and try it out.

There are about 5 different examples in there of various other things to get you started using
the various ways you might use this stuff.

One nice feature of the LOGO code is that implements WAIT the correct way without using delay().

You can read more about logo here https://en.wikipedia.org/wiki/Logo_(programming_language)

### Using in your sketch

Declare Logo like this:

You need to define your "primitives" which can do the various physical things your sketch needs to do.
So to flash an LED on or off it might be:

```
void ledOn(Logo &logo) {
  digitalWrite(LED_PIN, HIGH);
}
void ledOff(Logo &logo) {
  digitalWrite(LED_PIN, LOW);
}
```

Now you can declare the logo runtime and compiler ready to use.

```
LogoBuiltinWord builtins[] = {
  { "ON", &ledOn },
  { "OFF", &ledOff }
};
ArduinoTimeProvider time;
Logo logo(builtins, sizeof(builtins), &time, Logo::core);
LogoCompiler compiler(&logo);
```

Now in your "setup()", compile your LOGO program:

```
compiler.compiler("TO FLASH; ON WAIT 100 OFF WAIT 100; END;");
compiler.compiler("TO GO; FOREVER FLASH; END;");
compiler.compiler("TO STOP; OFF; END;");
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
TO wordname; BLOCK; END
```

#### Sentences

Sentences can be used and they are implemented as actual "unnamed words", like anonymous
closures or lambas.

```
[WORD WORD WORD]
```

#### Arguments

Argumemts to functions work BUT it is just a lexical trick, not scoped so recursion won't
work.

```
TO MULT :A :B; :A * :B; END;
MULT 10 20
```

Is functionally equivalent to:

```
TO MULT; :A * :B; END;
MAKE \"A 10 MAKE \"B 20 MULT
```

### LOGO words

## MAKE 

Make a variable. The syntax is:

```
MAKE "VARNAME VALUE
VALUE := [number | string] 
```

## FOREVER

Repeat the next word forever. The syntax is:

```
REPEAT WORD
WORD := [wordname | SENTENCE]
SENTENCE := "[" LITWORD ... "]"
LITWORD := the name of a word
```

## REPEAT

Repeat the next word a number of times. The syntax is:

```
REPEAT NUM WORD
NUM := [number | VAR]
VAR := :varname
WORD := [wordname | SENTENCE | LITERAL]
LITERAL := [number | string } VAR] 
VAR := :varname
SENTENCE := "[" LITWORD ... "]"
LITWORD := the name of a word
```

## IFELSE

Test an expression, it true put the first word on the stack else the second. The syntax is:

```
IFELSE EXPR THEN ELSE
EXPR := [wordname | SENTENCE | LITERAL]
LITERAL := [number | string } VAR] 
VAR := :varname
THEN := [wordname | SENTENCE | LITERAL]
ELSE := [wordname | SENTENCE | LITERAL]
SENTENCE := "[" LITWORD ... "]"
LITWORD := the name of a word
```

## =

Push a 1 on the stack if the operands are the same. The syntax is:

```
WORD = WORD
WORD := [wordname | SENTENCE | LITERAL]
SENTENCE := "[" LITWORD ... "]"
LITWORD := the name of a word
```

## WAIT (not working yet so not in current build)

Wait for milliseconds before executing the next word. The syntax is:

```
WAIT NUM WORD
NUM := [number | VAR]
WORD := [wordname | SENTENCE]
SENTENCE := "[" LITWORD ... "]"
LITWORD := the name of a word
```

## Arithmetic

+, -, * and / all work as usual but no parenthesis and just left to right.

## Static code

Using the compiler on the arduino takes a lot of program space up. There is a tool called "flashcode" that will
take a logo program and outpu a compiled version of that program taht can exist entirely in the 
flash memory of the Arduino.

To build the tool see below in development.

To use it:

$ tools/build/flashcode logo/rgb.lgo --name rgb

Which will output the runtime code and strings list as variables you simply include in your sketch. Then
you can dispense with the compiler completely and use your code like this:

```
ArduinoFlashString strings(strings_led);
ArduinoFlashCode code((const PROGMEM short *)code_led);
Logo logo(builtins, sizeof(builtins), &time, Logo::core, &strings, &code);
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

### OUTPUT from the Arduino to the PI

Actually pretty simple, just need examples.

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
