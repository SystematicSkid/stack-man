#include <corecrt.h>
#include <cstdint>
#include <cstdio>
#include <Windows.h>
#include <cassert>
#include <chrono>

/* VM */
#include "vm_ctx.hpp"
#include "vm.hpp"

/* Parser */
#include "parser/parser.hpp"


bool test_program(std::string target_program, std::size_t expected_result)
{
	for (int i = 0; i < 100; i++)
	{
		vm_parser* parser = (new vm_parser())
			->add_pass(new obfuscate_unfolding_pass(3/*iter*/, 3/*depth*/))
			->add_pass( new obfuscate_constant_pass( 4 ) )
			;
		

		std::uintptr_t program = NULL;
		std::int32_t size = 0;
		bool err = parser->parse_file(target_program.c_str(), &program, &size);

		if (!err)
		{
			printf("[ - ] Failed to parse program!\n");
			parser->display_errors();
			return 1;
		}

		delete parser;

		/* Create VM */
		stack_vm* vm = new stack_vm();
		/* Execute our program */
		std::int32_t result = vm->execute((uintptr_t)program);
		/* Delete program */
		delete[](std::uint8_t*)program;
		//delete vm;
		if (result != expected_result)
			return false;
	}
	return true;
}

int main( )
{
	printf("[ + ] Stack-Man!\n");
	srand(time(0));

	/* Test programs */
	//bool works = test_program("vms\\password_check.vm", 30);
	//printf("Test Result: %d\n", works);
	//return 0;
	//TestVMClass* klass = new TestVMClass();
	//printf("C: %d\n", klass->get_c());
	//return 0;
	

	/* Create parser */
	
	vm_parser* parser = (new vm_parser( ) )
		->add_pass( new obfuscate_unfolding_pass( 2/*iter*/, 2/*depth*/))
		->add_pass( new obfuscate_constant_pass( 2 ) )
		;
	
	std::string target_program = "vms\\test.vm";
	
	std::uintptr_t program = NULL;
	std::int32_t size = 0;
	bool err = parser->parse_file(target_program.c_str( ), &program, &size);
	
	if (!err)
	{
		printf( "[ - ] Failed to parse program!\n" );
		parser->display_errors( );
		return 1;
	}

	printf("Size: %d\n", size);

	/* Print all bytes */
	//for(int i = 0; i < size; i++)
	//	printf("0x%02X, ", *(std::uint8_t*)(program + i));
	//printf("\n");

	
	delete parser;

	/* Create VM */
	stack_vm* vm = new stack_vm( );
	/* Execute our program */
	std::int32_t result = vm->execute( (uintptr_t)program );
	
	printf("\nResult: %d\n", result);

	if (result == 1)
	{
		/* Display congrats */
		printf("[ + ] Congratulations on defeating the evil VM!\n");
		std::system( "pause");
		return 1;
	}

	printf("[ - ] Failed to defeat the evil VM!\n");
	std::system( "pause" );

	return 0;
}