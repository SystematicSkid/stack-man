#pragma once
#include <string>
#include "../instruction_token.hpp"
#include "pass.hpp"

class obfuscate_unfolding_pass : public pass
{
private:
	int obfuscation_level = 1;
public:
	/* Constructor, call original */
	obfuscate_unfolding_pass(int level) : pass() { obfuscation_level = level; }
public:
	/* Implement virtual functions */
	bool init() override;
	bool run(std::vector<instruction_token*>& instructions) override;
private:
	std::string infix_to_postfix( std::string_view expression );
	std::vector<instruction_token*> postfix_to_vm( std::string_view postfix );
};