#include <string>
#include <iostream>

#include "pliib.hpp"
#include "tinygfaidx.hpp"

using namespace std;
using namespace TINYGFAIDX;

int main(int argc, char** argv){

    if (argc < 2){
        cerr << "./index <fastaFile>" << endl;
    }

    tiny_gfaidx_t tf;
    createGFAIDX(argv[1], tf);
    writeGFAIDX(argv[1], tf);

    return 0;
}
