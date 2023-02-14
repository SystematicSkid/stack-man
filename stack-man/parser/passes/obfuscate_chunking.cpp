#ifdef _PARSER
#include "obfuscate_chunking.hpp"
#include <random>

bool obfuscate_chunking_pass::init()
{
	return true;
}

using chunk = std::vector<instruction_token*>;

bool obfuscate_chunking_pass::run( std::vector<instruction_token*>& instructions )
{
	std::int32_t accumulated_instructions = 0;
	label_token* current_label = nullptr;
	/* Random generator */
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<std::size_t> dis(1, this->obfuscation_level);
	/* List of chunks */
	std::vector<chunk> chunks;

	for (int i = 0; i < instructions.size();)
	{
		auto* inst = instructions[i];
		int chunk_length = 0;
		if (!inst->has_instruction())
		{
			auto* inst_label = (label_token*)(inst);
			/* Skip labels */
			i++;

			current_label = inst_label;
		}
		else
		{
			/* Add a label */
			current_label = new label_token();
			current_label->name = "chunk_" + std::to_string(inst->location);
			/* Set target */
			current_label->target = inst;
		}
		
		chunk new_chunk;
		
		new_chunk.push_back(current_label);
		

		std::size_t random_chunk_size = dis(gen);
		for (int k = 0; k < random_chunk_size; k++)
		{
			inst = instructions[i];
			/* Skip unconditional jumps */
			stack_vm::vm_instruction instruction = static_cast<stack_vm::vm_instruction>(inst->instruction);
			if (instruction == stack_vm::vm_instruction::jump || instruction == stack_vm::vm_instruction::ret)
			{
				/* Append instruciton to chunk */
				new_chunk.push_back(inst);
				/* Increment i */
				i++;
				break;
			}
			/* Skip if we encounter a label */
			if (!inst->has_instruction())
			{
				auto* inst_label = (label_token*)(inst);
				//printf("Encountered label %s\n", inst_label->name.c_str());
				
				/* Append instruciton to chunk */
				new_chunk.push_back(inst);
				/* Find instructions with target */
				//auto targeted_instruction = this->get_instructions_with_target(instructions, (label_token*)inst);
				/* Update target label */
				//for (auto* instruction : targeted_instruction)
				//{
				//	instruction->label = current_label->name;
				//}
				/* Increment i */
				i++;
				break;
			}
			/* Append instruciton to chunk */
			new_chunk.push_back(inst);
			/* Increment i */
			i++;
			/* Increment chunk length */
			chunk_length++;
			
			
		}
		
		/* If there is a previous chunk, add jump instruction to new chunk if it does not already have one */
		if (chunks.size() > 0)
		{
			/* Get last chunk */
			auto& last_chunk = chunks[chunks.size() - 1];
			/* Check if last instruction of last chunk of a jump or ret */
			auto* last_instruction = last_chunk[last_chunk.size() - 1];
			stack_vm::vm_instruction instruction = static_cast<stack_vm::vm_instruction>(last_instruction->instruction);
			if (instruction != stack_vm::vm_instruction::jump && instruction != stack_vm::vm_instruction::ret)
			{
				/* Create jump instruction */
				auto* jump_instruction = new instruction_token();
				jump_instruction->instruction = static_cast<std::uint8_t>(stack_vm::vm_instruction::jump);
				/* Set location */
				jump_instruction->location = last_instruction->location + last_instruction->get_size();
				jump_instruction->label = current_label->name;
				/* Add jump instruction to last chunk */
				last_chunk.push_back(jump_instruction);
			}
		}
		/* Add chunk to list */
		chunks.push_back(new_chunk);
	}

	/* Randomly sort all but first chunk */
	std::shuffle(chunks.begin() + 1, chunks.end(), gen);

	/* Clear instruction vector and replace with instructions from chunks */
	instructions.clear();
	for (auto& chunk : chunks)
	{
		for (auto& inst : chunk)
		{
			instructions.push_back(inst);
		}
	}

	return true;
}

std::vector<instruction_token*> obfuscate_chunking_pass::get_instructions_with_target(std::vector<instruction_token*> instructions, label_token* target)
{
	/* Return object */
	std::vector<instruction_token*> instructions_with_target;
	/* Loop through all instructions */
	for (auto& inst : instructions)
	{
		/* Check if instruction has a label */
		if (inst->has_label())
		{
			/* Check if label matches target */
			if (inst->label == target->name)
			{
				/* Add instruction to return object */
				instructions_with_target.push_back(inst);
			}
		}
	}
	return instructions_with_target;
}

#endif _PARSER