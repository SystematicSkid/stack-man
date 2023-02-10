#pragma once
#include <vector>

class pass
{
public:
	pass () = default;
	~pass( ) = default;
public:
	virtual bool init( ) = 0;
	virtual bool run( std::vector<instruction_token>& instructions ) = 0;
};

/* Passes */
#include "obfuscate_constant.hpp"