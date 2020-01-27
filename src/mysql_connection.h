/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_mysql_connection_h)
#	define _mysql_connection_h
#pragma warning(disable : 4996)
#	include "mysql_global.h"
namespace mysqlw {
	class mysqlw_export mysqlw_connection {
	public:
		connection_state conn_state;
	public:
		mysqlw_connection();
		mysqlw_connection(const connection_details* connection_info);
		~mysqlw_connection();
		int connect(const connection_details* connection_info);
		int connect();
		connection_pool* create_connection_pool();
		void free_connection_pool(connection_pool* cpool);
		void exit_all();
		void exit_nicely(connection_pool* cpool);
		void close_all_connection();
		int errcode();
		int switch_database(const connection_details* connection_info);
		const char* get_last_error();
		int errc;
	protected:
		const connection_details* _cinfo;
		int validate_cinfo(const connection_details* cinfo);
		connection_pool* _active_pools;
		char* _internal_error;
		void panic(const char* error, int code);
	};
}
#endif//!_mysql_connection_h
