#pragma once
#include <optional>
#include <vector>

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
	bool has_instruction( );
	bool has_constant( );
	bool has_register( ); 
};