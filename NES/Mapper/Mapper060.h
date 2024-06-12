#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper060                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper060 : public Mapper
{
public:
	Mapper060( NES* parent );

	void	Reset();
	void    Write( WORD addr, BYTE data );

protected:
	BYTE	patch;
	BYTE	game_sel;

private:
};

