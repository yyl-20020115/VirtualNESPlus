#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper061                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper061 : public Mapper
{
public:
	Mapper061( NES* parent );

	void	Reset();
	void    Write( WORD addr, BYTE data );

protected:
private:
};

