CXX:=g++
CXXFLAGS:= -O3 -fPIC -std=c++14 -pipe -D_FILE_OFFSET_BITS=64
DEBUGFLAGS:= -Wall -O0 -pg -ggdb -fPIC -std=c++14 -pipe -D_FILE_OFFSET_BITS=64
LD_INC_FLAGS:= -I./ -I./pliib


tgfa: tgfa.cpp tinygfa.hpp pliib/pliib.hpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LD_INC_FLAGS)

utils: indexseq getseq

indexseq: examples/index.cpp pliib/pliib.hpp tinygfaidx.hpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LD_INC_FLAGS)

getseq: examples/getseq.cpp pliib/pliib.hpp tinygfaidx.hpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LD_INC_FLAGS)


debug-tgfa: tgfa.cpp tinygfa.hpp pliib/pliib.hpp
	$(CXX) $(DEBUGFLAGS) -o $@ $^ $(LD_INC_FLAGS)


test: test_tinygfa.cpp tinygfa.hpp pliib/pliib.hpp pliib/catch.hpp
	$(CXX) $(DEBUGFLAGS) -o $@ $< $(LD_INC_FLAGS) && ./test

check: test
	./test

clean:
	$(RM) getseq
	$(RM) indexseq
	$(RM) tgfa
	$(RM) debug-tgfa
	$(RM) test

.PHONY: utils clean
