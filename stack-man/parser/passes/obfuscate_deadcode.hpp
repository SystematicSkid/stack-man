#pragma once
#include "../instruction_token.hpp"
#include "pass.hpp"
#include "../../vm.hpp"

class obfuscate_deadcode_pass : public pass
{
private:
	int obfuscation_level = 1;
public:
	/* Constructor, call original */
	obfuscate_deadcode_pass(int level) : pass() { obfuscation_level = level; }
public:
	/* Implement virtual functions */
	bool init() override;
	bool run(std::vector<instruction_token*>& instructions) override;
private:
	signed int get_instruction_stack_change( stack_vm::vm_instruction inst );
};