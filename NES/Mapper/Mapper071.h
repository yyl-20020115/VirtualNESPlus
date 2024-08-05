#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper071  Camerica                                                  //
//////////////////////////////////////////////////////////////////////////
class	Mapper071 : public Mapper
{
public:
	Mapper071( NES* parent );

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);
	void	Write(WORD addr, BYTE data);

protected:
	int rom_type, rom_bank;
private:
};
