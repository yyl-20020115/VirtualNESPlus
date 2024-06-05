#pragma once
//////////////////////////////////////////////////////////////////////////
// #### Goddess Metamorphosis
//////////////////////////////////////////////////////////////////////////
class	Mapper076 : public Mapper
{
public:
	Mapper076( NES* parent );

	void	Reset();
	void	Write( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg;
private:
};
