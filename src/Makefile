all: agaconv

doc:
	doxygen agaconv.doxygen

#development options (slows down compilation/execution, all passing)
# -fsanitize=leak -fsanitize=bounds -fsanitize=bounds-strict
#DEV_TEST_FLAGS=-fsanitize=address -ggdb -fno-omit-frame-pointer
#DEV_TEST_FLAGS=-fanalyzer -Wno-analyzer-null-dereference 

CXXFLAGS=-std=c++17 -Wall -Werror -Wfatal-errors $(DEV_TEST_FLAGS)

EXEC = agaconv
HEADERS = $(wildcard *.hpp)
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Main target
$(EXEC): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXEC) $(CXXFLAGS) -lpng

# To obtain object files (default rule, superflueous)
%.o: %.cpp 
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

check:
	./agaconv --help
	./agaconv --help-advanced
	./agaconv --verbose=2 --save-config test.config
	./agaconv --verbose=2 --load-config test.config --width=640 --save-config test.config
	rm test.config
	@echo "================="
	@echo "Smoke test passed"
	@echo "================="

clean:
	rm -f $(OBJECTS) $(EXEC)

depend:
	makedepend -Y -- $(CXXFLAGS) -- $(SOURCES) 2> /dev/null

# End of makefile
# DO NOT DELETE

