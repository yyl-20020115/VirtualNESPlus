#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper013  CPROM                                                     //
//////////////////////////////////////////////////////////////////////////
class	Mapper013 : public Mapper
{
public:
	Mapper013( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
