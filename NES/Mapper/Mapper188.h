#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper188  Bandai Karaoke Studio                                     //
//////////////////////////////////////////////////////////////////////////
class	Mapper188 : public Mapper
{
public:
	Mapper188( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);
protected:
private:
};
