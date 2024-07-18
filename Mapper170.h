#pragma once
#include "Mapper.h"

//////////////////////////////////////////////////////////////////////////
// Mapper070                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper170 : public Mapper
{
public:
	Mapper170(NES* parent);

	void	Reset();
	virtual bool    ReadAddress(WORD addr) override;
	virtual bool    WriteAddress(WORD addr) override;

	// $8000-$FFFF Memory write
	virtual	void	Write(WORD addr, BYTE data) override;

	// $8000-$FFFF Memory read(Dummy)
	virtual	BYTE	Read(WORD addr, BYTE& data) override;

protected:
	BYTE	reg;
private:
};
