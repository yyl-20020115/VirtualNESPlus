#pragma once
//////////////////////////////////////////////////////////////////////////
// Mapper047  NES-QJ                                                    //
//////////////////////////////////////////////////////////////////////////
#include "Mapper.h"
class	Mapper047 : public Mapper
{
public:
	Mapper047( NES* parent );

	void	Reset();
	void	Write( WORD addr, BYTE data );
	void	WriteLow(WORD addr, BYTE data);

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[8];
	BYTE	patch;
	BYTE	bank;
	BYTE	prg0, prg1;
	BYTE	chr01,chr23,chr4,chr5,chr6,chr7;
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
private:
	void	SetBank_CPU();
	void	SetBank_PPU();

};
