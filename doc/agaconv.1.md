% AGACONV(1) agaconv | Version 1.0
% Markus Schordan
% May 2023

# NAME
agaconv - converts videos into CDXL format for AGA Commodore-Amiga computers

# SYNOPSIS
agaconv INPUTVIDEO CDXLVIDEO [*OPTION*]

# DESCRIPTION

**AGAConv** is a CDXL video converter. The input video can be in any format that can
be processed by ffmpeg.  **AGAConv** uses ffmpeg to extract video and audio data and
encodes the extracted frames into the CDXL video format. **AGAConv** can generate
standard and custom CDXL files. In a standard CDXL file each frame has the exact
same size, in a custom CDXL file the size is optimized, and hence custom CDXL
video files are usually smaller than standard CDXL files.

The CDXL format was created by Commodore primarily for the Commodore CDTV,
released 1991, to permit playback of video from CD-ROM in the early 1990s. The
CDXL format is a simple streaming format, consisting of linear concatenated
chunks (packets), each with an uncompressed frame and associated audio data. In
addition to the original 12-bit colors, **AGAConv** also supports 24-bit colors, as
supported by Commodore Amiga AGA systems released in 1992+. **AGAConv** also ensures
32-bit padding of CDXL chunks, which can make a difference in video frame rate
of up to 50%. For standard CDXL files this is achieved by adjusting the
frequency. See option \--frequency for more details.

Standard 12-bit colors CDXL video files can be played with any CDXL player. AGABlaster
is a CDXL player that also supports 24-bit colors CDXL videos and utilizes the AGA
64-bit graphics access for best frame rate on Commodore-Amiga AGA systems.

The tool ffmpeg is automatically installed with the agaconv PPA
installer. **AGAConv** can be used for AGA8 to AGA2 and OCS5 to OCS2 conversions
without installing any additional tools. From those screen modes, AGA8 gives the best quality on AGA systems.

For converting videos into HAM6, HAM8, or EHB CDXL videos, the additional tool
ham_convert must be installed. See option \--hc-path for how to set the path to
ham_convert in the **AGAConv** config file and Section 'SEE ALSO' for web links.
Once configured, **AGAConv** integrates ham_convert into the conversion pipeline.

**AGAConv** also supports configuration files. A configuration file can be used
to set all options listed in section OPTIONS for each video conversion
parameter. A config file can be edited with a text editor.  Entries in a config
file have the same name as its corresponding command line option, with the only
difference that a dash in the command line option is written is an underscore
in the config file. For example, \--color-mode=aga8 is listed as color_mode =
aga8 in a config file. Only existing entries in a config
file set a value, for non-existing entries the (original) default value is
used. Command line options always override config file options. Entries in a
config file without a value are handled the same way as non-existing entries.

**AGAConv** can store a default config file in a default location. The entries
in the default config file can be changed through the command line with the
\--change-config option or the file can be edited with a text editor.  The
default config file is automatically generated when the option \--change-config
is used for the first time.

**AGAConv** also supports user config files. When a user config file is loaded
it is used instead of the default configuration. See the various config file
options and Section Examples for more details.

All conversion options have default values. A video can be converted without any
additional options.

# OPTIONS
agaconv takes the following options:

## Basic Options
\--color-mode STRING
: Color mode where STRING = aga8|..|aga2|ham8|ham6|ocs5|..|ocs2|ehb. The letters
specify the screen mode, the digit determines the number of bitplanes. The
higher the digit, the better is the quality and the more graphics data is stored
in the CDXL video file. 'agaX' selects 24-bit colors, OCS (Original Chip Set) selects
12-bit colors. The 'aga' and 'ocs' modes work out-of-the-box after installation
of agaconv. The three modes ham6, ham8 and ehb require the additional tool
ham_convert to be installed (also see option \--hc-path).
        
\--fps NUMBER
: Frames per second. The frames per second options defines how much graphics
data is stored per second in the CDXL video file. The higher the FPS, the more data is
stored, and the better is the quality due to a higher number of frames per
second. However, if the amount of data is beyond the I/O bandwidth of the
respective storage device from which the CDXL video file is loaded, the frame rate
will be limited by the bandwidth. The CDXL player AGABlaster will skip the
additional frames on disk and not load them into memory, effectively adapting
the frame rate (and quality) to the I/O bandwidth.
        
\--width NUMBER
: Width of video in pixels. The Amiga AGA graphics hardware
supports three different resolutions, lowers, hires, superhires with 1 to 8
bitplanes.

\--frequency NUMBER
: Set audio frequency. The Audio frequency determines how many bytes per second
are used for the audio data of the video. The higher the frequency, the better
is the audio quality.  Maximum value with the original Amiga audio hardware is
28836.
        
\--audio-mode mono|stereo
: Audio mode. The two modes mono and stereo can be selected. Default mode is stereo.
In stereo mode twice the amount of audio data is used in comparison to mono.

