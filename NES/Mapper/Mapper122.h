#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper122/184  SunSoft-1                                             //
//////////////////////////////////////////////////////////////////////////
class	Mapper122 : public Mapper
{
public:
	Mapper122( NES* parent );

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);

protected:
private:
};
