#include "../macros.hpp"
#ifdef _PARSER
#include "instruction_token.hpp"
#include "../vm.hpp"


std::vector<std::uint8_t> instruction_token::get_data( )
{
	/* Ensure we have an instruction */
	if ( !this->has_instruction( ) )
	{
		return std::vector<std::uint8_t>();
	}
	/* Our data vector  */
	std::vector<std::uint8_t> data;
	/* Push our instruction onto the vec */
	data.push_back( ( std::uint8_t )instruction );
	
	/* If we have a constant, construct and push */
	if ( this->has_constant( ) )
	{
		/* Construct our constant */
		std::uint64_t constant = this->constant.value_or( 0xDEADBEEF );
		/* Push our constant onto the vec */
		for ( int i = 0; i < sizeof( std::size_t ); i++ )
			data.push_back( ( std::uint8_t )( constant >> ( i * 8 ) ) & 0xFF );
	}
	/* If we have a register, construct and push */
	if ( this->has_register( ) )
	{
		/* Construct our register */
		std::uint8_t reg = this->reg.value_or( 0xFF );
		/* Push our register onto the vec */
		data.push_back( reg );
	}
	return data;
}

const bool instruction_token::has_instruction()
{
	return instruction != 0xFF;
}

const bool instruction_token::has_constant( )
{
	return this->constant.has_value( );
}

const bool instruction_token::has_register( )
{
	return this->reg.has_value( );
}

const bool instruction_token::has_label()
{
	return this->label.has_value( );
}

const std::size_t instruction_token::get_size( )
{
	return	( this->has_instruction( )	? sizeof( std::uint8_t ) : 0 ) +
			( this->has_constant( )		? sizeof( std::size_t ) : 0 ) +
			( this->has_label( )		? sizeof( std::size_t ) : 0 ) +
			( this->has_register( )		? sizeof( std::uint8_t ) : 0 );
}

std::string instruction_token::to_string()
{
	if (!this->has_instruction())
	{
		/* Cast to label token */
		label_token* label = (label_token*)this;
		return label->to_string();
	}
	std::string str = "[ " + std::to_string(this->location) + " ] " + stack_vm::get_instruction_name((stack_vm::vm_instruction)this->instruction);
	if (this->has_constant())
		str += " " + std::to_string(this->constant.value());
	if(this->has_register())
		str += " " + std::to_string(this->reg.value());
	if (this->has_label())
		str += " " + this->label.value();
	return str;
}

std::string label_token::to_string()
{
	return /*"[ " + std::to_string(this->location) + " ] " +*/ this->name + ":";
}

#endif _PARSER