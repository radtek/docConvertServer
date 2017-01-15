#include "stdafx.h"
#include "ConnectDB.h"
#include "CppSQLite3.h"
#include "docConvertServer.h"
#include "Mylock.h"

static CConnectDB *m_pConnectDb = NULL;

CConnectDB::CConnectDB()
{
	m_pDb = NULL;
	InitDB();
	InitializeCriticalSection(&m_csTConvert);
	InitializeCriticalSection(&m_csTConverted);
	InitializeCriticalSection(&m_csTConvertSuccess);
	InitializeCriticalSection(&m_csTConvertFailed);
}


CConnectDB::~CConnectDB()
{
	ReleaseDB();
	DeleteCriticalSection(&m_csTConvert);
	DeleteCriticalSection(&m_csTConverted);
	DeleteCriticalSection(&m_csTConvertSuccess);
	DeleteCriticalSection(&m_csTConvertFailed);
}

void CConnectDB::InitDB()
{
	try
	{
		if (m_pDb==NULL)
		{
			m_pDb = new CppSQLite3DB();
		}
		m_pDb->open("convert.db");
		// 判断是否存在表emp、建立表
		if (!m_pDb->tableExists("t_convert"))
		{
			m_pDb->execDML("create table t_convert(nid INTEGER PRIMARY KEY, fileid integer unique, softlink char(1024), filetype char(10), isoriginal integer, status integer, trytimes integer);");
		}
#if _DEBUG
#ifdef USE_SAVE_POST_SUCCESS_RECORD
		if (!m_pDb->tableExists("t_converted"))
		{
			m_pDb->execDML("create table t_converted(nid INTEGER PRIMARY KEY, fileid integer, time char(64), node float, txtstatus integer,pagenumber integer);");
		}
#endif
		if (!m_pDb->tableExists("t_convert_success")) 
		{
			m_pDb->execDML("create table t_convert_success(nid INTEGER PRIMARY KEY, fileid integer unique, node float, txtstatus integer, txturl char(1024), imgstatus interger, imgurl char(1024), pagenumber integer);");
		}
		if (!m_pDb->tableExists("t_convert_failed"))
		{
			m_pDb->execDML("create table t_convert_failed(nid INTEGER PRIMARY KEY, fileid integer unique, node float, txtstatus integer, txturl char(1024), pagenumber integer);");
		}
#endif
	}
	catch (...)
	{
	}


}

void CConnectDB::ReleaseDB()
{
	m_pDb->close();
	delete m_pDb;
	m_pDb = NULL;
}

int CConnectDB::insert_convert_table(p_st_tconvert tmsg)
{
	char sql[2000] = { 0 };
	sprintf_s(sql, 2000, "insert into t_convert (fileid, softlink, filetype, isoriginal, status, trytimes) values (%d, '%s', '%s', '%d', '%d', '%d');",
		tmsg->fileid, tmsg->softlink, strFileType[tmsg->filetype], tmsg->isoriginal, tmsg->status, tmsg->trytimes);
	int nRows = 0;
	CMylock mylock(m_csTConvert);
	try
	{
		nRows = m_pDb->execDML(sql);
	}
	catch (...)
	{
		nRows = 0;
	}
	return nRows;
}


int CConnectDB::delete_convert_table_all()
{
	char sql[1024] = { 0 };
	sprintf_s(sql, 1024, "delete from t_convert;");
	int nRows = 0;
	CMylock mylock(m_csTConvert);
	try
	{
		nRows = m_pDb->execDML(sql);
	}
	catch (...)
	{
		nRows = 0;
	}
	return nRows;
}

int CConnectDB::delete_convert_table(const int &nid)
{
	char sql[1024] = { 0 };
	sprintf_s(sql, 1024, "delete from t_convert where nid=%d;", nid);
	int nRows = 0;
	CMylock mylock(m_csTConvert);
	try
	{
		nRows = m_pDb->execDML(sql);
	}
	catch (...)
	{
		nRows = 0;
	}
	return nRows;
}

