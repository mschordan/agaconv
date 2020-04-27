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

#include <sstream>
#include <string>
#include <iostream>
#include <cassert>
#include "CDXLHeader.hpp"
#include "Options.hpp"
#include "CDXLFrame.hpp"

using namespace std;

CDXLHeader::CDXLHeader()
{
  info.encoding=RGB;
  info.stereo=0;
  info.planeArrangement=PA_UNDEFINED;
  modes.resolutionModes=0;
  modes.colorBitsFlag=0;
  modes.paddingModes=0;
}

CDXLHeader::CDXLHeader(IffBMHDChunk* bmhd, IffCMAPChunk* cmap, IffCAMGChunk* camg):CDXLHeader() {
  initialize(bmhd,cmap,camg);
}

void CDXLHeader::initialize(IffBMHDChunk* bmhd, IffCMAPChunk* cmap, IffCAMGChunk* camg) {
  videoWidth=bmhd->getWidth();
  videoHeight=bmhd->getHeight();
  numberOfBitplanes=bmhd->getNumPlanes();
  if(modes.colorBitsFlag) {
    // 24 bit colors
    paletteSize=cmap->numberOfColors()*3;
  } else {
    // 12 bit colors
    paletteSize=cmap->numberOfColors()*2;
  }
  if(camg->isHam()) {
    info.encoding=HAM;
    info.planeArrangement=BIT_LINE; // same as in ilbm
  } else {
    info.encoding=RGB;
    info.planeArrangement=BIT_LINE; // same as in ilbm
  }
  modes.resolutionModes=Options::GFX_LORES; // default
  if(camg->isHires()) {
    //cout<<"DEBUG: setting resolution mode to HIRES."<<endl;
    modes.resolutionModes=Options::GFX_HIRES;
  } else if(camg->isSuperHires()) {
    //cout<<"DEBUG: setting resolution mode to SUPERHIRES."<<endl;
    modes.resolutionModes=Options::GFX_SUPERHIRES;
  }
  if(camg->isLace()) {
    modes.resolutionModes|=(1<<3); // BIT 3: LACE
  }
  // possible refinement: fill up palette to numberOfBitplanes^2 if
  // tools cannot handle varying number of colors
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
  ss<<+paddingModes<<":"<<colorBitsFlagToString()<<":"<<+resolutionModes<<endl;
  return ss.str();
}

string CDXLInfo::stereoToString() {
  if(stereo==1)
    return "stereo";
  else
    return "mono";
}  

string CDXLGfxModes::colorBitsFlagToString() {
  if(colorBitsFlag==1)
    return "24Bit";
  else
    return "12Bit";
}  

UBYTE CDXLGfxModes::getUBYTE() {
  UBYTE byte=((UBYTE)((UBYTE)(paddingModes<<5))|((UBYTE)(colorBitsFlag<<4))|((UBYTE)resolutionModes));
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
  //cout<<"DEBUG: CDXLHeader::readChunk: RAW: "<<+byte<<endl;
  info.encoding=byte&0b00000111;
  info.stereo=(byte&0b00010000)>>4;
  info.planeArrangement=(byte&0b11100000)>>5;
  //cout<<"DEBUG: CDXLHeader::readChunk: pa:"<<+info.planeArrangement<<endl;
  //cout<<"DEBUG: CDXLHeader::readChunk: COMP: "<<+info.getUBYTE()<<":"<<info.toString()<<endl;

  currentChunkSize=readULONG();
  previousChunkSize=readULONG();
  reserved1=readUWORD();
  currentFrameNumber=readUWORD();
  videoWidth=readUWORD();
  videoHeight=readUWORD();
  numberOfBitplanes=readUWORD();
  paletteSize=readUWORD();
  channelAudioSize=readUWORD();
  frequency=readUWORD();
  //reserved2=readUWORD();
  fps=readUBYTE();
  UBYTE byte2=readUBYTE();
  modes.resolutionModes=byte2&0b00001111;
  modes.colorBitsFlag=(byte2&0b00010000)>>4;
  padding=readUWORD();
  modes.paddingModes=(padding>>12); // TODO: store padding modes in correct field
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
  writeUWORD(reserved1);
  writeUWORD(currentFrameNumber);
  writeUWORD(videoWidth);
  writeUWORD(videoHeight);
  writeUWORD(numberOfBitplanes);
  writeUWORD(paletteSize);
  if(info.stereo==0) {
    writeUWORD(channelAudioSize);
  } else {
    // also for stereo data only the mono-data size is reported in the CDXL file
    writeUWORD(channelAudioSize);
  }
  writeUWORD(frequency);
  //writeUWORD(reserved2);
  writeUBYTE(fps);
  UBYTE modesWithoutOldPadding=modes.getUBYTE()&0b00011111;
  //cout<<"DEBUG: Writing gfx modes (info2): 0x"<<std::hex<<+modesWithoutOldPadding<<std::dec<<endl;
  writeUBYTE(modesWithoutOldPadding); // TODO: change paddingModes to separate field in padding
  UWORD paddingSizes=(UWORD)((modes.paddingModes<<12)+(getColorPaddingBytes()<<8)+(getVideoPaddingBytes()<<4)+getAudioPaddingBytes());
  //cout<<"DEBUG: padding WORD: 0x"<<std::hex<<+paddingSizes<<std::dec<<endl;
  writeUWORD(paddingSizes);
  writeUWORD(0); // reserved3
  //cout<<"DEBUG:----"<<endl;
}

