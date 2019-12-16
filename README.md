# Multi Connection Pool MySQL C++ Wrapper for web_jsx (FCGI/CGI Application)
#Include header file
```c++
#include <my_sql.h>
```
Initilize my_sql instance
```c++
my_sql* sql = new my_sql();
```
Create connection detail
```c++
connection_details* con = new connection_details();
con->database = new std::string("test");
con->host = new std::string("localhost");
con->user = new std::string("root");
con->password = new std::string("*****");
con->unix_socket = NULL;
con->port = 0;
con->clientflag = 0;
```
Connect MySQL with this connection detail
```c++
if (sql->connect( con ) == connection_state::CLOSED) {
	std::cout << sql->get_last_error();
}
```
Drop Database
```c++
mysql->execute( 'DROP DATABASE IF EXISTS web_jsx_db' );
```
Create Database
```c++
mysql->execute( 'CREATE DATABASE IF NOT EXISTS web_jsx_db' );
```
Switch Database
```c++
mysql->switch_database("web_jsx_db");
```
Execute plain sql statement
```c++
sql->execute("CREATE TABLE IF NOT EXISTS Persons ( PersonID int,LastName varchar(255),FirstName varchar(255),Address varchar(255), City varchar(255))");
```
Read the status of current execution
```c++
if ( sql->has_error() ) {
  std::cout << sql->get_last_error();
}
```
Return query result
```c++
const char* ret = sql->execute('select Address from Persons limit 1');
```
Execute plain sql statement as like as data reader
```c++
int ret = sql->execute(select * from Persons, [](int index, std::vector<char*>& rows) {
	for (std::vector<char*>::iterator i = rows.begin(); i != rows.end(); ++i){
		std::cout << *i;
	}
	return;
});

if ( ret < 0 ) {
	std::cout << sql->get_last_error();
}
```
Close all connection pool
```c++
sql->close_all_connection();
```
Clear MySQL all connection pool and instance
```c++
sql->exit_all();

```
