#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper080  Taito X1-005                                              //
//////////////////////////////////////////////////////////////////////////
class	Mapper080 : public Mapper
{
public:
	Mapper080( NES* parent );

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);

protected:
private:
};
