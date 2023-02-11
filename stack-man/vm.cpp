#include "vm.hpp"

stack_vm::stack_vm()
{
	/* Create our context */
	this->ctx = new vm_ctx( );
}

stack_vm::~stack_vm( )
{
	/* Delete our context */
	delete ctx;
}

std::int32_t stack_vm::execute( std::uintptr_t program )
{
	/* Debug info */
	std::int32_t num_instructions = 0;
	
	/* Set counter to start of program */
	ctx->set_pc( program );

	/* Read first instruction */
	vm_instruction inst = *reinterpret_cast<vm_instruction*>( ctx->get_pc( ) );
	
	/* Execution flag */
	bool execute = true;

	/* Loop while executing */
	while ( execute )
	{
		/* Update debug info */
		num_instructions++;
		//printf("[ %p ] %s\n", ctx->get_pc(), get_instruction_name(inst).c_str());
		
		/* Switch over instructions */
		switch (inst)
		{
		case vm_instruction::push:
			/* Push a value onto the stack */
			push( *reinterpret_cast<std::size_t*>( (ctx->get_pc( ) + sizeof(vm_instruction)) ) );
			/* Increment pc */
			ctx->set_pc( ctx->get_pc( ) + sizeof(vm_instruction) + sizeof(std::size_t) );
			break;
		case vm_instruction::push_reg:
			/* Push a value onto the stack */
			push_reg( *reinterpret_cast<vm_register*>( (ctx->get_pc( ) + sizeof(vm_instruction)) ) );
			/* Increment pc */
			ctx->set_pc( ctx->get_pc( ) + sizeof(vm_instruction) + sizeof(vm_register) );
			break;
		case vm_instruction::pop:
			/* Pop a value off the stack */
			pop( );
			/* Increment pc */
			ctx->set_pc( ctx->get_pc( ) + sizeof(vm_instruction) );
			break;
		case vm_instruction::pop_reg:
			/* Pop value off the stack */
			pop_reg( *reinterpret_cast<vm_register*>( (ctx->get_pc( ) + sizeof(vm_instruction)) ) );
			/* Increment pc */
			ctx->set_pc( ctx->get_pc( ) + sizeof(vm_instruction) + sizeof(vm_register) );
			break;
		case vm_instruction::add:
			/* Add two values on the stack */
			add( );
			/* Increment pc */
			ctx->set_pc( ctx->get_pc( ) + sizeof(vm_instruction) );
			break;
		case vm_instruction::sub:
			/* Subtract two values on the stack */
			sub( );
			/* Increment pc */
			ctx->set_pc( ctx->get_pc( ) + sizeof(vm_instruction) );
			break;
		case vm_instruction::mul:
			/* Multiply two values on the stack */
			mul( );
			/* Increment pc */
			ctx->set_pc( ctx->get_pc( ) + sizeof(vm_instruction) );
			break;
		case vm_instruction::div:
			/* Divide two values on the stack */
			div( );
			/* Increment pc */
			ctx->set_pc( ctx->get_pc( ) + sizeof(vm_instruction) );
			break;
		case vm_instruction::mod:
			/* Modulo two values on the stack */
			mod( );
			/* Increment pc */
			ctx->set_pc( ctx->get_pc( ) + sizeof( vm_instruction ) );
			break;
		case vm_instruction::xor_:
			/* Xor two values on the stack */
			xor_( );
			/* Increment pc */
			ctx->set_pc( ctx->get_pc( ) + sizeof( vm_instruction ) );
			break;
		case vm_instruction::or_:
			/* OR two values on the stack */
			or_( );
			/* Increment pc */
			ctx->set_pc( ctx->get_pc( ) + sizeof( vm_instruction ) );
			break;
		case vm_instruction::and_:
			/* AND two values on the stack */
			and_( );
			/* Increment pc */
			ctx->set_pc( ctx->get_pc( ) + sizeof(vm_instruction) );
			break;
		case vm_instruction::cmp:
			/* Cmp two values on the stack */
			cmp( );
			/* Increment pc */
			ctx->set_pc( ctx->get_pc( ) + sizeof( vm_instruction ) );
			break;
		case vm_instruction::read_mem:
			/* Read memory data */
			read_mem( );
			/* Increment pc */
			ctx->set_pc( ctx->get_pc( ) + sizeof( vm_instruction ) );
			break;
		case vm_instruction::write_mem:
			/* Write memory data */
			write_mem( );
			/* Increment pc */
			ctx->set_pc( ctx->get_pc( ) + sizeof( vm_instruction ) );
			break;
		case vm_instruction::jump:
			/* Jump to a location */
			jump( *reinterpret_cast<std::int64_t*>( (ctx->get_pc( )  + sizeof( vm_instruction ) ) ) );
			break;
		case vm_instruction::jz:
		{
			/* Store pc */
			std::uintptr_t pc = ctx->get_pc( );
			/* Jump to a location if the top of the stack is zero */
			jz( *reinterpret_cast<std::int64_t*>( (ctx->get_pc( ) + sizeof( vm_instruction)) ) );
			/* If pc didn't change, next inst */
			if ( pc == ctx->get_pc( ) )
				ctx->set_pc( ctx->get_pc( ) + sizeof( vm_instruction ) + sizeof( std::int64_t ) );
			break;
		}
		case vm_instruction::jnz:
		{
			/* Store pc */
			std::uintptr_t pc = ctx->get_pc( );
			/* Jump to a location if the top of the stack is not zero */
			jnz( *reinterpret_cast<std::int64_t*>( (ctx->get_pc ( ) + sizeof( vm_instruction ) ) ) );
			/* If pc didn't change, next inst */
			if ( pc == ctx->get_pc( ) )
				ctx->set_pc( ctx->get_pc( ) + sizeof( vm_instruction ) + sizeof( std::int64_t ) );
			break;
		}
		case vm_instruction::call:
			/* Call dst, no need to set PC, handled by call handler */
			call( *reinterpret_cast<std::int64_t*>( (ctx->get_pc( ) + sizeof( vm_instruction ) ) ) );
			break;
		case vm_instruction::syscall:
			/* Call a syscall */
			syscall( );
			/* Increment pc */
			ctx->set_pc( ctx->get_pc( ) + sizeof(vm_instruction) );
			break;
		case vm_instruction::ret:
		{
			/* If this is our stack cookie, exit */
			if (ctx->is_stack_empty())
			{
				execute = false;
				break;
			}
			/* Pop return address */
			std::size_t return_address = pop( );
			
			/* Otherwise, jmp to return address */
			ctx->set_pc( return_address );
			break;
		}
		}

		/* Read next instruction */
		inst = *reinterpret_cast<vm_instruction*>( ctx->get_pc( ) );

		/* Read top of the stack */
		std::size_t top = *reinterpret_cast<std::size_t*>( ctx->get_stack( ) );
		//printf("[ + ] Top: %p\n", top);
	}

	//printf("\n[ + ] Executed %d instructions...\n", num_instructions);
	/* Return value on top of stack */
	return pop( );
}