\--height auto|NUMBER
: With 'auto' the height of the video is derived from the input video's aspect ratio. Otherwise
the absolute value NUMBER is used.

\--adjust-aspect FLOAT|hdstretched
: This option allows to adjust the aspect ratio of the video. The default value
of 1.0 keeps the ratio of width to height unmodified. A value greater than 1.0
stretches the video in height.

    With 'hdstretched' the predefined value of 1.35 is used. This value should work
    on most modern HD screens when using 'fill' or 'stretched' mode to display
    the Amiga's 4:3 screen on an HD screen. For other monitor configurations
    a floating point value FLOAT can be used.

\--change-config
: Change default configuration settings. Any other option can
be added on the command line and the respective value is then set as default
value in the default configuration. For example, it can be used with other
options, e.g. --hc-path=PATH, to set the path for the ham_convert tool. It can
also be used to set the default width to 640.

    Example: agaconv \--change-config \--width=640

    This sets the default width
    of a converted video to a width of 640. Option \--help shows with each
    option also its default values and can therefore be used to check the effect
    of \--change-config to the default values.

\--load-config FILE
: Load a user configuration file with name FILE instead of the default
configuration file.

\--save-config FILE
: Save the current configuration as FILE. This basically saves the current
default configuration in a separate file. Additional options can be specified on
the command line.
        
--std-cdxl
: Select standard CDXL format with 12-bit colors. In a standard CDXL file each
frame has to have the exact same size. **AGAConv** adjusts the provided frequency if
necessary, to generate a video where each frame has the exact same size. In
addition, it also ensures that all chunks in the CDXL file are 32-bit aligned,
which can improve I/O speed of up to 50% with certain I/O controllers. Therefore
the length of each audio data and graphics data chunk must be divisible by 4, to
ensure that the next chunk starts at a 32-bit aligned offset in the CDXL file.

\--std-cdxl24
: Same as --std-cdxl but with AGA 24-bit colors. The 24-bit colors are stored as
3-byte RGB values (same format as in ANIM and IFF palette chunks). The only
difference to a 12-bit standard CDXL file is that the the color values have 3
bytes instead of 2 bytes.

    Note, if you do not use this option, you get by default a custom CDXL
    Video with 24-bit colors which has the exact same quality, but is usually
    smaller.

\--hc-path PATH
: Absolute file path to the directory of the conversion tool ham_convert. This
setting is only necessary for the screen modes HAM6, HAM8, EHB. All other modes
do not require ham_convert and have a 10-15 times faster conversion
time. However, for enjoying the HAM modes ham_convert is the only option. For
this purpose the path to ham_convert needs to be added to the configuration (or
provided on the command line).

    For example, if ham_convert_X_Y_Z.zip was unpacked in the directory
    /home/john then the default path to ham_convert can be set with:

    agaconv --change-config --hc-path=/home/john/ham_convert

    This stores the path in the default config file and agaconv will use this
    path now with every invocation of agaconv to find ham_convert's Java jar
    file inside the directory /home/john/ham_convert. When this path is set,
    agaconv checks if ham_convert/ham_convert_X.Y.Z.jar exists. If it does not
    exist, it immediately issues an error message. Hence, when above command
    succeeds, one can be sure that the provided path is indeed correct. One can
    also specify the full path, including the name of the ham_convert's jar file
    itself, e.g. --hc-path="/home/john/ham_convert/ham_convert_1.9.0.jar"

    However, to simplify updates of ham_convert, agaconv does support to search
    for the Java jar file inside the ham_convert directory, and therefore it's
    best to only specify the path with the directory name 'ham_convert' alone.

    When updating ham_convert, one should first delete the ham_convert directory
    and then unzip the ham_convert_X_Y_Z.zip file. If one accidentally unzips a
    new version into the directory of an old version, **AGAConv** will detect that
    there is more than one jar file, and will issue an error message to make
    sure one does not accidentally use an old version of ham_convert.

\--cdxl-info FILE
: Show all info of frame 1 of given CDXL video FILE. This option is useful for
checking the values of converted CDXL videos.  Example: agaconv --cdxl-info
video.cdxl

\--verbose NUMBER
: Select how verbose the output is during conversion. The value 0 means that no
information is printed during conversion, except error messages. Verbose level 1
prints about 5-10 lines for one converted video.  Verbose level 2 and 3 produce
additional output for each converted frame.

\--version
: print program version and copyright.
        
\--help
: show basic command line options with default values. If a config file is used
then the default values are read from the config file and shown with this command.

\--help-advanced
: show advanced command line options with default values. If a config file is
used then the advanced default values are read from the config file and shown
with this command.

## ADVANCED OPTIONS
   
\--black-and-white
: Convert video to black-and-white colors. This mode uses ffmpeg's 'gray' option.

