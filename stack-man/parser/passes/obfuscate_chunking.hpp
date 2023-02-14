#pragma once
#ifdef _PARSER
#include "../instruction_token.hpp"
#include "pass.hpp"
#include "../../vm.hpp"

class obfuscate_chunking_pass : public pass
{
private:
	int obfuscation_level = 1;
public:
	/* Constructor, call original */
	obfuscate_chunking_pass(int level) : pass() { obfuscation_level = level; }
public:
	/* Implement virtual functions */
	bool init() override;
	bool run(std::vector<instruction_token*>& instructions) override;
private:
	std::vector<instruction_token*> get_instructions_with_target(std::vector<instruction_token*> instructions, label_token* target);
};
#endif _PARSER