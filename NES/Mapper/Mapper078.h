#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper078  Jaleco(Cosmo Carrier)                                     //
//////////////////////////////////////////////////////////////////////////
class	Mapper078 : public Mapper
{
public:
	Mapper078( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
