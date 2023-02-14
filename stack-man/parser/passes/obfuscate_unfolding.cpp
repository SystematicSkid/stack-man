#ifdef _PARSER
#include "obfuscate_unfolding.hpp"
#include "../../vm.hpp"
#include <random>
#include <stack>
#include <regex>

bool obfuscate_unfolding_pass::init( )
{
	return true;
}

bool obfuscate_unfolding_pass::run(std::vector<instruction_token*>& instructions)
{
	for (int i = 0; i < this->num_iterations; i++)
	{
		/* Iterate through all instructions backwards */
		for ( int k = instructions.size( ) - 1; k >= 0; k-- )
		{
			/* Get instruction */
			auto* instruction = instructions[k];

			stack_vm::vm_instruction inst = static_cast<stack_vm::vm_instruction>( instruction->instruction );
			
			if ( inst != stack_vm::vm_instruction::push )
				continue;

			/* Get unfolded push */
			auto unfolded = unfold_push( instruction, this->obfuscation_level );

			/* Replace instruciton */
			if (!replace_instruction(instructions, instruction, unfolded))
			{
				throw std::runtime_error("Failed to replace push");
				return false;
			}

		}
	}

	//printf("Unfold pass\n");
	//for (int i = 0; i < instructions.size(); i++)
	//{
	//	/* Print instruction */
	//	std::cout << instructions[i]->to_string() << std::endl;
	//}

	return true;
}

std::vector<std::string> obfuscate_unfolding_pass::infix_to_postfix( std::vector<std::string> expression )
{
	std::stack<std::string> stack;
	std::vector<std::string> postfix;

	/* Infix to Postfix */
	for (auto& token : expression)
	{
		/* If token is a number */
		if (token[0] >= '0' && token[0] <= '9')
		{
			postfix.push_back(token);
		}
		/* If token is an operator */
		else if (token == "+" || token == "-" || token == "*" || token == "/")
		{
			/* While stack is not empty and top of stack is not a left parenthesis */
			while (!stack.empty() && stack.top() != "(")
			{
				/* Pop from stack and add to postfix */
				postfix.push_back(stack.top());
				stack.pop();
			}
			/* Push token to stack */
			stack.push(token);
		}
		/* If token is a left parenthesis */
		else if (token == "(")
		{
			/* Push token to stack */
			stack.push(token);
		}
		/* If token is a right parenthesis */
		else if (token == ")")
		{
			/* While stack is not empty and top of stack is not a left parenthesis */
			while (!stack.empty() && stack.top() != "(")
			{
				/* Pop from stack and add to postfix */
				postfix.push_back(stack.top());
				stack.pop();
			}
			/* Pop from stack */
			stack.pop();
		}
	}

	/* While stack is not empty */
	while (!stack.empty())
	{
		/* Pop from stack and add to postfix */
		postfix.push_back(stack.top());
		stack.pop();
	}
	
	return postfix;
}

std::vector<instruction_token*> obfuscate_unfolding_pass::postfix_to_vm( std::vector<std::string> postfix )
{
	/* Output vector */
	std::vector<instruction_token*> output;

	/* Placeholder ptr */
	instruction_token* inst = nullptr;

	/* Loop through postfix */
	for (auto& c : postfix)
	{
		if (c == "+")
		{
			inst = new instruction_token( );
			inst->instruction = (std::uint8_t)stack_vm::vm_instruction::add;
			output.push_back( inst );
		}
		else if (c == "-")
		{
			inst = new instruction_token( );
			inst->instruction = (std::uint8_t)stack_vm::vm_instruction::sub;
			output.push_back( inst );
		}
		else if (c == "*")
		{
			inst = new instruction_token( );
			inst->instruction = (std::uint8_t)stack_vm::vm_instruction::mul;
			output.push_back( inst );
		}
		else if (c == "/")
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
			std::size_t constant_val = std::stoll( c );
			inst->constant = constant_val;
			output.push_back( inst );
		}
	}

	/* Return output */
	return output;
}

