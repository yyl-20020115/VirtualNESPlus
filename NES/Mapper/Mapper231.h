#pragma once
//////////////////////////////////////////////////////////////////////////
// Mapper231  20-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
class	Mapper231 : public Mapper
{
public:
	Mapper231( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
