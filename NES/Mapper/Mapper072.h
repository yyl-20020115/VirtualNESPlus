#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper072  Jaleco/Type1 lower bank switch                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper072 : public Mapper
{
public:
	Mapper072( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
