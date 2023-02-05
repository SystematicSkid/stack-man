#pragma once
#include <cstdint>
#include <stdint.h>
#include <windows.h>
#include <iostream>

class vm_ctx
{
public:
	vm_ctx( size_t stack_size = 0x1000 );
	~vm_ctx( );
private:
	/* Our program counter, current address to be executed */
	std::uintptr_t pc = NULL;
	/* Address of our stack */
	std::uintptr_t stack = NULL;
	std::uintptr_t original_stack = NULL;
	/* Allocated memory for our registers */
	std::uintptr_t* registers = nullptr;
public:
	void set_pc( std::uintptr_t pc );
	void set_stack( std::uintptr_t stack );
	void set_reg(std::uint8_t reg, std::uintptr_t val );

	std::uintptr_t get_pc( );
	std::uintptr_t get_stack( );
	std::uintptr_t get_reg( std::uint8_t reg );

	bool is_stack_empty( );
};