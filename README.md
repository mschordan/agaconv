# AGAConv

## Overview
This is a retro computing just-for-fun project. AGAConv is a CDXL video converter for [Commodore-Amiga computers](https://en.wikipedia.org/wiki/Amiga). It combines some existing tools to convert modern videos (MP4, etc.) into the CDXL format which can be played with [AGABlaster](https://mschordan.github.io/amiga/agablaster.html) on an AGA Commodore-Amiga computer. The first Commodore-Amiga computer with the AGA chipset was released in 1992. Commodore went bankrupt in April 1994.

The CDXL format was created by [Commodore](https://en.wikipedia.org/wiki/Commodore_International) primarily for the [Commodore CDTV](https://en.wikipedia.org/wiki/Commodore_CDTV), released 1991, to permit playback of video from CD-ROM in the early 1990s. The [CDXL format](https://wiki.multimedia.cx/index.php/CDXL) is a simple streaming format, consisting of linear concatenated chunks (packets), each with an uncompressed frame and associated audio data. In addition to the original 12-bit colors, AGAConv also supports 24-bit colors, as supported by Commodore Amiga AGA systems released in 1992+. It also ensures 32-bit padding of CDXL chunks, which can make a difference in video frame rate of up to 50%.

## Features
Overview of supported video/audio modes:

  * Resolutions: Lores, Hires, Superhires
  * Color modes:
     - AGA0-AGA8, 24 bit colors (requires ffmpeg)
     - OCS5-OCS3, 12 bit colors (requires ffmpeg) [V0.9.6+]
     - HAM8, 24 bit colors (requires ham_convert)
     - HAM6, EHB, 12 bit colors (requires ham_convert)
  * Audio
    -  8-bit mono/stereo sound (requires ffmpeg)
  * Encoding of custom CDXL videos
    - with 24-bit color palette and variable length frames (empty bitplanes are filtered, allowing for some simple "compression").
    - allows any frequency
  * Encoding of standard CDXL videos [V0.9.6+]
    - with 12-bit color palette (color palette uses 2 bytes for each 12-bit color value, with 4 bit per RGB value).
    - automatically adjusting the user-provided frequency (if necessary) such that all frames can have equal length and proper synchronization is maintained.

## How It Works

AGAConv uses ffmpeg to extract video frames from various video
formats, ffmpeg or ham_convert to convert extracted frames to PNG or
IFF files, and agaconv-encode to encode a list of PNG or IFF files and
an 8-bit PCM audio stream into a CDXL video. The CDXL video can be played
with [AGABlaster](https://mschordan.github.io/amiga/agablaster.html).

## Availability

AGAConv was tested on Ubuntu 18.04 and on Windows 10 with Microsoft's Ubuntu app/terminal (there is no native Windows support). The generated videos were tested on an Amiga 4000/60 with AGABlaster and Amiga OS 3.9 with original graphics and sound hardware.

* Binary distribution and video examples : [Website with downloads](https://mschordan.github.io/amiga/agaconv.html)

## Version

Version: 0.9.6

## Author

Author: Markus Schordan, 2019-2021.

## Acknowledgments
* Thanks to Jean Bollaërt for testing the pre-release version of AGAConv 0.9.4 on Windows 10 with Microsoft's Ubuntu app/terminal.
* Thanks to Sebastian Sieczko for adding command line options to his graphic converter [ham_convert](http://mrsebe.bplaced.net/blog/wordpress/?page_id=374) such that it could be integrated into the AGAConv 0.9.5 conversion pipeline.
     

## License

License: GPL v3+

## Where to Find More Info

* Website: [More infos and  downloads](https://mschordan.github.io/amiga/agaconv.html)
* Twitter: [Updates and announcements](https://twitter.com/AgaBlaster)
* Youtube: [My Youtube channel with some Amiga videos](https://www.youtube.com/c/markusschordan)

