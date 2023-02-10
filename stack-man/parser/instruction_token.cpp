#include "instruction_token.hpp"


std::vector<std::uint8_t> instruction_token::get_data( )
{
	/* Ensure we have an instruction */
	if ( !this->has_instruction( ) )
		return std::vector<std::uint8_t>( );
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
}

bool instruction_token::has_instruction()
{
	return instruction != 0xFF;
}

bool instruction_token::has_constant( )
{
	return this->constant.has_value( );
}

bool instruction_token::has_register( )
{
	return this->reg.has_value( );
}
