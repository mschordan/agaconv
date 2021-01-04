/*
    AGAConv - CDXL video converter for Commodore-Amiga computers
    Copyright (C) 2019-2021 Markus Schordan

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

#include "CDXLFrame.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
#include "Util.hpp"
#include "Options.hpp"

using namespace std;

CDXLFrame::CDXLFrame() {
}

CDXLFrame::~CDXLFrame() {
  if(video)
    delete video;
  if(audio)
    delete audio;
}

void CDXLFrame::setPaddingSize(ULONG paddingSize) {
  header.setPaddingSize(paddingSize);
}

ULONG CDXLFrame::getPaddingSize() {
  return header.getPaddingSize();
}

ULONG CDXLFrame::computePaddingBytes(ULONG alignment, ULONG size) {
  if(alignment==0) {
    return 0;
  } else {
    ULONG padBytes=alignment-(size%alignment);
    if(padBytes==alignment) {
      return 0;
    } else {
      return padBytes;
    }
  }
}

ULONG CDXLFrame::getLength() {
  ULONG paletteLength=palette.getLength();
  ULONG length=header.getLength()+paletteLength;
  if(video)
    length+=video->getDataSize();
  if(audio)
    length+=audio->getDataSize();
  ULONG paddingBytes=header.getTotalPaddingBytes();
  length+=paddingBytes;
  return length;
}

ULONG CDXLFrame::getColorPaddingBytes() {
  return header.getColorPaddingBytes();
}

ULONG CDXLFrame::getVideoPaddingBytes() {
  return header.getVideoPaddingBytes();
}

ULONG CDXLFrame::getAudioPaddingBytes() {
  return header.getAudioPaddingBytes();
}


void CDXLFrame::readChunk() {
  header.setFile(file);
  header.readChunk();
  palette.setFile(file);
  palette.setDataSize(header.getPaletteSize());
  
  if(!header.getColorBitsFlag()) {
    palette.setColorMode(CDXLPalette::COL_12BIT);
  } else {
    palette.setColorMode(CDXLPalette::COL_24BIT);
  }

  palette.readChunk();
  //ULONG paddingSize=header.getPaddingSize();
  //cout<<"DEBUG: Detected padding size: "<<paddingSize<<endl;
  ULONG padBytes=getColorPaddingBytes();
  for(ULONG i=0;i<padBytes;i++) {
    readUBYTE();
  }
  video=readByteSequence(file,header.getVideoSize());
  padBytes=getVideoPaddingBytes();
  for(ULONG i=0;i<padBytes;i++) {
    readUBYTE();
  }
  audio=readByteSequence(file,header.getTotalAudioSize());
  padBytes=getAudioPaddingBytes();
  for(ULONG i=0;i<padBytes;i++) {
    readUBYTE();
  }
}

void CDXLFrame::setOutFile(fstream* fstream) {
  // set outfile for all children
  header.setOutFile(fstream);
  palette.setOutFile(fstream);
  if(video)
    video->setOutFile(fstream);
  if(audio)
    audio->setOutFile(fstream);
  // set outfile for CDXLFrame object itself using overridden method
  Chunk::setOutFile(fstream); 
}


void CDXLFrame::writeChunk() {
  if(!header.isConsistent()) {
    cout<<"Error: inconsistent frame data. Bailing out."<<endl;
    exit(1);
  }
  ULONG numWrittenBytes=0;
  header.writeChunk();
  numWrittenBytes+=header.getLength();
  palette.writeChunk();
  numWrittenBytes+=palette.getLength();

  ULONG padBytes=getColorPaddingBytes();
  for(ULONG i=0;i<padBytes;i++) {
    this->writeUBYTE(0);
    numWrittenBytes+=1;
  }
  if(video) {
    video->writeData();
    numWrittenBytes+=video->getDataSize();
  }
  padBytes=getVideoPaddingBytes();
  for(ULONG i=0;i<padBytes;i++) {
    this->writeUBYTE(0);
    numWrittenBytes+=1;
  }
  if(audio) {
    audio->writeData();
    numWrittenBytes+=audio->getDataSize();
  }
  padBytes=getAudioPaddingBytes();
  for(ULONG i=0;i<padBytes;i++) {
    this->writeUBYTE(0);
    numWrittenBytes+=1;
  }
  if(header.getCurrentFrameSize()!=numWrittenBytes) {
    cout<<"Error: header.currentChunkSize mismatch with written bytes: "<<header.getCurrentFrameSize()<<" != "<<numWrittenBytes<<endl;
    exit(1);
  }
}

string CDXLFrame::toString() {
  stringstream ss;
  ss<<"Padding size: "<<getPaddingSize()<<endl;
  ss<<"Pad bytes: ";
  ss<<computePaddingBytes(getPaddingSize(),palette.getLength());
  ss<<endl;
  return header.toString()+ss.str();
}

void CDXLFrame::importVideo(IffILBMChunk* ilbm) {
  assert(ilbm);;
  ilbm->uncompressBODYChunk();
  IffBODYChunk* body=ilbm->getBODYChunk();
  assert(body);
  UWORD w=header.getVideoWidth();
  UWORD h=header.getVideoHeight();
  UWORD planes=header.getNumberOfBitplanes();
  assert(w%8==0);
  UWORD lineLengthInBytes=Util::wordAlignedLengthInBytes(w);
  ByteSequence* iffVideo=body->getUncompressedData();
  assert(iffVideo);
  UBYTE* source=nullptr;
  if(planes>0&&lineLengthInBytes>0) {
    source=iffVideo->address(0);
  }
  UWORD planeSize=h*lineLengthInBytes;
  // reserve bytes for all bitplanes as one contigeous memory
  // this loop converts interleaved ILBM to bitplanes
  ByteSequence* newBitPlanarVideo=new ByteSequence(planeSize*planes);
  for(UWORD y=0;y<h;y++) {
    for(UWORD p=0;p<planes;p++) {
      UBYTE* targetPlane=newBitPlanarVideo->address(p*planeSize)+y*lineLengthInBytes;
      for(UWORD i=0;i<lineLengthInBytes;i++) {
        assert(source!=nullptr);
        assert(targetPlane!=nullptr);
        *targetPlane++=*source++;
      }
    }
  }
  video=newBitPlanarVideo;
  delete iffVideo;
}

ByteSequence* CDXLFrame::readByteSequence(fstream* inFile, ULONG length) {
  ByteSequence* bs=new ByteSequence();
  bs->setInFile(inFile);
  bs->readData(length);
  return bs;
}
