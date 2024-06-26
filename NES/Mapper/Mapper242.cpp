#include "Mapper242.h"

Mapper242::Mapper242(NES* parent)
	: Mapper(parent)
{}
//////////////////////////////////////////////////////////////////////////
// Mapper242  Wai Xing Zhan Shi                                         //
//////////////////////////////////////////////////////////////////////////
void	Mapper242::Reset()
{
	SetPROM_32K_Bank( 0 );
}

void	Mapper242::Write( WORD addr, BYTE data )
{
	if( addr & 0x01 ) {
		SetPROM_32K_Bank( (addr&0xF8)>>3 );
	}
}
