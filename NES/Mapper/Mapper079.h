#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper079  Nina-3                                                    //
//////////////////////////////////////////////////////////////////////////
class	Mapper079 : public Mapper
{
public:
	Mapper079( NES* parent );

	void	Reset();
	void	WriteLow( WORD addr, BYTE data );

protected:
private:
};
