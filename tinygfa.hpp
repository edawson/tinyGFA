#ifndef TGFA_H_DEF
#define TGFA_H_DEF
#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
#include <functional>
#include <assert.h>

#include <pliib.hpp>


namespace tgfa{



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

    static const char VAL_TYPE_CHARS [7] = {'A', 'i', 'f', 'Z', 'J', 'H', 'B'};


    enum val_types {CHAR_VAL_TYPE,
        SIGNED_INT_VAL_TYPE,
        SINGLE_PREC_FLOAT_VAL_TYPE,
        STRING_VAL_TYPE,
        JSON_VAL_TYPE,
        BYTE_ARRAY_VAL_TYPE,
        NUMERIC_ARRAY_VAL_TYPE,
        NULL_VAL_TYPE,
        CUSTOM_VAL_TYPE};
    struct opt_elem{
        char* tag = nullptr;
        std::uint8_t type = NULL_VAL_TYPE;
        char* val = nullptr;
        void clear(){
            if (tag != nullptr)
                delete tag;
            if (val != nullptr)
                delete val;
        }

        ~opt_elem(){
            //clear();
        }

        void set(char**& splits, const std::size_t& split_count, std::size_t*& split_lens){
            assert(split_count == 3);
            pliib::strcopy(reinterpret_cast<const char*>(splits[0]), this->tag);
            switch(splits[1][0]){
                case 'A':
                    type = CHAR_VAL_TYPE;
                    break;
                case 'i':
                    type = SIGNED_INT_VAL_TYPE;
                    break;
                case 'f':
                    type = SINGLE_PREC_FLOAT_VAL_TYPE;
                    break;
                case 'Z':
                    type = STRING_VAL_TYPE;
                    break;
                case 'J':
                    type = JSON_VAL_TYPE;
                    break;
                case 'H':
                    type = BYTE_ARRAY_VAL_TYPE;
                    break;
                case 'B':
                    type = NUMERIC_ARRAY_VAL_TYPE;
                    break;
                default:
                    type = CUSTOM_VAL_TYPE;
                    break;
            };

            pliib::strcopy(reinterpret_cast<const char*>(splits[2]), this->val);
        }
        opt_elem(){

        }
        opt_elem(char**& splits, const std::size_t& split_count, std::size_t*& split_lens){
            set(splits, split_count, split_lens);
        }
        opt_elem(char*& tag_string){
            char** splits = new char*[3];
            std::size_t split_count = 3;
            std::size_t* split_lens = new std::size_t[3];
            std::size_t start = 0;
            std::size_t end = 0;
            while (tag_string[end] != ':'){
                ++end;
            }
            pliib::strcopy(reinterpret_cast<const char*>(tag_string + start), end - start, splits[0]);
            split_lens[0] = end - start;
            start = end+1;
            ++end;
            while (tag_string[end] != ':'){
                ++end;
            }
            pliib::strcopy(reinterpret_cast<const char*>(tag_string + start), end - start, splits[1]);
            split_lens[1] = end - start;
            start = end+1;
            ++end;
            while(tag_string[end] != '\n'){
                ++end;
            }
            pliib::strcopy(reinterpret_cast<const char*>(tag_string + start), end - start, splits[2]);
            split_lens[2] = end - start;

            
            set(splits, split_count, split_lens);
            pliib::destroy_splits(splits, split_count, split_lens);
        }
        
        std::string get_id(){
            return std::string(tag);
        }
        std::string get_val(){
            return std::string(val);
        }
        std::uint8_t get_type(){
            return type;
        }

        std::string to_string(){
            std::stringstream st;
            st << tag << ":" << VAL_TYPE_CHARS[type] << ":" << val;
            return st.str();
        }
        
    };
    struct header_elem{
        char* key = nullptr;
        std::uint8_t type;
        char* val = nullptr;
    };
    struct sequence_elem{
        char* id = nullptr;
        char* seq = nullptr;
        std::uint32_t seq_length;
        std::vector<opt_elem> tags;
        void clear(){
            if (id != nullptr)
                delete [] id;
            if (seq != nullptr)
                delete [] seq;
        }
        void set(char**& splits, std::size_t& split_count, std::size_t*& split_lens, double spec = 2){
            //clear();
            std::size_t index = 1;
            id = splits[index];
            ++index;
            if (spec == 2){
                seq_length = std::stoull(splits[index]);
                ++index;
                seq = splits[index];
                ++index;
            }
            else if (spec == 1){
                seq = splits[index];
                seq_length = strlen(seq);
                ++index;
            }
            while(index < split_count){
                opt_elem o(splits[index]);
                tags.push_back(o);
                ++index;
            }
        }
        sequence_elem(){

        }
        sequence_elem(char*& line, double spec = 2.0){
            char** splits;
            std::size_t split_count;
            std::size_t* split_sizes;
            pliib::split(line, '\t', splits, split_count, split_sizes);
            set(splits, split_count, split_sizes, spec);
        }
        sequence_elem(char**& splits, std::size_t split_count, std::size_t*& split_lens){
            set(splits, split_count, split_lens);
        }
        ~sequence_elem(){
           // clear();
        }

