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