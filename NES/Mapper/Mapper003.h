#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper003  CNROM                                                     //
//////////////////////////////////////////////////////////////////////////
class	Mapper003 : public Mapper
{
public:
	Mapper003( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