string CDXLHeader::toString() {
  stringstream ss;
  ss<<"File type: "<<fileTypeToString()<<endl;
  ss<<"Encoding: "<<info.encodingToString()<<endl;
  ss<<"Sound: "<<info.stereoToString()<<endl;
  ss<<"Plane arrangement: "<<info.planeArrangementToString()<<endl;
  ss<<"Current chunk size: "<<currentChunkSize<<endl;
  ss<<"Previous chunk size: "<<previousChunkSize<<endl;
  ss<<"reserved WORD: "<<reserved1<<endl;
  ss<<"Current frame number: "<<currentFrameNumber<<endl;
  ss<<"Video width: "<<videoWidth<<endl;
  ss<<"Video height: "<<videoHeight<<endl;
  ss<<"Number of bitplanes: "<<numberOfBitplanes<<endl;
  ss<<"[Video size]: "<<getVideoSize()<<endl;
  ss<<"Palette size: "<<paletteSize<<endl;
  ss<<"Channel Audio size: "<<getChannelAudioSize()<<endl;
  ss<<"Total Audio size: "<<getTotalAudioSize()<<endl;
  ss<<"Frequency: "<<frequency<<endl;
  //ss<<"reserved WORD: "<<reserved2<<endl;
  ss<<"Fps: "<<+fps<<endl;
  ss<<"Modes.resolutionModes: "<<+modes.resolutionModes<<endl;
  ss<<"Modes.ColorBitsFlag: "<<modes.colorBitsFlagToString()<<endl;
  ss<<"Modes.paddingModes: "<<+modes.paddingModes<<endl;
  ss<<"Padding: 0x"<<hex<<padding<<dec<<endl;
  ss<<"reserved WORD: "<<reserved3<<endl;
  ss<<"isConsistent: "<<(isConsistent()?"Yes":"No")<<endl;
  return ss.str();
}

ULONG CDXLHeader::getLength() {
  // length is fixed
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
  modes.colorBitsFlag=(short)flag;
}

bool CDXLHeader::getColorBitsFlag() {
  return modes.colorBitsFlag;
}

void CDXLHeader::setChannelAudioSize(ULONG size) {
  channelAudioSize=size;
}

void CDXLHeader::setFrameNr(UWORD nr) {
  currentFrameNumber=nr;
}

UWORD CDXLHeader::getNumberOfBitplanes() {
  return numberOfBitplanes;
}
UWORD CDXLHeader::getPaletteSize() {
  return paletteSize;
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
    cerr<<"Unsupport audio mode."<<endl;
    exit(1);
  }
}

ULONG CDXLHeader::getVideoSize() {
  // TODO: compute WORD alignment in getVideoSize?
  return (videoWidth*videoHeight/8)*numberOfBitplanes;
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

UWORD CDXLHeader::getCurrentFrameNr() {
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

void CDXLHeader::setPaddingModes(UBYTE modes) {
  this->modes.paddingModes=modes;
}

UBYTE CDXLHeader::getPaddingModes() {
  return this->modes.paddingModes;
}

ULONG CDXLHeader::getPaddingSize() {
  switch(this->modes.paddingModes) {
  case 0: return 0;
  case 1: return 1;
  case 2: return 2;
  case 3: return 4;
  case 4: return 8;
  default:
    cerr<<"Error: unknown padding mode: "<<modes.paddingModes<<endl;
    exit(1);
  }
}
void CDXLHeader::setPaddingSize(ULONG paddingSize) {
  switch(paddingSize) {
  case 0: modes.paddingModes=0;break;
  case 1: modes.paddingModes=1;break;
  case 2: modes.paddingModes=2;break;
  case 4: modes.paddingModes=3;break;
  case 8: modes.paddingModes=4;break;
  default:
    cerr<<"Error: unknown padding size: "<<paddingSize<<endl;
    exit(1);
  }
}

bool CDXLHeader::isConsistent() {
  ULONG chunkSize=this->getLength()+getPaletteSize()+getVideoSize()+getTotalAudioSize();
  ULONG padding=getColorPaddingBytes()+getVideoPaddingBytes()+getAudioPaddingBytes();
  chunkSize+=padding;
  if(chunkSize!=getCurrentFrameSize()) {
    cout<<"CDXLHeader inconsistent size: "<<chunkSize<<" != "<<getCurrentFrameSize()<<" (computed vs header's current chunk size)"<<endl;
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
