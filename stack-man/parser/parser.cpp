#include "parser.hpp"
#include "../vm.hpp"

bool vm_parser::parse_file( const char* file, std::uintptr_t* program, int* size )
{
	/* Reset for good measure */
	this->reset( );
	/* Open `file` */
	stream.open( file, std::ios::in );
	/* Check for error */
	if ( !stream.is_open( ) )
	{
		this->has_error = true;
		/* Add error */
		errors.push_back( "Failed to open file" );
		/* Return false */
		return false;
	}

	/* Set our current line */
	current_line = 0;

	/* Create program vector */
	std::vector<std::uint8_t> program_vector;

	/* Read every line */
	std::string line;
	while( std::getline( stream, line ) )
	{
		/* Increment line count */
		current_line++;
		current_program_line = program_vector.size( );
		/* Parse tokens */
		std::vector<std::string> tokens = get_tokens( line );
		/* Check if we have any, if not, go to next */
		if( tokens.empty( ) )
		{
			continue;
		}
		/* Check if we're parsing a label */
		std::size_t end_pos = line.find(':');
		if ( end_pos != std::string::npos )
		{
			this->handle_label( line );
			continue;
		}
		/* Parse instruction */
		instruction_token inst_token;
		inst_token.location = current_program_line;
		std::uint8_t instruction = parse_instruction( tokens[0] );
		inst_token.instruction = instruction;
		/* Add instruction to program */
		program_vector.push_back( instruction );
		/* If there is 2 tokens, it's going to have a constant */
		std::size_t token_count = tokens.size( );
		if ( token_count == 2 )
		{
			std::int64_t constant = 0;
			/* Check if second token contains an alphabetical character and doesn't start with '0x' */
			
			if (std::any_of(tokens[1].begin( ), tokens[1].end( ), [](char c) { return std::isalpha(c); }) 
				&& tokens[1].find("0x") == std::string::npos)
			{
				/* Attempt to parse register */
				if( instruction == (std::uint8_t)stack_vm::vm_instruction::push || instruction == (std::uint8_t)stack_vm::vm_instruction::pop )
				{
					std::uint8_t reg = parse_register( tokens[1], true );
					/* Check if valid */
					if (reg != 0xFF)
					{
						/* Increment instruction byte by '0x1' to convert to 'reg' instruction */
						inst_token.instruction += 1;
						instruction++;
						/* Remove last instruction from program vector */
						program_vector.pop_back( );
						/* Push instruction and register */
						program_vector.push_back( instruction );
						program_vector.push_back( reg );
						printf("Adding reg: %d\n", reg);
						inst_token.reg = reg;
						/* Nothing more to do */
						/* Push instruction token */
						this->instructions.push_back(inst_token);
						continue;
					}
				}
				

				/* Parse label */
				std::size_t label = parse_label( tokens[1] );
				/* Calculate distance based on current position */
				std::int64_t distance = label - ( current_program_line + sizeof(std::uint8_t) + sizeof(std::size_t) );
				//std::cout << "Symbol: " << tokens[1] << " | Distance: " << distance << " | Write Location: " << program_vector.size() << std::endl;
				
				constant = distance;
			}
			else
			{
				/* Parse constant */
				constant = parse_constant(tokens[1]);
			}
			
			inst_token.constant = constant;

			/* Write all 8 bytes to program vector */
			for (std::size_t i = 0; i < sizeof(std::size_t); i++)
			{
				/* Get byte */
				std::uint8_t byte = (constant >> (i * 8)) & 0xFF;
				/* Add to program vector */
				program_vector.push_back(byte);
			}
		}
		
		/* Push instruction token */
		this->instructions.push_back(inst_token);
	}

	this->finished_first_pass = true;

	/* Perform second pass for symbol table */
	if ( this->requires_second_pass )
	{
		/* Loop through our update symbol table */
		for (auto& [target_symbol, line] : symbol_refs)
		{
			std::string symbol = target_symbol;
			/* If symbol name contains a number as the final character, remove it */
			while (std::isdigit(symbol.back()))
			{
				symbol.pop_back();
			}
			/* Find symbol in symbol table */
			auto it = symbol_table.find( symbol );
			/* If symbol doesn't exist, throw error */
			if ( it == symbol_table.end( ) )
			{
				this->has_error = true;
				/* Add error */
				errors.push_back( "Symbol '" + symbol + "' does not exist" );
				/* Continue */
				continue;
			}
			

			/* Find instruction token at `line` */
			auto inst_it = std::find_if( this->instructions.begin( ), this->instructions.end( ), [line](const instruction_token& token) { return token.location == line; } );
			/* Get reference to instruction */
			instruction_token& inst = *inst_it;
			/* Get write location in program_vector */
			std::size_t write_location = line + sizeof( std::uint8_t );
			/* Calculate distance based on current position */
			std::int64_t distance = it->second - ( write_location + sizeof(std::size_t) );
			/* Set constant */
			inst.constant = distance;
			/* Debug */
			//std::cout << "Symbol: " << symbol << " | Distance: " << distance << " | Write Location: " << write_location << std::endl;
			/* Write all 8 bytes to program vector */
			for ( std::size_t i = 0; i < sizeof( std::size_t ); i++ )
			{
				/* Get byte */
				std::uint8_t byte = ( distance >> (i * 8) ) & 0xFF;
				/* Write to write location in program vector */
				program_vector[write_location + i] = byte;
			}
		}
		
	}
	

	/* Execute our pass system */
	for( auto& pass : this->passes )
	{
		if( pass->init( ) )
			pass->run( instructions );
	}

	/* Allocate memory for program */
	*program = (std::uintptr_t)malloc( program_vector.size( ) );
	/* Copy program vector to program */
	std::copy( program_vector.begin( ), program_vector.end( ), (std::uint8_t*)*program );

	if ( size != nullptr )
		*size = program_vector.size( );

	return !this->has_error;
}

