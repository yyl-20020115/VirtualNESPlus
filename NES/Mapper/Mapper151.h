#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper151 VS-Unisystem                                               //
//////////////////////////////////////////////////////////////////////////
class	Mapper151 : public Mapper
{
public:
	Mapper151( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);
   
protected:

private:
};
