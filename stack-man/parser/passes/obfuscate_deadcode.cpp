#include "../../macros.hpp"
#ifdef _PARSER
#include "obfuscate_deadcode.hpp"
#include "../../vm.hpp"
#include <random>
#include <map>

bool obfuscate_deadcode_pass::init()
{
	return true;
}

bool obfuscate_deadcode_pass::run(std::vector<instruction_token*>& instructions)
{
	return true;
}

signed int obfuscate_deadcode_pass::get_instruction_stack_change(stack_vm::vm_instruction inst)
{
	/* map<instruction, change>*/
	static std::map<stack_vm::vm_instruction, signed int> instruction_stack_change = {
		{ stack_vm::vm_instruction::push, 1 },
		{ stack_vm::vm_instruction::pop, -1 },
		{ stack_vm::vm_instruction::add, -1 },
		{ stack_vm::vm_instruction::sub, -1 },
		{ stack_vm::vm_instruction::mul, -1 },
		{ stack_vm::vm_instruction::div, -1 },
		{ stack_vm::vm_instruction::mod, -1 },
		{ stack_vm::vm_instruction::and_, -1 },
		{ stack_vm::vm_instruction::or_, -1 },
		{ stack_vm::vm_instruction::xor_, -1 },
		{ stack_vm::vm_instruction::cmp, -1 },
		{ stack_vm::vm_instruction::jump, 0 },
		{ stack_vm::vm_instruction::jz, -1 },
		{ stack_vm::vm_instruction::jnz, -1 },
		{ stack_vm::vm_instruction::call, 0 },
		{ stack_vm::vm_instruction::ret, 0 },
		{ stack_vm::vm_instruction::syscall, 0 }, /* variable */
		{ stack_vm::vm_instruction::read_mem, -1 },
		{ stack_vm::vm_instruction::write_mem, -3 },
		{ stack_vm::vm_instruction::push_reg, 1 },
		{ stack_vm::vm_instruction::pop_reg, -1 }
	};

	return instruction_stack_change[inst];
}

#endif _PARSER