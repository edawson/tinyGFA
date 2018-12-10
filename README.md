tinyGFA
-----------------
Parse, index and get random access to GFA files.

### Current Limitations
Right now, we only index the Sequence lines of the
GFA file. There is code to index other types of lines
but the implementation isn't yet finished.

### Overview
GFA files provide a way to represent graph data structures from
assemblies and variation graphs. GFA is analagous to FASTA for
graphs. We often want random access to FASTA sequences on disk
using the .FAI index format. tinyGFA seeks to bring the same
indexing utilities to GFA.


### Build / install
To use tinyGFA, add the below lines to your C++ code:  
```
    #include "tinyGFA/tinygfaidx.hpp"  
    #include "tinyGFA/pliib.hpp"

    using namespace GFAIDX;
```

tinyGFA should build with gcc (or clang if you've got OpenMP).  
To build, link the `.hpp` file and then build your code.

### Usage

