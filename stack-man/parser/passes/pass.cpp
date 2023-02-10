#include "pass.hpp"
#include <algorithm>

bool pass::insert_before(std::vector<instruction_token>& instructions, std::size_t index, instruction_token token)
{
}

bool pass::insert_after(std::vector<instruction_token*>& instructions, instruction_token* point, instruction_token* token)
{
    /* Get size of token to insert */
    std::size_t size = token->get_size();
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
        /* Return success */
        return true;
    }
    /* Return failure */
    return false;
}
