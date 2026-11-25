# MySQLW

MySQLW is a lightweight, high-performance C++ wrapper for MySQL, designed to simplify database interactions while maintaining efficiency and flexibility.

## Features

- Simple and clean API for MySQL operations
- High-performance query execution
- Secure parameterized queries
- Connection pooling for efficient resource management
- Lightweight and dependency-free
- Support for asynchronous operations (if applicable)

## Installation

### Prerequisites

- MySQL client library (`libmysqlclient`)
- C++17 or later
- CMake (for building the project)

### Build and Install

```sh
git clone https://github.com/fsystech/mysqlw.git
cd mysqlw
mkdir build && cd build
cmake ..
make
sudo make install
```

## Usage

### Basic Example

```cpp
#include "mysqlw.h"

int main() {
    mysqlw::Connection conn("host", "user", "password", "database");
    if (!conn.isConnected()) {
        std::cerr << "Failed to connect to database" << std::endl;
        return 1;
    }

    auto result = conn.query("SELECT * FROM users WHERE id = ?", 1);
    for (auto& row : result) {
        std::cout << "User ID: " << row["id"].as<int>() << "\n";
    }
    return 0;
}
```

## API Reference

### `mysqlw::Connection`
- `Connection(const std::string& host, const std::string& user, const std::string& password, const std::string& database)` - Initializes the connection.
- `bool isConnected() const` - Checks if the connection is active.
- `QueryResult query(const std::string& sql, Args... args)` - Executes a query with optional parameters.

### `mysqlw::QueryResult`
- `bool empty() const` - Checks if the result set is empty.
- `size_t size() const` - Returns the number of rows.
- `Row operator[](size_t index) const` - Access a specific row.

### `mysqlw::Row`
- `Column operator[](const std::string& columnName) const` - Access a specific column.
- `template<typename T> T as() const` - Convert the column value to the desired type.

## License

This project is licensed under the MIT License.

## Contributing

Contributions are welcome! Feel free to submit issues or pull requests to improve MySQLW.

## Contact

For any inquiries, reach out to [FSys Tech Ltd](https://github.com/fsystech).

