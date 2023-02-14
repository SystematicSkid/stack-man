#ifdef _PARSER
#include "obfuscate_constant.hpp"
#include "../../vm.hpp"
#include <random>

bool obfuscate_constant_pass::init()
{
	return true;
}

bool obfuscate_constant_pass::run(std::vector<instruction_token*>& instructions)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> dis(0, std::numeric_limits<std::size_t>::max());
	
    for (int i = 0; i < obfuscation_level; i++)
    {
        /* Iterate through all instructions backwards */
        for (int k = instructions.size() - 1; k >= 0; k--)
        {
			/* Get instruction */
			auto* instruction = instructions[k];
			
            stack_vm::vm_instruction inst = static_cast<stack_vm::vm_instruction>(instruction->instruction);
            /* Check if instruction has constant and is a push */
            if (instruction->has_constant() && inst == stack_vm::vm_instruction::push)
            {
                /* Get constant */
                std::size_t constant = instruction->constant.value( );
                /* Generate a random number using mt19937 from minimum size_t value to maximum size_t value */
				
				std::size_t random = dis(gen);
                /* Xor the constant by the random number */
                std::size_t obfuscated = constant ^ random;

                /* Set the constant value on the instruction */
                instruction->constant = obfuscated;
				
				//printf("Obfuscating constant %d to %d with random %d\n", constant, obfuscated, random);

                /* Push the random value */
                instruction_token* random_token = new instruction_token();
                random_token->instruction = static_cast<std::uint8_t>(stack_vm::vm_instruction::push);
                random_token->constant = random;
                if (!insert_after(instructions, instruction, random_token))
                {
                    throw std::runtime_error("Failed to insert push");
                }

                /* Insert a xor after */
                instruction_token* xor_token = new instruction_token();
                xor_token->instruction = static_cast<std::uint8_t>(stack_vm::vm_instruction::xor_);
                if (!insert_after(instructions, random_token, xor_token))
                {
                    throw std::runtime_error("Failed to insert xor");
                }
            }

        }
    }

    //printf("Constant pass\n");
    //for (int i = 0; i < instructions.size(); i++)
    //{
    //    /* Print instruction */
    //    std::cout << instructions[i]->to_string() << std::endl;
    //}
    //printf("Size: %d\n", instructions.size());

    return true;
}

#endif _PARSER