#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper032  Irem G101                                                 //
//////////////////////////////////////////////////////////////////////////
class	Mapper032 : public Mapper
{
public:
	Mapper032( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	patch;
	BYTE	reg;

private:
};
