#pragma once
#include <mysql.h>

class Database 
{
private:
	Database() 
	{
		mysql_init(&Conn);
		pConn = mysql_real_connect(&Conn, "localhost", "root", "0000", "YCH", 3306, nullptr, 0);
	};
	~Database() 
	{
		delete pConn;
		delete Result;
	};
public:
	static Database& Get()
	{
		static Database Instance;
		return Instance;
	}

	void Query(string query) 
	{
		int result = mysql_query(pConn, query.c_str());
		if (result)
		{
			PrintLog("Faile to Query");
			PrintLog(mysql_error(pConn));
		}
	}

	MYSQL_ROW GetRow(int index)
	{
		Row = mysql_fetch_row(Result);
		return Row;
	}

	bool CreateAccount(ST_UserInfo userinfo);
	bool IsValidID(ST_UserInfo userinfo);
	bool IsValidAccount(ST_UserInfo userinfo);

private:
	MYSQL Conn;
	MYSQL* pConn = nullptr;
	MYSQL_RES* Result = nullptr;
	MYSQL_ROW Row = nullptr;
	MYSQL_DATA Data = {0,};
};