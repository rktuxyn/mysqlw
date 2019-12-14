/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "mysql_connection.h"
namespace mysqlw {
	/** myssqlw_connection*/
	myssqlw_connection::myssqlw_connection() {
		_active_pools = NULL;
		errc = 0; _cinfo = NULL;
		_internal_error = new char;
		conn_state = connection_state::CLOSED;
	}

	myssqlw_connection::myssqlw_connection(const connection_details* connection_info) {
		_active_pools = NULL; errc = 0; _internal_error = new char;
		if (validate_cinfo(connection_info) < 0) {
			return;
		}
		this->_cinfo = connection_info;
		conn_state = connection_state::CLOSED;
	}

	void myssqlw_connection::panic(const char* error, int code = -1) {
		free(_internal_error);
		_internal_error = new char[strlen(error) + 1];
		strcpy(_internal_error, error);
		errc = code;
	}
	const char* myssqlw_connection::get_last_error() {
		if (errc >= 0) return "No Error Found!!!";
		return const_cast<const char*>(_internal_error);
	}
	void myssqlw_connection::close_all_connection() {
		if (conn_state == connection_state::CLOSED)return;
		if (_active_pools == NULL)return;
		connection_pool* cpool;
		for (cpool = _active_pools; cpool; cpool = cpool->next) {
			mysql_close(cpool->mysql);
		}
		while (_active_pools) {
			cpool = _active_pools;
			_active_pools = _active_pools->next;
			if (cpool->busy) {
				//fprintf(stderr,"destroying Database object before Connect object(s)\n");
			}
			delete cpool;
		}
		conn_state = connection_state::CLOSED;
		_active_pools = NULL;
	}
	void myssqlw_connection::exit_all() {
		if (_internal_error != NULL) {
			free(_internal_error); _internal_error = NULL;
		}
		close_all_connection();
	}
	myssqlw_connection::~myssqlw_connection() {
		this->_cinfo = NULL;
		this->exit_all();
	}
	int myssqlw_connection::validate_cinfo(const connection_details* cinfo) {
		if (cinfo->host == NULL || cinfo->host->empty()) {
			panic("host name required....");
			return -1;
		}
		if (cinfo->user == NULL || cinfo->user->empty()) {
			panic("user name required....");
			return -1;
		}
		if (cinfo->password == NULL || cinfo->password->empty()) {
			panic("password required....");
			return -1;
		}
		if (cinfo->database == NULL || cinfo->database->empty()) {
			panic("database name required....");
			return -1;
		}
		//cinfo->unix_socket = NULL;
		return 1;

	}
	int myssqlw_connection::connect() {
		if (this->_cinfo == NULL) {
			throw new std::exception("Connection info required...");
		}
		connection_pool* pool = create_connection_pool();/** open one connection*/
		if (errc < 0)return errc;
		free_connection_pool(pool);
		return errc;
	}
	int myssqlw_connection::connect(const connection_details* connection_info) {
		if (conn_state == connection_state::OPEN) {
			close_all_connection(); this->_cinfo = NULL;
		}
		if (validate_cinfo(connection_info) < 0) {
			return errc;
		}
		this->_cinfo = connection_info;
		connection_pool* cpool = create_connection_pool();/** open one connection*/
		if (errc < 0)return errc;
		free_connection_pool(cpool);
		return errc;
	}
	connection_pool* myssqlw_connection::create_connection_pool() {
		connection_pool* cpool = NULL;
		if (_active_pools != NULL) {
			for (cpool = _active_pools; cpool; cpool = cpool->next) {
				if (!cpool->busy) {
					break;
				}
			}
			if (cpool && cpool->busy) {
				cpool = NULL;
			}
		}
		if (!cpool) {
			cpool = new connection_pool;
			std::string err_str("");
			cpool->mysql = mysql_init(NULL);
			if (cpool->mysql == NULL) {
				panic("mysql_init() failed\n");
				return cpool;
			}
			if (!mysql_real_connect(
				/*MYSQL *mysql*/cpool->mysql,
				/*const char *host*/_cinfo->host->c_str(),
				/*const char *user*/_cinfo->user->c_str(),
				/*const char *passwd*/_cinfo->password->c_str(),
				/*const char *db*/_cinfo->database->c_str(),
				/*unsigned int port*/_cinfo->port,
				/*const char *unix_socket*/NULL,
				/*unsigned long clientflag*/_cinfo->clientflag
			)) {
				err_str.append("Conection error:").append(mysql_error(cpool->mysql));
				panic(err_str.c_str(), -2);
				return cpool;
			}
			if (mysql_select_db(cpool->mysql, _cinfo->database->c_str())) {
				err_str.append("mysql_select_db(").append(_cinfo->host->c_str()).append(",")
					.append(_cinfo->user->c_str()).append(",")
					.append(_cinfo->password->c_str()).append(",")
					.append(_cinfo->database->c_str()).append(") failed\n");
				panic(err_str.c_str(), -3);
				return cpool;
			}
			cpool->busy = 1;
			cpool->next = _active_pools;
			_active_pools = cpool;
			if (conn_state == connection_state::CLOSED) {
				conn_state = connection_state::OPEN;
			}
		}
		else {
			cpool->busy++;
		}
		return cpool;
	}

	void myssqlw_connection::free_connection_pool(connection_pool* cpool) {
		cpool->busy = 0;
	}

	int myssqlw_connection::errcode() {
		return errc;
	}
}