/*
    AGAConv - CDXL video converter for Commodore-Amiga computers
    Copyright (C) 2019, 2020 Markus Schordan

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

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <cstddef>
#include <sstream>
#include "Options.hpp"
#include "PngLoader.hpp"
#include <cassert>
#include "Util.hpp"
#include <png.h>

using namespace std;

PngLoader::PngLoader()
{
}

PngLoader::~PngLoader()
{
  if(_pngImageData)
    freePngImageData();
}

void PngLoader::printFileInfo() {
  string sep=" ";
  cout<<"Width: "<<_width;
  cout<<sep;
  cout<<"Height: "<<_height;
  cout<<sep;
  cout<<"color type: "<<+_colorType;
  cout<<sep;
  cout<<"Bit depth: "<<+_bitDepth;
  cout<<endl;
}
void PngLoader::readFile(string inFileName) {
  readPngFile(const_cast<char*>(inFileName.c_str()));
}

void PngLoader::readPngFile(char *filename) {
  FILE *fp = fopen(filename, "rb");

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png) abort();

  png_infop info = png_create_info_struct(png);
  if(!info) abort();

  if(setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);
  png_read_info(png, info);

  _width=png_get_image_width(png, info);
  _height=png_get_image_height(png, info);
  _colorType=png_get_color_type(png, info);
  _bitDepth=png_get_bit_depth(png, info);

  // Read any _colorType into 8bit depth, RGBA format.
  // See http://www.libpng.org/pub/png/libpng-manual.txt

  if(_bitDepth==16)
    png_set_strip_16(png);

  // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
  if(_colorType == PNG_COLOR_TYPE_GRAY && _bitDepth < 8)
    png_set_expand_gray_1_2_4_to_8(png);

  //if(png_get_valid(png, info, PNG_INFO_tRNS))
  //  png_set_tRNS_to_alpha(png);

  // These _colorType don't have an alpha channel then fill it with 0xff.
#if 1
  if(_colorType == PNG_COLOR_TYPE_RGB ||
     _colorType == PNG_COLOR_TYPE_GRAY ||
     _colorType == PNG_COLOR_TYPE_PALETTE)
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

  if(_colorType == PNG_COLOR_TYPE_GRAY ||
     _colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png);

  png_read_update_info(png, info);
#endif

  assert(_pngImageData==0);
  _pngImageData = (png_bytep*)malloc(sizeof(png_bytep) * _height);
  for(int y = 0; y < _height; y++) {
    _pngImageData[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
  }

  png_read_image(png, _pngImageData);

  // FFMPEG: Note: the _palette extracted by ffmpeg always contains 256
  // colors; the unused _palette entries are all black. Transparency
  // color must be turned off (it is on by default)
  // https://ffmpeg.org/ffmpeg-filters.html#toc-palettegen-1
  // doxygen doc of libpng: http://www-personal.umich.edu/~bazald/l/api/structpng__color__struct.html
  if(_colorType==PNG_COLOR_TYPE_PALETTE) {
    //cout<<"Found PNG_COLOR_TYPE_PALETTE: keep _palette."<<endl;
    //png_set_palette_to_rgb(png);
    png_colorp _palette=0; // _palette memory is allocated by png_get_PLTE
    png_uint_32 ret=png_get_PLTE(png,info,&_palette,&_numPaletteEntries);
    assert(ret==PNG_INFO_PLTE);
    // copy colors because _palette is destroyed by png_destroy_read_struct
    assert(rgbPalette.size()==0);
    for(int i=0;i<_numPaletteEntries;i++) {
      rgbPalette.push_back(RGBColor(_palette[i].red,_palette[i].green,_palette[i].blue));
    }
  }

  fclose(fp);
  png_destroy_read_struct(&png, &info, NULL);
}

int PngLoader::getByteWidth() {
  return (_width + 7) / 8;
}

void PngLoader::allocateIntermediateBitplanes(char**& bitplanes, int numBitPlanes) {
  bitplanes = (char**)malloc(sizeof(void*)*numBitPlanes);
  for (int i = 0; i < numBitPlanes; i++) {
    bitplanes[i] = (char*)calloc(getByteWidth()*_height, 1);
  }
}

void PngLoader::freeIntermediateBitplanes(char** bitplanes, int num) {
  for (int i = 0; i < num; i++) {
    free(bitplanes[i]);
  }
  free(bitplanes);
}

void PngLoader::freePngImageData() {
  for(int y = 0; y < _height; y++) {
    free(_pngImageData[y]);
  }
  free(_pngImageData);
  _pngImageData=0;
}

IffCMAPChunk* PngLoader::createIffCMAPChunk() {
  IffCMAPChunk* iffCMAPChunk=new IffCMAPChunk();
  for(auto rgbColor : rgbPalette) {
    iffCMAPChunk->addColor(rgbColor);
  }
  return iffCMAPChunk;
}

IffCAMGChunk* PngLoader::createIffCAMGChunk(IffBMHDChunk* bmhdChunk) {
  IffCAMGChunk* camgChunk=new IffCAMGChunk();

  switch(options.resMode) {
  case Options::GFX_UNSPECIFIED: {
    assert(bmhdChunk);
    UWORD _width=bmhdChunk->getWidth();
    if(_width>=0 && _width<=320) {
      camgChunk->setLores();
    } else if(_width>=321 && _width<=640) {
      camgChunk->setHires();
    } else if(_width>=641 && _width<=1280) {
      camgChunk->setSuperHires();
    }
    break;
  }
  case Options::GFX_LORES:
    camgChunk->setLores();
    break;
  case Options::GFX_HIRES:
    camgChunk->setHires();
    break;
  case Options::GFX_SUPERHIRES:
    camgChunk->setSuperHires();
    break;
  case Options::GFX_ULTRAHIRES:
    camgChunk->setUltraHires(); // not supported, issues error message.
    break;
  default:
    cerr<<"Error: Unsupported graphics mode provided in options."<<endl;
    exit(1);
  }
  return camgChunk;
}

// uses videoFrame
IffBMHDChunk* PngLoader::createIffBMHDChunk() {
  IffBMHDChunk* bmhdChunk=new IffBMHDChunk(this->_width,this->_height,this->getOptimizedBitDepth());
  return bmhdChunk;
}

UBYTE PngLoader::getOptimizedBitDepth() {
  int numConvertedBitPlanes=Util::ULONGLog2(rgbPalette.size());
  if(Util::ULONGPow(2,numConvertedBitPlanes)!=rgbPalette.size())
    numConvertedBitPlanes++;
  assert(numConvertedBitPlanes<=255);
  return (UBYTE)numConvertedBitPlanes;
}

// the PNG palette extracted by ffmpeg may contain more color entries than colors that are actually used in the picture.
// This routine determines the used colors, and remaps the color indexes to the new color scheme.
// #newNumColors <= numColors
void PngLoader::optimizePngPalette() {
    const int colorOffset=1; // offset (currently fixed to 1 to reserve background color)
  // max colors is 256
  int const maxCol=256;
  int colorNrCount[maxCol]={}; // only those color indexes are mapped that have more than 0 uses
  UBYTE colorNrMapping[maxCol]={}; // defines which color-index should be mapped to which new color_index 
  for(int i=0;i<maxCol;i++) colorNrMapping[i]=-1;
  for (int y = 0; y < _height; y++) {
    for (int x = 0; x < _width; x++) {
      UBYTE palette_index = *(UBYTE*)((_pngImageData[y])+x);
      assert(palette_index>=0 && palette_index<maxCol);
      colorNrCount[palette_index]++;
    }
  }
  int numUsedCol=0;
  int checkSum=0;
  //  cout<<"Color counts:";
  for(int i=0;i<maxCol;i++) {
    //cout<<+colorNrCount[i]<<" ";
    checkSum+=colorNrCount[i];
    if(colorNrCount[i]>0) {
      numUsedCol++;
    }
  }
  assert(checkSum==_width*_height);
  //cout<<"\nNumber of used colors: "<<numUsedCol<<endl;
  int newColLastCol=colorOffset;
  for(int i=0;i<maxCol;i++) {
    if(colorNrCount[i]>0) {
      colorNrMapping[i]=newColLastCol;
      newColLastCol++;
    }
  }
  // newColLastCol is the index of the next required color nr. This equals the total number of colors.
  if(newColLastCol>maxCol) {
    cerr<<"\n\n";
    cerr<<"Error: input image palette has too many colors to reserve offset color(s)."<<endl;
    cerr<<"       input image has "<<numUsedCol<<" colors. With "<<colorOffset<<" offset color(s) this requires "<<newColLastCol<<" colors. Maximum is 256 colors."<<endl;
    cerr<<"Note : ffmpeg sometimes extracts one more color than requested."<<endl;
    exit(1);
  }
#if 0
  // print mapping
  cout<<"ColorNrMapping:";
  for(int i=0;i<maxCol;i++) {
    if(colorNrCount[i]==0)
      cout<<i<<"->X"<<" ";
    else
      cout<<i<<"->"<<+colorNrMapping[i]<<" ";
  }
  cout<<endl;
#endif
  
  // perform remapping
  // remap all body data: iterate over all pixels and remap color index
  for (int y = 0; y < _height; y++) {
    for (int x = 0; x < _width; x++) {
      UBYTE palette_index = *((_pngImageData[y])+x);
      *((_pngImageData[y])+x)=colorNrMapping[palette_index];
    }
  }
  // rewrite colormap
  // create copy of colormap (required because mapping can go in both directions when reserving colors)
  std::vector<RGBColor> rgbPaletteCopy=rgbPalette;
  assert(rgbPaletteCopy.size()==rgbPalette.size());
  for(size_t i=0;i<rgbPalette.size();i++) {
    if(colorNrCount[i]>0)
      rgbPalette[colorNrMapping[i]]=rgbPaletteCopy[i];
  }
  // set all offset colors to black
  for(int i=0;i<colorOffset;i++) {
    rgbPalette[i]=RGBColor(0,0,0);
  }
  rgbPalette.resize(colorOffset+numUsedCol);
}

IffBODYChunk* PngLoader::createIffBODYChunk() {
  if(_width % 8 !=0) {
    cerr<<"Error: video _width is not a multiple of 8. Not supported."<<endl;
    exit(1);
  }
  IffBODYChunk* bodyChunk=new IffBODYChunk();
  int numBitPlanes=_bitDepth;
  char** bitplanes;
  allocateIntermediateBitplanes(bitplanes,numBitPlanes);
  // create bitplanes from PNG chunky data
  for (int y = 0, writeIndex = 0; y < _height; y++) {
    for (int byte = 0;byte < getByteWidth(); byte++) {
      for (int bit = 0; bit < 8; bit++) {	
        int x = byte * 8 + 7 - bit;
        // offset in paletted chunky image 
        int palette_index = *((_pngImageData[y])+x);
        for (int plane_index = 0; plane_index < numBitPlanes; plane_index++) {
          char* plane = bitplanes[plane_index];
          plane[writeIndex] |= ((palette_index >> plane_index) & 1) << bit;
        }
      }
      writeIndex++;
    }
  }

  // add bitplanes to bodyChunk (plane 0 .. n)
  int numConvertedBitPlanes=getOptimizedBitDepth();
  for (int y = 0; y < _height; y++) {
    for (int plane_index = 0; plane_index < numConvertedBitPlanes; plane_index++) {
      char* plane = bitplanes[plane_index];
      for(int i=0;i<getByteWidth();i++) {
        bodyChunk->add(plane[y*getByteWidth()+i]);
      }
      // align to 16 bit in each bitplane line (ilbm padding byte)
      if((getByteWidth() % 2) == 1) {
        bodyChunk->add(0);
      }
    }
  }
  freeIntermediateBitplanes(bitplanes, numBitPlanes);
  return bodyChunk;
}

IffILBMChunk* PngLoader::createILBMChunk() {
  optimizePngPalette();
  IffCMAPChunk* cmapChunk=createIffCMAPChunk();
  IffBMHDChunk* bmhdChunk=createIffBMHDChunk();
  IffCAMGChunk* camgChunk=createIffCAMGChunk(bmhdChunk);
  IffBODYChunk* bodyChunk=createIffBODYChunk();

  // conistency check
  if(!(((ULONG)((bmhdChunk->getWidth()/8)*bmhdChunk->getHeight())*bmhdChunk->getNumPlanes())==(ULONG)bodyChunk->getDataSize())) {
    cerr<<"Internal consistency error: "<<endl;
    ULONG w=bmhdChunk->getWidth()/8;
    ULONG h=bmhdChunk->getHeight();
    ULONG p=bmhdChunk->getNumPlanes();
    cerr<<"w:"<<w<<" h:"<<h<<" p:"<<p<<endl;
    cerr<<"planesize :"<<w*h<<endl;
    cerr<<"total size:"<<w*h*p<<endl;
    cerr<<"data size :"<<((ULONG)bodyChunk->getDataSize())<<endl;
    exit(1);
  }

  //cout<<"DEBUG: creating ilbmChunk."<<endl;
  // create ilbmChunk
  IffILBMChunk* ilbmChunk=new IffILBMChunk();
  ilbmChunk->insertFirst(bmhdChunk);
  ilbmChunk->insertLast(camgChunk);
  ilbmChunk->insertLast(cmapChunk);
  ilbmChunk->insertLast(bodyChunk);
  return ilbmChunk;
}
