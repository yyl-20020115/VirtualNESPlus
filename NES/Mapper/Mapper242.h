#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper242  Wai Xing Zhan Shi                                         //
//////////////////////////////////////////////////////////////////////////
class	Mapper242 : public Mapper
{
public:
	Mapper242( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
