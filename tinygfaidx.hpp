#ifndef TINY_GFAIDX_HPP
#define TINY_GFAIDX_HPP

#include <sys/types.h>
#include <cstdio>

#pragma once
#define _FILE_OFFSET_BITS 64
#ifdef WIN32
#define ftell64(a)     _ftelli64(a)
#define fseek64(a,b,c) _fseeki64(a,b,c)
typedef __int64 off_type;
#elif defined(__APPLE__) || defined(__FreeBSD__)
#define ftell64(a)     ftello(a)
#define fseek64(a,b,c) fseeko(a,b,c)
typedef off_t off_type;
#else
#define ftell64(a)     ftello(a)
#define fseek64(a,b,c) fseeko(a,b,c)
typedef __off64_t off_type;
#endif


#include <string>
#include <sstream>
#include <ostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <map>
#include <cstdint>
#include <assert.h>
#include <algorithm>
#include <sys/stat.h>
#include "pliib.hpp"
#include "tinygfa.hpp"



/**
 *  Defines a FAI-style index for GFA files
 *  Stores all GFA entities in a big map
 *  where the identifiers are prefixed with the line type
 *  (i.e. s, e, g, f, p, etc.) and an underscore.
 */

namespace TINYGFAIDX{
    
    double GLOBAL_TGFI_SPEC = 2.0;

    // From https://stackoverflow.com/questions/4157687/using-char-as-a-key-in-stdmap
    struct custom_char_comparator
    {
        bool operator()(char const *a, char const *b) const
        {
            return std::strcmp(a, b) < 0;
        }
    };
    /**
     * Base line type. Defines a sufficient amount of information
     * to build a stable index.
     */
    struct tiny_gfaidx_base_entry_t{
        char type = '\0';
        char* iden = nullptr;
        char* edge_source = nullptr;
        char* edge_sink = nullptr;
        bool from_end = true;
        bool to_start = true;
        int iden_len = 0;
        std::size_t offset = -1;
        // Length of the line (character count), excluding the NULL char
        std::size_t line_char_len = -1;
        std::size_t unknown_id = 0;

        tiny_gfaidx_base_entry_t(){
        }
        tiny_gfaidx_base_entry_t(std::string& line){

        }
        tiny_gfaidx_base_entry_t(const char* line, std::size_t offset, std::size_t line_len){
            this->type = line[0];
            std::size_t ind = 2;
            while (line[ind] != '\t'){
                ++ind;
            }
            pliib::substr(line, 2, ind+1, this->iden);
            if (this->type == 'E' || this->type == 'L'){
                    char* mutable_line = const_cast<char*>(line);
                    tgfa::edge_elem e(mutable_line);
                    pliib::strcopy(e.source_id, edge_source);
                    pliib::strcopy(e.sink_id, edge_sink);
                    this->from_end = e.source_orientation_forward;
                    this->to_start = e.sink_orientation_forward;
                    pliib::strcopy(make_edge_id().c_str(), iden);
            }
            this->iden_len = ind-2;
            this->offset = offset;
            this->line_char_len = line_len;
        }
        std::string make_edge_id(){
            std::ostringstream st;
            st << edge_source << ";" <<
                (from_end ? "+" : "-") << ";" <<
                edge_sink << ";" << (to_start ? "+" : "-");
            return st.str();
        }

        inline std::string to_string(){
            std::ostringstream st;
            write_to_stream(st);
            return st.str();
        }

        std::ostream& write_to_stream(std::ostream& os){
            os << type << '\t' << iden << '\t';
            if (type == 'E' || type == 'L'){
                os << make_edge_id() << '\t';
            }
            os << offset << '\t' << line_char_len;
            return os;
        }

    };

    struct custom_gfaidx_entry_t_comparator
    {
        bool operator()(tiny_gfaidx_base_entry_t const *a, tiny_gfaidx_base_entry_t const *b) const
        {
            return a->offset < b->offset;
        }
    };

