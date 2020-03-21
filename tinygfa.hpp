#ifndef TGFA_H_DEF
#define TGFA_H_DEF
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
#include <functional>

#include <pliib.hpp>

namespace tgfa{

    enum val_types {STRING_TYPE, INT_TYPE, FLOAT_TYPE};
    struct opt_elem{
        char* opt_id = nullptr;
        std::uint8_t type;
        char* val = nullptr;
        void clear(){
            if (opt_id != nullptr)
                delete [] opt_id;
            if (val != nullptr)
                delete [] val;
        }
        
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
        char* seq_id = nullptr;
        char* seq = nullptr;
        std::uint32_t seq_length;
        std::vector<opt_elem> tags;
        void clear(){
            if (seq_id != nullptr)
                delete [] seq_id;
            if (seq != nullptr)
                delete [] seq;
        }
        void set(char** splits, std::size_t split_count, std::size_t* split_lens, int spec = 2){
            clear();
            seq_id = splits[1];
            if (spec == 1){

            }
            else if (spec == 2){

            }
            for (std::size_t i = 3; i < split_count; ++i){
            }


        }
        sequence_elem(char** splits, std::size_t split_count, std::size_t* split_lens){
            set(splits, split_count, split_lens);
        }
        ~sequence_elem(){
            clear();
        }

        inline std::ostream& output(std::ostream& os, int spec = 2){
            if (spec == 2){
                os << 'S' << '\t' << seq_id << '\t' << seq_length << '\t' << seq << '\t';
            }
            else if (spec == 1){
                os << 'S' << '\t' << seq_id  << '\t' << seq << '\t';

            }
            else {
                std::cerr << "Invalid spec: " << spec << ". Please provide a valid spec (1 or 2)" << std::endl;
                exit(9);
            }
            return os;
        }

        inline std::string to_string(int spec = 2){
            std::stringstream st;
            output(st, spec);
            return st.str();
        }
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

        edge_elem(char**& tokens, std::size_t& token_count, std::size_t*& token_sizes){

        }
    };
    struct group_elem{
        char* group_id;
        bool ordered = true;
        std::uint64_t segment_count;
        char** segment_ids = nullptr;
        std::vector<opt_elem> tags;

        group_elem(char**& tokens, std::size_t& token_count, std::size_t*& token_sizes){
            group_id = tokens[1];
            std::size_t num_seg_splits;
            std::size_t* seg_split_lens;
            pliib::split(tokens[2], ',', segment_ids, num_seg_splits, seg_split_lens);
            ordered = tokens[0][0] == 'O';
            segment_count = num_seg_splits;
        }
        ~group_elem(){
            delete [] group_id;
            delete [] segment_ids;
        }
    };


    enum GFA_LINE_TYPES {HEADER_LINE,SEQUENCE_LINE,FRAGMENT_LINE,EDGE_LINE,GAP_LINE,GROUP_LINE,PATH_LINE,LINK_LINE,CONTAINED_LINE,WALK_LINE};

    static inline int determine_line_type(const char* line){
        if (line[0] == 'H'){
            return HEADER_LINE;
        }
        else if (line[0] == 'S'){
            return SEQUENCE_LINE;
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

    struct gfa_stat_t{
            std::uint64_t sequence_count = 0;
            std::uint64_t edge_count = 0;
            std::uint64_t group_count = 0;
            std::uint64_t path_count = 0;
    };

    // Cheers to pfultz2.com/blog, 2014 09 02
    template<typename T>
    typename std::remove_reference<T>::type *addr(T &&t){
        return &t;
    }
    const constexpr auto null_func = true ? nullptr : addr([](auto x){});


    inline bool parse_gfa_file(const char* filename,
            auto& seq_func,
            auto& edge_func,
            auto& group_func,
            gfa_stat_t& stats){


        std::ifstream instream;
        instream.open(filename, std::ifstream::in);

        if (!instream.good()){
            std::cerr << "Error: file " << filename << " could not be properly opened." << std::endl;
            exit(9);
        }

        return parse_gfa_file(instream, seq_func, edge_func, group_func, stats);


    }

    inline bool parse_gfa_file(std::ifstream& instream,
            auto& seq_func,
            auto& edge_func,
            auto& group_func,
            gfa_stat_t& stats){

            bool ret = true;

            if (!instream.good()){
                std::cerr << "Error: input stream failure." << std::endl;
                exit(9);
            }

            std::streamsize max_ln_size = 250000;
            char* line = new char[max_ln_size];
            char** splits;
            std::size_t split_count;
            std::size_t* split_lens;
            while(instream.getline(line, max_ln_size)){
                auto line_type = determine_line_type(line);
                if (line_type == SEQUENCE_LINE){
                    pliib::split(line, '\t', splits, split_count, split_lens);
                        sequence_elem s (splits, split_count, split_lens);
                        seq_func(s);
                        pliib::destroy_splits(splits, split_count, split_lens);
                    ++stats.sequence_count;
                }
                else if (line_type == EDGE_LINE || line_type == LINK_LINE || line_type == CONTAINED_LINE){
                    //if (edge_func != tgfa_empty_func){
                        pliib::split(line, '\t', splits, split_count, split_lens);
                        edge_elem e(splits, split_count, split_lens);
                        edge_func(e);
                        pliib::destroy_splits(splits, split_count, split_lens);
                    //}
                    ++stats.edge_count;
                }
                else if (line_type == GROUP_LINE){
                    //if (group_func != tgfa_empty_func){
                        pliib::split(line, '\t', splits, split_count, split_lens);
                        group_elem g(splits, split_count, split_lens);
                        group_func(g);
                        pliib::destroy_splits(splits, split_count, split_lens);
                    //}
                    ++stats.group_count;
                }
            }
            delete [] line;

            return ret;

    }

}
#endif
