#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper201  21-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
class	Mapper201 : public Mapper
{
public:
	Mapper201( NES* parent );

	void	Reset();
	void	Write( WORD addr, BYTE data );

protected:
private:
};
