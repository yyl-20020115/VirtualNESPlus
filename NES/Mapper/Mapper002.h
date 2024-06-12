#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper002  UNROM                                                     //
//////////////////////////////////////////////////////////////////////////
class	Mapper002 : public Mapper
{
public:
	Mapper002( NES* parent );

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);
	void	Write(WORD addr, BYTE data);

protected:
	BYTE	patch;
private:
};
