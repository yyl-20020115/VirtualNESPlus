#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper225  72-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
class	Mapper225 : public Mapper
{
public:
	Mapper225( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
