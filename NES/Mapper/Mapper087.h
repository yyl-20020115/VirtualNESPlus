#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper087  Konami 74161/32                                           //
//////////////////////////////////////////////////////////////////////////
class	Mapper087 : public Mapper
{
public:
	Mapper087( NES* parent );

	void	Reset();
	void	WriteLow( WORD addr, BYTE data );

protected:
private:
};