void stack_vm::push( std::size_t val )
{
	/* *stack = val */
	ctx->set_stack( ctx->get_stack( ) - sizeof(std::size_t) );
	*reinterpret_cast<std::size_t*>( ctx->get_stack( ) ) = val;
}

void stack_vm::push_reg( vm_register reg )
{
	/* Read register from context */
	std::uintptr_t reg_val = ctx->get_reg( (std::uint8_t)reg );
	push( (std::size_t)reg_val);
}

std::size_t stack_vm::pop( )
{
	
	/* Get value */
	std::size_t val = *reinterpret_cast<std::size_t*>( ( ctx->get_stack( ) ) );
	/* stack += 8 */
	ctx->set_stack( ctx->get_stack( ) + sizeof(std::size_t) );
	/* Return value for internal use */
	return val;
}

std::size_t stack_vm::pop_reg(vm_register reg)
{
	std::size_t val = pop( );
	ctx->set_reg( (std::uint8_t)reg, (std::uintptr_t)val );
	return val;
}

void stack_vm::add( )
{
	/* Pop operands from stack */
	std::size_t op1 = pop( );
	std::size_t op2 = pop( );

	/* Push result onto stack */
	std::size_t result = op1 + op2;
	push( result );
}

void stack_vm::sub( )
{
	/* Pop operands from stack */
	std::size_t op1 = pop( );
	std::size_t op2 = pop( );

	/* Push result onto stack */
	std::size_t result = op2 - op1;
	push( result );
}

