#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper093  SunSoft (Fantasy Zone)                                    //
//////////////////////////////////////////////////////////////////////////
class	Mapper093 : public Mapper
{
public:
	Mapper093( NES* parent );

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);

protected:
private:
};