vm_parser* vm_parser::add_pass( pass* pass )
{
	this->passes.push_back( pass );
	return this;
}

std::vector<std::string> vm_parser::get_tokens( std::string line )
{
	std::vector<std::string> ret;
	/* Ensure current line and stream is good */
	if( this->current_line < 0 || line.empty( ) )
		return ret;

	/* Convert all tabs (\t) to spaces */
	std::replace( line.begin( ), line.end( ), '\t', ' ' );
		
	std::string token;
	std::istringstream token_stream( line );

	/* Parse all tokens */
	while ( std::getline( token_stream, token, ' ' ) )
	{
		if ( token.find( '#' ) != std::string::npos )
			break;
		if( token.empty( ) )
			continue;
		ret.push_back( token );
	}

	/* If we have more than 2 tokens, there is an error */
	if ( ret.size(  ) > 2 )
	{
		this->has_error = true;
		this->errors.push_back( "Too many tokens on line " + std::to_string( this->current_line ) );
	}

	/* Return our tokens */
	return ret;
}

bool vm_parser::handle_label( std::string line )
{
	/* Ensure line contains a ':' */
	std::size_t end_pos = line.find( ':' );
	if (end_pos == std::string::npos )
		return false;
	/* Remove ':' from the line */
	std::string label = line.substr( 0, end_pos );

	/* Ensure label is not empty */
	if( label.empty( ) )
	{
		this->has_error = true;
		this->errors.push_back("Empty label on line " + std::to_string(this->current_line));
		return false;
	}

	/* Check for any non alphanumeric, include underscores characters in the line */

	auto is_valid_char = []( char c )
	{
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
	};
	
	for (char c : label)
	{
		if (!is_valid_char(c))
		{
			this->has_error = true;
			this->errors.push_back("Invalid character " + std::to_string(c) + " in label on line " + std::to_string(this->current_line));
			return false;
		}
	}

	/* Add symbol to symbol table */
	this->symbol_table[ label ] = this->current_program_line;
	
	return true;
}

