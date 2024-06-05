#include "Mapper194.h"
//////////////////////////////////////////////////////////////////////////
// #### Mapper194 Labyrinth Temple Dubba
//////////////////////////////////////////////////////////////////////////


Mapper194::Mapper194(NES* parent)
	: Mapper(parent)
{}

void	Mapper194::Reset()
{
	SetPROM_32K_Bank( PROM_32K_SIZE-1 );
}

void	Mapper194::Write( WORD addr, BYTE data )
{
	SetPROM_8K_Bank( 3, data );
}
