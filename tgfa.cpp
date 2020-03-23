#include <iostream>
#include <string>
#include <getopt.h>
#include "tinygfa.hpp"


int main_stats(int argc, char** argv){
    std::string gfile;

    if (argc <= 2){
        std::cerr << "Please provide a GFA file." << std::endl;
        return -1;
    }

    optind = 2;
    int c;
    while (true){
        static struct option long_options[] =
        {
            {"help", no_argument, 0, 'h'},
            {"num-nodes", no_argument, 0, 'n'},
            {"num-edges", no_argument, 0, 'e'},
            {"length", no_argument, 0, 'l'},
            {"all", no_argument, 0, 'a'},
            {"paths", no_argument, 0, 'p'},
            {"assembly", no_argument, 0, 'A'},
            {"version", no_argument, 0, 'v'},

            {0,0,0,0}
        };
    
        int option_index = 0;
        c = getopt_long(argc, argv, "hpaAnel", long_options, &option_index);
        if (c == -1){
            break;
        }

        switch (c){

            case '?':
            case 'h':
                // nodes, edges, all stats, edges, paths
                exit(0);
            default:
                abort();
        }
    }

    gfile = argv[optind];

    tgfa::gfa_stat_t stats;

    auto s_func = [&](tgfa::sequence_elem s){std::cout << "S:" << s.seq_id << ":" << s.seq_length << std::endl;};
    auto e_func = [](tgfa::edge_elem e){};
    auto g_func = [](tgfa::group_elem g){};

    parse_gfa_file(gfile.c_str(),
            s_func,
            e_func,
            g_func,
            stats);
    std::cout << stats.to_string() << std::endl;

}

int main_index(int argc, char** argv){

    return 0;
}


int main(int argc, char** argv){
    if (argc < 2){
        std::cerr << "No subcommand provided. Please provide a subcommand." << std::endl;
        return 1;
    }
     
    if (strcmp(argv[1], "stats") == 0){
        return main_stats(argc, argv);
    }
    else{
        std::cerr << "Invalid subcommand [" << argv[1] << "]. Please choose a valid subcommand" << std::endl;
    }
    return 0;
}
