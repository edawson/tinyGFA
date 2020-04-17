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
    create_gfa_index(argv[1], tf);
    write_gfa_index(argv[1], tf);

    return 0;
}
