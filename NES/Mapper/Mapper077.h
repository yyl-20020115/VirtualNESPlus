#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper077  Irem Early Mapper #0                                      //
//////////////////////////////////////////////////////////////////////////
class	Mapper077 : public Mapper
{
public:
	Mapper077( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
