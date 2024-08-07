#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper075  Konami VRC1/Jaleco D65005                                 //
//////////////////////////////////////////////////////////////////////////
class	Mapper075 : public Mapper
{
public:
	Mapper075( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[2];
private:
};
