#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper180  Nichibutsu                                                //
//////////////////////////////////////////////////////////////////////////
class	Mapper180 : public Mapper
{
public:
	Mapper180( NES* parent );

	void	Reset();
	void	Write( WORD addr, BYTE data );

protected:
private:
};
