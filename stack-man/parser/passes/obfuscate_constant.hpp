#pragma once
#include "../instruction_token.hpp"
#include "pass.hpp"

class obfuscate_constant_pass : public pass
{
public:
	/* Implement virtual functions */
	bool init( ) override;
	bool run( std::vector<instruction_token>& instructions ) override;
};