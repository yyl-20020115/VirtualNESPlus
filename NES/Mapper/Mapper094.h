#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper094  Capcom 74161/32                                           //
//////////////////////////////////////////////////////////////////////////
class	Mapper094 : public Mapper
{
public:
	Mapper094( NES* parent );

	void	Reset();
	void	Write( WORD addr, BYTE data );

protected:
private:
};
