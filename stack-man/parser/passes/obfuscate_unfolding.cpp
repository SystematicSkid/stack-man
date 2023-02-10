#include "obfuscate_unfolding.hpp"
#include "../../vm.hpp"
#include <random>

bool obfuscate_unfolding_pass::init( )
{
	return true;
}

bool obfuscate_unfolding_pass::run(std::vector<instruction_token*>& instructions)
{
	return true;
}

std::string obfuscate_unfolding_pass::infix_to_postfix(std::string_view expression)
{
	return std::string();
}

std::vector<instruction_token*> obfuscate_unfolding_pass::postfix_to_vm(std::string_view postfix)
{
	return std::vector<instruction_token*>();
}