void stack_vm::mul( )
{
	/* Pop operands from stack */
	std::size_t op1 = pop( );
	std::size_t op2 = pop( );

	/* Push result onto stack */
	std::size_t result = op1 * op2;
	push( result );
}

void stack_vm::div()
{
	/* Pop operands from stack */
	std::size_t op1 = pop( );
	std::size_t op2 = pop( );

	/* Push result onto stack */
	std::size_t result = op2 / op1;
	push( result );
}

void stack_vm::mod( )
{
	/* Pop operands from stack */
	std::size_t op1 = pop( );
	std::size_t op2 = pop( );

	/* Push result onto stack */
	std::size_t result = op1 % op2;
	push( result );
}

void stack_vm::xor_()
{
	/* Pop operands from stack */
	std::size_t op1 = pop( );
	std::size_t op2 = pop( );

	/* Push result onto stack */
	std::size_t result = op1 ^ op2;
	push( result );
}

void stack_vm::or_()
{
	/* Pop operands from stack */
	std::size_t op1 = pop( );
	std::size_t op2 = pop( );

	/* Push result onto stack */
	std::size_t result = op1 | op2;
	push( result );
}

void stack_vm::cmp( )
{
	/* Pop both operands from the stack */
	std::size_t op1 = pop( );
	std::size_t op2 = pop( );

	/* Subtract to get result */
	std::size_t result = op1 - op2;

	/* Set flags according to result */
	/* Flags are 1 byte, 8 bits */
	/* 0000 0000 */
	/* CF OF SF ZF are the only ones used */
	/* CF = Carry Flag */
	std::size_t flags = 0;
	if ( op2 > op1 )
		flags |= 0x1;
	/* OF = Overflow Flag */
	/* Set if sign bit of op1 != sign bit of result */
	if ( ( ( op1 >> ( sizeof(std::size_t) * 8 - 1) ) & 0x1 ) != ( ( result >> (sizeof(std::size_t) * 8 - 1) ) & 0x1 ) )
		flags |= 0x2;
	/* SF = Sign Flag */
	/* Set if sign bit of result = 1 */
	if ( result >> ( sizeof(std::size_t) * 8 - 1 ) )
		flags |= 0x4;
	/* ZF = Zero Flag */
	/* Set if result = 0 */
	if ( result == 0 )
		flags |= 0x8;

	/* Done with flags, push onto stack */
	push( flags );
}

void stack_vm::read_mem()
{
	/* First arg: Pointer to read */
	std::size_t mem = pop( );
	/* Second arg: Size to read, max sizeof(std::size_t) */
	std::size_t size = pop( );
	if ( size > sizeof(std::size_t) )
	{
		return;
	}

	std::size_t buffer = 0;
	for (int i = 0; i < size; i++)
	{
		/* Read each byte of mem */
		std::size_t byte = *reinterpret_cast<std::uint8_t*>( mem + i );
		/* Shift byte to correct position */
		byte <<= i * 8;
		/* Add to buffer */
		buffer |= byte;
	}
	/* Push buffer */
	push( buffer );
}

