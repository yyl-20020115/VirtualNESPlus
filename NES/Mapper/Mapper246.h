#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper246  Phone Serm Berm                                           //
//////////////////////////////////////////////////////////////////////////
class	Mapper246 : public Mapper
{
public:
	Mapper246( NES* parent );

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);

protected:
private:
};
