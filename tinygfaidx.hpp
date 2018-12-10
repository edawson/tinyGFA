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



/**
 *  Defines a FAI-style index for GFA files
 *  Stores all GFA entities in a big map
 *  where the identifiers are prefixed with the line type
 *  (i.e. s, e, g, f, p, etc.) and an underscore.
 */

namespace TINYGFAIDX{

    enum LINETYPES {SEQUENCE = 0, EDGE = 1, PATH = 3, GAP = 4, FRAGMENT = 5, HEADER = 6, UNORDERED_GROUP = 7, ORDERED_GROUP = 8};
    const static char LINEPREFIX [8] = { 's', 'e', 'p', 'g', 'f', 'h', 'u', 'o'};
    const static unordered_map<char, int> lineTypeToInt= {
        std::make_pair('S',  SEQUENCE),
        std::make_pair('E', EDGE),
        std::make_pair('G', GAP),
        std::make_pair('P', PATH),
        std::make_pair('F', FRAGMENT),
        std::make_pair('H', HEADER),
        std::make_pair('U', UNORDERED_GROUP),
        std::make_pair('O', ORDERED_GROUP)
    };
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
    int type = 0;
    char* iden = NULL;
    int iden_len = 0;
    int64_t offset = -1;
    int32_t line_char_len = -1;

    tiny_gfaidx_base_entry_t(){
        type = 0;
        iden = NULL;
        offset = -1;
        line_char_len = -1;
    };
    tiny_gfaidx_base_entry_t(std::vector<std::string> splits){
        assert(splits.size() == 4);
        this->type = std::stoi(splits[0]);
        this->iden_len = splits[1].length();
        this->iden = new char[this->iden_len + 1];
        memcpy(this->iden, splits[1].c_str(), splits[1].length() * sizeof(char));
        this->iden[this->iden_len] = '\0';
        this->offset = std::stoull(splits[2]);
        this->line_char_len = std::stoi(splits[3]);
    };

    inline std::string to_string(){
        stringstream st;
        st << type << '\t' << iden << '\t' << offset << '\t' << line_char_len << endl;
    };

    void write_to_stream(std::ostream& os){
        os << type << '\t' << iden << '\t' << offset << '\t' << line_char_len << endl;
    };

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
    std::map<char*, tiny_gfaidx_base_entry_t*, custom_char_comparator> iden_to_entry;
    FILE* gfa = NULL;
    uint64_t edge_counter = -1;
    uint64_t node_counter = -1;


    void close(){
        if (gfa != NULL){
            fclose(gfa);
        }
        for (auto k : iden_to_entry){
            //delete [] k.second->iden;
            //delete k.second;
        }
    };


    ~tiny_gfaidx_t(){
        close();
    };

    // Strip off the first character of an identifier (its type)
    char* getRawID( const char* iden, int len){
        char* id = new char[len];
        for (int i = 1; i < len; ++i){
            id[i - 1] = iden[i];
        }
        id[len - 1] = '\n';
        return id;
    };

    char* getTypeID(const char* iden, int len, int type) const{
        char* id = new char(len + 1);
        id[0] = LINEPREFIX[type];
        for (int i = 1; i < len + 1; ++i){
            id[i] = iden[i - 1];
        }
        id[len + 1 - 1] = '\n';
        return id;
    };

    void add(tiny_gfaidx_base_entry_t*& entry){
        iden_to_entry[ getTypeID(entry->iden, entry->iden_len, entry->type)] = entry;
    };

    bool hasBaseID(const char* s) const {
        return (iden_to_entry.count( (char*) s ) != 0);
    };

    bool hasSequenceID(const char* s) const {
        return hasBaseID(getTypeID( (char*) s, strlen(s), SEQUENCE));   
    };

    void get(const char* iden, tiny_gfaidx_base_entry_t*& entry) const {
        entry = iden_to_entry.at( (char*) iden);
    };

    void getSequenceEntry(const char* iden, tiny_gfaidx_base_entry_t*& entry) const{
        entry = iden_to_entry.at( getTypeID(iden, strlen(iden), SEQUENCE) );
    };

    void write(ostream& os) const {
        std::vector<tiny_gfaidx_base_entry_t*> sorted_entries;
        for (auto x : iden_to_entry){
            sorted_entries.push_back(x.second);
        }
        std::sort(sorted_entries.begin(), sorted_entries.end(), custom_gfaidx_entry_t_comparator());
        for (auto x : sorted_entries){
            x->write_to_stream(os);
        }
    };

