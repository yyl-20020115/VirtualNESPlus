#pragma once
//////////////////////////////////////////////////////////////////////////
// Mapper241  Fon Serm Bon                                              //
//////////////////////////////////////////////////////////////////////////
class	Mapper241 : public Mapper
{
public:
	Mapper241( NES* parent );

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