\--black-background
: Reserve black background color. This is only relevant on OCS systems, on which
one may see a flashing background if the background color register $0180 is not
black and changing color throughout the video. On AGA systems CDXL players such
as AGABlaster use the special AGA mode with a blank background, and therefore
the background color can be used as well without producing a flashing
background.

\--dither STRING
: Sets ffmpeg dithering mode when rescaling video, where
STRING=floyd_steinberg|bayer:bayer_scale=X|sierra2. This option allows to use
any dithering mode that ffmpeg supports. It appears that floyd_steinberg works
best for most videos. However, since this string is simply passed through to
ffmpeg one can experiment also with other ffmpeg dithering modes.

\--screen-mode STRING
: screen (resolution) mode, where STRING = auto|lores|hires|superhires. The
default setting is 'auto', which means that the screen resolution is derived
from the provided width parameter. For example if the provided width it less
than or equal to 320 lores resolution is selected. If a value higher than 320
and less or equal 640 is selected then hires modes is selected. Similar with
superhires.

\--install-config
: Install the default config file in the respective OS specific location. On
Linux this location is ~/.config/agaconv/default.config. The option
\--change-config automatically installs a config file if it was not installed
yet.

\--uninstall-config
: Uninstall the default config file. With this option **AGAConv** removes the
default config file as well as the directory agaconv from the ./config
directory. Note this operation is only successful if no other file is in the
./config/agaconv directory, e.g. after editing the default config file there
might be some backup files in this directory. The directory is only removed if
it is empty after removing the default config file.

\--reset-config
: Reset the default config file to original values. This reestablishes the
original default values in the default config file, but keeps the hc_path entry,
if it was set. All other values are reset. Keeping the hc_path is a convenience
feature, because this is the only value that must be set manually after
installation if one wishes to use ham_convert for HAM conversions. Therefore
this option also reads the existing default config file before regenerating a
new default config file.

\--tmp-dir-prefix DIRNAME
: Sets the prefix of the temporary directory name as DIRNAME. The default name
is "tmp-agaconv" and it is recommended to keep this setting unmodified.

    During conversion **AGAConv** generates by default a local directory with this
    name as prefix from where the command 'agaconv' was invoked. It extends this
    prefix with the name of the video and some more parameters and the process
    ID, to ensure the name of the directory is unique. This allows to run
    multiple instances of **AGAConv** in parallel without writing into the same
    directory.

    DIRNAME can also be set to an absolute path, such as
    \-\-tmp-dir-prefix=/home/user/tmp-agaconv \--change-config

    The temporary directory is removed after every conversion.  It is only
    removed if it is indeed empty after all generated files have been
    removed. **AGAConv** does not remove any other files than those that ffmpeg or
    ham_convert can generate for a given video file and some extra files that
    **AGAConv** itself generates during conversion. If an error occurs or the
    process is killed during conversion, then the temporary directory is not
    removed.

    With option \--verbose=2 **AGAConv** prints additional information about the removed
    files at the end of a conversion.

\--keep-tmp-dir
: Keep the temporary directory. The temporary directory is removed by default
after each conversion. This option is only relevant if one wants to inspect the
generated PNG or IFF files that are generated for each frame.

\--hc-ham-quality NUMBER
: This is a ham_convert HAM quality option for setting the quality level in the
HAM generation. Default is 1 and the range for HAM8 is 0..3.  Values greater or
equal 2 are significantly slower and require a lot of patience for longer
videos. Therefore the default is 1.

\--hc-dither STRING
: ham_convert dither mode where STRING=auto|none|fs|bayer8x8. The default value
is 'fs' (Floyd-Steinberg algorithm, as with ffmpeg). If 'none' is selected then
no dither is applied. With 'auto' the default ham_convert mode is used (which
currently is 'none').

\--hc-propagation NUMBER
: ham_convert error propagation factor, requires hc_dither = fs, default:
'auto', value range is 0..100. See ham_convert documentation for more details.

\--hc-diversity NUMBER
: ham_convert diversity X=0-6 for ehb, X=0-9 for other modes, not supported in
ham8. Default is 'auto', value range is 0..9. See ham_convert documentation for
more details.

\--hc-quant STRING
: ham_convert quantization algorithm where STRING=wu|neuquant. Default setting
is 'auto'. See ham_convert documentation for more details.

\--iff-info FILE
: Show IFF file info for a given IFF FILE. This can be used to inspect IFF files
generated with ham_convert. To keep the IFF files at the end of a conversion,
the option \--keep-tmp-dir can be used.

\--in-file FILE
: Set the input file name. This option is available for tool generated
config files.

\--out-file FILE
: Set the output file name. This option is available for tool generated
config files.

# ENVIRONMENT

AGAConv uses the following environment variables

