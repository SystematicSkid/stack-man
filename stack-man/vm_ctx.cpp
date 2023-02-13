#include "vm_ctx.hpp"
#include <cstddef>

vm_ctx::vm_ctx(size_t stack_size)
{
	/* Allocate memory for our stack */
	stack = (std::uintptr_t)malloc( stack_size ) + stack_size;
	original_stack = stack;
	/* Set our program counter to the start of the stack */
	pc = stack;

	/* Allocate memory for our registers */
	registers = (std::uintptr_t*)malloc( sizeof(std::uintptr_t) * 16 );

	/* Initialize all registers to 0xDEADBEEF */
	for ( std::size_t i = 0; i < 16; i++ )
		registers[i] = 0xDEADBEEF;
}

vm_ctx::~vm_ctx( )
{
	/* Free our stack */
	free( (void*)original_stack );
}

void vm_ctx::set_pc( std::uintptr_t new_pc )
{
	this->pc = new_pc;
}

void vm_ctx::set_stack(std::uintptr_t stack)
{
	this->stack = stack;
}

void vm_ctx::set_reg(std::uint8_t reg, std::uintptr_t val)
{
	if (reg >= 16)
	{
		/* Error */
		//printf( "[ ! ] Attempting to set register out of bounds: %d\n", reg );
		return;
	}
	this->registers[ reg ] = val;
}

std::uintptr_t vm_ctx::get_pc( )
{
	return this->pc;
}

std::uintptr_t vm_ctx::get_stack()
{
	return this->stack;
}

std::uintptr_t vm_ctx::get_reg(std::uint8_t reg)
{
	if (reg >= 16)
	{
		/* Error */
		//printf( "[ ! ] Attempting to set register out of bounds: %d\n", reg );
		return 0xDEADBEEF;
	}

	return this->registers[ reg ];
}

bool vm_ctx::is_stack_empty()
{
	/* Return true if the only thing left on the stack is the return value */
	return stack == ( original_stack - sizeof(std::size_t) );
}
