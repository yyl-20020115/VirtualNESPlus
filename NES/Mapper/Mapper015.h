#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper015  100-in-1 chip                                             //
//////////////////////////////////////////////////////////////////////////
class	Mapper015 : public Mapper
{
public:
	Mapper015( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