std::vector<instruction_token*> obfuscate_unfolding_pass::unfold_push( instruction_token* instruction, int depth )
{
	/* Output */
	std::vector<instruction_token*> output;
	
	/* Ensure we actually have a constant to push */
	if( !instruction->has_constant( ) )
		return output;

	/* Random number generator */
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> op_dis(0, 2);
	std::uniform_int_distribution<> arithmetic_dis_large(1, 100000);
	std::uniform_int_distribution<> arithmetic_dis_small(1, 10);
	
	signed long long current_value = instruction->constant.value( );
	
	/* Our math expression */
	std::string expression = "";
	for (int i = 0; i < depth; i++)
	{
		/* Pick a random operation */
		/* 0 = + */
		/* 1 = - */
		/* 2 = * */
		int operation = op_dis(gen);
		int iter = 0;
		switch (operation)
		{
		default:
			/* Throw erorr */
			std::runtime_error("[ obfuscate_unfolding_pass::unfold_push ] Invalid Operation!");
			break;
		case 0:
		{
			/* Pick a random number */
			signed long long rand_int = arithmetic_dis_large(gen);
			/* Ensure the addition won't cause overflow */
			while (current_value + rand_int > std::numeric_limits<std::size_t>::max())
			{
				rand_int = arithmetic_dis_large(gen);
				iter++;
				if (iter > 100)
				{
					rand_int = 0;
					break;
				}
			}
			/* Update current value */
			current_value += rand_int;
			/* Prepend subtraction to expression to get the original value back */
			expression = " - " + std::to_string(rand_int) + ") " + expression;
			break;
		}
		case 1:
		{
			/* Pick a random number */
			signed long long rand_int = arithmetic_dis_large(gen);
			/* Check if 64-bit integer is negative based on sign bit */
			
			/* Ensure the subtraction won't cause underflow */
			while (current_value - rand_int < 0)
			{
				rand_int = arithmetic_dis_large(gen);
				iter++;
				if (iter > 100)
				{
					rand_int = 0;
					break;
				}
					
			}
			/* Update current value */ 
			current_value -= rand_int;
			/* Prepend addition to expression to get the original value back */
			expression = " + " + std::to_string(rand_int) + ") " + expression;
			break;
		}
		case 2:
		{
			/* Pick a random number */
			signed long long rand_int = arithmetic_dis_small(gen);
			/* Ensure the multiplication won't cause overflow */
			while (current_value * rand_int > std::numeric_limits<std::size_t>::max())
			{
				rand_int = arithmetic_dis_small(gen);
				iter++;
				if (iter > 100)
				{
					rand_int = 1;
					break;
				}
			}
			/* Update current value */
			current_value *= rand_int;
			/* Prepend division to expression to get the original value back */
			expression = " / " + std::to_string(rand_int) + ") " + expression;
			break;
		}

		}

	}
	

	/* Prepend current value to expression */
	expression = std::to_string(current_value) + " " + expression;
	/* Prepend iteration number of parentheses */
	for (int j = 0; j < depth; j++)
		expression = "( " + expression;
	
	/* 
		Regex to parse the expression into a vector of strings
		Input: "(8 * 6) + 2"
		Output: {"(", "8", "*", "6", ")", "+", "2"}
	*/
	std::regex re("\\s*([\\(\\)\\+\\-\\*\\/]|\\d+)\\s*");
	std::sregex_token_iterator it(expression.begin(), expression.end(), re, 1);
	std::sregex_token_iterator end;
	std::vector<std::string> tokens;
	while (it != end)
	{
		tokens.push_back(*it);
		it++;
	}

	/* Convert infix to postfix */
	std::vector<std::string> postfix = infix_to_postfix( tokens );
	/* Convert postfix to vm instructions */
	std::vector<instruction_token*> instructions = postfix_to_vm(postfix);
	
	/* Return instructions */
	return instructions;
}

#endif _PARSER