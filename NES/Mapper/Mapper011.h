#pragma once
//////////////////////////////////////////////////////////////////////////
// Mapper011  Color Dreams                                              //
//////////////////////////////////////////////////////////////////////////
class	Mapper011 : public Mapper
{
public:
	Mapper011( NES* parent );

	void	Reset();
	void	Write( WORD addr, BYTE data );

protected:
private:
};
