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

#include "CDXLHeader.hpp"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "AGAConvException.hpp"
#include "CDXLFrame.hpp"
#include "Options.hpp"
#include "Util.hpp"

using namespace std;

namespace AGAConv {

CDXLHeader::CDXLHeader()
{
  info.encoding=RGB;
  info.stereo=0;
  info.planeArrangement=PA_UNDEFINED;
  modes.resolutionModes=0;
  modes.colorDepthFlag=0;
  modes.killEHBFlag=0;
}

CDXLHeader::CDXLHeader(IffBMHDChunk* bmhd, IffCMAPChunk* cmap, IffCAMGChunk* camg):CDXLHeader() {
  initialize(bmhd,cmap,camg);
}

void CDXLHeader::initialize(IffBMHDChunk* bmhd, IffCMAPChunk* cmap, IffCAMGChunk* camg) {
  videoWidth=bmhd->getWidth();
  videoHeight=bmhd->getHeight();
  numberOfBitplanes=bmhd->getNumPlanes();
  paletteSize=cmap->numberOfColors()*getColorBytes();
  if(camg->isHam()) {
    info.encoding=HAM;
    info.planeArrangement=BIT_PLANAR;
  } else {
    info.encoding=RGB;
    info.planeArrangement=BIT_PLANAR;
  }
  modes.resolutionModes=Options::GFX_UNSPECIFIED; // Default
  // Lores is determined based on hires/superhires, not explicitly
  if(camg->isLores())
    modes.resolutionModes=Options::GFX_LORES;
  else if(camg->isHires())
    modes.resolutionModes=Options::GFX_HIRES;
  else if(camg->isSuperHires())
    modes.resolutionModes=Options::GFX_SUPERHIRES;
  if(camg->isLace()) {
    modes.resolutionModes|=(1<<3); // BIT 3: LACE
  }
}

string CDXLHeader::fileTypeToString() {
  switch(fileType) {
  case CUSTOM: return "custom";break;
  case STANDARD: return "standard";break;
  case SPECIAL: return "special";break;
  default: return "unknown file type";
  }
}

string CDXLInfo::encodingToString() {
  switch(encoding) {
  case RGB: return "RGB";break;
  case HAM: return "HAM";break;
  case YUV: return "YUV";break;
  case AVM_DCTV: return "DCTV";break;
  default: return "unknown video encoding";
  }
}

string CDXLGfxModes::toString() {
  stringstream ss;
  ss<<killEHBFlagToString()<<":"<<colorDepthFlagToString()<<":"<<+resolutionModes<<endl;
  return ss.str();
}

string CDXLInfo::stereoToString() {
  if(stereo==1)
    return "stereo";
  else
    return "mono";
}  

string CDXLGfxModes::colorDepthFlagToString() {
  if(colorDepthFlag==1)
    return "24bit";
  else
    return "12bit";
}  

string CDXLGfxModes::killEHBFlagToString() {
  if(killEHBFlag==1)
    return "1";
  else
    return "0";
}  

string CDXLGfxModes::resolutionModesToString() {
  switch(static_cast<Options::GFX_RESOLUTION>(resolutionModes)) {
  case Options::GFX_UNSPECIFIED: return "unspecified";
  case Options::GFX_LORES: return "lores";
  case Options::GFX_HIRES: return "hires";
  case Options::GFX_SUPERHIRES: return "superhires";
  case Options::GFX_ULTRAHIRES: return "ultrahires";
  case Options::GFX_AUTO: return "auto";
    // Intentionally empty to trigger compiler warning
  }
  throw AGAConvException(308, "Internal error: wrong resolution code:"+std::to_string(resolutionModes));
}
  
UBYTE CDXLGfxModes::getUBYTE() {
  UBYTE byte=((UBYTE)((UBYTE)(killEHBFlag<<5))|((UBYTE)(colorDepthFlag<<4))|((UBYTE)resolutionModes));
  return byte;
}


string CDXLInfo::planeArrangementToString() {
  switch(planeArrangement) {
  case BIT_PLANAR: return "bit planar";break;
  case BYTE_PLANAR: return "byte planar";break;
  case CHUNKY: return "chunky";break;
  case BIT_LINE: return "bit line";break;
  case BYTE_LINE: return "byte line";break;
  default: return "unknown bitplane arrangement";
  }
}
UBYTE CDXLInfo::getUBYTE() {
  UBYTE byte=((UBYTE)(planeArrangement<<5))|((UBYTE)(stereo<<4))|((UBYTE)encoding);
  return byte;
}

string CDXLInfo::toString() {
  return encodingToString()+" "+stereoToString()+" "+planeArrangementToString();
}

void CDXLHeader::readChunk() {
  fileType=readUBYTE();
  UBYTE byte=readUBYTE();
  info.encoding=byte&0b00000111;
  info.stereo=(byte&0b00010000)>>4;
  info.planeArrangement=(byte&0b11100000)>>5;

  currentChunkSize=readULONG();
  previousChunkSize=readULONG();
  currentFrameNumber=readULONG();
  videoWidth=readUWORD();
  videoHeight=readUWORD();
  numberOfBitplanes=readUWORD();
  paletteSize=readUWORD();
  channelAudioSize=readUWORD();
  frequency=readUWORD();
  fps=readUBYTE();
  UBYTE byte2=readUBYTE();
  modes.resolutionModes=byte2&0b00001111;     // Bits 0-3
  modes.colorDepthFlag=(byte2&0b00010000)>>4; // Bit 4
  modes.killEHBFlag=(byte2&0b00100000)>>5;    // Bit 5
  padding=readUWORD();
  setPaddingModes(padding>>12);
  reserved3=readUWORD();
}

void CDXLHeader::setFileType(CDXLFileType fileType) {
  this->fileType=fileType;
}

void CDXLHeader::writeChunk() {
  writeUBYTE(fileType);
  writeUBYTE(info.getUBYTE());
  writeULONG(currentChunkSize);
  writeULONG(previousChunkSize);
  writeULONG(currentFrameNumber);
  writeUWORD(videoWidth);
  writeUWORD(videoHeight);
  writeUWORD(numberOfBitplanes);
  writeUWORD(paletteSize);
  if(info.stereo==0) {
    writeUWORD(channelAudioSize);
  } else {
    // For stereo data only the mono-data size is reported in the CDXL file
    writeUWORD(channelAudioSize);
  }
  writeUWORD(frequency);
  writeUBYTE(fps);
  UBYTE modesWithoutReserved=modes.getUBYTE()&0b00111111; // Includes kill ehb bit 5
  writeUBYTE(modesWithoutReserved);
  UWORD paddingSizes=(UWORD)((getPaddingModes()<<12)+(getColorPaddingBytes()<<8)+(getVideoPaddingBytes()<<4)+getAudioPaddingBytes());
  writeUWORD(paddingSizes);
  writeUWORD(0); // reserved3
}

string CDXLHeader::toString() {
  stringstream ss;
  const uint16_t colWidth=24;
  ss<<setw(colWidth)<<left<<"File type: "<<fileTypeToString()<<endl;
  ss<<setw(colWidth)<<left<<"Encoding: "<<info.encodingToString()<<endl;
  ss<<setw(colWidth)<<left<<"Sound: "<<info.stereoToString()<<endl;
  ss<<setw(colWidth)<<left<<"Plane arrangement: "<<info.planeArrangementToString()<<endl;
  ss<<setw(colWidth)<<left<<"Current chunk size: "<<currentChunkSize<<endl;
  ss<<setw(colWidth)<<left<<"Previous chunk size: "<<previousChunkSize<<endl;
  ss<<setw(colWidth)<<left<<"Current frame number: "<<currentFrameNumber<<endl;
  ss<<setw(colWidth)<<left<<"Video width: "<<videoWidth<<endl;
  ss<<setw(colWidth)<<left<<"Video height: "<<videoHeight<<endl;
  ss<<setw(colWidth)<<left<<"Number of bitplanes: "<<numberOfBitplanes<<endl;
  ss<<setw(colWidth)<<left<<"[Video size]: "<<getVideoSize()<<endl;
  ss<<setw(colWidth)<<left<<"Palette size: "<<paletteSize<<endl;
  ss<<setw(colWidth)<<left<<"[Number of colors]: "<<getNumberOfColors()<<endl;
  ss<<setw(colWidth)<<left<<"[Bytes per color]: "<<getColorBytes()<<endl;
  ss<<setw(colWidth)<<left<<"Channel Audio size: "<<getChannelAudioSize()<<endl;
  ss<<setw(colWidth)<<left<<"Total Audio size: "<<getTotalAudioSize()<<endl;
  ss<<setw(colWidth)<<left<<"Frequency: "<<frequency<<endl;
  ss<<setw(colWidth)<<left<<"Fps: "<<+fps<<endl;
  ss<<setw(colWidth)<<left<<"Screen resolution mode: "<<modes.resolutionModesToString()<<endl;
  ss<<setw(colWidth)<<left<<"Color depth: "<<modes.colorDepthFlagToString()<<endl;
  ss<<setw(colWidth)<<left<<"KillEHBFlag: "<<modes.killEHBFlagToString()<<endl;
  ss<<setw(colWidth)<<left<<"[paddingMode]: "<<paddingModesToString()<<endl;
  ss<<setw(colWidth)<<left<<"Padding: "
    <<std::hex<<"0x"  // make base explicit, because std::showbase does not show 0x for zero.
    <<std::setfill('0')
    <<std::internal           // Fill between the prefix and the number
    <<std::setw(3)
    <<(padding&0xfff)<<std::dec<<endl;
  ss<<setfill(' ');
  ss<<setw(colWidth)<<left<<"reserved WORD: "<<+reserved3<<endl;
  ss<<setw(colWidth)<<left<<"[isConsistent]: "<<(isConsistent()?"Yes":"No")<<endl;
  return ss.str();
}

ULONG CDXLHeader::getLength() {
  // Length is fixed
  return 32; 
}

void CDXLHeader::setPreviousChunkSize(ULONG size) {
  previousChunkSize=size;
}

void CDXLHeader::setCurrentChunkSize(ULONG size) {
  currentChunkSize=size;
}

void CDXLHeader::setSoundMode(CDXLSoundMode mode) {
  info.stereo=mode;
}

CDXLSoundMode CDXLHeader::getSoundMode() {
  if(info.stereo==0) {
    return MONO;
  } else {
    return STEREO;
  }
}

void CDXLHeader::setPlaneArrangement(CDXLPlaneArrangement planeArrangement) {
  info.planeArrangement=planeArrangement;
}

void CDXLHeader::setEncoding(CDXLVideoEncoding encoding) {
  info.encoding=encoding;
}

void CDXLHeader::setColorBitsFlag(bool flag) {
  modes.colorDepthFlag=(short)flag;
}

bool CDXLHeader::getColorBitsFlag() {
  return modes.colorDepthFlag;
}

void CDXLHeader::setKillEHBFlag(bool flag) {
  modes.killEHBFlag=(short)flag;
}

bool CDXLHeader::getKillEHBFlag() {
  return modes.killEHBFlag;
}

void CDXLHeader::setChannelAudioSize(ULONG size) {
  channelAudioSize=size;
}

void CDXLHeader::setFrameNr(ULONG nr) {
  currentFrameNumber=nr;
}

ULONG CDXLHeader::getFrameNr() {
  return currentFrameNumber;
}

UWORD CDXLHeader::getNumberOfBitplanes() {
  return numberOfBitplanes;
}

void CDXLHeader::setNumberOfBitplanes(UWORD num) {
  numberOfBitplanes=num;
}

UWORD CDXLHeader::getColorBytes() {
  if(modes.colorDepthFlag)
    return 3;
  else
    return 2;
}

UWORD CDXLHeader::getPaletteSize() {
  return paletteSize;
}

void CDXLHeader::setPaletteSize(UWORD size) {
  paletteSize=size;
}

UWORD CDXLHeader::getNumberOfColors() {
  return getPaletteSize()/getColorBytes();
}

UWORD CDXLHeader::getChannelAudioSize() {
  return channelAudioSize;
}

UWORD CDXLHeader::getTotalAudioSize() {
  switch(getSoundMode()){
  case STEREO:
    return getChannelAudioSize()*2;
  case MONO:
    return getChannelAudioSize();
  default:
    throw AGAConvException(304, "CDXLHeader: Unsupport audio mode.");
  }
}

ULONG CDXLHeader::getVideoSize() {
  return (Util::wordAlignedLengthInBytes(videoWidth)*videoHeight)*numberOfBitplanes;
}

ULONG CDXLHeader::getPreviousFrameSize() {
  return previousChunkSize;
}

ULONG CDXLHeader::getCurrentFrameSize() {
  return currentChunkSize;
}

UWORD CDXLHeader::getVideoWidth() {
  return videoWidth;
}

UWORD CDXLHeader::getVideoHeight() {
  return videoHeight;
}

ULONG CDXLHeader::getCurrentFrameNr() {
  return currentFrameNumber;
}

UWORD CDXLHeader::getFrequency() {
  return frequency;
}

void CDXLHeader::setFrequency(UWORD frequency) {
  this->frequency=frequency;
}

void CDXLHeader::setFps(UBYTE fps) {
  this->fps=fps;
}

void CDXLHeader::setResolutionModes(UBYTE modes) {
  this->modes.resolutionModes=modes;
}

void CDXLHeader::setPaddingModes(UBYTE mode) {
  this->paddingModes=mode;
}

UBYTE CDXLHeader::getPaddingModes() {
  return this->paddingModes;
}

ULONG CDXLHeader::getPaddingSize() {
  switch(this->getPaddingModes()) {
  case 0: return 0;
  case 1: return 1;
  case 2: return 2;
  case 3: return 4;
  case 4: return 8;
  default:
    throw AGAConvException(120, "unknown padding mode: "+std::to_string(this->getPaddingModes()));
  }
}
void CDXLHeader::setPaddingSize(ULONG paddingSize) {
  switch(paddingSize) {
  case 0: setPaddingModes(0);break;
  case 1: setPaddingModes(1);break;
  case 2: setPaddingModes(2);break;
  case 4: setPaddingModes(3);break;
  case 8: setPaddingModes(4);break;
  default:
    throw AGAConvException(121, "unknown padding size: "+std::to_string(paddingSize));
  }
}

string CDXLHeader::paddingModesToString() {
  stringstream ss;
  if(getPaddingSize()==0) {
    return "unspecified";
  } else {
    ss<<getPaddingSize()*8<<"bit aligned";
    return ss.str();
  }
}

bool CDXLHeader::isConsistent() {
  ULONG chunkSize=this->getLength()+getPaletteSize()+getVideoSize()+getTotalAudioSize()+getTotalPaddingBytes();
  if(chunkSize!=getCurrentFrameSize()) {
    cout<<"CDXLHeader inconsistent size: "<<chunkSize<<" != "<<getCurrentFrameSize()<<" (computed chunk size vs CDXL frame size)"<<endl;
    cout<<"Length            : "<<this->getLength()<<endl;
    cout<<"Palette size      : "<<getPaletteSize()<<endl;
    cout<<"Video size        : "<<getVideoSize()<<endl;
    cout<<"Total Audio size  : "<<getTotalAudioSize()<<endl;
    cout<<"Total Padding size: "<<getTotalPaddingBytes()<<endl;
    cout<<"Chunk size        : "<<chunkSize<<endl;
    cout<<"Current Frame size: "<<getCurrentFrameSize()<<endl;
    return false;
  } else {
    return true;
  }
}

ULONG CDXLHeader::getColorPaddingBytes() {
  return CDXLFrame::computePaddingBytes(getPaddingSize(),getPaletteSize());
}

ULONG CDXLHeader::getVideoPaddingBytes() {
  return CDXLFrame::computePaddingBytes(getPaddingSize(),getVideoSize());
}

ULONG CDXLHeader::getAudioPaddingBytes() {
  return CDXLFrame::computePaddingBytes(getPaddingSize(),getTotalAudioSize());
}

ULONG CDXLHeader::getTotalPaddingBytes() {
  return getColorPaddingBytes()+getVideoPaddingBytes()+getAudioPaddingBytes();
}

} // namespace AGAConv