int CConnectDB::update_convert_table(p_st_tconvert tmsg)
{
	char sql[1024] = { 0 };
	int nRows = 0;
	sprintf_s(sql, 1024, "update t_convert set status=%d, trytimes=%d where nid=%d;",tmsg->status, tmsg->trytimes, tmsg->nid);
	try
	{
		CMylock mylock(m_csTConvert);
		try
		{
			nRows = m_pDb->execDML(sql);
		}
		catch (...)
		{
			nRows = 0;
		}
	}
	catch (...)
	{
		return -1;
	}
	return nRows;
}

int CConnectDB::query_convert_table(list<p_st_tconvert> &v_convert, const int &ncount, const int status)
{
	char sql[1024] = { 0 };
	if (status == -1)
	{
		sprintf_s(sql, 1024, "select * from t_convert limit 0,%d;",ncount);
	}
	else
	{
		sprintf_s(sql, 1024, "select * from t_convert where status = '%d' limit 0,%d;", status, ncount);
	}
	CMylock mylock(m_csTConvert);
	CppSQLite3Query q = m_pDb->execQuery(sql);


	while(!q.eof())
	{
		p_st_tconvert msg = new st_tconvert;
		msg->nid = atoi(q.fieldValue(0));
// 		int datelen = strlen(q.fieldValue(1));
// 		msg->datetime = new char[datelen+1];
// 		memcpy(msg->datetime, q.fieldValue(1),datelen);
// 		msg->datetime[datelen] = '\0';
		msg->fileid = stoi(q.fieldValue(1));
		int softlen = strlen(q.fieldValue(2));
		msg->softlink = new char[softlen+1];
		memcpy(msg->softlink, q.fieldValue(2), softlen);
		msg->softlink[softlen] = '\0';
		if (strcmp(q.fieldValue(3), "pdf") == 0)
		{
			msg->filetype = MSPDF;
		}
		else if (strcmp(q.fieldValue(3), "ppt") == 0 || strcmp(q.fieldValue(3), "pptx") == 0)
		{
			msg->filetype = MSPPT;
		}
		else if (strcmp(q.fieldValue(3), "xls") == 0 || strcmp(q.fieldValue(3), "xlsx") == 0)
		{
			msg->filetype = MSEXCEL;
		}
		else if (strcmp(q.fieldValue(3), "doc") == 0 || strcmp(q.fieldValue(3), "docx") == 0)
		{
			msg->filetype = MSWORD;
		}

		msg->isoriginal = atoi(q.fieldValue(4));
		msg->status = 1;
		msg->trytimes = atoi(q.fieldValue(6));

// 		update_convert_table(msg);

		v_convert.push_back(msg);

		q.nextRow();
	}

	return v_convert.size();
}

int CConnectDB::count_convert_table(const int &status)
{
	char sql[1024] = { 0 };
	if (status == -1)
	{
		sprintf_s(sql, 1024, "select * from t_convert;");
	}
	else
	{
		sprintf_s(sql, 1024, "select * from t_convert where status = %d;", status);
	}
	CMylock mylock(m_csTConvert);
	CppSQLite3Query q = m_pDb->execQuery(sql);


	int ncount = 0;
	while (!q.eof())
	{
		ncount++;
		q.nextRow();
	}
	return ncount;
}

//////////////////////////////////////////////////////////////////////////
#ifdef USE_SAVE_POST_SUCCESS_RECORD
int CConnectDB::insert_converted_table(p_st_tconverted tmsg)
{
	char sql[1024] = { 0 };
	sprintf_s(sql, 1024, "insert into t_converted (fileid, time, node, txtstatus, pagenumber) values (%d, datetime('now','localtime'), '%s', '%d', '%d');",
		tmsg->fileid, tmsg->node, tmsg->txtstatus, tmsg->pagenumber);
	CMylock mylock(m_csTConvert);
	int nRows = m_pDb->execDML(sql);
	return nRows;
}

int CConnectDB::insert_converted_table(list<p_st_tconverted> listmsg)
{
	int nRows = 0;
	list<p_st_tconverted>::iterator it = listmsg.begin();
	while (listmsg.end() != it)
	{
		p_st_tconverted tmsg = *it;
		char sql[1024] = { 0 };
		sprintf_s(sql, 1024, "insert into t_converted (fileid, time, node, txtstatus, pagenumber) values (%d, datetime('now','localtime'), '%s', '%d', '%d');",
			tmsg->fileid, tmsg->node, tmsg->txtstatus+1, tmsg->pagenumber);
		CMylock mylock(m_csTConvert);
		nRows = m_pDb->execDML(sql);
		delete tmsg;
		tmsg = NULL;
		it++;
	}
	listmsg.clear();
	return nRows;
}

