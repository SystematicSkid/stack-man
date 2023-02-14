#pragma once
#ifdef _PARSER
#include <optional>
#include <vector>
#include <string>

class instruction_token
{
public:
	instruction_token() = default;
	~instruction_token() = default;

public:
	std::uint8_t instruction = 0xFF;
	std::size_t location;
	std::optional<std::size_t> constant;
	std::optional<std::uint8_t> reg;
	std::optional<std::string> label;

public:
	std::vector<std::uint8_t> get_data( );
	const bool has_instruction( );
	const bool has_constant( );
	const bool has_register( ); 
	const bool has_label( );
	const std::size_t get_size( );
	std::string to_string( );
};

class label_token : public instruction_token
{
public:
	label_token() = default;
	~label_token() = default;
public:
	std::string name;
	instruction_token* target;

	std::string to_string( );
};
#endif _PARSER