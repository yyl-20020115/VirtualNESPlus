#pragma once
//////////////////////////////////////////////////////////////////////////
// Hyper Shot                                                           //
//////////////////////////////////////////////////////////////////////////
class	EXPAD_HyperShot : public EXPAD
{
public:
	EXPAD_HyperShot( NES* parent );

	void	Reset();

	void	Strobe();
	BYTE	Read4017();

protected:
	BYTE	shotbits;

private:
};
