/**
* MIT License
*
* Copyright (c) 2022 FSys Tech Ltd.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_my_sql_h)
#	define _my_sql_h
#	include "mysql_connection.h"
#	include "mysql_query.h"
#	include <vector>
#if !defined(mysqlw_export)
#	define mysqlw_export __declspec(dllexport)
#endif//!mysqlw_export
#pragma warning(disable : 4996)
class mysqlw_export my_sql {
public:
	explicit my_sql();
	~my_sql();
	connection_state connect(mysqlw::connection_details* connection_info);
	connection_state connect();
	connection_state state();
	const char* get_last_error();
	template<class _func>
	int execute(const char* sql, _func func);
	const char* execute(const char* sql);
	bool has_error();
	void exit_all();
	void close_all_connection();
	int switch_database(const char* database_name);
private:
	char* _internal_error;
	int _errc;
	void panic(const char* error, int code);
	mysqlw::mysqlw_connection* _con;
	mysqlw::connection_details* _connection_inf;
	void clear_conn_info();
};
#endif//!_my_sql_h
