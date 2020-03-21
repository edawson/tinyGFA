CXX:=g++
CXXFLAGS:= -O3 -fPIC -std=c++14 -pipe -D_FILE_OFFSET_BITS=64
DEBUGFLAGS:= -O0 -pg -ggdb -fPIC -std=c++14 -pipe -D_FILE_OFFSET_BITS=64
LD_INC_FLAGS:= -I./ -I./pliib

utils: indexseq

indexseq: examples/index.cpp pliib/pliib.hpp tinygfaidx.hpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LD_INC_FLAGS)

tgfa: tgfa.cpp tinygfa.hpp pliib/pliib.hpp
	$(CXX) $(DEBUGFLAGS) -o $@ $^ $(LD_INC_FLAGS)

clean:
	$(RM) getseq
	$(RM) indexseq

.PHONY: utils clean
