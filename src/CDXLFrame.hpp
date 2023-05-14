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

#ifndef CDXL_FRAME_HPP
#define CDXL_FRAME_HPP

#include <fstream>
#include <vector>

#include "ByteSequence.hpp"
#include "CDXLBlock.hpp"
#include "CDXLHeader.hpp"
#include "CDXLPalette.hpp"
#include "IffChunk.hpp"
#include "IffILBMChunk.hpp"

namespace AGAConv {

// Audio:= standard uncompressed signed 8-bit PCM.
typedef ByteSequence CDXLAudio;

// Video:= Video is encoded differently based on the info byte in the header.
// - RGB is encoded as an index into the palette for the current frame.
// - HAM is encoded by using the palette for the current frame.
typedef ByteSequence CDXLVideo;

class CDXLFrame : public CDXLBlock {
 public:
  CDXLFrame();
  ~CDXLFrame();
  CDXLHeader header;
  CDXLPalette palette;
  CDXLVideo* video=nullptr;
  CDXLAudio* audio=nullptr;
  void readChunk() override;
  void writeChunk() override;
  void setOutFile(std::fstream* fstream) override;
  ULONG getLength();
  // Alignment defines the boundaries for which pad bytes should be computed
  // e.g. to align for LONG (=4 bytes): computePadBytes(4,13) = 3,
  // computePadBytes(4,4) = 0, computePadBytes(4,2) = 2
  static ULONG computePaddingBytes(ULONG alignment, ULONG size);
  std::string toString();
  // requires the header to be properly initialized
  void importVideo(IffILBMChunk* body);
  ByteSequence* readByteSequence(std::fstream* inFile, ULONG length);
  void setPaddingSize(ULONG);
  ULONG getPaddingSize();
  ULONG getColorPaddingBytes();
  ULONG getVideoPaddingBytes();
  ULONG getAudioPaddingBytes();
 private:
};

} // namespace AGAConv

#endif
