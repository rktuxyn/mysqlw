/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_mysql_query_h)
#define _mysql_query_h
#if !defined(_mysql_global_h)
#include "mysql_global.h"
#endif//!_mysql_global_h
#if !defined(_mysql_connection_h)
#include "mysql_connection.h"
#endif//!_mysql_connection_h
#pragma warning(disable : 4996)
namespace mysqlw {
	class mysqlw_export mysqlw_query {
	public:
		mysqlw_query();
		mysqlw_query(mysqlw_connection* sql_connection);
		~mysqlw_query();
		int try_execute(const char* sql);
		const char* get_mysql_eror();
		MYSQL_RES* _execute(const char* sql);
		int execute_query(const char* sql);
		const char* get_fast_col_val();
		void free_result();
		void free_connection();
		__int64 insert_id();
		MYSQL_FIELD* fetch_field();
		char* fetch_fieldname();
		int ping();
	protected:
		mysqlw_connection* _sql_connection;
		connection_pool* _cpool;
		MYSQL_RES* _res;
		MYSQL_ROW _row;
	};
}
#endif//!_mysql_query_h
