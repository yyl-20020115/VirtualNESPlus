#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper227  1200-in-1                                                 //
//////////////////////////////////////////////////////////////////////////
class	Mapper227 : public Mapper
{
public:
	Mapper227( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
