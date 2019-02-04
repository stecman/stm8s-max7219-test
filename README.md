# MAX7219 Digital Clock (STM8)

A prototype for a project that was [later implemented on an Atmel ATTiny13A](https://github.com/stecman/avr-doomclock)
using a Linux toolchain for STM8(S) microcontrollers.

This counts up hours, minutes and seconds on six 7-segment digits, driven by a MAX7219.
The current time is set correctly at compile time. The clock source is not precise, so the clock
loses or gains at least 10 seconds per day on my STM8S003F3. Brightness is set based on the time of day.

This is not maintained, but hopefully serves as an example of STM8 timers and SPI.

## Setup

Grab the code:

```sh
git clone https://github.com/stecman/stm8s-max7219-test.git
cd stm8s-max7219-test

# Pull in SDCC compatible STM8S peripheral library
git submodule init
git submodule update
```

Once the toolchain below is available:

```sh
# Build
scons

# Flash through STLinkV2
scons flash
```

## Linux Toolchain

### SCons (build tool)

This should be available in your distribution's pacakage manager. It can also be installed via Python's `pip` package manager.

### SDCC (compiler)

[SDCC](http://sdcc.sourceforge.net/) may be available in your distro's package manager. I recommend installing from source to get the most recent release, as the Debian and Ubuntu repos can be a few versions behind:

```sh
# with Git
git clone https://github.com/svn2github/sdcc

# or with SVN
svn co http://svn.code.sf.net/p/sdcc/code/trunk sdcc

cd sdcc/sdcc
./configure
make
sudo make install
```

### stm8flash (flashing tool)

[stm8flash](https://github.com/vdudouyt/stm8flash) uses an STLink V1/V2 to program STM8 devices through their SWIM interface.

This needs to be compiled from source currently, which is simple:

```sh
git clone https://github.com/vdudouyt/stm8flash
cd stm8flash
make
sudo make install
```