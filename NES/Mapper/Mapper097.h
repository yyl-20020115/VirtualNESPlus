#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper097  Irem 74161                                                //
//////////////////////////////////////////////////////////////////////////
class	Mapper097 : public Mapper
{
public:
	Mapper097( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
