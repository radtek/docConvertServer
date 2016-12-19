#include "stdafx.h"
#include "Mylock.h"

#define USE_LOCK 1

CMylock::CMylock(CRITICAL_SECTION &cs)
:m_cs(cs)
{
#if USE_LOCK
	EnterCriticalSection(&m_cs);
#endif
}


CMylock::~CMylock()
{
#if USE_LOCK
	LeaveCriticalSection(&m_cs);
#endif
}
