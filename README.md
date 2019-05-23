# displaymode

`displaymode` is a command-line utility for changing the display resolution on Mac OS X.  It will work on Mac OS X 10.6 or later.

## Usage

To change the resolution of the main display to 1440x900, run the command:

```
./displaymode t 1440 900
```

You can get a list of available resolutions by running:

```
./displaymode d
```
The output will be a table like:

```
2560 x 1600 *
1280 x 800
2880 x 1800
```

where each row is the width x height in pixels, with an `*` to indicate the current mode.

## Other options

`./displaymode h` prints a summary of the options.

`./displaymode v` prints the version and copyright notice.