        std::size_t num_tags(){
            return tags.size();
        }

        opt_elem get_tag(const char* tag){
            for (auto o : tags){
                if (std::strcmp(tag, o.get_id().c_str()) == 0){
                    return o;
                }
            }
            std::cerr << "ERROR: invalid tag name: " << tag << "." << std::endl;
            std::cerr << "Valid tags are: ";
            for (auto o : tags){
                std::cerr << o.get_id() << std::endl;
            }
            throw 20;
        }

        opt_elem get_tag(std::string tag){
            return get_tag(tag.c_str());
        }

        inline std::ostream& output(std::ostream& os, double spec = 2){
            if (spec == 2){
                os << 'S' << '\t' << id << '\t' << seq_length << '\t' << seq << '\t';
            }
            else if (spec == 1){
                os << 'S' << '\t' << id  << '\t' << seq << '\t';

            }
            else {
                std::cerr << "Invalid spec: " << spec << ". Please provide a valid spec (1 or 2)" << std::endl;
                exit(9);
            }
            return os;
        }

        inline std::string to_string(double spec = 2){
            std::stringstream st;
            output(st, spec);
            return st.str();
        }
    };

    static const std::uint8_t EDGE_T_SINK_END_OFFSET = 0x1;
    static const std::uint8_t EDGE_T_SINK_START_OFFSET = 0x2;
    static const std::uint8_t EDGE_T_SOURCE_END_OFFSET = 0x3;
    static const std::uint8_t EDGE_T_SOURCE_START_OFFSET = 0x4;
    struct edge_elem{
        char* edge_id = nullptr;
        char* source_id = nullptr;
        char* sink_id = nullptr;
        bool source_orientation_forward;
        bool sink_orientation_forward;
        std::pair<uint32_t, uint32_t> source_pos;
        std::pair<uint32_t, uint32_t> sink_pos;
        std::uint8_t ends = 0x0;
        char* alignment = nullptr;
        std::vector<opt_elem> tags;

        edge_elem(){

        }
        ~edge_elem(){
        }
        edge_elem(char**& tokens, std::size_t& token_count, std::size_t*& token_sizes){
            set(tokens, token_count, token_sizes);
        }

        edge_elem(char*& line){
            char** splits;
            std::size_t split_count;
            std::size_t* split_lengths;
            pliib::split(line, '\t', splits, split_count, split_lengths);
            set(splits, split_count, split_lengths);
        }

