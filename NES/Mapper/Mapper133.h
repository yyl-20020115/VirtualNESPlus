#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper133  SACHEN CHEN                                               //
//////////////////////////////////////////////////////////////////////////
class	Mapper133 : public Mapper
{
public:
	Mapper133( NES* parent );

	void	Reset();
	void	WriteLow( WORD addr, BYTE data );

protected:
private:
};
