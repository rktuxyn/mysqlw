/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "mysql_query.h"
namespace mysqlw {
	mysqlw_query::mysqlw_query() {
		_sql_connection = 0;
		_cpool = NULL; _res = NULL; _row = NULL; _rowcount = 0;
		//_results.set_size(0);
	}
	mysqlw_query::mysqlw_query(myssqlw_connection* __sql_connection) {
		_sql_connection = __sql_connection;
		_cpool = _sql_connection->create_connection_pool();
		_res = NULL; _row = NULL;
		_rowcount = 0;
		//_results.set_size(0);
	}
	mysqlw_query::~mysqlw_query() {
		if (_res) {
			mysql_free_result(_res);
		}
		if (_cpool) {
			_sql_connection->free_connection_pool(_cpool);
			_cpool = NULL;
		}
	}
	void mysqlw_query::free_connection() {
		if (_cpool) {
			_sql_connection->free_connection_pool(_cpool);
			_cpool = NULL;
		}
	}
	int mysqlw_query::try_execute(const char* sql) {
		if (_sql_connection && _cpool && !_res) {
			if (!mysql_query(_cpool->mysql, sql)) {
				return 1;
			}
		}
		return 0;
	}
	const char* mysqlw_query::get_mysql_eror() {
		return mysql_error(_cpool->mysql);
	}
	int mysqlw_query::execute_query(const char* sql) {
		if (_sql_connection && _cpool && !_res) {
			int rs = mysql_query(_cpool->mysql, sql);
			if (mysql_errno(_cpool->mysql) || mysql_warning_count(_cpool->mysql)) {
				return -1;
			}
			_res = mysql_store_result(_cpool->mysql);
			return rs;
		}
		return -1;
	}
	MYSQL_RES* mysqlw_query::_execute(const char* sql) {
		if (_sql_connection && _cpool && !_res) {
			if (try_execute(sql)) {
				_res = mysql_store_result(_cpool->mysql);
			}
		}
		return _res;
	}
	void mysqlw_query::free_result() {
		if (_sql_connection && _cpool && _res) {
			mysql_free_result(_res);
			_res = 0; _row = NULL;
		}
	}
	__int64 mysqlw_query::insert_id() {
		if (_sql_connection && _cpool) {
			return mysql_insert_id(_cpool->mysql);
		}
		return -1;
	}
	MYSQL_FIELD* mysqlw_query::fetch_field() {
		if (_sql_connection && _cpool && _res) {
			return mysql_fetch_field(_res);
		}
		return 0;
	}
	char* mysqlw_query::fetch_fieldname() {
		MYSQL_FIELD* field = 0;

		if (_sql_connection && _cpool && _res) {
			field = mysql_fetch_field(_res);
		}
		if (field == NULL)return new char;
		return field->name;
	}
	int mysqlw_query::ping() {
		if (_cpool) {
			return mysql_ping(_cpool->mysql);
		}
		return -1;
	}
}