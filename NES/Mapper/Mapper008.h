#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper008  FFE F3xxx                                                 //
//////////////////////////////////////////////////////////////////////////
class	Mapper008 : public Mapper
{
public:
	Mapper008( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
