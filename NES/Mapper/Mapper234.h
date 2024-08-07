#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper234  Maxi-15                                                   //
//////////////////////////////////////////////////////////////////////////
class	Mapper234 : public Mapper
{
public:
	Mapper234( NES* parent );

	void	Reset();
	virtual void	Write(WORD addr, BYTE data) override;
	virtual BYTE	Read( WORD addr, BYTE& data) override;

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[2];
private:
	void	SetBank();
};
