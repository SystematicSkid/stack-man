#pragma once
#ifdef _PARSER
#include "../instruction_token.hpp"
#include "pass.hpp"

class obfuscate_constant_pass : public pass
{
private:
	int obfuscation_level = 1;
public:
	/* Constructor, call original */
	obfuscate_constant_pass( int level ) : pass( ) { obfuscation_level = level; }
public:
	/* Implement virtual functions */
	bool init( ) override;
	bool run( std::vector<instruction_token*>& instructions ) override;
};
#endif _PARSER