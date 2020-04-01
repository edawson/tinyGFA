#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "pliib.hpp"
#include "tinygfa.hpp"
#include <fstream>
#include <string>
#include <algorithm>
#include <cstdint>

using namespace std;

TEST_CASE("GFA opt_elems are properly produced and formatted", "[opt_elem"){
	
	tgfa::opt_elem all_null;
	REQUIRE(all_null.tag == nullptr);
	REQUIRE(all_null.type == tgfa::NULL_VAL_TYPE);
	REQUIRE(all_null.val == nullptr);
	
	string opt_string = "SN:Z:ggg";
	char* opt_string_raw;
	pliib::strcopy(opt_string.c_str(), opt_string_raw);
	tgfa::opt_elem o(opt_string_raw);
	REQUIRE(o.tag != nullptr);
	REQUIRE(std::strcmp(o.tag, "SN") == 0);
	REQUIRE(o.get_id() == "SN");
	REQUIRE(o.get_type() == tgfa::STRING_VAL_TYPE);
	REQUIRE(o.get_val() == "ggg");
}


TEST_CASE("GFA 1.0 sequence element parsing produces valid sequence elements.", "[sequence_elem]"){

    string gfa_line = "S\t64\tACCTGGC";
	char* gfa_line_raw;
	pliib::strcopy(gfa_line.c_str(), gfa_line_raw);
	tgfa::sequence_elem s(gfa_line_raw, 1.0);
	
	REQUIRE(s.seq_length == 7);
	REQUIRE(std::strcmp(s.seq, "ACCTGGC") == 0);
	REQUIRE(std::strcmp(s.id, "64") == 0);
	delete [] gfa_line_raw;
}

TEST_CASE("GFA 1.0 sequence element parsing handles tags.", "[sequence_elem tags"){
	string gfa_line = "S\t64\tACCTGGC\tLN:i:7\tSN:Z:1\tSO:i:10\tSR:i:2";
	char* gfa_line_raw;
	pliib::strcopy(gfa_line.c_str(), gfa_line_raw);
	tgfa::sequence_elem s(gfa_line_raw, 1.0);
	REQUIRE(s.num_tags() == 4);
	REQUIRE(s.get_tag("SN").get_val() == "1");
	REQUIRE(s.get_tag("SN").get_type() == tgfa::STRING_VAL_TYPE);
	REQUIRE(s.get_tag("SR").get_type() == tgfa::SIGNED_INT_VAL_TYPE);
	delete [] gfa_line_raw;
}

TEST_CASE("GFA 2.0 sequence element parsing produces valid sequence elements.", "[sequence_elem]"){

}


TEST_CASE("Sequence elements can have both numeric and alphanumeric IDs.", "[sequence_elem]"){

}


TEST_CASE("Link elements are parsed correctly into edges", "[edge_elem]"){

}


TEST_CASE("GFA 1.0 paths are parsed into groups correctly", "[group_elem]"){

}






