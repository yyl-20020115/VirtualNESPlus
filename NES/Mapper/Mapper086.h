#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper086  Jaleco Early Mapper #2                                    //
//////////////////////////////////////////////////////////////////////////
class	Mapper086 : public Mapper
{
public:
	Mapper086( NES* parent );

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);

	void	VSync();

protected:
	BYTE	reg, cnt;
private:
};
