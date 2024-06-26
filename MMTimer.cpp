//
// Multimedia timer support
//
#include "DebugOut.h"
#include "MMTimer.h"

// #### destructor,
static	CMMTimer	MMTimer;

BOOL	CMMTimer::m_bInitialize = FALSE;

BOOL	CMMTimer::m_bHigh = FALSE;
SQWORD	CMMTimer::m_hpFrequency = 0;

CMMTimer::CMMTimer()
{
	// #### ...just to guarantee the 1ms unit for that purpose only...
	if( !m_bInitialize ) {
		if( ::timeBeginPeriod( 1 ) == TIMERR_NOERROR )
			m_bInitialize = TRUE;
	}

	// #### High Performance Counter
	if( ::QueryPerformanceFrequency( (LARGE_INTEGER*)&m_hpFrequency ) ) {
		DEBUGOUT( "CMMTimer:Use high performance counter.(QueryPerformanceCounter)\n" );
		m_bHigh = TRUE;
	}
}

CMMTimer::~CMMTimer()
{
	if( m_bInitialize ) {
		::timeEndPeriod( 1 );
		m_bInitialize = FALSE;
	}
}

SQWORD	CMMTimer::GetMMTimer()
{
	if( m_bHigh ) {
		SQWORD	freq;
		::QueryPerformanceCounter( (LARGE_INTEGER*)&freq );

		return	freq;
	}

	return	(SQWORD)::timeGetTime();
}

FLOAT	CMMTimer::CalcTimeDifference( SQWORD t0, SQWORD t1 )
{
	if( m_bHigh ) {
		return	(FLOAT)(1000.0*(double)(t1-t0)/(double)m_hpFrequency);
	}

	return	(FLOAT)(t1-t0)*1000.0f;
}

