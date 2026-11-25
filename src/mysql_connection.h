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
#if !defined(_mysql_connection_h)
#	define _mysql_connection_h
#pragma warning(disable : 4996)
#	include "mysql_global.h"
namespace mysqlw {
	class mysqlw_export mysqlw_connection {
	public:
		connection_state conn_state;
	public:
		explicit mysqlw_connection();
		explicit mysqlw_connection(const connection_details* connection_info);
		~mysqlw_connection();
		int connect(const connection_details* connection_info);
		int connect();
		connection_pool* create_connection_pool();
		void free_connection_pool(connection_pool* cpool);
		void exit_all();
		void exit_nicely(connection_pool* cpool);
		void close_all_connection();
		//int errcode();
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
