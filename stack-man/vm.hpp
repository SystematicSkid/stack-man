#pragma once
#include <cstdio>
#include <stdint.h>
#include <unordered_map>
#include <string>
#include "vm_ctx.hpp"

class stack_vm
{
public:
	stack_vm( );
	~stack_vm( );
private:
	/* Context */
	vm_ctx* ctx;

public:
	/* Execute a virtual program */
	std::int32_t execute( std::uintptr_t program );


public:
	/* Our instruction byte representations */
	enum class vm_instruction : std::uint8_t
	{
		push = 0, // constant
		push_reg, // reg
		pop,
		pop_reg, // reg
		add,
		sub,
		mul,
		div,
		mod,
		xor_,
		and_,
		or_,
		cmp,
		read_mem,
		write_mem,
		jump, // label
		jz, // label
		jnz, // label
		syscall,
		ret,
		invalid = 0xFF
	};

	/* Our list of registers */
	enum class vm_register : std::uint8_t
	{
		r0 = 0,
		r1,
		r2,
		r3,
		r4,
		r5
	};

	enum class vm_syscall : std::uint8_t
	{
		print = 0,
		get_input,
		malloc,

		max
	};

private:
	/* Instructions */
	void push( std::size_t val );
	void push_reg( vm_register reg );
	std::size_t pop( );
	std::size_t pop_reg( vm_register reg );
	void add( );
	void sub( );
	void mul( );
	void div( );
	void mod( );
	void xor_( );
	void and_( );
	void or_( );
	void cmp( );
	void read_mem( );
	void write_mem( );
	void jump( std::int64_t dst );
	void jz( std::int64_t dst );
	void jnz( std::int64_t dst );
	void syscall( );
	void ret( );
	
public:

	/* String representation of `vm_instruction` enum */
	static inline std::unordered_map<std::string, vm_instruction> instruction_map
	{
		{ "push", vm_instruction::push },
		{ "pop", vm_instruction::pop },
		{ "add", vm_instruction::add },
		{ "sub", vm_instruction::sub },
		{ "mul", vm_instruction::mul },
		{ "div", vm_instruction::div },
		{ "mod", vm_instruction::mod },
		{ "xor", vm_instruction::xor_ },
		{ "and", vm_instruction::and_ },
		{ "or", vm_instruction::or_ },
		{ "cmp", vm_instruction::cmp },
		{ "read", vm_instruction::read_mem },
		{ "write", vm_instruction::write_mem },
		{ "jump", vm_instruction::jump },
		{ "jz", vm_instruction::jz },
		{ "jnz", vm_instruction::jnz },
		{ "syscall", vm_instruction::syscall },
		{ "ret", vm_instruction::ret }
	};

	/* String representation of `vm_register` enum */
	static inline std::unordered_map<std::string, vm_register> register_map
	{
		{ "r0", vm_register::r0 },
		{ "r1", vm_register::r1 },
		{ "r2", vm_register::r2 },
		{ "r3", vm_register::r3 },
		{ "r4", vm_register::r4 },
		{ "r5", vm_register::r5 },
	};
};