IffCMAPChunk.o: AGAConvException.hpp IffCMAPChunk.hpp IffDataChunk.hpp
IffCMAPChunk.o: IffChunk.hpp AmigaTypeDefs.hpp Chunk.hpp RGBColor.hpp
CDXLDecode.o: CDXLDecode.hpp Stage.hpp Options.hpp Util.hpp AmigaTypeDefs.hpp
CDXLDecode.o: CDXLFrame.hpp ByteSequence.hpp CDXLBlock.hpp IffChunk.hpp
CDXLDecode.o: Chunk.hpp CDXLHeader.hpp IffBMHDChunk.hpp IffCAMGChunk.hpp
CDXLDecode.o: IffCMAPChunk.hpp IffDataChunk.hpp RGBColor.hpp CDXLPalette.hpp
CDXLDecode.o: IffILBMChunk.hpp IffBODYChunk.hpp AGAConvException.hpp
CDXLDecode.o: CommandLineParser.hpp Configuration.hpp OSLayer.hpp
IffANIMForm.o: IffANIMForm.hpp IffChunk.hpp AmigaTypeDefs.hpp Chunk.hpp
IffANIMForm.o: IffILBMChunk.hpp IffBMHDChunk.hpp IffBODYChunk.hpp
IffANIMForm.o: ByteSequence.hpp IffDataChunk.hpp RGBColor.hpp
IffANIMForm.o: IffCAMGChunk.hpp IffCMAPChunk.hpp AGAConvException.hpp
IffSXHDChunk.o: IffSXHDChunk.hpp IffChunk.hpp AmigaTypeDefs.hpp Chunk.hpp
IffCAMGChunk.o: IffCAMGChunk.hpp IffChunk.hpp AmigaTypeDefs.hpp Chunk.hpp
IffCAMGChunk.o: AGAConvException.hpp
IffANHDChunk.o: IffANHDChunk.hpp IffChunk.hpp AmigaTypeDefs.hpp Chunk.hpp
IffSBDYChunk.o: IffSBDYChunk.hpp IffDataChunk.hpp IffChunk.hpp
IffSBDYChunk.o: AmigaTypeDefs.hpp Chunk.hpp
StageChunkInfo.o: StageChunkInfo.hpp Options.hpp Util.hpp AmigaTypeDefs.hpp
StageChunkInfo.o: Stage.hpp IffANIMForm.hpp IffChunk.hpp Chunk.hpp
StageChunkInfo.o: IffILBMChunk.hpp IffBMHDChunk.hpp IffBODYChunk.hpp
StageChunkInfo.o: ByteSequence.hpp IffDataChunk.hpp RGBColor.hpp
StageChunkInfo.o: IffCAMGChunk.hpp IffCMAPChunk.hpp AGAConvException.hpp
CDXLPalette.o: CDXLPalette.hpp CDXLBlock.hpp IffChunk.hpp AmigaTypeDefs.hpp
CDXLPalette.o: Chunk.hpp IffCMAPChunk.hpp IffDataChunk.hpp RGBColor.hpp
CDXLPalette.o: AGAConvException.hpp Options.hpp Util.hpp
CDXLHeader.o: CDXLHeader.hpp CDXLBlock.hpp IffChunk.hpp AmigaTypeDefs.hpp
CDXLHeader.o: Chunk.hpp IffBMHDChunk.hpp IffCAMGChunk.hpp IffCMAPChunk.hpp
CDXLHeader.o: IffDataChunk.hpp RGBColor.hpp AGAConvException.hpp
CDXLHeader.o: CDXLFrame.hpp ByteSequence.hpp CDXLPalette.hpp IffILBMChunk.hpp
CDXLHeader.o: IffBODYChunk.hpp Options.hpp Util.hpp
OSLayerFallback.o: OSLayerFallback.hpp OSLayer.hpp
IffBODYChunk.o: IffBODYChunk.hpp ByteSequence.hpp AmigaTypeDefs.hpp
IffBODYChunk.o: IffDataChunk.hpp IffChunk.hpp Chunk.hpp RGBColor.hpp
IffBODYChunk.o: AGAConvException.hpp
StageILBMFileInfo.o: AGAConvException.hpp IffILBMChunk.hpp IffBMHDChunk.hpp
StageILBMFileInfo.o: IffChunk.hpp AmigaTypeDefs.hpp Chunk.hpp
StageILBMFileInfo.o: IffBODYChunk.hpp ByteSequence.hpp IffDataChunk.hpp
StageILBMFileInfo.o: RGBColor.hpp IffCAMGChunk.hpp IffCMAPChunk.hpp
StageILBMFileInfo.o: Options.hpp Util.hpp StageILBMFileInfo.hpp Stage.hpp
PngLoader.o: PngLoader.hpp AGAConvException.hpp FrameLoader.hpp
PngLoader.o: IffILBMChunk.hpp IffBMHDChunk.hpp IffChunk.hpp AmigaTypeDefs.hpp
PngLoader.o: Chunk.hpp IffBODYChunk.hpp ByteSequence.hpp IffDataChunk.hpp
PngLoader.o: RGBColor.hpp IffCAMGChunk.hpp IffCMAPChunk.hpp Stage.hpp
PngLoader.o: Options.hpp Util.hpp
Chunk.o: Chunk.hpp AmigaTypeDefs.hpp
IffBMHDChunk.o: IffBMHDChunk.hpp IffChunk.hpp AmigaTypeDefs.hpp Chunk.hpp
ByteSequence.o: ByteSequence.hpp AmigaTypeDefs.hpp
RGBColor.o: RGBColor.hpp AmigaTypeDefs.hpp IffDataChunk.hpp IffChunk.hpp
RGBColor.o: Chunk.hpp
AGAConvException.o: AGAConvException.hpp
CDXLBlock.o: CDXLBlock.hpp IffChunk.hpp AmigaTypeDefs.hpp Chunk.hpp
agaconv.o: CDXLDecode.hpp Stage.hpp Options.hpp Util.hpp AmigaTypeDefs.hpp
agaconv.o: CDXLFrame.hpp ByteSequence.hpp CDXLBlock.hpp IffChunk.hpp
agaconv.o: Chunk.hpp CDXLHeader.hpp IffBMHDChunk.hpp IffCAMGChunk.hpp
agaconv.o: IffCMAPChunk.hpp IffDataChunk.hpp RGBColor.hpp CDXLPalette.hpp
agaconv.o: IffILBMChunk.hpp IffBODYChunk.hpp CDXLEncode.hpp
agaconv.o: FileSequenceConversion.hpp AGAConvException.hpp
agaconv.o: CommandLineParser.hpp Configuration.hpp OSLayer.hpp
agaconv.o: ExternalToolDriver.hpp StageAnimEdit.hpp StageChunkInfo.hpp
agaconv.o: StageILBMFileInfo.hpp
CDXLFrame.o: CDXLFrame.hpp ByteSequence.hpp AmigaTypeDefs.hpp CDXLBlock.hpp
CDXLFrame.o: IffChunk.hpp Chunk.hpp CDXLHeader.hpp IffBMHDChunk.hpp
CDXLFrame.o: IffCAMGChunk.hpp IffCMAPChunk.hpp IffDataChunk.hpp RGBColor.hpp
CDXLFrame.o: CDXLPalette.hpp IffILBMChunk.hpp IffBODYChunk.hpp Util.hpp
CDXLFrame.o: Options.hpp AGAConvException.hpp
Util.o: Util.hpp AmigaTypeDefs.hpp AGAConvException.hpp
OSLayer.o: OSLayer.hpp OSLayerFallback.hpp OSLayerLinux.hpp
StageAnimEdit.o: StageAnimEdit.hpp Options.hpp Util.hpp AmigaTypeDefs.hpp
StageAnimEdit.o: Stage.hpp AGAConvException.hpp CommandLineParser.hpp
StageAnimEdit.o: Configuration.hpp OSLayer.hpp IffANIMForm.hpp IffChunk.hpp
StageAnimEdit.o: Chunk.hpp IffILBMChunk.hpp IffBMHDChunk.hpp IffBODYChunk.hpp
StageAnimEdit.o: ByteSequence.hpp IffDataChunk.hpp RGBColor.hpp
StageAnimEdit.o: IffCAMGChunk.hpp IffCMAPChunk.hpp IffDPANChunk.hpp
StageAnimEdit.o: IffSBDYChunk.hpp IffSXHDChunk.hpp
Options.o: Options.hpp Util.hpp AmigaTypeDefs.hpp AGAConvException.hpp
Configuration.o: Configuration.hpp OSLayer.hpp Options.hpp Util.hpp
Configuration.o: AmigaTypeDefs.hpp AGAConvException.hpp
IffDataChunk.o: IffDataChunk.hpp IffChunk.hpp AmigaTypeDefs.hpp Chunk.hpp
ExternalToolDriver.o: ExternalToolDriver.hpp CDXLEncode.hpp ByteSequence.hpp
ExternalToolDriver.o: AmigaTypeDefs.hpp CDXLFrame.hpp CDXLBlock.hpp
ExternalToolDriver.o: IffChunk.hpp Chunk.hpp CDXLHeader.hpp IffBMHDChunk.hpp
ExternalToolDriver.o: IffCAMGChunk.hpp IffCMAPChunk.hpp IffDataChunk.hpp
ExternalToolDriver.o: RGBColor.hpp CDXLPalette.hpp IffILBMChunk.hpp
ExternalToolDriver.o: IffBODYChunk.hpp FileSequenceConversion.hpp
ExternalToolDriver.o: AGAConvException.hpp Options.hpp Util.hpp Stage.hpp
ExternalToolDriver.o: OSLayer.hpp
CDXLEncode.o: CDXLEncode.hpp ByteSequence.hpp AmigaTypeDefs.hpp CDXLFrame.hpp
CDXLEncode.o: CDXLBlock.hpp IffChunk.hpp Chunk.hpp CDXLHeader.hpp
CDXLEncode.o: IffBMHDChunk.hpp IffCAMGChunk.hpp IffCMAPChunk.hpp
CDXLEncode.o: IffDataChunk.hpp RGBColor.hpp CDXLPalette.hpp IffILBMChunk.hpp
CDXLEncode.o: IffBODYChunk.hpp FileSequenceConversion.hpp
CDXLEncode.o: AGAConvException.hpp Options.hpp Util.hpp Stage.hpp
CDXLEncode.o: PngLoader.hpp FrameLoader.hpp
CommandLineParser.o: CommandLineParser.hpp Configuration.hpp OSLayer.hpp
CommandLineParser.o: Options.hpp Util.hpp AmigaTypeDefs.hpp
CommandLineParser.o: AGAConvException.hpp
OSLayerLinux.o: OSLayerLinux.hpp
FileSequenceConversion.o: FileSequenceConversion.hpp AGAConvException.hpp
FileSequenceConversion.o: IffILBMChunk.hpp IffBMHDChunk.hpp IffChunk.hpp
FileSequenceConversion.o: AmigaTypeDefs.hpp Chunk.hpp IffBODYChunk.hpp
FileSequenceConversion.o: ByteSequence.hpp IffDataChunk.hpp RGBColor.hpp
FileSequenceConversion.o: IffCAMGChunk.hpp IffCMAPChunk.hpp Options.hpp
FileSequenceConversion.o: Util.hpp Stage.hpp IffUnknownChunk.hpp
IffUnknownChunk.o: IffUnknownChunk.hpp IffChunk.hpp AmigaTypeDefs.hpp
IffUnknownChunk.o: Chunk.hpp IffDataChunk.hpp
IffDPANChunk.o: IffDPANChunk.hpp IffChunk.hpp AmigaTypeDefs.hpp Chunk.hpp
IffDPANChunk.o: AGAConvException.hpp
IffILBMChunk.o: IffILBMChunk.hpp IffBMHDChunk.hpp IffChunk.hpp
IffILBMChunk.o: AmigaTypeDefs.hpp Chunk.hpp IffBODYChunk.hpp ByteSequence.hpp
IffILBMChunk.o: IffDataChunk.hpp RGBColor.hpp IffCAMGChunk.hpp
IffILBMChunk.o: IffCMAPChunk.hpp AGAConvException.hpp IffANHDChunk.hpp
IffILBMChunk.o: IffDPANChunk.hpp IffSBDYChunk.hpp IffSXHDChunk.hpp
IffILBMChunk.o: IffUnknownChunk.hpp
