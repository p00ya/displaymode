# displaymode

`displaymode` is a command-line utility for changing the display resolution on Mac OS X.  It will work on Mac OS X 10.6 or later.

## Installation

## With Xcode

If you have Xcode (or the Xcode command line tools) installed, it's best to compile it yourself.  From the terminal:

1. `git clone https://github.com/p00ya/displaymode.git`
2. `cd displaymode`
3. `clang -std=c11 -lm -framework CoreFoundation -framework CoreGraphics -o displaymode displaymode.c`

## Without Xcode

If you don't have Xcode installed, then you can download the binary (note this is not a zip file) then give the OS permission to run it.  From the terminal:

1. `curl -LO https://github.com/p00ya/displaymode/releases/latest/download/displaymode`
2. `xattr -d com.apple.quarantine displaymode`
3. `chmod a+x displaymode`
4. `codesign -f -s "-" -v displaymode`

## Usage

To change the resolution of the main display to 1440x900, run the command:

```
./displaymode t 1440 900
```

To change the resolution of the secondary display to 1440x900, run the command:

```
./displaymode t 1440 900 1
```

You can also specify a particular refresh rate:

```
./displaymode t 1440 900 @60
```

You can get a list of active displays and available resolutions by running:

```
./displaymode d
```
The output will be a table like:

```
Display 0 (MAIN):
2560 x 1600 @60.0Hz *
1280 x 800 @60.0Hz
2880 x 1800 @60.0Hz
640 x 480 @60.0Hz !

Display 1:
800 x 600 @75.0Hz *
```

where each row is the width x height in pixels.  `*` indicates the current mode, and `!` indicates modes that are not usable for the desktop.

## Other options

`./displaymode h` prints a summary of the options.

`./displaymode v` prints the version and copyright notice.
