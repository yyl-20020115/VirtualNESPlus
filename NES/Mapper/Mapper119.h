#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper119  Nintendo MMC3                                             //
//////////////////////////////////////////////////////////////////////////
class	Mapper119 : public Mapper
{
public:
	Mapper119( NES* parent );

	void	Reset();
	void	Write( WORD addr, BYTE data );

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	patch;
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;
	BYTE	we_sram;
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;

private:
	void	SetBank_CPU();
	void	SetBank_PPU();
};
