#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper092  Jaleco/Type1 Higher bank switch                           //
//////////////////////////////////////////////////////////////////////////
class	Mapper092 : public Mapper
{
public:
	Mapper092( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
