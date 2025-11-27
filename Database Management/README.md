# Student Database System (C++ & MySQL)

A simple console-based Student Management System built with C++ and MySQL. It supports CSV import, student search, and basic encryption for sensitive data.

## Features

- **Import CSV**: Load student data from a CSV file.
- **Search Student**: Search by NRP.
  - **Public View**: Shows basic info.
  - **Admin View**: Shows decrypted medical history and emergency contacts.
- **Delete All Data**: Truncate the database table.
- **Encryption**: Uses Hex encoding + XOR for sensitive fields.

## Prerequisites

- **C++ Compiler** (MinGW recommended)
- **MySQL Server** (e.g., XAMPP)
- **MySQL Connector/C** libraries

## Setup

1.  **Database Setup**:

    - Run the SQL commands in `schema.sql` to create the database and table.

    ```sql
    source schema.sql;
    ```

2.  **Compilation**:

    - Ensure `libmysql.dll` is in the same directory as the executable.
    - Compile using `g++`:

    ```bash
    g++ main.cpp -o app_final.exe -I"C:\xampp\mysql_libs\include" -L"C:\xampp\mysql_libs\lib" -lmysql
    ```

    _(Adjust the include/lib paths to match your MySQL Connector installation)_

3.  **Run**:
    ```bash
    .\app_final.exe
    ```

## Usage

1.  Prepare your data in `students.csv` (see `students_sample.csv` for format).
2.  Run the app and select **Option 1** to import data.
3.  Use **Option 2** or **Option 3** to search.

## Security Note

This project uses a simple XOR encryption for demonstration purposes. **Do not use for highly sensitive production data.**
