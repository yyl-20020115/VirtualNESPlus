#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper022  Konami VRC2 type A                                        //
//////////////////////////////////////////////////////////////////////////
class	Mapper022 : public Mapper
{
public:
	Mapper022( NES* parent );

	void	Reset();
	void	Write( WORD addr, BYTE data );

protected:
private:
};
