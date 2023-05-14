/*
    AGAConv - CDXL video converter for Commodore-Amiga computers
    Copyright (C) 2019-2023 Markus Schordan

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CDXL_ENCODE_HPP
#define CDXL_ENCODE_HPP

#include "ByteSequence.hpp"
#include "CDXLFrame.hpp"
#include "FileSequenceConversion.hpp"
#include "Options.hpp"

class PngFile;

namespace AGAConv {

class CDXLEncode : public FileSequenceConversion {
 public:
  void preVisitFirstFrame() override;
  void visitILBMChunk(IffILBMChunk*) override;
  void postVisitLastILBMChunk(IffILBMChunk* ilbmChunk);
  void run(Options& options);

  // AUDIO
  ByteSequence* readAudioData();
  int getMonoAudioDataLength();

  // PNG
  void visitPngFile(std::string pngFileName) override;

  // IFF/ILBM
  void processILBMChunk(IffILBMChunk* ilbmChunk);
  void importOptions(CDXLFrame& frame);
  void importILBMChunk(CDXLFrame& frame, IffILBMChunk* ilbmChunk);
  void importAudio(CDXLFrame& frame);
  
 protected:
  std::fstream _outFile;
  std::fstream _sndFile;
  ULONG _currentFrameNr=1;
  bool _writeFile=false;
  int _audioMode=0; // modes: 1:mono, 2:stereo
  int _monoAudioDataLength=0;
  int _totalAudioDataLength=0;
  int _totalMonoAudioDataLengthSum=0;
  UWORD _frequency=0;
  bool _24BitColors=false;
  UBYTE _fps=0;
  UBYTE _resolutionModes=0;
  UBYTE _paddingModes=0;
  int _frameLenSum=0;
  ULONG _previousFrameSize=0;
private:
  void addColorsForTargetPlanes(int targetPlanes, IffCMAPChunk* cmapChunk);
  void addColorsForTargetPlanes(int targetPlanes, CDXLPalette& palette);
  void checkFrequencyForStdCdxl(Options& options);
};

} // namespace AGAConv

#endif
