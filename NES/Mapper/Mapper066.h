#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper066  Bandai 74161                                              //
//////////////////////////////////////////////////////////////////////////
class	Mapper066 : public Mapper
{
public:
	Mapper066( NES* parent );

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);
	void	Write(WORD addr, BYTE data);

protected:
private:
};
