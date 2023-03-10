#include "../../macros.hpp"
#ifdef _PARSER
#include "pass.hpp"
#include <algorithm>
#include <numeric>

bool pass::update_label(std::vector<instruction_token*>& instructions, instruction_token* old_target, instruction_token* new_target)
{
    /* Find all labels with old target */
    for (auto* inst : instructions)
    {
        if (inst->has_instruction())
            continue;
		label_token* label = (label_token*)inst;
		if (label->target == old_target)
		{
			label->target = new_target;
		}
    }
    return true;
}

bool pass::insert_before(std::vector<instruction_token*>& instructions, instruction_token* point, instruction_token* token)
{
    /* Get size of token to insert */
    std::size_t size = token->get_size();
    /* Insertion location */
    std::size_t location = point->location;
    /* Find insertion point */
    auto it = std::find_if(instructions.begin(), instructions.end(), [point](const auto* instruction) {
        return instruction == point;
        });
    if (it != instructions.end())
    {
        /* Update location of all instructions after */
        std::for_each(it, instructions.end(), [size](auto& instruction) {
            instruction->location += size;
            });
        /* Insert our token */
        instructions.insert(it, std::move(token));
        /* Set our token's location */
        token->location = location;
        /* Return success */
        return true;
    }
    /* Return failure */
    return false;
}

bool pass::insert_after(std::vector<instruction_token*>& instructions, instruction_token* point, instruction_token* token)
{
    /* Get size of token to insert */
    std::size_t size = token->get_size();
    /* Insertion location */
	std::size_t location = point->location + point->get_size( );
    /* Find insertion point */
    auto it = std::find_if(instructions.begin(), instructions.end(), [point](const auto* instruction) {
        return instruction == point;
        });
    if (it != instructions.end())
    {
        /* Update location of all instructions after */
        std::for_each(it + 1, instructions.end(), [size](auto& instruction) {
            instruction->location += size;
            });
        /* Insert our token */
        instructions.insert(it + 1, std::move(token));
        /* Set our token's location */
        token->location = location;
        /* Return success */
        return true;
    }
    /* Return failure */
    return false;
}

bool pass::replace_instruction(std::vector<instruction_token*>& instructions, instruction_token* point, std::vector<instruction_token*> tokens)
{
    /* Get size of our insertion */
    std::size_t size = std::accumulate(tokens.begin(), tokens.end(), 0, [](std::size_t size, auto* token) {
        return size + token->get_size();
        });
    /* Find insertion point */
    auto it = std::find(instructions.begin(), instructions.end(), point);
    if (it != instructions.end())
    {
        /* Insert all instructions */
        instruction_token* insert_point = point;
        for (auto* new_inst : tokens)
        {
            insert_after(instructions, insert_point, new_inst);
            insert_point = new_inst;
        }
        std::size_t remove_size = point->get_size();
        /* Remove the old instruction using remove_if */
		instructions.erase(std::remove_if(instructions.begin(), instructions.end(), [point](auto* instruction) {
			return instruction == point;
			}), instructions.end());
		/* Update label */
		update_label(instructions, point, tokens[0]);

        /* free memory */
        delete point;
        /* Update iterator */
		it = std::find(instructions.begin(), instructions.end(), tokens[0]);
        /* Reduce the size of all instructions from previous point to end */
		std::for_each(it, instructions.end(), [remove_size](auto& instruction) {
			instruction->location -= remove_size;
			});
        /* Return success */
        return true;
    }
    /* Return failure */
    return false;
}
#endif _PARSER