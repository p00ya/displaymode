// displaymode - a utility for changing the display resolution on Mac OS X.
//
// Copyright 2019 Dean Scarff.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Compilation:
//   clang -std=c11 -framework CoreFoundation -framework CoreGraphics -o displaymode displaymode.c
//
// Usage (to change the resolution to 1440x900):
//   displaymode t 1440 900

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>

// States for the main invocation "option".
//
// The enum value of the alphabetical options matches the letter that should
// be used on the command line.
enum Option {
    kOptionMissing = 0,
    kOptionInvalid = 1,
    kOptionInvalidMode = 2,
    kOptionSupportedModes = 'd',
    kOptionHelp = 'h',
    kOptionConfigureMode = 't',
    kOptionVersion = 'v',
};

// Positions in argv of various expected parameters.
enum {
    kArgvOptionIndex = 1,
    kArgvWidthIndex = 2,
    kArgvHeightIndex = 3,
};

// Represents the command-line arguments after parsing.
struct ParsedArgs {
    enum Option option;
    const char * literal_option;
    unsigned long width;
    unsigned long height;
};

// Parses the command-line arguments and returns them.
struct ParsedArgs ParseArgs(int argc, const char * argv[]) {
    struct ParsedArgs parsed_args = { 0 };

    if (argc <= 1) {
        return parsed_args;
    }
    if (1 != strlen(argv[kArgvOptionIndex])) {
        return parsed_args;
    }

    // Validate the command.
    parsed_args.literal_option = argv[kArgvOptionIndex];
    // All options are single-letter.
    const char option = argv[kArgvOptionIndex][0];
    switch (option) {
        case kOptionSupportedModes:
        case kOptionHelp:
        case kOptionConfigureMode:
        case kOptionVersion:
            parsed_args.option = option;
            break;
    }

    // Parse "width height" mode specification.
    if (option == kOptionConfigureMode) {
        if (kArgvHeightIndex <= argc) {
            const unsigned long width =
                strtoul(argv[kArgvWidthIndex], NULL, 10);
            const unsigned long height =
                strtoul(argv[kArgvHeightIndex], NULL, 10);
            if (0 < width && 0 < height) {
                parsed_args.width = width;
                parsed_args.height = height;
            } else {
                parsed_args.option = kOptionInvalidMode;
            }
        } else {
            parsed_args.option = kOptionInvalidMode;
        }
    }
    return parsed_args;
}

const char kUsage[] =
    "Usage:\n\n"
    "  displaymode [options...]\n\n"
    "Options:\n"
    "  t <width> <height>\n"
    "      sets the main display's resolution to width x height\n\n"
    "  d\n"
    "      prints available resolutions for the main display\n\n"
    "  h\n"
    "      prints this message\n\n"
    "  v\n"
    "      prints version and copyright notice\n";

// Prints a message describing how to invoke the tool on the command line.
void ShowUsage() {
    puts(kUsage);
}

// Prints all display modes for the main display.  Returns 0 on success.
int PrintModes() {
    CGDirectDisplayID display = CGMainDisplayID();

    CGDisplayModeRef current_mode = CGDisplayCopyDisplayMode(display);

    CFArrayRef modes = CGDisplayCopyAllDisplayModes(display, NULL);
    const CFIndex count = CFArrayGetCount(modes);

    for (CFIndex index = 0; index < count; index++) {
        CGDisplayModeRef mode =
            (CGDisplayModeRef) CFArrayGetValueAtIndex(modes, index);
        const size_t width = CGDisplayModeGetWidth(mode);
        const size_t height = CGDisplayModeGetHeight(mode);
        const char * current = (CFEqual(mode, current_mode)) ? "*" : "";
        printf("%zu x %zu %s\n", width, height, current);
    }
    CFRelease(modes);
    CGDisplayModeRelease(current_mode);
    return EXIT_SUCCESS;
}

// Returns the first mode whose resolution matches the width and height
// specified in `parsed_args'.  Returns NULL if no modes matched.
// The caller owns the returned mode.
CGDisplayModeRef GetModeMatching(const struct ParsedArgs * parsed_args) {
    CGDirectDisplayID display = CGMainDisplayID();
    CFArrayRef modeList = CGDisplayCopyAllDisplayModes(display, NULL);
    const CFIndex count = CFArrayGetCount(modeList);

    CGDisplayModeRef matched_mode = NULL;
    // Set matched_mode to the first display mode matching the requested
    // resolution.
    for (CFIndex index = 0; index < count; index++) {
        CGDisplayModeRef const mode =
            (CGDisplayModeRef) CFArrayGetValueAtIndex(modeList, index);
        const size_t width = CGDisplayModeGetWidth(mode);
        const size_t height = CGDisplayModeGetHeight(mode);
        if ((int) width == parsed_args->width &&
            (int) height == parsed_args->height) {
            matched_mode = CGDisplayModeRetain(mode);
            break;
        }
    }
    CFRelease(modeList);
    return matched_mode;
}

// Changes the resolution permanently for the user.
int ConfigureMode(const struct ParsedArgs * parsed_args) {
    CGDisplayModeRef mode = GetModeMatching(parsed_args);
    if (NULL == mode) {
        fprintf(stderr, "Could not find a mode for resolution %lux%lu", parsed_args->width, parsed_args->height);
        return -1;
    }
    CGDirectDisplayID display = CGMainDisplayID();

    // Save the original resolution.
    CGDisplayModeRef original_mode = CGDisplayCopyDisplayMode(display);
    const size_t original_width = CGDisplayModeGetWidth(original_mode);
    const size_t original_height = CGDisplayModeGetHeight(original_mode);
    CGDisplayModeRelease(original_mode);

    // Change the resolution.
    CGDisplayConfigRef config;
    CGError e;
    if ((e = CGBeginDisplayConfiguration(&config))) {
        fprintf(stderr, "CGBeginDisplayConfiguration CGError: %d\n", e);
        return e;
    }
    if ((e = CGConfigureDisplayWithDisplayMode(config, display, mode, NULL))) {
        fprintf(stderr, "CGConfigureDisplayWithDisplayMode CGError: %d\n", e);
        return e;
    }
    if ((e = CGCompleteDisplayConfiguration(config, kCGConfigurePermanently))) {
        fprintf(stderr, "CGCompleteDisplayConfiguration CGError: %d\n", e);
        return e;
    }
    CGDisplayModeRelease(mode);

    printf("Changed display resolution from %zux%zu to %lux%lu\n",
           original_width, original_height,
           parsed_args->width, parsed_args->height);
    return EXIT_SUCCESS;
}

int main(int argc, const char * argv[]) {
    const struct ParsedArgs parsed_args = ParseArgs(argc, argv);
    switch (parsed_args.option) {
        case kOptionMissing:
            fputs("Missing option; server mode is not supported\n\n", stderr);
            ShowUsage();
            break;

        case kOptionInvalid:
            fprintf(stderr, "Invalid option: '%s'\n\n", parsed_args.literal_option);
            ShowUsage();
            break;

        case kOptionInvalidMode:
            fputs("Invalid mode\n", stderr);
            break;

        case kOptionConfigureMode:
            return ConfigureMode(&parsed_args);

        case kOptionHelp:
            ShowUsage();
            return EXIT_SUCCESS;

        case kOptionSupportedModes:
            return PrintModes();

        case kOptionVersion:
            puts("displaymode 1.0\nCopyright 2019 Dean Scarff");
            return EXIT_SUCCESS;
        default:
            break;
    }
    return EXIT_FAILURE;
}