        bool sink_end(){
            return ends & EDGE_T_SINK_END_OFFSET;
        };
        bool sink_begin(){
            return ends & EDGE_T_SINK_START_OFFSET;
        };
        bool source_end(){
            return ends & EDGE_T_SOURCE_END_OFFSET;
        };
        bool source_begin(){
            return ends & EDGE_T_SOURCE_START_OFFSET;
        };
        void set_end_begin(std::uint8_t& ends, std::uint8_t offset){
            ends |= offset;
        }
        void unset_end_begin(std::uint8_t& ends, std::uint8_t offset){
            ends &= ~(offset);
        }
        void set(char**& tokens, std::size_t& token_count, std::size_t*& token_sizes){
            if (token_count > 0){
                // Check if the line is a GFA1 L/C line,
                // or a GFA2 E line
                int line_type = determine_line_type(tokens[0]);
                if (line_type == EDGE_LINE){
                    edge_id = tokens[1];
                    source_orientation_forward = tokens[2][token_sizes[2] - 1] == '+';
                    sink_orientation_forward = tokens[3][token_sizes[3] - 1] == '+';
                    pliib::strcopy(tokens[2], source_id);
                    pliib::strip(source_id, token_sizes[2], '+');
                    pliib::strip(source_id, strlen(source_id), '-');
                    pliib::strcopy(tokens[3], sink_id);
                    pliib::strip(sink_id, token_sizes[3], '+');
                    pliib::strip(sink_id, strlen(sink_id), '-');

                    tokens[4][token_sizes[4] - 1] == '$' ? set_end_begin(ends,EDGE_T_SOURCE_START_OFFSET) : unset_end_begin(ends,EDGE_T_SOURCE_START_OFFSET);
                    tokens[5][token_sizes[5] - 1] == '$' ? set_end_begin(ends,EDGE_T_SOURCE_END_OFFSET) : unset_end_begin(ends,EDGE_T_SOURCE_END_OFFSET);
                    tokens[6][token_sizes[6] - 1] == '$' ? set_end_begin(ends,EDGE_T_SINK_START_OFFSET) : unset_end_begin(ends,EDGE_T_SINK_START_OFFSET);
                    tokens[7][token_sizes[7] - 1] == '$' ? set_end_begin(ends,EDGE_T_SINK_END_OFFSET) : unset_end_begin(ends,EDGE_T_SINK_END_OFFSET);

                    alignment = tokens[8];
                    for (std::size_t i = 9; i < token_count; ++i){
                        opt_elem o(tokens[i]);
                        tags.push_back(o);
                    }

                    

                }
                else if(line_type == CONTAINED_LINE){
                    std::cerr << "Containment lines not implemented [tinyfa::edge_elem]" << std::endl;
                    throw 20;
                }
                else if (line_type == LINK_LINE){
                    pliib::strcopy(tokens[1], source_id);
                    source_orientation_forward = *tokens[2] == '+';
                    pliib::strcopy(tokens[3], sink_id);
                    sink_orientation_forward = *tokens[4] == '+';
                    set_end_begin(ends, EDGE_T_SOURCE_START_OFFSET);
                    set_end_begin(ends, EDGE_T_SOURCE_END_OFFSET);
                    set_end_begin(ends, EDGE_T_SINK_START_OFFSET);
                    set_end_begin(ends, EDGE_T_SINK_END_OFFSET);
                    pliib::strcopy(tokens[5], alignment);
                    for (std::size_t i = 6; i < token_count; ++i){
                        opt_elem o(tokens[i]);
                        tags.push_back(o);
                    }
                }
                else{
                    std::cerr << "Invalid line type " << tokens[0] << std::endl;
                    throw 20;
                }
            }
        }

        void clear(){
            delete [] source_id;
            delete [] sink_id;
        }

                std::size_t num_tags(){
            return tags.size();
        }

        opt_elem get_tag(const char* tag){
            for (auto o : tags){
                if (std::strcmp(tag, o.get_id().c_str()) == 0){
                    return o;
                }
            }
            std::cerr << "ERROR: invalid tag name: " << tag << "." << std::endl;
            std::cerr << "Valid tags are: ";
            for (auto o : tags){
                std::cerr << o.get_id() << std::endl;
            }
            throw 20;
        }

        opt_elem get_tag(std::string tag){
            return get_tag(tag.c_str());
        }
    };
    struct group_elem{
        char* id = nullptr;
        bool ordered = true;
        std::uint64_t segment_count = 0;
        char** segment_ids = nullptr;
        bool* segment_orientations = nullptr;
        char** overlaps = nullptr;
        std::size_t overlap_count = 0;
        std::size_t* overlap_lengths = nullptr;
        std::vector<opt_elem> tags;

        group_elem(char**& tokens, std::size_t& token_count, std::size_t*& token_sizes){
            set(tokens, token_count, token_sizes);
        }
        void set(char**& tokens, std::size_t& token_count, std::size_t*& token_sizes){
            id = tokens[1];
            std::size_t num_seg_splits;
            std::size_t* seg_split_lens;
            pliib::split(tokens[2], ',', segment_ids, num_seg_splits, seg_split_lens);
            segment_orientations = new bool [num_seg_splits];
            for (std::size_t i = 0; i < num_seg_splits; ++i){
                segment_orientations[i] = segment_ids[i][seg_split_lens[i] - 1] == '+';
                pliib::slice(segment_ids[i], 0, seg_split_lens[i] - 1, segment_ids[i]);
            }
            ordered = tokens[0][0] == 'O' || tokens[0][0] == 'P';
            segment_count = num_seg_splits;

            if (token_count > 3){
                if (tokens[3][0] != '*'){
                    pliib::split(tokens[3], ',', overlaps, overlap_count, overlap_lengths);
                }
            }
            

        }
        group_elem(char*& line){
            char** splits;
            std::size_t split_count;
            std::size_t* split_lengths;
            pliib::split(line, '\t', splits, split_count, split_lengths);
            set(splits, split_count, split_lengths);
        }
        ~group_elem(){
            //delete group_id;
            //delete segment_ids;
        }
        char* segment_name(const std::uint64_t& index){
            if (index > segment_count){
                std::cerr << "Error: segment index " << index << " out of bounds. Max segment id: " << segment_count << std::endl;
            }
            return segment_ids[index];
        }
        bool segment_orientation(const std::uint64_t& index){
            if (index > segment_count){
                std::cerr << "Error: segment index " << index << " out of bounds. Max segment id: " << segment_count << std::endl;
            }
            return segment_orientations[index];
        }

