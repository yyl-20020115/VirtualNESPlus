#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper229  31-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
class	Mapper229 : public Mapper
{
public:
	Mapper229( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
