#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper034  Nina-1                                                    //
//////////////////////////////////////////////////////////////////////////
class	Mapper034 : public Mapper
{
public:
	Mapper034( NES* parent );

	void	Reset();
	void	WriteLow( WORD addr, BYTE data );
	void	Write( WORD addr, BYTE data );

protected:
private:
};
