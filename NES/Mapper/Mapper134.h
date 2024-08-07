#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper133  SACHEN CHEN                                               //
//////////////////////////////////////////////////////////////////////////
class	Mapper134 : public Mapper
{
public:
	Mapper134( NES* parent );

	void	Reset();
	void	WriteLow( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE    cmd, prg, chr;
private:
};