HOME
: This environment variable is used to determine the user's home directory and store a default config file as $HOME/.config/agaconv/default.config

    The default config file is generated when the options \--install-config or \--change-config are used.
    It can be removed with \--uninstall-config

# EXIT STATUS

Exit status 0
: If no error occurs.

Exit status 1
: If any error occurs due to wrong command line arguments, errors in the default
config file, or during conversion, an error message is printed and the program exits with return code 1. If any invoked external tool
fails, AGAConv also returns an exit status of 1 and reports the invoked tool's status return code in the error message.

    Every error is reported with an identifying error number and an error message on stderr.

# EXAMPLES

agaconv video.mp4 video.cdxl

: Converts the mpeg video into a CDXL video. By default the CDXL video is encoded with
a width of 320, lores resolution, 24-bit colors, AGA8 with 8 bitplanes (256
colors per frame), 24 FPS, and stereo audio with 28000 Hz.

agaconv video.mp4 video.cdxl \--fps=25
: Encodes the CDXL video with 25 frames per second and default values, using
24-bit colors, AGA8 with 8 bitplanes (256 colors per frame), stereo audio with
28000 Hz,

agaconv video.mp4 video.cdxl \--fps=25 \--width=640
: Encodes the CDXL video with 25 frames per second and a width of 640
pixels. **AGAConv** derives from the width that the video must be encoded for a
Hires resolution and adjusts the aspect ratio accordingly. All other parameters
have default values, using 24-bit colors, AGA8 with 8 bitplanes (256 colors per
frame), and stereo audio with 28000 Hz.

agaconv video.mp4 video.cdxl \--monitor-mode=stretched
: Encodes the CDXL video such that it can be played on an HD screen where the
display is stretched (or filled) to full screen. If the Amiga produces a 4:3
output, this option can be used to stretch the video (in y-direction) such that
when displayed its aspect ratio is correct.

agaconv \--cdxl-info video.cdxl
: Shows all entries of the first frame of the CDXL file video.cdxl

agaconv video.mp4 video.cdxl \--std-cdxl
: Encodes the CDXL video as standard 12-bit colors CDXL video in AGA8 (256
colors per frame). For standard CDXL videos **AGAConv** adjusts the frequency
such that all frames have the same size and all data chunks are 32-bit aligned.

agaconv video.mp4 video.cdxl \--std-cdxl \--color-mode=aga7
: Encodes the CDXL video with 12-bit colors and 7 bitplanes (128 colors per
frame). All other values are default **AGAConv** values.

agaconv video.mp4 video.cdxl \--color-mode=ham8
: Encodes the CDXL video in HAM8 format. HAM8 uses 24-bit colors. This requires
the tool ham_convert to be installed and the path to ham_convert must be set in
the **AGAConv** config file.

agaconv video.mp4 video.cdxl \--color-mode=ocs5 \--fps=20 \--frequency=18000 \--audio-mode=mono
: Encodes the CDXL video with 5 bitplanes (32 colors) with 12-bit colors for an
OCS Amiga system, with 20 frames per second, and a frequency of 18000 Hz in mono
audio mode. **AGAConv** uses dithering by default, which gives best
results. Option \--dither allows to experiment with other dithering modes that
are supported by ffmpeg.

agaconv \--width=640 \--save-config my-hires-video.config
: This sets the width to 640 in the saved configuration file
my-hires-video.config. When converting a video this can be used with agaconv
\--load-config my-hires-video.config inputvideo.mp4 outputvideo.cdxl

agaconv \--width=640 \--change-config
: Change the default configuration to use 640 as width. On a classic Amiga this
will be displayed as a hires video with width 640 by **AGABlaster**. The option
\--help will also display as default width value 640.

agaconv \--help
: This option shows all basic help options with its respective default
values. The default values are defined by the configuration. If the default
config has been changed by using the option \--change-config then the option
\--help will list those (changed) default values. Hence, \--help can also be
used to check the default values.

agaconv \--load-config my-user.config \--change-config
: Load the user configuration my-user.config and set it as default configuration.

agaconv \--load-config my-user1.config \--save-config my-user2.config
: Load my-user1.config and save it as my-user2.config. This is essentially the
same as copying the file, but checks all values to be in correct ranges. It also
checks that the hc_path (if set) refers to an existing version of
ham_convert. The \--load|save-config options can be combined with the
change-config option for the default configuration.

# COPYRIGHT

Copyright (C) 2019-2023 Markus Schordan. License GPLv3+: GNU GPL version 3 or
later <http://gnu.org/licenses/gpl.html>. This is free software: you are free to
change and redistribute it. There is NO WARRANTY, to the extent permitted by
law.

# SEE ALSO

* **AGAConv** webpage: <https://cutt.ly/AGAConv>
* **AGABlaster** webpage: <https://cutt.ly/AGABlaster>

# BUGS

No known bugs.