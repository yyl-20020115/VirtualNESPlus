#pragma once
//////////////////////////////////////////////////////////////////////////
// Mapper140                                                            //
//////////////////////////////////////////////////////////////////////////
#include "Mapper.h"
class	Mapper140 : public Mapper
{
public:
	Mapper140( NES* parent );

	void	Reset();
	void    WriteLow( WORD addr, BYTE data );

protected:
private:
};

