#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper244                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper244 : public Mapper
{
public:
	Mapper244( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
