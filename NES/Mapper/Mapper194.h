#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// #### Mapper194 Labyrinth Temple Dabaa
//////////////////////////////////////////////////////////////////////////
class	Mapper194 : public Mapper
{
public:
	Mapper194( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
