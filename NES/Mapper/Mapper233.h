#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper233  42-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
class	Mapper233 : public Mapper
{
public:
	Mapper233( NES* parent );

	void	Reset();
	void	Write( WORD addr, BYTE data );

protected:
private:
};
