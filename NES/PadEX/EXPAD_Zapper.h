#pragma once
//////////////////////////////////////////////////////////////////////////
// Zapper                                                               //
//////////////////////////////////////////////////////////////////////////
#pragma once
class	EXPAD_Zapper : public EXPAD
{
public:
	EXPAD_Zapper( NES* parent );

	void	Reset();

	BYTE	Read4017();

	void	Sync();
	void	SetSyncData( INT type, LONG data );
	LONG	GetSyncData( INT type );

protected:
	LONG	zapper_x;
	LONG	zapper_y;
	BYTE	zapper_button;
	BYTE	zapper_offscreen;

private:
};
