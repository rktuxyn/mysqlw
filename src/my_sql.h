/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
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
	my_sql();
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
