#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper240  Gen Ke Le Zhuan                                           //
//////////////////////////////////////////////////////////////////////////
class	Mapper240 : public Mapper
{
public:
	Mapper240( NES* parent );

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);

protected:
private:
};
