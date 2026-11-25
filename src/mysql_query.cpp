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
#	include "mysql_query.h"
namespace mysqlw {
	mysqlw_query::mysqlw_query(connection_pool* cpool) {
		_cpool = cpool;
		_res = NULL; _row = NULL;
	}
	mysqlw_query::~mysqlw_query() {
		if (_res) {
			mysql_free_result(_res);
		}
		free_connection();
	}
	void mysqlw_query::free_connection() {
		if (_cpool) {
			_cpool->busy = 0;
			_cpool = NULL;
		}
	}
	void mysqlw_query::exit_nicely() {
		if (_cpool) {
			mysql_close(_cpool->conn);
			_cpool->error_msg = NULL;
			delete _cpool->conn; _cpool->conn = NULL;
			_cpool->busy = -1;
		}
	}
	int mysqlw_query::try_execute(const char* sql) {
		if ( _cpool && !_res) {
			if (!mysql_query(_cpool->conn, sql)) {
				return 1;
			}
		}
		return 0;
	}
	const char* mysqlw_query::get_mysql_eror() {
		return mysql_error(_cpool->conn);
	}
	int mysqlw_query::execute_query(const char* sql) {
		if ( _cpool && !_res) {
			int rs = mysql_query(_cpool->conn, sql);
			if (mysql_errno(_cpool->conn) /*|| mysql_warning_count(_cpool->mysql)*/) {
				return -1;
			}
			_res = mysql_store_result(_cpool->conn);
			return rs;
		}
		return -1;
	}
	const char* mysqlw_query::get_first_col_val() {
		if ( _cpool && _res) {
			if (mysql_num_rows(_res) == 0) return NULL;
			int n_fields = mysql_num_fields(_res);
			if (n_fields == 0)return NULL;
			MYSQL_ROW row = mysql_fetch_row(_res);
			char* resp = row[0];
			const char* result = const_cast<const char*>(resp);
			row = NULL;
			return result;
		}
		return NULL;
	}
	MYSQL_RES* mysqlw_query::_execute(const char* sql) {
		if ( _cpool && !_res) {
			if (try_execute(sql)) {
				_res = mysql_store_result(_cpool->conn);
			}
		}
		return _res;
	}
	void mysqlw_query::free_result() {
		if ( _cpool && _res) {
			mysql_free_result(_res);
			_res = 0; _row = NULL;
		}
	}
	__int64 mysqlw_query::insert_id() {
		if ( _cpool) {
			return mysql_insert_id(_cpool->conn);
		}
		return -1;
	}
	MYSQL_FIELD* mysqlw_query::fetch_field() {
		if ( _cpool && _res) {
			return mysql_fetch_field(_res);
		}
		return 0;
	}
	char* mysqlw_query::fetch_fieldname() {
		MYSQL_FIELD* field = 0;

		if (_cpool && _res) {
			field = mysql_fetch_field(_res);
		}
		if (field == NULL)return new char;
		return field->name;
	}
	int mysqlw_query::ping() {
		if (_cpool) {
			return mysql_ping(_cpool->conn);
		}
		return -1;
	}
}