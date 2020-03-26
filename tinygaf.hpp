#include <string>
#include <fstream>
#include <vector>
#include <mmap.h>
#include <cstdint>
#include <functional>

#include <pliib.hpp>

namespace tgfa{

    struct gaf_elem{
        std::uint8_t mapq = 255;

        char* query_name = nullptr;
        std::uint32_t query_length = 0;
        std::uint32_t query_start = 0;
        std::uint32_t query_end = 0;
        bool plus_strand = true;

        char* path_match = nullptr;
        std::uint32_t path_length = 0;
        std::uint32_t path_start = 0;
        std::uint32_t path_end = 0;
        std::uint32_t residue_matches = 0;
        std::uint32_t aln_block_length = 0;

        gaf_elem(){

        }
        ~gaf_elem(){

        }
        gaf_elem(char**& splits, std::size_t& num_splits, std::size_t*& split_sizes, int spec = 1){

        };
        void set(char**& splits, std::size_t& num_splits, std::size_t*& split_sizes, int spec = 1){
        }
        void clear(){

        }
        std::string to_string(){

        }
        std::ostream& output(std::ostream& os){
            return os;
        }
    }

    struct gaf_stats{

    }

    inline void parse_gaf_file(const char*& filename,
            auto gaf_func,
            gaf_stats stats,
            double spec = 0.1){
        std::ifstream instream;
        instream.open(filename, std::ifstream::in);

        if (!instream.good()){
            std::cerr << "Error: file " << filename << " could not be properly opened [parse_gaf_file] ." << std::endl;
            exit(9);
        }

        return parse_gaf_file(instream, gaf_func, stats, spec);

    }

    inline void parse_gaf_file(std::ifstream& instream,
            auto gaf_func,
            gaf_stats stats,
            double spec = 0.1){

    }


}