    struct custom_base_entry_comparator
    {
        bool operator()(tiny_gfaidx_base_entry_t const * a, tiny_gfaidx_base_entry_t const * b){
            return std::strcmp(a->iden, b->iden) < 0;
        }
    };

    struct tiny_gfaidx_t{
        std::map<std::string, tiny_gfaidx_base_entry_t*> header_map;
        std::map<std::string, tiny_gfaidx_base_entry_t*> seq_map;
        std::map<std::string, tiny_gfaidx_base_entry_t*> edge_map;
        std::map<std::string, tiny_gfaidx_base_entry_t*> group_map;
        FILE* gfa = NULL;
        std::uint64_t edge_counter = 0;
        std::uint64_t node_counter = 0;
        std::uint64_t group_counter = 0;

        void close(){
            if (gfa != NULL){
                fclose(gfa);
            }
        };

        ~tiny_gfaidx_t(){
            close();
        };
        void add(tiny_gfaidx_base_entry_t*& entry){
            if (entry->type == 'S'){
                seq_map[entry->iden] = entry;
            }
            else if (entry->type == 'H'){
                //header_map[entry->iden] = entry;
            }
            else if (entry->type == 'E'){
                edge_map[entry->iden] = entry;
            }
            else if (entry->type == 'L'){
                edge_map[entry->iden] = entry;
            }
            else if (entry->type == 'U'){
                group_map[entry->iden] = entry;
            }
            else if (entry->type == 'O'){
                group_map[entry->iden] = entry;
            }
            else if (entry->type == 'P'){
                group_map[entry->iden] = entry;
            }
        };

        void get_sequence_by_id(std::string sequence_name, tgfa::sequence_elem& seq){
            char* sname = const_cast<char*>(sequence_name.c_str());
            if (has_sequence(sname)){
                tiny_gfaidx_base_entry_t* entry = seq_map.at(sname);
                char* seqln = new char[entry->line_char_len + 1];
                fseek64(gfa, entry->line_char_len, SEEK_SET);
                if (fread(seqln, sizeof(char), entry->line_char_len, gfa)){
                    seq.clear();
                    seq = tgfa::sequence_elem(seqln, GLOBAL_TGFI_SPEC);
                }
            }
        }
        void get_edge_by_id(std::string edge_name, tgfa::edge_elem& edge){
            if (has_edge(edge_name)){
                tiny_gfaidx_base_entry_t* entry = edge_map.at(edge_name);
                char* edgeln = new char[entry->line_char_len + 1];
                fseek64(gfa, entry->line_char_len, SEEK_SET);
                if (fread(edgeln, sizeof(char), entry->line_char_len, gfa)){
                    edge = tgfa::edge_elem(edgeln);
                }
            }
        }
        void get_edge_by_source(std::string source_name, tgfa::edge_elem& edge){
            std::cerr << "Not implemented [ get_edge_by_source ]" << std::endl;
            exit(9);
        }
        void get_edge_by_sink(std::string sink_name, tgfa::edge_elem& edge){
            std::cerr << "Not implemented [ get_edge_by_sink ]" << std::endl;
            exit(9); 
        }
        void get_edge_by_link(std::string source_name, bool from_end,
                std::string sink_name, bool to_start,
                tgfa::edge_elem& edge){
            std::cerr << "Not implemented [ get_edge_by_link ]" << std::endl;
            exit(9);
        }
        void get_group_by_id(std::string group_name, tgfa::group_elem& group){
            if (has_group(group_name)){
                tiny_gfaidx_base_entry_t* entry = group_map.at(group_name);
                char* groupln = new char[entry->line_char_len + 1];
                fseek64(gfa, entry->line_char_len, SEEK_SET);
                if (fread(groupln, sizeof(char), entry->line_char_len, gfa)){
                    group = tgfa::group_elem(groupln);
                }
            }
        }

        bool has_sequence(const std::string& sequence_name) const {
            return has_sequence(sequence_name.c_str());
        }
        bool has_sequence(const char* sequence_name) const {
            return seq_map.find(sequence_name) != seq_map.end(); 
        }

