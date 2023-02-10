#include "obfuscate_constant.hpp"
#include "../../vm.hpp"

bool obfuscate_constant_pass::init()
{
	return true;
}

bool obfuscate_constant_pass::run(std::vector<instruction_token>& instructions)
{
	/* Iterate through all instructions */
	for (auto& instruction : instructions)
	{
		printf("[ %ld ] %s\n", instruction.location, stack_vm::get_instruction_name((stack_vm::vm_instruction)instruction.instruction).c_str());
		if(instruction.has_constant())
			printf("\tConstant: %ld\n", instruction.constant.value());
		if (instruction.has_register())
			printf("\tRegister: %d\n", instruction.reg.value());
	}

	return true;
}
