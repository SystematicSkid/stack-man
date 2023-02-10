#include "obfuscate_unfolding.hpp"
#include "../../vm.hpp"
#include <random>
#include <stack>

bool obfuscate_unfolding_pass::init( )
{
	return true;
}

bool obfuscate_unfolding_pass::run(std::vector<instruction_token*>& instructions)
{
	return true;
}

std::string obfuscate_unfolding_pass::infix_to_postfix( std::string_view expression )
{
	/* Perform Shunting-Yard algorithm */
	std::string output;
	std::stack<char> operator_stack;
	
	for (auto& c : expression)
	{
		if (c == ' ')
			continue;

		if (c == '(')
		{
			operator_stack.push(c);
		}
		else if (c == ')')
		{
			while (operator_stack.top() != '(')
			{
				output += operator_stack.top();
				operator_stack.pop();
			}
			operator_stack.pop();
		}
		else if (c == '+' || c == '-' || c == '*' || c == '/')
		{
			while (!operator_stack.empty() && operator_stack.top() != '(')
			{
				output += operator_stack.top();
				operator_stack.pop();
			}
			operator_stack.push(c);
		}
		else
		{
			output += c;
		}
	}
	
	while (!operator_stack.empty())
	{
		output += operator_stack.top();
		operator_stack.pop();
	}
}

std::vector<instruction_token*> obfuscate_unfolding_pass::postfix_to_vm(std::string_view postfix)
{
	/* Output vector */
	std::vector<instruction_token*> output;

	/* Placeholder ptr */
	instruction_token* inst = nullptr;

	/* Loop through postfix */
	for (auto& c : postfix)
	{
		if (c == '+')
		{
			inst = new instruction_token( );
			inst->instruction = (std::uint8_t)stack_vm::vm_instruction::add;
			output.push_back( inst );
		}
		else if (c == '-')
		{
			inst = new instruction_token( );
			inst->instruction = (std::uint8_t)stack_vm::vm_instruction::sub;
			output.push_back( inst );
		}
		else if (c == '*')
		{
			inst = new instruction_token( );
			inst->instruction = (std::uint8_t)stack_vm::vm_instruction::mul;
			output.push_back( inst );
		}
		else if (c == '/')
		{
			inst = new instruction_token( );
			inst->instruction = (std::uint8_t)stack_vm::vm_instruction::div;
			output.push_back( inst );
		}
		else
		{
			inst = new instruction_token( );
			inst->instruction = (std::uint8_t)stack_vm::vm_instruction::push;
			/* Convert number string to integer */
			inst->constant = std::stoi( std::string( 1, c ) );
			output.push_back( inst );
		}
	}

	/* Return output */
	return output;
}