        bool has_edge(const std::string& edge_name) const {
            return has_edge(edge_name.c_str());
        }
        bool has_edge(const char* edge_name) const {
            return edge_map.find(edge_name) != edge_map.end();
        }

        bool has_group(const std::string& group_name) const {
            return has_group(group_name.c_str());
        }
        bool has_group(const char* group_name) const {
            return group_map.find(group_name) != group_map.end();
        }

        std::ostream& write(std::ostream& os) const {
            for (auto& h : header_map){
                (h.second)->write_to_stream(os);
                os << std::endl;
            }
            for (auto& s : seq_map){
                (s.second)->write_to_stream(os);
                os << std::endl;
            }
            for (auto& e : edge_map){
                (e.second)->write_to_stream(os);
                os << std::endl;
            }
            for (auto& g : group_map){
                (g.second)->write_to_stream(os);
                os << std::endl;
            }
            return os;
        }

        void write(const char* filename) const{
            std::ofstream ofi;
            ofi.open(filename);
            if (ofi.good()){
                write(ofi);
            }
        };

    };


    inline void create_gfa_index(const char* gfaName, tiny_gfaidx_t& gfai){
        std::uint64_t line_number = 0;
        std::uint64_t base_seq_id = 0;
        std::uint64_t base_edge_id = 0;
        std::uint64_t offset = 0;
        std::size_t line_length = 0;

        std::ifstream gfaFile;
        gfaFile.open(gfaName);


        if (!(gfai.gfa = fopen(gfaName, "r"))){
            std::cerr << "Error: couldn't open GFA file " << gfaName << std::endl;
            exit(1);
        }

        //tiny_gfaidx_base_entry_t* entry = new tiny_gfaidx_base_entry_t();

        std::string line;
        if (gfaFile.is_open()){
            while(std::getline(gfaFile, line)){
	            line_length = line.length();
                if (line[0] == 'S' || line[0] == 'H' ||
                        line[0] == 'E' || line[0] == 'L' ||
                        line[0] == 'P' || line[0] == 'O' ||
                        line[0] == 'U'){
                    tiny_gfaidx_base_entry_t* entry = new tiny_gfaidx_base_entry_t(line.c_str(), offset, line_length);

                    gfai.add(entry);
                }
                //int line_type = tgfa::determine_line_type(line.c_str());
                offset += line_length + 1;
            }
        }

        gfaFile.close();

    };

    inline void write_gfa_index(const char* gfaName, const tiny_gfaidx_t& gfai ){
        std::string gfn(gfaName);
        gfn = gfn + ".gfai";
        std::ofstream ofi( gfn.c_str() );
        if (ofi.good()){
            gfai.write(ofi);
        }
    };

    inline bool has_gfa_index(const char* fileName){
        struct stat statFileInfo;
        std::string indexFileName(fileName);
        indexFileName = indexFileName + ".gfai";
        return stat(indexFileName.c_str(), &statFileInfo) == 0;
    };

    inline char* get_index_file_name(const char* fileName){
        int len = strlen(fileName);
        static const char* file_ext = ".gfai";
        char* ret = new char[len + 6];
        ret[len + 4] = '\0';
        strcpy(ret, fileName);
        strcpy(ret + len, file_ext);
        return ret;
    };

    inline void parse_gfa_index(const char* GFAFileName, tiny_gfaidx_t& gfaidx){
        std::ifstream ifi;
        char* ifn = get_index_file_name(GFAFileName);
        ifi.open( (const char*) ifn);

        if (!(gfaidx.gfa = fopen(GFAFileName, "r"))){
            std::cerr << "Error: couldn't open gfa file " << GFAFileName << std::endl;
        }

        if (ifi.is_open()){
            std::string line;
            while(std::getline(ifi, line)){
                    tiny_gfaidx_base_entry_t* t = new tiny_gfaidx_base_entry_t(line);
                    gfaidx.add(t);
            }
        }
        else{
            std::cerr << "Couldn't open index " << GFAFileName << "." << std::endl;
        }

        ifi.close();
        delete [] ifn;
    };

}

#endif
