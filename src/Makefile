all: agaconv-encode

docs:
	doxygen agaconv-encode.doxygen
	@echo
	@echo "Doxygen html documentation available at src/html/index.html"

VERSION=0.9.5
DISTRFILENAME=agaconv-$(VERSION)

#development options (slows down compilation/execution, all passing)
# -fsanitize=leak -fsanitize=bounds -fsanitize=bounds-strict
#DEV_TEST_FLAGS=-fsanitize=address -ggdb -fno-omit-frame-pointer
#DEV_TEST_FLAGS=-fanalyzer -Wno-analyzer-null-dereference 

CXXFLAGS=-Wall -Werror -Wfatal-errors $(DEV_TEST_FLAGS)

EXEC = agaconv-encode
HEADERS = $(wildcard *.hpp)
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Main target
$(EXEC): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXEC) $(CXXFLAGS) -lpng

# To obtain object files (default rule, superflueous)
%.o: %.cpp 
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXEC)
	rm -f agaconv-*.tgz
	rm -rf html

depend:
	makedepend -Y -- $(CXXFLAGS) -- $(SOURCES) 2> /dev/null

# End of makefile
# DO NOT DELETE

CDXLFrame.o: CDXLFrame.hpp IffChunk.hpp ByteSizeTypeDefs.hpp Chunk.hpp
CDXLFrame.o: CDXLHeader.hpp CDXLBlock.hpp IffBMHDChunk.hpp IffCMAPChunk.hpp
CDXLFrame.o: IffDataChunk.hpp RGBColor.hpp IffCAMGChunk.hpp CDXLPalette.hpp
CDXLFrame.o: ByteSequence.hpp IffILBMChunk.hpp IffBODYChunk.hpp Util.hpp
CDXLFrame.o: Options.hpp
IffCMAPChunk.o: IffCMAPChunk.hpp IffDataChunk.hpp IffChunk.hpp
IffCMAPChunk.o: ByteSizeTypeDefs.hpp Chunk.hpp RGBColor.hpp
StageAnimEdit.o: StageAnimEdit.hpp Stage.hpp Options.hpp IffANIMForm.hpp
StageAnimEdit.o: IffChunk.hpp ByteSizeTypeDefs.hpp Chunk.hpp IffILBMChunk.hpp
StageAnimEdit.o: IffCMAPChunk.hpp IffDataChunk.hpp RGBColor.hpp
StageAnimEdit.o: IffBMHDChunk.hpp IffCAMGChunk.hpp IffBODYChunk.hpp
StageAnimEdit.o: ByteSequence.hpp IffDPANChunk.hpp IffSXHDChunk.hpp
StageAnimEdit.o: IffSBDYChunk.hpp CommandLineParser.hpp AGAConvException.hpp
Options.o: Options.hpp
IffSBDYChunk.o: IffSBDYChunk.hpp IffDataChunk.hpp IffChunk.hpp
IffSBDYChunk.o: ByteSizeTypeDefs.hpp Chunk.hpp
IffCAMGChunk.o: IffCAMGChunk.hpp IffChunk.hpp ByteSizeTypeDefs.hpp Chunk.hpp
IffANHDChunk.o: IffANHDChunk.hpp IffChunk.hpp ByteSizeTypeDefs.hpp Chunk.hpp
CDXLFileGeneration.o: CDXLFileGeneration.hpp FileSequenceConversion.hpp
CDXLFileGeneration.o: Stage.hpp Options.hpp AGAConvException.hpp
CDXLFileGeneration.o: IffILBMChunk.hpp IffChunk.hpp ByteSizeTypeDefs.hpp
CDXLFileGeneration.o: Chunk.hpp IffCMAPChunk.hpp IffDataChunk.hpp
CDXLFileGeneration.o: RGBColor.hpp IffBMHDChunk.hpp IffCAMGChunk.hpp
CDXLFileGeneration.o: IffBODYChunk.hpp ByteSequence.hpp
IffDataChunk.o: IffDataChunk.hpp IffChunk.hpp ByteSizeTypeDefs.hpp Chunk.hpp
CDXLBlock.o: CDXLBlock.hpp IffChunk.hpp ByteSizeTypeDefs.hpp Chunk.hpp
FrameLoader.o: FrameLoader.hpp
CommandLineParser.o: CommandLineParser.hpp Options.hpp Util.hpp
CommandLineParser.o: ByteSizeTypeDefs.hpp
CDXLEncode.o: CDXLEncode.hpp FileSequenceConversion.hpp Stage.hpp Options.hpp
CDXLEncode.o: AGAConvException.hpp IffILBMChunk.hpp IffChunk.hpp
CDXLEncode.o: ByteSizeTypeDefs.hpp Chunk.hpp IffCMAPChunk.hpp
CDXLEncode.o: IffDataChunk.hpp RGBColor.hpp IffBMHDChunk.hpp IffCAMGChunk.hpp
CDXLEncode.o: IffBODYChunk.hpp ByteSequence.hpp CDXLFrame.hpp CDXLHeader.hpp
CDXLEncode.o: CDXLBlock.hpp CDXLPalette.hpp Util.hpp PngLoader.hpp
CDXLEncode.o: FrameLoader.hpp
CDXLPalette.o: CDXLPalette.hpp CDXLBlock.hpp IffChunk.hpp
CDXLPalette.o: ByteSizeTypeDefs.hpp Chunk.hpp IffCMAPChunk.hpp
CDXLPalette.o: IffDataChunk.hpp RGBColor.hpp Options.hpp
IffSXHDChunk.o: IffSXHDChunk.hpp IffChunk.hpp ByteSizeTypeDefs.hpp Chunk.hpp
IffBODYChunk.o: IffBODYChunk.hpp IffDataChunk.hpp IffChunk.hpp
IffBODYChunk.o: ByteSizeTypeDefs.hpp Chunk.hpp RGBColor.hpp ByteSequence.hpp
CDXLDecode.o: CDXLDecode.hpp Stage.hpp Options.hpp CDXLFrame.hpp IffChunk.hpp
CDXLDecode.o: ByteSizeTypeDefs.hpp Chunk.hpp CDXLHeader.hpp CDXLBlock.hpp
CDXLDecode.o: IffBMHDChunk.hpp IffCMAPChunk.hpp IffDataChunk.hpp RGBColor.hpp
CDXLDecode.o: IffCAMGChunk.hpp CDXLPalette.hpp ByteSequence.hpp
CDXLDecode.o: IffILBMChunk.hpp IffBODYChunk.hpp CommandLineParser.hpp
CDXLDecode.o: AGAConvException.hpp Util.hpp
PngLoader.o: Options.hpp PngLoader.hpp Stage.hpp FrameLoader.hpp
PngLoader.o: AGAConvException.hpp RGBColor.hpp ByteSizeTypeDefs.hpp
PngLoader.o: IffDataChunk.hpp IffChunk.hpp Chunk.hpp IffILBMChunk.hpp
PngLoader.o: IffCMAPChunk.hpp IffBMHDChunk.hpp IffCAMGChunk.hpp
PngLoader.o: IffBODYChunk.hpp ByteSequence.hpp Util.hpp
Chunk.o: Options.hpp Chunk.hpp ByteSizeTypeDefs.hpp
CDXLHeader.o: CDXLHeader.hpp CDXLBlock.hpp IffChunk.hpp ByteSizeTypeDefs.hpp
CDXLHeader.o: Chunk.hpp IffBMHDChunk.hpp IffCMAPChunk.hpp IffDataChunk.hpp
CDXLHeader.o: RGBColor.hpp IffCAMGChunk.hpp Options.hpp CDXLFrame.hpp
CDXLHeader.o: CDXLPalette.hpp ByteSequence.hpp IffILBMChunk.hpp
CDXLHeader.o: IffBODYChunk.hpp Util.hpp
Stage.o: Stage.hpp Options.hpp
IffUnknownChunk.o: IffUnknownChunk.hpp IffChunk.hpp ByteSizeTypeDefs.hpp
IffUnknownChunk.o: Chunk.hpp IffDataChunk.hpp
IffANIMForm.o: IffChunk.hpp ByteSizeTypeDefs.hpp Chunk.hpp IffANIMForm.hpp
IffANIMForm.o: IffILBMChunk.hpp IffCMAPChunk.hpp IffDataChunk.hpp
IffANIMForm.o: RGBColor.hpp IffBMHDChunk.hpp IffCAMGChunk.hpp
IffANIMForm.o: IffBODYChunk.hpp ByteSequence.hpp
IffDPANChunk.o: IffDPANChunk.hpp IffChunk.hpp ByteSizeTypeDefs.hpp Chunk.hpp
RGBColor.o: RGBColor.hpp ByteSizeTypeDefs.hpp IffDataChunk.hpp IffChunk.hpp
RGBColor.o: Chunk.hpp
ByteSequence.o: ByteSequence.hpp ByteSizeTypeDefs.hpp
IffChunk.o: IffChunk.hpp ByteSizeTypeDefs.hpp Chunk.hpp Options.hpp
FileSequenceConversion.o: Options.hpp FileSequenceConversion.hpp Stage.hpp
FileSequenceConversion.o: AGAConvException.hpp IffILBMChunk.hpp IffChunk.hpp
FileSequenceConversion.o: ByteSizeTypeDefs.hpp Chunk.hpp IffCMAPChunk.hpp
FileSequenceConversion.o: IffDataChunk.hpp RGBColor.hpp IffBMHDChunk.hpp
FileSequenceConversion.o: IffCAMGChunk.hpp IffBODYChunk.hpp ByteSequence.hpp
FileSequenceConversion.o: IffUnknownChunk.hpp Util.hpp
StageILBMFileInfo.o: Options.hpp StageILBMFileInfo.hpp Stage.hpp
StageILBMFileInfo.o: AGAConvException.hpp IffILBMChunk.hpp IffChunk.hpp
StageILBMFileInfo.o: ByteSizeTypeDefs.hpp Chunk.hpp IffCMAPChunk.hpp
StageILBMFileInfo.o: IffDataChunk.hpp RGBColor.hpp IffBMHDChunk.hpp
StageILBMFileInfo.o: IffCAMGChunk.hpp IffBODYChunk.hpp ByteSequence.hpp
IffBMHDChunk.o: IffBMHDChunk.hpp IffChunk.hpp ByteSizeTypeDefs.hpp Chunk.hpp
AGAConvException.o: AGAConvException.hpp
StageChunkInfo.o: StageChunkInfo.hpp Stage.hpp Options.hpp IffANIMForm.hpp
StageChunkInfo.o: IffChunk.hpp ByteSizeTypeDefs.hpp Chunk.hpp
StageChunkInfo.o: IffILBMChunk.hpp IffCMAPChunk.hpp IffDataChunk.hpp
StageChunkInfo.o: RGBColor.hpp IffBMHDChunk.hpp IffCAMGChunk.hpp
StageChunkInfo.o: IffBODYChunk.hpp ByteSequence.hpp AGAConvException.hpp
IffILBMChunk.o: IffChunk.hpp ByteSizeTypeDefs.hpp Chunk.hpp IffILBMChunk.hpp
IffILBMChunk.o: IffCMAPChunk.hpp IffDataChunk.hpp RGBColor.hpp
IffILBMChunk.o: IffBMHDChunk.hpp IffCAMGChunk.hpp IffBODYChunk.hpp
IffILBMChunk.o: ByteSequence.hpp IffANHDChunk.hpp IffSXHDChunk.hpp
IffILBMChunk.o: IffSBDYChunk.hpp IffDPANChunk.hpp IffUnknownChunk.hpp
agaconv-encode.o: IffANIMForm.hpp IffChunk.hpp ByteSizeTypeDefs.hpp Chunk.hpp
agaconv-encode.o: IffILBMChunk.hpp IffCMAPChunk.hpp IffDataChunk.hpp
agaconv-encode.o: RGBColor.hpp IffBMHDChunk.hpp IffCAMGChunk.hpp
agaconv-encode.o: IffBODYChunk.hpp ByteSequence.hpp IffDPANChunk.hpp
agaconv-encode.o: IffSXHDChunk.hpp IffSBDYChunk.hpp CommandLineParser.hpp
agaconv-encode.o: Options.hpp StageAnimEdit.hpp Stage.hpp
agaconv-encode.o: StageILBMFileInfo.hpp StageChunkInfo.hpp CDXLEncode.hpp
agaconv-encode.o: FileSequenceConversion.hpp AGAConvException.hpp
agaconv-encode.o: CDXLFrame.hpp CDXLHeader.hpp CDXLBlock.hpp CDXLPalette.hpp
agaconv-encode.o: CDXLDecode.hpp
Util.o: Util.hpp ByteSizeTypeDefs.hpp
