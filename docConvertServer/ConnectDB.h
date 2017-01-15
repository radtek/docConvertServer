#pragma once
#ifndef CONNECTDB_H_
#define CONNECTDB_H_

#include "mystruct.h"
#include <string>
#include <list>
#include <vector>
using namespace std;


class CppSQLite3DB;
class CConnectDB
{
public:
	CConnectDB();
	~CConnectDB();

	static CConnectDB *GetInstance();

	void InitDB();
	void ReleaseDB();

	int insert_convert_table(p_st_tconvert tmsg);

	int delete_convert_table_all();

	int delete_convert_table(const int &nid);

	int update_convert_table(p_st_tconvert tmsg);

	int query_convert_table(list<p_st_tconvert> &v_convert, const int &ncount, const int status = -1);

	int count_convert_table(const int &status = 0);

#ifdef USE_SAVE_POST_SUCCESS_RECORD
	int insert_converted_table(p_st_tconverted tmsg);
	int insert_converted_table(list<p_st_tconverted> listmsg);
#endif

	int insert_convert_success_table(p_st_tconverted tmsg);

	int delete_convert_success_table(const int &nid);

	int update_convert_success_table(p_st_tconverted tmsg);

	vector<p_st_tconverted> query_convert_success_table(const int &nid);

	int insert_convert_failed_table(p_st_tconverted tmsg);

	int delete_convert_failed_table(const int &nid);

	int update_convert_failed_table(p_st_tconverted tmsg);

	vector<p_st_tconverted> query_convert_failed_table(const int &nid);

private:
	CppSQLite3DB *m_pDb;
	CRITICAL_SECTION m_csTConvert;
	CRITICAL_SECTION m_csTConverted;
	CRITICAL_SECTION m_csTConvertSuccess;
	CRITICAL_SECTION m_csTConvertFailed;
};
#endif
