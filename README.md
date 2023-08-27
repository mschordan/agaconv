# AGAConv

## Overview
This is a retro computing just-for-fun project. AGAConv is a CDXL video converter for [Commodore-Amiga computers](https://en.wikipedia.org/wiki/Amiga). It can convert modern videos (MP4, etc.) into the CDXL format which can be played with [AGABlaster](https://cutt.ly/AGABlaster) on an AGA Commodore-Amiga computer. The first Commodore-Amiga computer with the AGA chipset was released in 1992. Commodore went bankrupt in April 1994.

The CDXL format was created by [Commodore](https://en.wikipedia.org/wiki/Commodore_International) primarily for the [Commodore CDTV](https://en.wikipedia.org/wiki/Commodore_CDTV), released 1991, to permit playback of video from CD-ROM in the early 1990s. The [CDXL format](https://wiki.multimedia.cx/index.php/CDXL) is a simple streaming format, consisting of linear concatenated chunks (packets), each with an uncompressed frame and associated audio data. In addition to the original 12-bit colors, AGAConv also supports 24-bit colors, as supported by Commodore Amiga AGA systems released in 1992+.

AGAConv utilizes ffmpeg to extract video frames and audio data from videos and encodes the extracted data into the CDXL format.

## Features
Overview of supported features:

  * Configuration file allows to set any options as default.
  * Each CDXL frame with its own color palette
  * Color modes:
     - AGA8 to AGA2, 24 bit colors
     - OCS5 to OCS2, 12 bit colors
     - HAM8, 24 bit colors (requires [ham_convert](http://mrsebe.bplaced.net/blog/wordpress/?page_id=374))
     - HAM6, EHB, 12 bit colors (requires [ham_convert](http://mrsebe.bplaced.net/blog/wordpress/?page_id=374))
  * Screen (resolution) modes:
     - Lores, Hires, Superhires
  * Audio
    -  8-bit mono/stereo
  * Encoding of custom CDXL videos (minimal number of bitplanes in each frame)
    - With 24-bit color palette and variable length frames
    - Any audio frequency
  * Encoding of standard CDXL videos (fixed number of bitplanes)
    - Audio Frequency is automatically adjusted such that all CDXL frames have the same size
    - 12-bit colors (default)
    - 24-bit colors are supported in RGB 3-byte format (same as IFF/ANIM CMAP chunk body data)
  * More information about video conversions can be found on the [AGAConv website](https://cutt.ly/AGAConv).

## How It Works

AGAConv uses ffmpeg to extract video frames and audio data from any video format supported by ffmpeg and encodes them into a CDXL video. For HAM conversions it can integrate the tool [ham_convert](http://mrsebe.bplaced.net/blog/wordpress/?page_id=374) into the conversion pipeline. 
The CDXL video can be played with [AGABlaster](https://cutt.ly/AGABlaster) on a Commodore-Amiga computer.

# Building from source

AGAConv can be built from source and run on Ubuntu as follows:

* **sudo apt install make g++ libpng-dev man-db ffmpeg**
* **cd agaconv**
* **make -j6**

Builds the excutable 'bin/agaconv'.

* '**make view-man**' shows the man page (without installing it)
* '**make clean**' removes all generated files.

# Binary Distribution - Ubuntu PPA Installer

* A binary distribution of AGAConv 1.0+ is available as PPA package and can be installed with apt on Ubuntu.
* Instructions for the binary PPA installation can be found on the [AGAConv website](https://cutt.ly/AGAConv).
 
# Where to find more information

* Website: [AGAConv website with binary PPA installer instructions](https://cutt.ly/AGAConv)
* Twitter: [AGAConv and AGABlaster announcements](https://twitter.com/AGABlaster)
* Youtube: [AGABlaster channel with Amiga AGAConv videos](https://www.youtube.com/@AGABlaster)

# Version

Version: 1.0.3

# Author

Author: Markus Schordan, Copyright 2019-2023.

# License

License: GPL v3+ open-source

# Acknowledgments
* Thanks to Jean Bollaërt for testing the pre-release version of AGAConv 0.9.4 on Windows 10 with Microsoft's Ubuntu app/terminal.
* Thanks to Sebastian Sieczko for adding command line options to his graphic converter [ham_convert](http://mrsebe.bplaced.net/blog/wordpress/?page_id=374) such that it could be integrated into the AGAConv conversion pipeline.