void stack_vm::write_mem()
{
	/* First arg: Pointer to read */
	std::size_t mem = pop( );
	/* Second arg: Value to write */
	std::size_t val = pop( );
	/* Third arg: Size to write, max sizeof(std::size_t) */
	std::size_t size = pop( );
	
	if ( size > sizeof(std::size_t) )
	{
		return;
	}
	
	for (int i = 0; i < size; i++)
	{
		/* Write val to mem byte by byte */
		std::uint8_t byte = ( val >> ( i * 8 ) ) & 0xFF;
		*reinterpret_cast<std::uint8_t*>( mem +  i ) = byte;
	}
}

void stack_vm::and_( )
{
	/* Pop operands from stack */
	std::size_t op1 = pop( );
	std::size_t op2 = pop( );

	/* Push result onto stack */
	std::size_t result = op1 & op2;
	push( result );
}

void stack_vm::jump( std::int64_t dst )
{
	/* Calculate new pc */
	/* New pc = pc + jmp_size + dst */
	std::uintptr_t new_pc = this->ctx->get_pc( ) + sizeof( vm_instruction ) + sizeof( std::uintptr_t ) + dst;
	
	/* Set pc */
	this->ctx->set_pc( new_pc );
}

void stack_vm::jz( std::int64_t dst )
{
	/* 0000 0000 */
	/* CF OF SF ZF are the only ones used */
	/* Pop flags off stack */
	std::size_t flags = pop( );
	/* Read ZF flag */
	std::size_t zf = ( flags >> 3 ) & 0x1;
	if ( zf )
		jump( dst );
}

void stack_vm::jnz( std::int64_t dst )
{
	/* 0000 0000 */
	/* CF OF SF ZF are the only ones used */
	/* Pop flags off stack */
	std::size_t flags = pop( );
	/* Read ZF flag */
	std::size_t zf = ( flags >> 3 ) & 0x1;
	if ( !zf )
		jump( dst );
}

void stack_vm::call( std::int64_t dst )
{
	/* Get return address */
	std::size_t return_address = ctx->get_pc( ) + sizeof( std::size_t ) + sizeof( vm_instruction );
	/* Push return address */
	push( return_address );
	/* Jump to dst */
	jump( dst );
}

void stack_vm::syscall( )
{
	/* Get syscall index */
	vm_syscall syscall_index = static_cast<vm_syscall>( pop( ) );
	if( syscall_index >= vm_syscall::max )
	{
		return;
	}

	switch( syscall_index )
	{
	case vm_syscall::print:
	{
		char character = (char)pop( );
		std::string str;
		while ( character != NULL )
		{
			/* Convert character to char and append to str */
			str.push_back(character);
			character = (char)pop( );
		}
		printf( "%s", str.c_str( ) );
		break;
	}
	case vm_syscall::get_input:
	{
		/* arg1 = buffer size */
		/* arg0 = buffer */

		/* Get buffer */
		std::uintptr_t buffer = pop( );
		/* Get max size */
		std::size_t max_size = pop( );
		std::string tmp;
		/* Get input */
		std::getline( std::cin, tmp );
		std::memcpy( (void*)buffer, tmp.data( ), tmp.size( ) );
		/* Push buffer */
		push( buffer );
		break;
	}
	case vm_syscall::malloc:
	{
		/* Get allocation size */
		std::size_t alloc_size = pop( );
		/* Allocate memory */
		std::uintptr_t alloc = (std::uintptr_t)calloc( alloc_size, sizeof( std::uint8_t ) );
		/* Push allocated memory */
		push( alloc );
		break;
	}
	}
}

void stack_vm::ret( )
{
	printf( "[ ! ] End of subroutine...\n" );
	return;
}

std::string stack_vm::get_instruction_name(vm_instruction inst)
{
	for (auto& [name, instuction] : instruction_map)
	{
		if (instuction == inst)
			return name;
	}
	return "unknown";
}
