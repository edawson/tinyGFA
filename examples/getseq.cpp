#include <string>
#include <iostream>

#include "pliib.hpp"
#include "tinygfaidx.hpp"

using namespace std;
using namespace TINYGFAIDX;

int main(int argc, char** argv){


    tiny_gfaidx_t tf;
    if (argc < 2){
        cerr << "./getseq <GFA File> <seq> [<start> <end>]" << endl;
        exit(1);
    }

    if (!checkGFAIDXFileExists(argv[1])){
        createGFAIDX(argv[1], tf);
    }
    else{
        parseGFAIDX(argv[1], tf);
    }
    if (argc == 3){
        char* test = argv[2];
        char* seq;
        getSequence(tf, test, seq);
        cout << seq << endl;
        delete [] seq;
    }
    else if (argc == 5){
        char* test = argv[2];
        int start = atoi(argv[3]);
        int end = atoi(argv[4]);
        char* seq;
        cerr << "./getseq <GFA File> <seq> [<start> <end>]  NOT IMPLEMENTED" << endl;
        exit(9);
        //getSequence(tf, test, seq, start, end);
        cout << seq << endl;
        delete [] seq;
    }
    else{
        cerr << "./getseq <GFA File> <seq> [<start> <end>]" << endl;
        exit(1);
    }

    return 0;
}
