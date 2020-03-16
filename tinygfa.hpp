#include <string>
#include <fstream>
#include <vector>
#include <mmap.h>
#include <cstdint>
#include <functional>

#include <pliib.hpp>

namespace tgfa{

    enum val_types {STRING_TYPE, INT_TYPE, FLOAT_TYPE}
    struct opt_elem{
        char* opt_id;
        std::uint8_t type;
        char* val;
        
        std::string get_id(){
            return std::string(opt_id);
        }
        std::string get_val(){
            return std::string(val);
        }
        std::uint8_t get_type(){
            return type;
        }
        
    };
    struct header_elem{

    };
    struct sequence_elem{
        char* seq_id;
        char* seq;
        std::uint32_t seq_length;
        std::vector<opt_elem> tags;
    };
    struct edge_elem{
        char* edge_id;
        char* source_id;
        char* sink_id;
        bool source_orientation_forward;
        bool sink_orientation_forward;
        std::pair<uint32_t, uint32_t> source_pos;
        std::pair<uint32_t, uint32_t> sink_pos;
        std::uint8_t ends;
        char* alignment;
        std::vector<opt_elem> tags;

        edge_elem(char** tokens, int token_count, int*& token_sizes){

        }
    };
    struct group_elem{
        char* group_id;
        bool ordered = true;
        std::uint64_t segment_count = 0;
        char** segment_ids = nullptr;
        vector<opt_elem> tags;

        group_elem(char** tokens, int token_count, int*& token_sizes){
            group_id = tokens[1];
            //for (std::size_t i = 0; i < token_sizes[2]; ++i){
            //    segment_count += (tokens[2][i] == ',');
            //}
            //segment_ids = new char*[segment_count];
            //char** seg_splits;
            //int num_seg_splits;
            int* seg_split_lens;
            pliib::split(tokens[2], ',', segment_ids, segment_count, seg_split_lens);
            ordered = strcmp(tokens[0][0], 'O') == 0;
        }
        ~group_elem(){
            delete [] group_id;
            delete [] segment_ids;
        }
    };
    bool parse_line(char*& line){
        char** splits;
        int num_splits;
        int* split_sizes;

        pliib::split(line, '\t', splits, num_splits, split_sizes);
    };


    enum GFA_LINE_TYPES {HEADER_LINE,SEGMENT_LINE,FRAGMENT_LINE,EDGE_LINE,GAP_LINE,GROUP_LINE,PATH_LINE,LINK_LINE,CONTAINED_LINE,WALK_LINE};

    static inline int determine_line_type(const char* line){
        if (line[0] == 'H'){
            return HEADER_LINE;
        }
        else if (line[0] == 'S'){
            return SEGMENT_LINE;
        }
        else if (line[0] == 'E'){
            return EDGE_LINE;
        }
        else if (line[0] == 'L'){
            return LINK_LINE;
        }
        else if (line[0] == 'C'){
            return CONTAINED_LINE;
        }
        else if (line[0] == 'U' || line[0] == 'O'){
            return GROUP_LINE;
        }
        else if (line[0] == 'P'){
            return PATH_LINE;
        }
        else if (line[0] == 'W'){
            return WALK_LINE;
        }
        else if (line[0] == 'G'){
            return GAP_LINE;
        }
        else if (line[0] == 'F'){
            return FRAGMENT_LINE;
        }
        else{
            return -1;
        }
    };


    inline bool parse_gfa_file(const char* filename,
            std::function<bool(tgfa::sequence_elem&)> seq_func,
            std::function<bool(tgfa::edge_elem&)> edge_func,
            std::function<bool(tgfa::group_elem&)> group_func){

    }

    inline bool parse_gfa_file(std::ifstream instream,
            std::function<bool(tgfa::sequence_elem&)> seq_func,
            std::function<bool(tgfa::edge_elem&)> edge_func,
            std::function<bool(tgfa::group_elem&)> group_func){

    }

}
