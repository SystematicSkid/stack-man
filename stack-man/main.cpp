#include <corecrt.h>
#include <cstdint>
#include <cstdio>
#include <Windows.h>
#include <cassert>

/* VM */
#include "vm_ctx.hpp"
#include "vm.hpp"

/* Parser */
#include "parser.hpp"

int main( )
{
	printf("[ + ] Stack-Man!\n");

	/* Create parser */
	vm_parser* parser = new vm_parser( );
	
	std::string target_program = "vms\\strcmp.vm";
	
	std::uintptr_t program = NULL;
	bool err = parser->parse_file(target_program.c_str( ), &program);
	
	if (!err)
	{
		printf( "[ - ] Failed to parse program!\n" );
		parser->display_errors( );
		return 1;
	}

	delete parser;
	

	/* Create VM */
	stack_vm* vm = new stack_vm( );
	/* Execute our program */
	std::int32_t result = vm->execute( program );

	printf( "Result: %d\n", result );
	

	return 0;
}