#endif

//////////////////////////////////////////////////////////////////////////
int CConnectDB::insert_convert_success_table(p_st_tconverted tmsg)
{
	char sql[1024] = { 0 };
	sprintf_s(sql, 1024, "insert into t_convert_success (fileid, filetype, node, txtstatus, txturl, imgstatus, imgurl, pagenumber) values (%d, '%s', '%s', '%d', '%s', '%d', '%s', '%d');",
		tmsg->fileid, strFileType[tmsg->filetype], tmsg->node, tmsg->txtstatus, tmsg->txturl, tmsg->imgstatus, tmsg->imgurl, tmsg->pagenumber);
	EnterCriticalSection(&m_csTConvertSuccess);
	int nRows = m_pDb->execDML(sql);
	LeaveCriticalSection(&m_csTConvertSuccess);
	return nRows;
}

int CConnectDB::delete_convert_success_table(const int &nid)
{
	char sql[1024] = { 0 };
	sprintf_s(sql, 1024, "delete from t_convert_success where nid=%d;", nid);
	EnterCriticalSection(&m_csTConvertSuccess);
	int nRows = m_pDb->execDML(sql);
	LeaveCriticalSection(&m_csTConvertSuccess);
	return nRows;
}

int CConnectDB::update_convert_success_table(p_st_tconverted tmsg)
{
	char sql[1024] = { 0 };
	sprintf_s(sql, 1024, "update t_convert_success set txtstatus=%d, imgstatus=%d where nid=%d;", tmsg->txtstatus, tmsg->imgstatus, tmsg->nid);
	EnterCriticalSection(&m_csTConvertSuccess);
	int nRows = m_pDb->execDML(sql);
	LeaveCriticalSection(&m_csTConvertSuccess);
	return nRows;
}

vector<p_st_tconverted> CConnectDB::query_convert_success_table(const int &nid)
{
	char sql[1024] = { 0 };

	sprintf_s(sql, 1024, "select * from t_convert_failed where nid = %d;", nid);

	EnterCriticalSection(&m_csTConvertSuccess);
	CppSQLite3Query q = m_pDb->execQuery(sql);
	LeaveCriticalSection(&m_csTConvertSuccess);

	vector<p_st_tconverted> v_converted;

	while (!q.eof())
	{
		p_st_tconverted msg = new st_tconverted;
		msg->nid = atoi(q.fieldValue(0));
// 		int datelen = strlen(q.fieldValue(1));
// 		msg->datetime = new char[datelen + 1];
// 		memcpy(msg->datetime, q.fieldValue(1), datelen);
// 		msg->datetime[datelen] = '\0';
		msg->fileid = atoi(q.fieldValue(1));
		if (strcmp(q.fieldValue(2), "pdf") == 0)
		{
			msg->filetype = MSPDF;
		}
		else if (strcmp(q.fieldValue(2), "ppt") == 0 || strcmp(q.fieldValue(2), "pptx") == 0)
		{
			msg->filetype = MSPPT;
		}
		else if (strcmp(q.fieldValue(2), "xls") == 0 || strcmp(q.fieldValue(2), "xlsx") == 0)
		{
			msg->filetype = MSEXCEL;
		}
		else if (strcmp(q.fieldValue(2), "doc") == 0 || strcmp(q.fieldValue(2), "docx") == 0)
		{
			msg->filetype = MSWORD;
		}

		int nodelen = strlen(q.fieldValue(3));
		msg->node = new char[nodelen + 1];
		memcpy(msg->node, q.fieldValue(3), nodelen);
		msg->node[nodelen] = '\0';
		msg->txtstatus = atoi(q.fieldValue(4));
		int txturllen = strlen(q.fieldValue(5));
		msg->txturl = new char[txturllen + 1];
		memcpy(msg->txturl, q.fieldValue(5), txturllen);
		msg->txturl[txturllen] = '\0';
		msg->imgstatus = atoi(q.fieldValue(6));
		int imgurllen = strlen(q.fieldValue(7));
		msg->imgurl = new char[imgurllen + 1];
		memcpy(msg->imgurl, q.fieldValue(7), imgurllen);
		msg->imgurl[imgurllen] = '\0';
		msg->pagenumber = atoi(q.fieldValue(8));

		v_converted.push_back(msg);

		q.nextRow();
	}
	return v_converted;
}


