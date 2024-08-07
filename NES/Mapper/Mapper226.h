#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper226  76-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
class	Mapper226 : public Mapper
{
public:
	Mapper226( NES* parent );

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
