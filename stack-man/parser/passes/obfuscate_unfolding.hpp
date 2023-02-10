#pragma once
#include "../instruction_token.hpp"
#include "pass.hpp"

class obfuscate_unfolding : public pass
{
private:
	int obfuscation_level = 1;
public:
	/* Constructor, call original */
	obfuscate_unfolding(int level) : pass() { obfuscation_level = level; }
public:
	/* Implement virtual functions */
	bool init() override;
	bool run(std::vector<instruction_token*>& instructions) override;
};