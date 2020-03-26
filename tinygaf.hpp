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
        gaf_elem(char**& splits, std::size_t& num_splits, std::size_t*& split_sizes){

        };
        void set(char**& splits, std::size_t& num_splits, std::size_t*& split_sizes){
        }
        void clear(){

        }
        std::string to_string(){

        }
        std::ostream& output(std::ostream& os){
            return os;
        }
    }
}
