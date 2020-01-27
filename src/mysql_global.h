/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_mysql_global_h)
#	define _mysql_global_h
#pragma warning(disable : 4996)
//5:27 PM 12/14/2019
#	include <winsock2.h>
#	include <mysql.h>
#	include <iostream>
#	include <exception>
#if !defined(mysqlw_export)
#	define mysqlw_export __declspec(dllexport)
#endif//!mysqlw_export
#	include "connection_state.h"
namespace mysqlw {
	/** opendbstruct - database connection handle*/
	typedef struct conn_pool {
		struct conn_pool* next;    /* pointer to next member*/
		MYSQL* conn;                  /* MySQL connection handle*/
		connection_state conn_state;
		int busy;                   /* connection busy flag*/
		int error_code;
		const char* error_msg;
	}connection_pool;
	typedef struct {
		std::string* host;
		std::string* user;
		std::string* password;
		std::string* database;
		unsigned int port;
		const char* unix_socket;
		unsigned long clientflag;
	}connection_details;
}
#endif//!_mysql_global_h
//