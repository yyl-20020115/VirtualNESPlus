#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper193 MEGA SOFT (NTDEC) : Fighting Hero                          //
//////////////////////////////////////////////////////////////////////////
class	Mapper193 : public Mapper
{
public:
	Mapper193( NES* parent );

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);

protected:
private:
};
