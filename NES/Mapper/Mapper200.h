#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper200  1200-in-1                                                 //
//////////////////////////////////////////////////////////////////////////
class	Mapper200 : public Mapper
{
public:
	Mapper200( NES* parent );

	void	Reset();
	void	Write( WORD addr, BYTE data );

protected:
private:
};
