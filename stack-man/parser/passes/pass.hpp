#pragma once
#include <vector>
#include "../instruction_token.hpp"

class pass
{
public:
	pass () = default;
	~pass( ) = default;
public:
	virtual bool init( ) = 0;
	virtual bool run( std::vector<instruction_token*>& instructions ) = 0;
public:
	bool insert_before( std::vector<instruction_token>& instructions, std::size_t index, instruction_token token );
	bool insert_after( std::vector<instruction_token*>& instructions, instruction_token* point, instruction_token* token );
	bool replace_instruction( std::vector<instruction_token*>& instructions, instruction_token* point, std::vector<instruction_token*> tokens );
};

/* Passes */
#include "obfuscate_constant.hpp"
#include "obfuscate_unfolding.hpp"