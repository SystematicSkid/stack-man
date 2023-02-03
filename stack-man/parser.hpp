#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>

class vm_parser
{
public:
	vm_parser( ) = default;
	~vm_parser( ) = default;
	
public:
	bool parse_file( const char* file, std::uintptr_t* program );

	void reset( );
	void display_errors( );
private:
	std::ifstream stream;
	std::int32_t current_line;
	std::uint32_t current_program_line;
	bool has_error;
	bool requires_second_pass;
	bool finished_first_pass;
	std::vector<std::string> errors;
	std::unordered_map<std::string, std::int32_t> symbol_refs;
	std::unordered_map<std::string, std::int32_t> symbol_table;

private:
	std::vector<std::string> get_tokens( std::string line );
	bool handle_label( std::string line );
	std::size_t parse_constant( std::string constant );
	std::uint8_t parse_instruction( std::string instruction );
	std::uint8_t parse_register( std::string reg, bool no_except = true );
	std::size_t parse_label( std::string label );
};