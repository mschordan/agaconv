/*
    AGAConv - CDXL video converter for Commodore-Amiga computers
    Copyright (C) 2019-2024 Markus Schordan

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

#ifndef PNG_FILE_READER_HPP
#define PNG_FILE_READER_HPP

#include <map>
#include <png.h>
#include <string>
#include <vector>

#include "AGAConvException.hpp"
#include "FrameLoader.hpp"
#include "IffILBMChunk.hpp"
#include "RGBColor.hpp"
#include "Stage.hpp"

namespace AGAConv {

/* Read a sequence of png files and allow to operate on each
   file.
 */
class PngLoader : public FrameLoader {

 public:
  PngLoader();
  virtual ~PngLoader();
  //! Called before first frame is read. Can be used for initialization.
  virtual void readFile(std::string FileName);

  void printFileInfo();

  //! Creates all chunks and a valid ILBM chunk from the read PNG.
  IffILBMChunk* createILBMChunk(Options& options);

  /*! A png cannot contain HAM or HalfBrite info. LACE is currently not
    supported.  Therefore the only information that needs to be set
    is Hires or SuperHires.  This info is either guessed from the
    size (>320, >640) or enforced according to a command line
    provided resolution. 
  */
  IffCMAPChunk* createIffCMAPChunk();
  IffCAMGChunk* createIffCAMGChunk(IffBMHDChunk* bmhdChunk, Options& options);
  IffBMHDChunk* createIffBMHDChunk();
  IffBODYChunk* createIffBODYChunk();

  void optimizePngPalette(Options& options);
  UBYTE getOptimizedBitDepth();
  std::string colorTypeToString();

protected:
  //! Png is stored as array of pointers, where each pointer refers to one scanline.
  //! If paletted, each byte contains an index value, which refers to the respective color in the palette.
  void readPngFile(char *filename);
  void freePngImageData();
  int _width=0, _height=0;
  png_byte _colorType;
  png_byte _bitDepth=0;
  png_bytep* _pngImageData = 0;
  png_colorp _palette=0;
  int _numPaletteEntries=0;
  //int _optimizedNumPaletteEntries=-1;
  //int _optimizedBitDepth=-1;
  std::vector<RGBColor> rgbPalette;

 private:
  int getByteWidth();
  void allocateIntermediateBitplanes(char**& bitplanes, int num);
  void freeIntermediateBitplanes(char** bitplanes, int num);
  
};

} // namespace AGAConv

#endif
