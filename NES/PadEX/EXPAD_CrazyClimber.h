#pragma once
//////////////////////////////////////////////////////////////////////////
// Crazy climber                                                        //
//////////////////////////////////////////////////////////////////////////
class	EXPAD_CrazyClimber : public EXPAD
{
public:
	EXPAD_CrazyClimber( NES* parent );

	void	Strobe();

	void	Sync();
	void	SetSyncData( INT type, LONG data );
	LONG	GetSyncData( INT type );

protected:
	BYTE	bits;

private:
};
