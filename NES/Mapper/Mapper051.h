#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper051  11-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
class	Mapper051 : public Mapper
{
public:
	Mapper051( NES* parent );

	void	Reset();
	void	Write( WORD addr, BYTE data );
	void	WriteLow( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	int     mode, bank;

private:
	void	SetBank_CPU();
};
