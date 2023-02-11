#pragma once
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

public:
	std::vector<std::uint8_t> get_data( );
	const bool has_instruction( );
	const bool has_constant( );
	const bool has_register( ); 
	const std::size_t get_size( );
	std::string to_string( );
};