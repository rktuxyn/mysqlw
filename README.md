# mysql_wrapper (C++)
MySQL C++ Wrapper for web_jsx (FCGI/CGI Application)<br/>
```c++
#include <my_sql.h>

my_sql* sql = new my_sql();

connection_details* con = new connection_details();
con->database = new std::string("test");
con->host = new std::string("localhost");
con->user = new std::string("root");
con->password = new std::string("*****");
con->unix_socket = NULL;
con->port = 0;
con->clientflag = 0;

if (sql->connect( con ) == connection_state::CLOSED) {
	std::cout << sql->get_last_error();
}

int ret = sql.execute("CREATE TABLE Persons ( PersonID int,LastName varchar(255),FirstName varchar(255),Address varchar(255), City varchar(255))");

if ( ret < 0 ) {
	std::cout << sql->get_last_error();
}

int ret = sql->execute(query_str.c_str(), [](int index, std::vector<char*>& rows) {
	for (std::vector<char*>::iterator i = rows.begin(); i != rows.end(); ++i){
		std::cout << *i;
	}
	return;
});

if ( ret < 0 ) {
	std::cout << sql->get_last_error();
}

sql->close_all_connection();

sql->exit_all();

```
