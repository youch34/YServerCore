#include "stdafx.h"
#include "Database.h"

bool Database::CreateAccount(ST_UserInfo userinfo)
{
	//WriteLock(LockDB);
	string str = "INSERT INTO users VALUE ('" + (string)userinfo.Id + "','" + (string)userinfo.Pw + "')";
	Query(str);
	return IsValidAccount(userinfo);
}

bool Database::IsValidID(ST_UserInfo userinfo)
{
	//ReadLock(LockDB);
	string str = "SELECT * FROM users WHERE id='" + (string)userinfo.Id + "'";
	Query(str);
	Result = mysql_store_result(pConn);
	Row = mysql_fetch_row(Result);
	if (Result->row_count > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Database::IsValidAccount(ST_UserInfo userinfo)
{
	//ReadLock(LockDB);
	string str = "SELECT * FROM users WHERE id='" + (string)userinfo.Id + "' and pw='" + (string)userinfo.Pw + "'";
	Query(str);
	Result = mysql_store_result(pConn);
	Row = mysql_fetch_row(Result);
	if (Result->row_count > 0)
	{
		Log::PrintLog("%s is Valid Account",Row[0]);
		return true;
	}
	else 
	{
		Log::PrintLog("%s is Not Valid Account", userinfo.Id);
		return false;
	}
}
