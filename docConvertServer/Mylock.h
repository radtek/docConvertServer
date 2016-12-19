#pragma once

#include <mutex>

class CMylock
{
public:
	CMylock(CRITICAL_SECTION &cs);
	~CMylock();

private:
	CRITICAL_SECTION &m_cs;

};

