#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper108                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper108 : public Mapper
{
public:
	Mapper108( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
