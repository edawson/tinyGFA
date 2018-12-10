CXX:=g++
CXXFLAGS:= -O3 -fPIC -std=c++11 -pipe -D_FILE_OFFSET_BITS=64
LD_INC_FLAGS:= -I./ -I./pliib

utils: indexseq


indexseq: examples/index.cpp pliib/pliib.hpp tinygfaidx.hpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LD_INC_FLAGS)

clean:
	$(RM) getseq
	$(RM) indexseq

.PHONY: utils clean
