#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper107  Magic Dragon Mapper                                       //
//////////////////////////////////////////////////////////////////////////
class	Mapper107 : public Mapper
{
public:
	Mapper107( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
