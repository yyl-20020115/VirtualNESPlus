#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper062                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper062 : public Mapper
{
public:
	Mapper062( NES* parent );

	void	Reset();
	void    Write( WORD addr, BYTE data );

protected:
private:
};

