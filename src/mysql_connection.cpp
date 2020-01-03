/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "mysql_connection.h"
namespace mysqlw {
	/** mysqlw_connection*/
	mysqlw_connection::mysqlw_connection() {
		_active_pools = NULL;
		errc = 0; _cinfo = NULL;
		_internal_error = new char;
		conn_state = connection_state::CLOSED;
	}

	mysqlw_connection::mysqlw_connection(const connection_details* connection_info) {
		_active_pools = NULL; errc = 0; _internal_error = new char;
		if (validate_cinfo(connection_info) < 0) {
			return;
		}
		this->_cinfo = connection_info;
		conn_state = connection_state::CLOSED;
	}

	void mysqlw_connection::panic(const char* error, int code = -1) {
		free(_internal_error);
		_internal_error = new char[strlen(error) + 1];
		strcpy(_internal_error, error);
		errc = code;
	}
	
	const char* mysqlw_connection::get_last_error() {
		if (errc >= 0) return "No Error Found!!!";
		return const_cast<const char*>(_internal_error);
	}
	
	void mysqlw_connection::close_all_connection() {
		if (conn_state == connection_state::CLOSED)return;
		if (_active_pools == NULL)return;
		connection_pool* cpool;
		for (cpool = _active_pools; cpool; cpool = cpool->next) {
			mysql_close(cpool->conn);
		}
		while (_active_pools) {
			cpool = _active_pools;
			_active_pools = _active_pools->next;
			if (cpool->busy) {
				//fprintf(stderr,"destroying Database object before Connect object(s)\n");
			}
			cpool->conn = NULL;
			delete cpool;
		}
		conn_state = connection_state::CLOSED;
		if (_active_pools != NULL)
			delete _active_pools;
		_active_pools = NULL;
	}
	
	void mysqlw_connection::exit_all() {
		if (_internal_error != NULL) {
			free(_internal_error); _internal_error = NULL;
		}
		close_all_connection();
	}

	void mysqlw_connection::exit_nicely(connection_pool* cpool){
		if (cpool == NULL || (cpool != NULL && cpool->conn == NULL))return;
		mysql_close(cpool->conn); delete cpool->conn; cpool->conn = NULL;
		cpool->busy = -1;
		cpool->conn_state = connection_state::CLOSED;
	}
	
	mysqlw_connection::~mysqlw_connection() {
		this->_cinfo = NULL;
		this->exit_all();
	}
	
	int mysqlw_connection::validate_cinfo(const connection_details* cinfo) {
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
	
	int mysqlw_connection::connect() {
		if (this->_cinfo == NULL) {
			throw new std::exception("Connection info required...");
		}
		connection_pool* cpool = create_connection_pool();/** open one connection*/
		if (errc < 0)return errc;
		free_connection_pool(cpool);
		return errc;
	}
	
	int mysqlw_connection::connect(const connection_details* connection_info) {
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
	
	connection_pool* mysqlw_connection::create_connection_pool() {
		connection_pool* cpool = NULL;
		if (_active_pools != NULL) {
			for (cpool = _active_pools; cpool; cpool = cpool->next) {
				if (cpool->busy < 0 || !cpool->busy) break;
			}
			if (cpool && cpool->busy) {
				cpool = NULL;
			}
		}
		if (!cpool || (cpool != NULL && (cpool->busy < 0 || cpool->conn_state == connection_state::CLOSED))) {
			if (!cpool) {
				cpool = new connection_pool;
			}
			std::string err_str("");
			cpool->conn = mysql_init(NULL);
			if (cpool->conn == NULL) {
				panic("mysql_init() failed\n");
				return cpool;
			}
			if (!mysql_real_connect(
				/*MYSQL *mysql*/cpool->conn,
				/*const char *host*/_cinfo->host->c_str(),
				/*const char *user*/_cinfo->user->c_str(),
				/*const char *passwd*/_cinfo->password->c_str(),
				/*const char *db*/_cinfo->database->c_str(),
				/*unsigned int port*/_cinfo->port,
				/*const char *unix_socket*/NULL,
				/*unsigned long clientflag*/_cinfo->clientflag
			)) {
				err_str.append("Conection error:").append(mysql_error(cpool->conn));
				cpool->error_code = -2;
				cpool->error_msg = err_str.c_str();
				panic(err_str.c_str(), -2);
				cpool->conn_state = connection_state::CLOSED;
			}
			else {
				cpool->conn_state = connection_state::OPEN;
			}
			if (cpool->conn_state == connection_state::OPEN) {
				if (mysql_select_db(
					/*MYSQL *mysql*/cpool->conn,
					/*const char *db*/_cinfo->database->c_str()
				)) {
					err_str.append("mysql_select_db(").append(_cinfo->host->c_str()).append(",")
						.append(_cinfo->user->c_str()).append(",")
						.append(_cinfo->password->c_str()).append(",")
						.append(_cinfo->database->c_str()).append(") failed\n");
					cpool->error_code = -3;
					cpool->error_msg = err_str.c_str();
					panic(err_str.c_str(), -3);
					cpool->conn_state = connection_state::CLOSED;
				}
				else {
					cpool->conn_state = connection_state::OPEN;
				}
			}
			if (cpool->conn_state == connection_state::OPEN) {
				cpool->error_code = 0;
				cpool->error_msg = NULL;
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

	void mysqlw_connection::free_connection_pool(connection_pool* cpool) {
		cpool->busy = 0;
		cpool->error_code = 0;
		cpool->error_msg = NULL;
	}

	int mysqlw_connection::errcode() {
		return errc;
	}
	int mysqlw_connection::switch_database(const connection_details* connection_info){
		if (conn_state == connection_state::CLOSED ) {
			panic("No active connectio found...");
			return errc; 
		}
		if (_active_pools == NULL) {
			panic("No active connectio found...");
			return errc;
		}
		if (validate_cinfo(connection_info) < 0) {
			return errc;
		}
		connection_pool* cpool;
		while (_active_pools) {
			if (_active_pools->next == NULL) {
				if (_active_pools->busy) {
					mysql_close(_active_pools->conn);
					_active_pools = _active_pools->next;
					continue;
				}
				break;
			}
			cpool = _active_pools;
			_active_pools = _active_pools->next;
			delete cpool;
		}
		_cinfo = NULL; _cinfo = connection_info;
		if (_active_pools == NULL) {
			return connect();
		}
		std::string err_str("");
		if (mysql_select_db(
			/*MYSQL *mysql*/_active_pools->conn,
			/*const char *db*/_cinfo->database->c_str()
		)) {
			err_str.append("mysql_select_db(")
				.append(_cinfo->host->c_str()).append(",")
				.append(_cinfo->user->c_str()).append(",")
				.append(_cinfo->password->c_str()).append(",")
				.append(_cinfo->database->c_str()).append(") failed\n");
			panic(err_str.c_str(), -3);
			close_all_connection();
			return errc;
		}
		return 0;
	}
}