#pragma once
#ifdef _PARSER
#include <string>
#include "../instruction_token.hpp"
#include "pass.hpp"

class obfuscate_unfolding_pass : public pass
{
private:
	int obfuscation_level = 1;
	int num_iterations = 1;
public:
	/* Constructor, call original */
	obfuscate_unfolding_pass(int iters, int level) : pass() { obfuscation_level = level; num_iterations = iters; }
public:
	/* Implement virtual functions */
	bool init() override;
	bool run(std::vector<instruction_token*>& instructions) override;
private:
	std::vector<std::string> infix_to_postfix( std::vector<std::string> expression );
	std::vector<instruction_token*> postfix_to_vm( std::vector<std::string> postfix );
	std::vector<instruction_token*> unfold_push( instruction_token* instruction, int depth );
};
#endif _PARSER