# displaymode

`displaymode` is a command-line utility for changing the display resolution on Mac OS X.  It will work on Mac OS X 10.6 or later.

## Usage

To change the resolution of the main display to 1440x900, run the command:

```
./displaymode t 1440 900
```

To change the resolution of the secondary display to 1440x900, run the command:

```
./displaymode t 1440 900 1
```

You can get a list of active displays and available resolutions by running:

```
./displaymode d
```
The output will be a table like:

```
Display 0:
2560 x 1600 @60.0Hz *
1280 x 800 @60.0Hz
2880 x 1800 @60.0Hz
640 x 480 @60.0Hz !

Display 1:
800 x 600 @75.0Hz *
```

where each row is the width x height in pixels.  `*` indicates the current mode, and `!` indicates modes that are not usable for the desktop.

Display 0 is always the main display.

## Other options

`./displaymode h` prints a summary of the options.

`./displaymode v` prints the version and copyright notice.
