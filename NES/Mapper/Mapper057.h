#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper057                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper057 : public Mapper
{
public:
	Mapper057( NES* parent );

	void	Reset();
	void    Write( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg;
private:
};

