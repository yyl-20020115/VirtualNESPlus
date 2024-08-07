#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper183  Gimmick (Bootleg)                                         //
//////////////////////////////////////////////////////////////////////////
class	Mapper183 : public Mapper
{
public:
	Mapper183( NES* parent );

	void	Reset();
	void	Write( WORD addr, BYTE data );

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[8];
	BYTE	irq_enable;
	INT		irq_counter;
private:
};

