#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_my_sql_h)
#define _my_sql_h
#if !defined(_WINSOCK2API_)
#include <winsock2.h>
#endif//!_WINSOCK2API_
#if !defined(_mysql_h)
#include <mysql.h>
#endif//_mysql_h
#if !defined(mysqlw_export)
#define mysqlw_export __declspec(dllexport)
#endif//!mysqlw_export
#if !defined(_connection_state_h)
#include "connection_state.h"
#endif//!_connection_state_h
namespace mysqlw {
	typedef struct conn_pool {
		struct conn_pool* next;    /* pointer to next member*/
		MYSQL* mysql;                  /* MySQL connection handle*/
		int busy;                   /* connection busy flag*/
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
	class mysqlw_export myssqlw_connection {
	public:
		connection_state conn_state;
	public:
		myssqlw_connection();
		myssqlw_connection(const connection_details* connection_info);
		~myssqlw_connection();
		int connect(const connection_details* connection_info);
		int connect();
		connection_pool* create_connection_pool();
		void free_connection_pool(connection_pool* cpool);
		void exit_all();
		void close_all_connection();
		int errcode();
		const char* get_last_error();
		int errc;
	protected:
		const connection_details* _cinfo;
		int validate_cinfo(const connection_details* cinfo);
		connection_pool* _active_pools;
		char* _internal_error;
		void panic(const char* error, int code);
	};
	class mysqlw_export mysqlw_query {
	public:
		mysqlw_query();
		mysqlw_query(myssqlw_connection* sql_connection);
		~mysqlw_query();
		int try_execute(const char* sql);
		const char* get_mysql_eror();
		MYSQL_RES* _execute(const char* sql);
		int execute_query(const char* sql);
		void free_result();
		void free_connection();
		__int64 insert_id();
		MYSQL_FIELD* fetch_field();
		char* fetch_fieldname();
		int ping();
	protected:
		myssqlw_connection* _sql_connection;
		connection_pool* _cpool;
		MYSQL_RES* _res;
		MYSQL_ROW _row;
		int _rowcount;
	};
}
class mysqlw_export my_sql {
public:
	my_sql();
	~my_sql();
	connection_state connect(mysqlw::connection_details* connection_info);
	connection_state connect();
	connection_state state();
	const char* get_last_error();
	template<class _func>
	int execute(const char* sql, _func func);
	int execute(const char* sql);
	void exit_all();
	void close_all_connection();
private:
	char* _internal_error;
	int _errc;
	void panic(const char* error, int code);
	mysqlw::myssqlw_connection* _con;
	mysqlw::connection_details* _connection_inf;
};
template<class _func>
int my_sql::execute(const char* sql, _func func) {
	if (this->state() == connection_state::CLOSED) {
		this->panic("Connection not initilized yet...", -1);
		return _errc;
	}
	mysqlw::mysqlw_query* query = new mysqlw::mysqlw_query(this->_con);
	MYSQL_RES* res = query->_execute(sql);
	if (res == NULL) {
		this->panic(query->get_mysql_eror(), -1);
		delete query;
		return _errc;
	}
	if (res->row_count == 0) {
		//this->panic("Can not display list of errors/warnings", -1);
		query->free_result();
		delete query;
		return 0;
	}
	int count = 0;
	int n_fields = mysql_num_fields(res);
	if (n_fields != 0) {
		size_t len = 0;
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(res)) != NULL) {
			std::vector<char*>* rows = new std::vector<char*>();
			for (int i = 0; i < n_fields; i++) {
				char* c = row[i];
				len = strlen(c);
				char* copy = new char[len + 1];
				strcpy(copy, c);
				rows->push_back(copy);
			}
			func(count, *rows); count++;
			rows->clear(); delete rows;
			row = NULL;
		}
	}
	query->free_result(); query->free_connection();
	delete query; res = NULL;
	return count;
}
#endif//!_my_sql_h