    void write(const char* filename) const{
        std::ofstream ofi;
        ofi.open(filename);
        if (ofi.good()){
            write(ofi);
        }
    };

} ;


inline void createGFAIDX(const char* gfaName, tiny_gfaidx_t& gfai){
    uint64_t line_number = 0;
    uint64_t base_seq_id = 0;
    uint64_t base_edge_id = 0;
    uint64_t offset = 0;
    size_t line_length = 0;

    std::string line;
    std::ifstream gfaFile;
    gfaFile.open(gfaName);

    std::vector<tuple<uint64_t, uint64_t, uint32_t>> edge_cache;
    std::vector<tuple<uint64_t, uint64_t, uint32_t>> path_cache;
    std::vector<tuple<uint64_t, uint64_t, uint32_t>> ordered_group_cache;
    std::vector<tuple<uint64_t, uint64_t, uint32_t>> unordered_group_cache;

    if (!(gfai.gfa = fopen(gfaName, "r"))){
        cerr << "Error: couldn't open GFA file " << gfaName << endl;
        exit(1);
    }
   
    tiny_gfaidx_base_entry_t* entry = new tiny_gfaidx_base_entry_t();

    if (gfaFile.is_open()){
        while(std::getline(gfaFile, line)){
            line_length = line.length();
            if (line[0] == 'S'){
                vector<string> splits = pliib::split(line, '\t');
                entry->type = SEQUENCE;
                entry->iden_len = splits[1].length();
                entry->iden = new char[ entry->iden_len + 1 ];
                std::strcpy(entry->iden, splits[1].c_str());
                entry->iden[entry->iden_len] = '\0';
                entry->offset = offset;
                entry->line_char_len = line_length;
                gfai.add(entry);
                entry = new tiny_gfaidx_base_entry_t();
            }
            
            offset += line_length + 1;
        }
    }

    gfaFile.close();

};

inline void writeGFAIDX(const char* gfaName, const tiny_gfaidx_t& gfai ){
    std::string gfn(gfaName);
    gfn = gfn + ".gfai";
    std::ofstream ofi( gfn.c_str() );
    if (ofi.good()){
        gfai.write(ofi);
    }
};

inline bool checkGFAIDXFileExists(const char* fileName){
    struct stat statFileInfo;
    string indexFileName(fileName);
    indexFileName = indexFileName + ".gfai";
    return stat(indexFileName.c_str(), &statFileInfo) == 0;
};

inline char* indexFileName(const char* fileName){
    int len = strlen(fileName);
    static const char* file_ext = ".gfai";
    char* ret = new char[len + 6];
    ret[len + 4] = '\0';
    strcpy(ret, fileName);
    strcpy(ret + len, file_ext);
    return ret;
};

inline void parseGFAIDX(const char* GFAFileName, tiny_gfaidx_t& gfaidx){
    std::ifstream ifi;
    char* ifn = indexFileName(GFAFileName);
    ifi.open( (const char*) ifn);

    if (!(gfaidx.gfa = fopen(GFAFileName, "r"))){
        cerr << "Error: couldn't open gfa file " << GFAFileName << endl;
    }

    if (ifi.is_open()){
        std::string line;
        while(std::getline(ifi, line)){
            if (line[0] == 'S'){
                vector<string> splits = pliib::split(line.c_str(), '\t');
                tiny_gfaidx_base_entry_t* t = new tiny_gfaidx_base_entry_t(splits);
                gfaidx.add(t);
            }
        }
    }
    else{
        cerr << "Couldn't open index " << GFAFileName << "." << endl;
    }
    
    ifi.close();
    delete [] ifn;
};

inline void getSequence( const tiny_gfaidx_t& gfai, const char* seqname, char*& seq){
    
    tiny_gfaidx_base_entry_t* entry;
    if (gfai.hasSequenceID(seqname)){
        gfai.getSequenceEntry(seqname, entry);
        uint32_t sz = entry->line_char_len;
        seq = new char[sz + 1];
        fseek64(gfai.gfa, entry->offset, SEEK_SET);
        if (fread(seq, sizeof(char), sz, gfai.gfa)){
           seq[sz] = '\0';
        }

    }
    else{
        cerr << "No sequence ID " << seqname << " found." << endl;
    }
};


}

#endif
