#include "Mapper122.h"

Mapper122::Mapper122(NES* parent) 
	: Mapper(parent) 
{}
//////////////////////////////////////////////////////////////////////////
// Mapper122/184  SunSoft-1                                             //
//////////////////////////////////////////////////////////////////////////
void	Mapper122::Reset()
{
	SetPROM_32K_Bank( 0, 1, 2, 3 );
}

void	Mapper122::WriteLow( WORD addr, BYTE data )
{
	if( addr == 0x6000 ) {
		SetVROM_4K_Bank( 0,  data & 0x07 );
		SetVROM_4K_Bank( 4, (data & 0x70)>>4 );
	}
}