std::size_t vm_parser::parse_constant( std::string constant )
{
	/* Attempt to convert `constant` to std::size_t */
	try
	{
		/* Parse hex */
		if( constant.find("0x") != std::string::npos )
			return std::stoull(constant, nullptr, 16);
		/* Parse base10 */
		return std::stoull(constant, nullptr, 10);
	}
	catch ( std::exception& e )
	{
		this->has_error = true;
		this->errors.push_back( "Invalid constant on line " + std::to_string( this->current_line ) );
		return 0;
	}
}

std::uint8_t vm_parser::parse_instruction( std::string instruction )
{
	/* Search for 'instruction' in stack_vm::instruction_map */
	auto it = stack_vm::instruction_map.find( instruction );
	if ( it == stack_vm::instruction_map.end( ) )
	{
		this->has_error = true;
		this->errors.push_back("Invalid instruction on line " + std::to_string( this->current_line ) );
		return (std::uint8_t)stack_vm::vm_instruction::invalid;
	}
	return (std::uint8_t)( it->second );
}

std::uint8_t vm_parser::parse_register( std::string reg, bool no_except )
{
	/* If reg doesn't start with 'r', return */
	if ( reg[0] != 'r' )
	{
		if ( !no_except )
		{
			/* Throw error */
			this->has_error = true;
			this->errors.push_back("Invalid register on line " + std::to_string(this->current_line));
		}
		return 0xFF;
	}

	/* Find register in map */
	auto it = stack_vm::register_map.find( reg );
	if ( it == stack_vm::register_map.end( ) )
	{
		/* Throw error */
		this->has_error = true;
		this->errors.push_back( "Invalid register on line " + std::to_string( this->current_line ) );
		return 0xFF;
	}
	
	return (std::uint8_t)( it->second );
}

std::size_t vm_parser::parse_label( std::string label )
{
	/* Ensure label is not empty */
	if ( label.empty( ) )
	{
		this->has_error = true;
		this->errors.push_back("Empty label on line " + std::to_string(this->current_line));
		return 0;
	}

	auto is_valid_char = [](char c)
	{
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
	};

	/* Check for any non alphanumeric characters in the line */
	for (char c : label)
	{
		if (!is_valid_char(c))
		{
			this->has_error = true;
			this->errors.push_back("Invalid character in label on line " + std::to_string(this->current_line));
			return 0;
		}
	}

	/* Search for label in symbol table */
	auto it = this->symbol_table.find( label );
	if ( it == this->symbol_table.end( ) )
	{
		/* Label has not been stored yet, require second pass */
		this->requires_second_pass = true;
		if ( this->finished_first_pass )
		{
			/* Label doesn't exist, throw error */
			this->has_error = true;
			this->errors.push_back( "Label '" + label + "' does not exist on line " + std::to_string( this->current_line ) );
		}
		else
		{
			/* Check if label already exists in tabel */
			int label_num = 0;
			while (this->symbol_refs.find(label) != this->symbol_refs.end() )
			{
				/* Append label num to label */
				label += std::to_string( label_num );
				label_num++;
			}
			/* Add to references */
			this->symbol_refs[ label ] = this->current_program_line;
		}
		return 0;
	}

	return it->second;
}

void vm_parser::reset()
{
	/* Reset all file specific variables */
	this->stream.close( );
	this->current_line = -1;
	this->current_program_line = 0;
	this->has_error = false;
	this->requires_second_pass = false;
	this->finished_first_pass = false;
	this->errors.clear( );
	this->symbol_refs.clear( );
	this->symbol_table.clear( );
	this->instructions.clear( );
}

void vm_parser::display_errors()
{
	if ( !this->has_error )
		return;
	for( auto err : this->errors )
		printf("[ ! ] %s\n", err.c_str( ) );
}