        std::string overlap_tail(std::size_t index){
            assert(index < overlap_count);
            assert(overlap_count > 0);
            return std::string(overlaps[index]);
        }
        std::string overlap_head(std::size_t index){
            assert(index > 0);
            assert(overlap_count > 0);
            return std::string(overlaps[index - 1]);
        }
    };

    struct gfa_func_t{
        std::vector<std::function<void(sequence_elem)>> seq_funcs;
        std::vector<std::function<void(edge_elem)>> edge_funcs;
        std::vector<std::function<void(group_elem)>> group_funcs;
    };

    struct gfa_stat_t{
            std::uint64_t sequence_count = 0;
            std::uint64_t edge_count = 0;
            std::uint64_t group_count = 0;
            std::uint64_t path_count = 0;
            double N50 = 0.0;
            double N90 = 0.0;
            std::string to_string(){
                std::stringstream st;
                st << "Number of sequences (nodes): " << sequence_count << std::endl <<
                    "Number of edges: " << edge_count << std::endl <<
                    "Number of groups: " << group_count << std::endl <<
                    "Number of paths (ordered groups): " << path_count;
                return st.str();
            }
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
            gfa_stat_t& stats,
            int spec = 2){

        return parse_gfa_file(filename, seq_func, true, edge_func, true, group_func, true, stats, spec);
        
    }

    inline bool parse_gfa_file(const char* filename,
            auto& seq_func,
            bool process_seqs,
            auto& edge_func,
            bool process_edges,
            auto& group_func,
            bool process_groups,
            gfa_stat_t& stats,
            double spec = 2){

        std::ifstream instream;
        instream.open(filename, std::ifstream::in);

        if (!instream.good()){
            std::cerr << "Error: file " << filename << " could not be properly opened." << std::endl;
            exit(9);
        }

        return parse_gfa_file(instream, seq_func, process_seqs, edge_func, process_edges, group_func, process_groups, stats, spec);


    }
    
    inline void parse_gfa_file(std::istream& instream,
            std::function<void(sequence_elem&)> seqfunc,
            std::function<void(edge_elem&)> edge_func,
            std::function<void(group_elem&)> group_func,
            double spec = 1.0){
        
         if (!instream.good()){
                std::cerr << "Error: input stream failure." << std::endl;
                exit(9);
            }
        parse_gfa_file(static_cast<std::ifstream&>(instream), seqfunc, edge_func, group_func,1);
    }



    inline bool parse_gfa_file(std::ifstream& instream,
            auto& seq_func,
            auto& edge_func,
            auto& group_func,
            gfa_stat_t& stats,
            double spec = 2.0){
            return parse_gfa_file(instream, seq_func, true, edge_func, true, group_func, true, stats, spec);
    }
    inline bool parse_gfa_file(std::ifstream& instream,
            auto& seq_func,
            bool process_seqs,
            auto& edge_func,
            bool process_edges,
            auto& group_func,
            bool process_groups,
            gfa_stat_t& stats,
            double spec = 2.0){

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

            sequence_elem s;
            edge_elem e;
            //group_elem g;
            while(instream.getline(line, max_ln_size)){
                auto line_type = determine_line_type(line);
                if (line_type == SEQUENCE_LINE){
                    if (process_seqs){
                        pliib::split(line, '\t', splits, split_count, split_lens);
                        s.set(splits, split_count, split_lens, spec);
                        seq_func(s);
                        pliib::destroy_splits(splits, split_count, split_lens);
                        ++stats.sequence_count;
                    }
                }
                else if (line_type == EDGE_LINE || line_type == LINK_LINE || line_type == CONTAINED_LINE){
                    if (process_edges){
                        pliib::split(line, '\t', splits, split_count, split_lens);
                        e.set(splits, split_count, split_lens);
                        edge_func(e);
                        e.clear();
                        pliib::destroy_splits(splits, split_count, split_lens);
                    }
                    ++stats.edge_count;
                }
                else if (line_type == GROUP_LINE || line_type == PATH_LINE){
                    if (process_groups){
                        pliib::split(line, '\t', splits, split_count, split_lens);
                        group_elem g(splits, split_count, split_lens);
                        group_func(g);
                        pliib::destroy_splits(splits, split_count, split_lens);
                        ++stats.group_count;
                        if (g.ordered){
                            ++stats.path_count;
                        }
                    }
                }
            }
            delete [] line;

            return ret;
    }
}
#endif
