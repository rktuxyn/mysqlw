/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "my_sql.h"

my_sql::my_sql(){
	_con = NULL;
	_errc = 0;
	_internal_error = new char;
	_connection_inf = NULL;
}
void my_sql::clear_conn_info() {
	if (_connection_inf != NULL) {
		if (_connection_inf->host != NULL)
			delete _connection_inf->host;
		if (_connection_inf->database != NULL)
			delete _connection_inf->database;
		if (_connection_inf->user != NULL)
			delete _connection_inf->user;
		if (_connection_inf->password != NULL)
			delete _connection_inf->password;
		delete _connection_inf;
		_connection_inf = NULL;
	}
}
my_sql::~my_sql(){
	this->clear_conn_info();
	this->exit_all();
}
connection_state my_sql::connect() {
	if (state() != connection_state::CLOSED)return _con->conn_state;
	if (_con == NULL) {
		if (_connection_inf == NULL) {
			panic("Not initilized...", -1);
			return connection_state::CLOSED;
		}
		_con = new mysqlw::mysqlw_connection();
		_con->connect(_connection_inf);
	}
	else {
		_con->connect();
	}
	if (_con->conn_state == connection_state::CLOSED) {
		panic(_con->get_last_error(), _con->errc);
		return _con->conn_state;
	}
	return _con->conn_state;
}
connection_state my_sql::connect(mysqlw::connection_details* connection_info){
	if (_con != NULL) {
		this->exit_all();
		this->clear_conn_info();
	}
	_con = new mysqlw::mysqlw_connection();
	_con->connect(connection_info);
	if (_con->conn_state == connection_state::CLOSED) {
		panic(_con->get_last_error(), _con->errc);
		return _con->conn_state;
	}
	_connection_inf = connection_info;
	return _con->conn_state;
}

connection_state my_sql::state(){
	return _con == NULL ? connection_state::CLOSED: _con->conn_state;
}

const char* my_sql::get_last_error(){
	if (_errc >= 0) return "No Error Found!!!";
	return const_cast<const char*>(_internal_error);
}
void my_sql::close_all_connection() {
	if (state() == connection_state::CLOSED)return;
	_con->close_all_connection();
}
int my_sql::switch_database(const char* database_name){
	if (state() == connection_state::CLOSED) {
		this->panic("No active connectio found...", -1);
		return -1;
	}
	if (_connection_inf == NULL || (_connection_inf != NULL && _connection_inf->database->empty())) {
		panic("Connection information not found...", -1);
		return _errc;
	}
	_connection_inf->database->clear(); delete _connection_inf->database;
	_connection_inf->database = new std::string(database_name);
	return _con->switch_database(_connection_inf);
}
bool my_sql::has_error() {
	return _errc < 0;
}
const char* my_sql::execute(const char* sql){
	const char* result = '\0';
	if (state() == connection_state::CLOSED) {
		this->panic("No active connection state found..", -1);
		return result;
	}
	mysqlw::connection_pool* cpool = this->_con->create_connection_pool();
	if (cpool->error_code < 0) {
		this->panic(cpool->error_msg, cpool->error_code);
		this->_con->exit_nicely(cpool);
		return result;
	}
	mysqlw::mysqlw_query* query = new mysqlw::mysqlw_query( cpool );
	int rec = query->execute_query(sql);
	if (rec < 0) {
		this->panic(query->get_mysql_eror(), -1);
	}
	else {
		result = query->get_first_col_val();
	}
	query->free_result(); query->free_connection();
	delete query;
	return result;
}
void my_sql::exit_all(){
	if (state() == connection_state::CLOSED)return;
	_con->exit_all(); delete _con; _con = NULL;
	free(_internal_error);
}

void my_sql::panic(const char* error, int code = -1) {
	free(_internal_error);
	_internal_error = new char[strlen(error) + 1];
	strcpy(_internal_error, error);
	_errc = code;
}
