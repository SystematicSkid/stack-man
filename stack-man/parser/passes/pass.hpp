#pragma once
#ifdef _PARSER
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
	bool update_label(std::vector<instruction_token*>& instructions, instruction_token* old_target, instruction_token* new_target);
	bool insert_before( std::vector<instruction_token*>& instructions, instruction_token* point, instruction_token* token);
	bool insert_after( std::vector<instruction_token*>& instructions, instruction_token* point, instruction_token* token );
	bool replace_instruction( std::vector<instruction_token*>& instructions, instruction_token* point, std::vector<instruction_token*> tokens );
};

/* Passes */
#include "obfuscate_constant.hpp"
#include "obfuscate_unfolding.hpp"
#include "obfuscate_chunking.hpp"
#endif _PARSER