//////////////////////////////////////////////////////////////////////////
int CConnectDB::insert_convert_failed_table(p_st_tconverted tmsg)
{
	char sql[1024] = { 0 };
	sprintf_s(sql, 1024, "insert into t_convert_failed (fileid, filetype, node, txtstatus, txturl, pagenumber) values ( %d, '%s', '%s', '%d', '%s', '%d');",
		tmsg->fileid, strFileType[tmsg->filetype], tmsg->node, tmsg->txtstatus, tmsg->txturl, tmsg->pagenumber);
	EnterCriticalSection(&m_csTConvertFailed);
	int nRows = m_pDb->execDML(sql);
	LeaveCriticalSection(&m_csTConvertFailed);
	return nRows;
}

int CConnectDB::delete_convert_failed_table(const int &nid)
{
	char sql[1024] = { 0 };
	sprintf_s(sql, 1024, "delete from t_convert_failed where nid=%d;", nid);
	EnterCriticalSection(&m_csTConvertFailed);
	int nRows = m_pDb->execDML(sql);
	LeaveCriticalSection(&m_csTConvertFailed);
	return nRows;
}

int CConnectDB::update_convert_failed_table(p_st_tconverted tmsg)
{
	char sql[1024] = { 0 };
	sprintf_s(sql, 1024, "update t_convert_failed set txtstatus=%d, imgstatus=%d where nid=%d;", tmsg->txtstatus, tmsg->imgstatus, tmsg->nid);
	EnterCriticalSection(&m_csTConvertFailed);
	int nRows = m_pDb->execDML(sql);
	LeaveCriticalSection(&m_csTConvertFailed);
	return nRows;
}

vector<p_st_tconverted> CConnectDB::query_convert_failed_table(const int &nid)
{
	char sql[1024] = { 0 };

	sprintf_s(sql, 1024, "select * from t_convert_failed where nid = %d;", nid);

	EnterCriticalSection(&m_csTConvertFailed);
	CppSQLite3Query q = m_pDb->execQuery(sql);
	LeaveCriticalSection(&m_csTConvertFailed);

	vector<p_st_tconverted> v_converted;

	while (!q.eof())
	{
		p_st_tconverted msg = new st_tconverted;
		msg->nid = atoi(q.fieldValue(0));
		msg->fileid = atoi(q.fieldValue(1));
		if (strcmp(q.fieldValue(2), "pdf") == 0)
		{
			msg->filetype = MSPDF;
		}
		else if (strcmp(q.fieldValue(2), "ppt") == 0 || strcmp(q.fieldValue(2), "pptx") == 0)
		{
			msg->filetype = MSPPT;
		}
		else if (strcmp(q.fieldValue(2), "xls") == 0 || strcmp(q.fieldValue(2), "xlsx") == 0)
		{
			msg->filetype = MSEXCEL;
		}
		else if (strcmp(q.fieldValue(2), "doc") == 0 || strcmp(q.fieldValue(2), "docx") == 0)
		{
			msg->filetype = MSWORD;
		}
		int nodelen = strlen(q.fieldValue(3));
		msg->node = new char[nodelen + 1];
		memcpy(msg->node, q.fieldValue(3), nodelen);
		msg->node[nodelen] = '\0';
		msg->txtstatus = atoi(q.fieldValue(4));
		int txturllen = strlen(q.fieldValue(5));
		msg->txturl = new char[txturllen + 1];
		memcpy(msg->txturl, q.fieldValue(5), txturllen);
		msg->txturl[txturllen] = '\0';
		msg->pagenumber = atoi(q.fieldValue(6));

		v_converted.push_back(msg);

		q.nextRow();
	}
	return v_converted;
}

CConnectDB *CConnectDB::GetInstance()
{
	if (m_pConnectDb == NULL)
	{
		m_pConnectDb = new CConnectDB();
	}
	return m_pConnectDb;
}

