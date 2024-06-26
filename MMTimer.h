//
// Multimedia timer support
//
#ifndef	__CMMTIMER_INCLUDED__
#define	__CMMTIMER_INCLUDED__

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include "typedef.h"

class	CMMTimer
{
public:
	CMMTimer();
	~CMMTimer();

	static	SQWORD	GetMMTimer();
	static	FLOAT	CalcTimeDifference( SQWORD t0, SQWORD t1 );

protected:
private:
	static	BOOL	m_bInitialize;

	// #### Can use high-resolution counters
	static	BOOL	m_bHigh;
	// #### High-resolution counter frequency
	static	SQWORD	m_hpFrequency;
};

#endif	// !__CMMTIMER_INCLUDED__

