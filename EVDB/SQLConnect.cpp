// Windows API for general Windows functionality
#include <windows.h>

// SQL data types and basic SQL functionality
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>

// Input/output stream functionality
#include <iostream>
#include <fstream>
#include <sstream>

// Time-related functionality
#include <chrono>
#include <ctime>

// String manipulation and formatting
#include <iomanip>
#include <string>
#include <algorithm>

// Data storage and manipulation
#include <vector>
#include <cstdlib>
#include <mutex>

// COM object libraries
#include <oaidl.h>
#include <ole2.h>
#include <olectl.h>
#include <comdef.h>
#include <Objbase.h>

// Define a mutex
std::mutex csvMutex;

// Define SQLTCHAR as SQLCHAR (unsigned char) for compatibility with ODBC functions
#define SQLTCHAR SQLCHAR

// Link against the ODBC library
#pragma comment(lib, "odbc32.lib")

// Define the CLSID of Excel Application
// {00024500-0000-0000-C000-000000000046}
const CLSID CLSID_Application = {0x00024500, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

//To compile:
    //g++ -g SQLConnect.cpp -o SQLConnect.exe -lodbc32
    //g++ -g SQLConnect.cpp -o SQLConnect.exe -lodbc32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32 -luser32 -lkernel32

//TO DO:
// 3. Combine CSV files as one workbook
// 4. Correct export option to write to fresh sheet every time
// 5. Setup automatic synchronization
// 6. Point export to Excel workbook on OneDrive

// Define SQLTCHAR as SQLCHAR (unsigned char) for compatibility with ODBC functions
#define SQLTCHAR SQLCHAR

// Link against the ODBC library
#pragma comment(lib, "odbc32.lib")

//Recursive script A to flex on the haters
void printUniversityAsciiArt() {
    std::cout << R"(
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#/*,...  ..,*(%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%.                                           ,&@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@/               .(%%%%%%%%%%%%%%%%%%%%%%%%%%%/.               %@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%            @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#         /%%%%%%%%%%%%%%%%%%%%%%%%%%%%#/**,,%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%,         @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@        ,%%%%%%%%%%%%%%%%%%%%%%%%%%%,,,,,,,,,,,,,,,%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%         @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@       .%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%,,,,,,%%%,,,,,%%%%%%%%%%%%%%%%%%%%#,,,,(%%%%%%%%%%%%%%        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@       /%%%%%%%%%%%%,,,%%%%%%%%%%%%%%%%%%%%%%*,,,,,%%%,,,,,%%%%%%%%%%%%%%%%%%%,,,,,,,,,,,%%%%%%%%%%%%%%.      ,@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@      ,%%%%%%%%%%%,,,,,,,,,,%%%%%%%%%%%%%%%%%%%%/,,,,,,,,,,,,%%%%%%%%%%%%%%%%%%%,,,,,/,,,,,%%%%%%%%%%%%%%%%%%%       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@      %%%%%%%%%%%%%%%,,,,,#,,,,,,%%%%%%%%%%%%%%%%%%#,,,,,%%%,,,,,/%%%%%%%%%%%%%%%,,,,,,%%,,,,,%%%%%%%%%%%%%%%%%%%%%%(      @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@      %%%%%%%%%%%%%%%%%%%,,,,,%%,,,,,,%%%%%%%%%%%%%%%%%,,,,,%%%,,,,,,%%%%%%%%%%%%%%,,,,,,%%,,,,,(%%%%%%%%%%%%%%%%%%%%%%%%%#      @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@.     %%%%%%%%%%%%%%%%%%%%%%#,,,,,%%/,,,,,,%%%%%%%%%%%%%/,,,,,,,,,,,,,,%%%%%%%%%%%%,,,,,,,,,,,,,,,,%%%%%%%%%%%%%%%%%%%%%#%%%%%%%(     #@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@     *%%%%%%%%%%%%%%%%%%%%%%%%%,,,,,,,,,,,,,,,,,%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%,,,,,,,%%%%,,,,,,%%%%%%%%%%%%%%%%%%%,,,,,%%%%%%%%      @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@&     %%%%%%%%%%%%%%%%%%%%%%%%%%%%%,,,,,,%%%%,,,,,,%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#,,,,,,/%%%%%%%%%%%%%%%%%*,,,,,,,%%%%%%%%%     @@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@(     %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#,,,,,#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%(,,,,%%%%%%%%%%%%%%%%,,,,,,,,,%%%%%%%%%%%%     @@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@#    .%%%%%%%(%%%%%%%%%%%%%%%%%%%%%%%%%,,,,,#%%%%%%%%%%%%%%%#.                             ,%%%%%%%%%%%%%%%%%%%%%%%%%%%%%,,,,,,,,,,,/%%%%%%%%%%%%%%     @@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@     %%%%%%%,,,,,*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%(                                             #%%%%%%%%%%%%%%%%%,,,,,,,,,%,,,,,%%%%#,,,,,%%%%%%%     @@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@     %%%%%%%,,,,,,,,(%%%%%%%,,,,%%%%%%%%%%%%%%%%%%%.                                                       /%%%%%%%%%%%%,,,,,,/%%#,,,,,,,,,,,,,,,,,%%%%%%%     @@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@.    %%%%%%%%%%,,,,,,,,,,%%%%,,,,,,,,%%%%%%%%%%%%                                                                .%%%%%%%%%%*,%%%%%,,,,,,,,,,,,,,,,,(%%%%%%%%(    @@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@    ,%%%%%%%%%%%%%%%#,,,,,,,,%%,,,,,,%%%%%%%%%*                                                                       %%%%%%%%%%%%%#%%%%%%,,,,,,,%%%%%%%%%%%%%%%     @@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@/    %%%%%%%%%%%%%%%%%%%%/,,,,,,,,,,(%%%%%%%%,                                                                             (%%%%%%%%%%%%%,,,,,,,/%%%%%%%%%%%%%%%%%%%    @@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@     %%%%%%%%%%%%%%%%%%%%%%%%,,,,,,%%%%%%%%#                                                                                   %%%%%%%%%,,,,,,,%%%%%%%%%%%%%%%%%%%%%%%    *@@@@@@@@@@@@@@
@@@@@@@@@@@@@@    (%%%%%%%%%%%%%%%%%%%%%%%%%%,,,%%%%%%%%                                                                                        (%%%%%%%%,,,,#%%%%%%%%%%%%%%%%%%%%%%%%     @@@@@@@@@@@@@
@@@@@@@@@@@@@    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                                          .%                            /%%%(               .%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*    @@@@@@@@@@@@
@@@@@@@@@@@@    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                                           (%%%.                       %%%%%%%%%%               *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%(    @@@@@@@@@@@
@@@@@@@@@@@    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*                                             %%%%%%%%/            *%%%%%%%%%%%%%%                  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%(    @@@@@@@@@@
@@@@@@@@@@    %%%%%%#,,,,,,,,,,,,,,,/,,,,%%%%%%%%                                                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#                     %%%%%%%%%%%,%%%%%%%#/,,,,,,,%%%%%%,    @@@@@@@@@
@@@@@@@@@    /%%%%%%,,,,,,,,,,,,,,,,,,,,#%%%%%%/                                                     #%%%%%%%%%%%%%%%%%%%%%%%%%%%#                       %%%%%%%/,,,,,,,,,,,,,,,,,,,,%%%%%%    *@@@@@@@@
@@@@@@@@(    %%%%%%,,,,,,,,%%%%,,,*,,,,(%%%%%%                                                                   %%%%%%%%%%%%%%%%                         #%%%%%%,,,,,,,,,,,,(%%%,,,,,%%%%%%    @@@@@@@@
@@@@@@@@    %%%%%%%%%,,,,,,,,,,,,*%%%%%%%%%%%                                                                  %%%%%%%%%%%%%%%%%                           *%%%%%%%%%%%,,,,%,,,,,,,,,,#%%%%%%    @@@@@@@
@@@@@@@    (%%%%%%%%%%%%%*,,,,,,,,,,*%%%%%%%                                                                 %%%%%%%%, %%%%%%%%                             /%%%%%%%%%#,,,,,,,,,,,%%%%%%%%%%%    #@@@@@@
@@@@@@@    %%%%%%%%%%%%%%%%%%#,,,,,,,%%%%%%,                                                               (%%%%%%%,  .%%%%%%%.                              %%%%%%%,,,,,,,,,#%%%%%%%%%%%%%%%%    @@@@@@
@@@@@@.   /%%%%%%%%%%%%%%%%%%%%%,,,,%%%%%%%                                                              .%%%%%%%#    %%%%%%%%                                %%%%%%#,,,,%%%%%%%%%%%%%%%%%%%%%    &@@@@@
@@@@@@    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                                                              %%%%%%%%     %%%%%%%%                                 ,%%%%%%(%%%%%%%%%%%%%%%%%%%%%%%%#    @@@@@
@@@@@@    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                                                            %%%%%%%%/      %%%%%%%/                                  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    @@@@@
@@@@@*   ,%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                                                           ,%%%%%%%%       %%%%%%%%                                   (%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    @@@@@
@@@@@    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                                                 .        %%%%%%%%%       /%%%%%%%,                                    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%.   %@@@@
@@@@@    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                                              .%%%      (%%%%%%%%         %%%%%%%%%%%%%%(                              %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%/   *@@@@
@@@@@    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                                              %%%%%    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                             %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%/   ,@@@@
@@@@@    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                                             (%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                             %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%/   *@@@@
@@@@@    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                                              %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#**%%%%.                             %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%.   %@@@@
@@@@@*   ,%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                                                %%%%%%%%%%%%%%%%#/      (%%%%%%%%     #                               (%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    @@@@@
@@@@@@    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                                                 %%%%%%%%%             %%%%%%%%#                                     %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    @@@@@
@@@@@@    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                                               ,%%%%%%%%(              %%%%%%%%                                     ,%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#    @@@@@
@@@@@@.   /%%%%%%%%%%%%%%%%%%*,,,,,,%%%%%%%                  *%%                        %%%%%%%%%                %%%%%%%%                                     %%%%%%%,,,*%%%%%%%%%%%%%%%%%%%%%    &@@@@@
@@@@@@@    %%%%%%%%%(,,,,,,,,,,,,,,,,%%%%%%,               /%%%                        %%%%%%%%%                 %%%%%%%%                                    %%%%%%%,,,,,,%%%%%%%(%%%%%%%%%%%%    @@@@@@
@@@@@@@    (%%%%%*,,,,,,,,,,(%%%%*,,,,%%%%%%              *%%%%                      %%%%%%%%%                   %%%%%%%%                                   /%%%%%%,,,,#%%%,,,,%%,,,,,,,%%%%%    #@@@@@@
@@@@@@@@    %%%%%%,,,,%%%%%%%%%%,,,,,,#%%%%%%             %%%%%%                   %%%%%%%%%,                    %%%%%%%%*                                 *%%%%%%,,,,,,,,,,,,%%%%%,,,,%%%%%%    @@@@@@@
@@@@@@@@(    %%%%%(,,,,,,,%%%%%%*,#%%%%%%%%%%%            .%%%%%%%              *%%%%%%%%%/                      %%%%%%%%%            %                   #%%%%%%#,,,,,,,,,,,,,,,,,,,,%%%%%%    @@@@@@@@
@@@@@@@@@    /%%%%%,,,,,,,(%%%%%%%%%%%%%%%%%%%%/           /%%%%%%%%%/     .%%%%%%%%%%%%.                        *%%%%%%%%%%%%#%%%%%%%%                  %%%%%%%%%%%%%%%%%%%(,,,,,,,,/%%%%%    /@@@@@@@@
@@@@@@@@@@    %%%%%%%%%%%%%%%%%%%%%%%,,,,,%%%%%%%            %%%%%%%%%%%%%%%%%%%%%%%%#                            /%%%%%%%%%%%%%%%%%%*                  %%%%%%%%%%%%%%%%%%%%%%%%#,,,,%%%%%,    @@@@@@@@@
@@@@@@@@@@@    %%%%%%%%%%%%%%%%%%%,,,,,,,,,%%%%%%%*             %%%%%%%%%%%%%%%%%%                                  #%%%%%%%%%%%%%(                   %%%%%%%%,,,,,,,%%%%%%%%%%%%%%%%%%%%(    @@@@@@@@@@
@@@@@@@@@@@@    %%%%%%%%%%%%%%,,,,,,,,,%,,,,,%%%%%%%                  .**,.                                                                         ,%%%%%%%,,,,,,,,,,,,,%%%%%%%%%%%%%%%(    @@@@@@@@@@@
@@@@@@@@@@@@@    %%%%%%*,,,,,,,,,,,,,%%(,,,,,/%%%%%%%%                                                                                            .%%%%%%%#,,,,,%%%%%,,,,,,,,%%%%%%%%%%*    @@@@@@@@@@@@
@@@@@@@@@@@@@@    (%%%%%(,,,,,(%%%,,,,,,,,,,%%%%%%%%%%%%                                                                                        /%%%%%%%%,,,,,,,,,,,%%%%%,,,,,,/%%%%%%     @@@@@@@@@@@@@
@@@@@@@@@@@@@@@     %%%%%%%%%%%*,,,,,%%#%%%%%%%%%%%%%%%%%%#                                                                                   %%%%%%%%%%%%%%%%%/,,,,,,,,%%,,,,,%%%%%%    /@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@/    %%%%%%%,,,,,,,*%%%%%%%%%%%%,,,,%%%%%%%%%.                                                                             #%%%%%%%%/,,,%%%%%%%%%%%/,,,,,,,,,%%%%%%%    @@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@    ,%%%%%%,,,,,%%%%%%%%%%%%%,,,,,,,,%%%%%%%%%*                                                                       %%%%%%%%%%,,,,,,,%%%%%%%%%%%%%%,,,,,%%%%%%     @@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@,    %%%%%%%%%%%%%%%%%%%%,,,,,,,,(%%%%%%%%%%%%%%%                                                                .%%%%%%%%%%%%%%*,,,,,,,,%%%%%%%%%%%%%/%%%%%%(    @@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@     %%%%%%%%%%%%%%%%,,,,,,%%%%%%%%(,,,%%%%%%%%%%%%.                                                       /%%%%%%%%%%%*,,#%%%%%%%%,,,,,,%%%%%%%%%%%%%%%%%     @@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@     %%%%%%%%%(,,,,,,,*%%%%%%%%%%,,,,,,%%%%%%%%%%%%%%%/                                             #%%%%%%%%%%%%(,,,,,,,,%%%%%%%%%,,,,,,,,(%%%%%%%%%%     @@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@#    .%%%%%%%,,,,,,%%%%%%%%%%%%,,,,,,,%%%%%%%%%%%%%%%%%%%%%%%                              .%%%%%%%%%%%%%%%%%,,,,,,,,%%,,,%%%%%%%%%%%,,,,,,,%%%%%%%     @@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@#     %%%%%%%,,,%%%%%%%%%%%,,,,,,,,(%%%%,,,,,,%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%,,,,,#,,,,,/%%%%%%%%%%%%%%%,,,#%%%%%%%     @@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@&     %%%%%%%%%%%%%%%%%%,,,,,,,,,%,,,,,,,,,,%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*(%%%%%%%%%,,,,%#,,,,,%%%%%%%%%%%%%%%%%%%%%%%     @@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@     *%%%%%%%%%%%%,,,,,,%%,,,,,,,,,,,,/%%%%%%%,,,,,,,,,,,%%%%%%%%%%%%%%%%%%%%%%%/,,%%%%*,,,,,%%%%%%%%%%(%%%%%,,,,,,%%%%%%%%%%%%%%%%%%      @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@.     %%%%%%%%,,,,,,,%%%,,,,#%,,,,,%%%%%%%%,,,,,%*,,,,,%%%%%,,,,,,,,,,,%%%%%,,,,,,,%%%,,,,/%%%%%%%%%%%%%%%%%%,,,,,,,%%%%%%%%%%%%(     #@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@      %%%%%%%%%,%%%%%(%%%*,,,,#%%%%%%%%/,,,,%%%%#,,%%%%%%,,,,/(/,,,,#%%%%%%,,,,,,,%%,,,,%%%%%%%%%%%%%%%%%%*,,,,,,*%%%%%%%%%      @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@      %%%%%%%%%%%%%%%,,,,,%%%%%%%%%%%,,,,,,,,,%%%%%%%%,,,,#%%,,,,%%%%%%%,,,,,,,,,,,,,,%%%%%%%%%%%%%%%%%%%,,%%%%%%%%%#      @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@      ,%%%%%%%%%%,,,,,,%%%%%%%%%%%%%%%%*,,,,%%%%%%%,,,,%%%,,,,%%%%%%%%,,,,(,,,,,,,,#%%%%%%%%%%%%%%%%%%%%%%%%%%       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@       /%%%%%%%%%%%%%%%%%%%%%,,,,%%%,,,,,%%%%%%%,,,,%%%,,,,%%%%%%%%%,,,,%%,,,,,,,%%%%%%%%%%%%%%%%%%%%%%.      *@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@       .%%%%%%%%%%%%%%%%,,,,,,,,,,,,%%%%%%%%,,,,%%%,,,,%%%%%%%%%,,,,,%%%,,,,,*%%%%%%%%%%%%%%%%%        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@        ,%%%%%%%%%%%%%%%/,,,,,,%%%%%%%%%,,,,,,,,,,,%%%%%%%%%,,,,,%%%%%#%%%%%%%%%%%%%%%         @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%         /%%%%%%%%%%%%%%%%%%%%%%%%%%,,,,,,,,,%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%.         @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@           .%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#            @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@/               ,(%%%%%%%%%%%%%%%%%%%%%%%%%%%/.               #@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&.                                           ,@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%(,...   ...*(&@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@   
    )" << std::endl;
}

// Functions for time keeping that may get used later
//////////////////////////////////////////////////////////////////////////////////
std::string getCurrentDate(){
    // Get the current system time
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    // Convert the system time to a time_t object
    std::time_t timeNow = std::chrono::system_clock::to_time_t(now);

    // Convert the time_t object to a tm struct (broken down time)
    std::tm* tmNow = std::localtime(&timeNow);

    // Create a stringstream to format the date
    std::stringstream ss;
    ss << tmNow->tm_year + 1900 << '-'
       << std::setfill('0') << std::setw(2) << tmNow->tm_mon + 1 << '-'
       << std::setfill('0') << std::setw(2) << tmNow->tm_mday;

    return ss.str();
}

std::string getCurrentTime(){
    // Get the current system time
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    // Convert the system time to a time_t object
    std::time_t timeNow = std::chrono::system_clock::to_time_t(now);

    // Convert the time_t object to a tm struct (broken down time)
    std::tm* tmNow = std::localtime(&timeNow);

    // Create a stringstream to format the time
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << tmNow->tm_hour << ':'
       << std::setfill('0') << std::setw(2) << tmNow->tm_min << ':'
       << std::setfill('0') << std::setw(2) << tmNow->tm_sec;

    return ss.str();
}
/////////////////////////////////////////////////////////////////////////////////////

// Function to display SQL error messages
void showSQLError(SQLHANDLE handle, SQLSMALLINT handleType)
{
    SQLTCHAR SQLState[1024];
    SQLTCHAR message[1024];
    SQLINTEGER nativeError;
    SQLSMALLINT msgSize;

    SQLRETURN ret = SQLGetDiagRec(handleType, handle, 1, SQLState, &nativeError, message, sizeof(message), &msgSize);
    if (SQL_SUCCEEDED(ret))
    {
        std::cerr << "SQL Error: " << message << " (" << SQLState << ")" << std::endl;
    }
}

//Ping the server and print the first 50 rows of keys
void demoOption(SQLHANDLE sqlConnectionHandle, SQLHANDLE& sqlStatementHandle)
{
    std::cout << "Demo option selected." << std::endl;

    // Close the existing statement handle if it's already allocated
    if (sqlStatementHandle != SQL_NULL_HANDLE)
    {
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStatementHandle); // Close the current statement handle
        sqlStatementHandle = SQL_NULL_HANDLE; // Reset the handle to NULL
    }

    // Allocate a new statement handle
    if (SQLAllocHandle(SQL_HANDLE_STMT, sqlConnectionHandle, &sqlStatementHandle) != SQL_SUCCESS)
    {
        showSQLError(sqlStatementHandle, SQL_HANDLE_STMT);
        return;
    }

    // Set the cursor type as SQL_CURSOR_STATIC to ensure it remains the same throughout fetch operations
    if (SQLSetStmtAttr(sqlStatementHandle, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_STATIC, SQL_IS_INTEGER) != SQL_SUCCESS)
    {
        showSQLError(sqlStatementHandle, SQL_HANDLE_STMT);
        return;
    }

    // Enable scrolling for the cursor
    SQLSetStmtAttr(sqlStatementHandle, SQL_ATTR_CURSOR_SCROLLABLE, (SQLPOINTER)SQL_SCROLLABLE, SQL_IS_INTEGER);

    // Ping the SQL Server to check if it's working
    SQLRETURN retCode = SQLExecDirect(sqlStatementHandle, (SQLCHAR*)"SELECT 1;", SQL_NTS);
    if (retCode != SQL_SUCCESS)
    {
        showSQLError(sqlStatementHandle, SQL_HANDLE_STMT);
        return;
    }

    // Fetch the result of the ping
    SQLLEN rowCount = 0;
    SQLLEN indicator = 0;
    SQLCHAR resultValue[2]; // We expect a single character result ("1")

    retCode = SQLFetch(sqlStatementHandle);
    if (retCode != SQL_SUCCESS && retCode != SQL_SUCCESS_WITH_INFO)
    {
        showSQLError(sqlStatementHandle, SQL_HANDLE_STMT);
        return;
    }

    retCode = SQLGetData(sqlStatementHandle, 1, SQL_C_CHAR, resultValue, sizeof(resultValue), &indicator);
    if (retCode != SQL_SUCCESS && retCode != SQL_SUCCESS_WITH_INFO)
    {
        showSQLError(sqlStatementHandle, SQL_HANDLE_STMT);
        return;
    }

    if (indicator == SQL_NULL_DATA)
    {
        std::cout << "Ping result: NULL" << std::endl;
    }
    else
    {
        std::cout << "Ping result: " << resultValue << std::endl;
    }

    // Close the cursor after fetching rows
    SQLFreeStmt(sqlStatementHandle, SQL_CLOSE);

    // Sample SQL query to select all data from a table (replace "YOUR_TABLE_NAME" with your table's name)
    std::string query = "SELECT * FROM Inventory;";

    // Execute the query
    if (SQLExecDirect(sqlStatementHandle, (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS)
    {
        showSQLError(sqlStatementHandle, SQL_HANDLE_STMT);
        return;
    }

    // Fetch and print the column headers with formatted width
    SQLSMALLINT numColumns = 0;
    SQLCHAR columnName[1024];
    SQLSMALLINT columnNameLen = 0;
    SQLSMALLINT columnType = 0;
    SQLULEN columnSize = 0;
    SQLSMALLINT decimalDigits = 0;
    SQLSMALLINT nullable = 0;

    SQLNumResultCols(sqlStatementHandle, &numColumns);

    std::cout << "Column Headers:" << std::endl;
    for (SQLSMALLINT col = 1; col <= numColumns; ++col) {
        SQLDescribeCol(sqlStatementHandle, col, columnName, sizeof(columnName), &columnNameLen, &columnType, &columnSize, &decimalDigits, &nullable);
        std::cout << std::setw(15) << columnName << "  ";
    }
    std::cout << std::endl;

    // Fetch and print the first 50 rows with formatted width
    SQLLEN demoRowCount = 0;
    while (SQLFetch(sqlStatementHandle) == SQL_SUCCESS && demoRowCount < 50) {
        // Process the data of the current row and print it out
        SQLCHAR column1[1024]; // Change the size according to your data
        SQLCHAR column2[1024];
        SQLCHAR column3[1024];
        SQLCHAR column4[1024];
        SQLCHAR column5[1024];

        SQLLEN len1, len2, len3, len4, len5; // Replace with actual column lengths

        SQLGetData(sqlStatementHandle, 1, SQL_C_CHAR, column1, sizeof(column1), &len1);
        SQLGetData(sqlStatementHandle, 2, SQL_C_CHAR, column2, sizeof(column2), &len2);
        SQLGetData(sqlStatementHandle, 3, SQL_C_CHAR, column3, sizeof(column3), &len3);
        SQLGetData(sqlStatementHandle, 4, SQL_C_CHAR, column4, sizeof(column4), &len4);
        SQLGetData(sqlStatementHandle, 5, SQL_C_CHAR, column5, sizeof(column5), &len5);

        // Print the data of the current row with formatted width
        std::cout << "Row " << demoRowCount + 1 << ": ";
        std::cout << std::setw(15) << column1 << "  ";
        std::cout << std::setw(15) << column2 << "  ";
        std::cout << std::setw(15) << column3 << "  ";
        std::cout << std::setw(15) << column4 << "  ";
        std::cout << std::setw(15) << column5 << std::endl;

        demoRowCount++;
    }

    std::cout << "Total rows fetched: " << demoRowCount << std::endl;

    // Close the cursor after fetching rows
    SQLFreeStmt(sqlStatementHandle, SQL_CLOSE);
}

// Function to handle the "Search" option
void searchOption(SQLHANDLE sqlConnectionHandle, SQLHANDLE& sqlStatementHandle)
{
    std::cout << "Search option selected." << std::endl;

    // Prompt the user to enter the column name to search by
    std::string columnName;
    std::cout << "Enter the column name to search by (Title, [On Hand], Needed, Missing, Extra): ";
    std::cin.ignore(); // Ignore any previous newline characters
    std::getline(std::cin, columnName);

    // Prompt the user to enter the value to search for
    std::string searchValue;
    std::cout << "Enter the value to search for: ";
    std::getline(std::cin, searchValue);

    // Construct the SQL query using the LIKE operator for partial matching
    std::string query = "SELECT * FROM Inventory WHERE " + columnName + " LIKE '%" + searchValue + "%';";

    // Close the existing statement handle if it's already allocated
    if (sqlStatementHandle != SQL_NULL_HANDLE) {
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStatementHandle); // Close the current statement handle
        sqlStatementHandle = SQL_NULL_HANDLE; // Reset the handle to NULL
    }

    // Allocate a new statement handle
    if (SQLAllocHandle(SQL_HANDLE_STMT, sqlConnectionHandle, &sqlStatementHandle) != SQL_SUCCESS) {
        showSQLError(sqlStatementHandle, SQL_HANDLE_STMT);
        return;
    }

    // Prepare the statement with the parameter marker
    if (SQLPrepare(sqlStatementHandle, (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS) {
        showSQLError(sqlStatementHandle, SQL_HANDLE_STMT);
        return;
    }

    // Execute the query
    if (SQLExecute(sqlStatementHandle) != SQL_SUCCESS) {
        showSQLError(sqlStatementHandle, SQL_HANDLE_STMT);
        return;
    }

    // Fetch and print the column headers with formatted width
    SQLSMALLINT numColumns = 0;
    SQLCHAR columnNameHeader[1024];
    SQLSMALLINT columnNameLen = 0;
    SQLSMALLINT columnType = 0;
    SQLULEN columnSize = 0;
    SQLSMALLINT decimalDigits = 0;
    SQLSMALLINT nullable = 0;

    SQLNumResultCols(sqlStatementHandle, &numColumns);

    std::cout << "Column Headers:" << std::endl;
    for (SQLSMALLINT col = 1; col <= numColumns; ++col) {
        SQLDescribeCol(sqlStatementHandle, col, columnNameHeader, sizeof(columnNameHeader), &columnNameLen, &columnType, &columnSize, &decimalDigits, &nullable);
        std::cout << std::setw(15) << columnNameHeader << "  ";
    }
    std::cout << std::endl;

    // Fetch and print the rows that match or are closest to the search criteria with formatted width
    SQLINTEGER rowCount = 0;
    while (SQLFetch(sqlStatementHandle) == SQL_SUCCESS && rowCount < 50) {
        // Print the data of the current row with formatted width
        std::cout << "Row " << rowCount + 1 << ": ";
        for (SQLSMALLINT col = 1; col <= numColumns; ++col) {
            SQLCHAR columnValue[1024];
            SQLLEN len; // Replace with actual column length

            SQLGetData(sqlStatementHandle, col, SQL_C_CHAR, columnValue, sizeof(columnValue), &len);

            std::cout << std::setw(15) << columnValue << "  ";
        }
        std::cout << std::endl;

        rowCount++;
    }

    if (rowCount == 0) {
        std::cout << "No matching rows found." << std::endl;
    } else {
        std::cout << "Total matching rows: " << rowCount << std::endl;
    }

    // Close the cursor after executing the query
    SQLFreeStmt(sqlStatementHandle, SQL_CLOSE);
}

// Function to handle the "Checkout Option"
void checkOutOption(SQLHANDLE sqlConnectionHandle, SQLHANDLE& sqlStatementHandle) {
    std::cout << "Checkout option selected." << std::endl;

    // Default column name to "Title"
    std::string columnName = "Title";

    std::cin.ignore();
    // Prompt the user to enter the value to search for
    std::string searchValue;
    std::cout << "Enter the value to search for in the " << columnName << " column: ";
    std::getline(std::cin, searchValue);

    // Construct the SQL query using the LIKE operator for partial matching
    std::string query = "SELECT * FROM Inventory WHERE " + columnName + " LIKE '%" + searchValue + "%';";

    // Close the existing statement handle if it's already allocated
    if (sqlStatementHandle != SQL_NULL_HANDLE)
    {
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStatementHandle);
        sqlStatementHandle = SQL_NULL_HANDLE;
    }

    // Allocate a new statement handle
    if (SQLAllocHandle(SQL_HANDLE_STMT, sqlConnectionHandle, &sqlStatementHandle) != SQL_SUCCESS)
    {
        showSQLError(sqlStatementHandle, SQL_HANDLE_STMT);
        return;
    }

    // Set the cursor type as SQL_CURSOR_DYNAMIC to allow fetching in any direction
    if (SQLSetStmtAttr(sqlStatementHandle, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_DYNAMIC, SQL_IS_INTEGER) != SQL_SUCCESS)
    {
        showSQLError(sqlStatementHandle, SQL_HANDLE_STMT);
        return;
    }

    // Set the cursor scrollable to allow fetching in any direction
    if (SQLSetStmtAttr(sqlStatementHandle, SQL_ATTR_CURSOR_SCROLLABLE, (SQLPOINTER)SQL_SCROLLABLE, SQL_IS_INTEGER) != SQL_SUCCESS)
    {
        showSQLError(sqlStatementHandle, SQL_HANDLE_STMT);
        return;
    }

    // Execute the search query
    if (SQLExecDirect(sqlStatementHandle, (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS)
    {
        showSQLError(sqlStatementHandle, SQL_HANDLE_STMT);
        return;
    }

    // Fetch and print the column headers with formatted width
    SQLSMALLINT numColumns = 0;
    SQLCHAR columnNameHeader[1024];
    SQLSMALLINT columnNameLen = 0;
    SQLSMALLINT columnType = 0;
    SQLULEN columnSize = 0;
    SQLSMALLINT decimalDigits = 0;
    SQLSMALLINT nullable = 0;

    SQLNumResultCols(sqlStatementHandle, &numColumns);

    std::cout << "Column Headers:" << std::endl;
    for (SQLSMALLINT col = 1; col <= numColumns; ++col) {
        SQLDescribeCol(sqlStatementHandle, col, columnNameHeader, sizeof(columnNameHeader), &columnNameLen, &columnType, &columnSize, &decimalDigits, &nullable);
        std::cout << std::setw(15) << columnNameHeader << "  ";
    }
    std::cout << std::endl;

    // Fetch and print the rows that match or are closest to the search criteria with formatted width
    SQLINTEGER rowCount = 0;
    while (SQLFetch(sqlStatementHandle) == SQL_SUCCESS && rowCount < 50) {
        // Print the data of the current row with formatted width
        std::cout << "Row " << rowCount + 1 << ": ";
        for (SQLSMALLINT col = 1; col <= numColumns; ++col) {
            SQLCHAR columnValue[1024];
            SQLLEN len; // Replace with actual column length

            SQLGetData(sqlStatementHandle, col, SQL_C_CHAR, columnValue, sizeof(columnValue), &len);

            std::cout << std::setw(15) << columnValue << "  ";
        }
        std::cout << std::endl;

        rowCount++;
    }

    if (rowCount == 0) {
        std::cout << "No matching rows found." << std::endl;
    } else {
        std::cout << "Total matching rows: " << rowCount << std::endl;
    }

    // Prompt the user to select a row by entering the row number
    int selectedRow;
    std::cout << "Select a row by entering the row number (1-" << rowCount << "): ";
    std::cin >> selectedRow;

    // Move the cursor to the selected row
    SQLUSMALLINT rowStatus;
    if (SQLFetchScroll(sqlStatementHandle, SQL_FETCH_ABSOLUTE, selectedRow) != SQL_SUCCESS) {
        showSQLError(sqlStatementHandle, SQL_HANDLE_STMT);
        return;
    }

    // Fetch the selected row (title) from the database
    SQLCHAR selectedTitle[1024];
    SQLLEN lenSelectedTitle;

    if (SQLGetData(sqlStatementHandle, 1, SQL_C_CHAR, selectedTitle, sizeof(selectedTitle), &lenSelectedTitle) != SQL_SUCCESS) {
        showSQLError(sqlStatementHandle, SQL_HANDLE_STMT);
        return;
    }

    // Prompt the user for first name, last name, and keeping or returning the item
    std::string firstName, lastName, keepOrReturn;

    // Input validation for firstName variable
    std::cout << "Enter your first name: ";
    std::cin.ignore(); // Ignore any previous newline characters
    std::getline(std::cin, firstName);

    // Input validation for lastName variable
    std::cout << "Enter your last name: ";
    std::getline(std::cin, lastName);

    // Input validation for keepOrReturn variable (case-insensitive)
    while (true) {
        std::cout << "Are you keeping or returning the item? ";
        std::getline(std::cin, keepOrReturn);

        // Convert the input to lowercase for case-insensitive comparison
        std::transform(keepOrReturn.begin(), keepOrReturn.end(), keepOrReturn.begin(), ::tolower);

        if (keepOrReturn == "keep" || keepOrReturn == "return") {
            break;
        }
        std::cout << "Invalid input. Please enter either 'keep' or 'return'." << std::endl;
    }

    // Close the cursor and unbind parameters before re-executing the query
    SQLFreeStmt(sqlStatementHandle, SQL_UNBIND);
    SQLFreeStmt(sqlStatementHandle, SQL_RESET_PARAMS);
    SQLCloseCursor(sqlStatementHandle);

    // Prepare a new statement handle for the INSERT query
    SQLHANDLE insertStatementHandle;
    if (SQLAllocHandle(SQL_HANDLE_STMT, sqlConnectionHandle, &insertStatementHandle) != SQL_SUCCESS) {
        showSQLError(insertStatementHandle, SQL_HANDLE_STMT);
        return;
    }

    // Prepare the SQL query to insert data into the checkout table
    std::string insertQuery = "INSERT INTO dbo.checkout (fname, lname, item, keep_or_return) VALUES (?, ?, ?, ?);";

    // Prepare the statement with parameter markers
    if (SQLPrepare(insertStatementHandle, (SQLCHAR*)insertQuery.c_str(), SQL_NTS) != SQL_SUCCESS) {
        showSQLError(insertStatementHandle, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, insertStatementHandle);
        return;
    }

    // Bind the parameters for the INSERT query
    SQLLEN lenFirstName = firstName.length();
    SQLLEN lenLastName = lastName.length();
    SQLLEN lenKeepOrReturn = keepOrReturn.length();

    SQLBindParameter(insertStatementHandle, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, lenFirstName, 0, (SQLPOINTER)firstName.c_str(), firstName.length(), &lenFirstName);
    SQLBindParameter(insertStatementHandle, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, lenLastName, 0, (SQLPOINTER)lastName.c_str(), lastName.length(), &lenLastName);
    SQLBindParameter(insertStatementHandle, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, lenSelectedTitle, 0, (SQLPOINTER)selectedTitle, lenSelectedTitle, &lenSelectedTitle);
    SQLBindParameter(insertStatementHandle, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, lenKeepOrReturn, 0, (SQLPOINTER)keepOrReturn.c_str(), keepOrReturn.length(), &lenKeepOrReturn);

    // Execute the prepared statement to insert data into the checkout table
    if (SQLExecute(insertStatementHandle) != SQL_SUCCESS) {
        showSQLError(insertStatementHandle, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, insertStatementHandle);
        return;
    }

    std::cout << "Checkout completed successfully." << std::endl;

    // Close the cursor and free the insert statement handle after everything is done
    SQLFreeStmt(insertStatementHandle, SQL_CLOSE);
    SQLFreeHandle(SQL_HANDLE_STMT, insertStatementHandle);
}

// Function to handle the "Checkin Option"
void checkInOption(SQLHANDLE sqlConnectionHandle, SQLHANDLE& sqlStmtHandle){
    std::cout << "Check In option selected." << std::endl;

    // Get user input for first and last name
    std::string firstName, lastName;
    std::cout << "Enter your first name: ";
    std::cin.ignore();
    std::getline(std::cin, firstName);

    std::cout << "Enter your last name: ";
    std::getline(std::cin, lastName);

    // Construct the SQL query to search for matching rows in the "checkout" table
    std::string query = "SELECT * FROM checkout WHERE fname = '" + firstName + "' AND lname = '" + lastName + "';";

    // Execute the query and display the results
    SQLAllocHandle(SQL_HANDLE_STMT, sqlConnectionHandle, &sqlStmtHandle);

    if (SQLExecDirect(sqlStmtHandle, (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS) {
        showSQLError(sqlStmtHandle, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        return;
    }

    SQLSMALLINT numColumns = 0;
    SQLNumResultCols(sqlStmtHandle, &numColumns);

    // Display column headers with formatted width
    std::cout << "Column Headers:" << std::endl;
    for (SQLSMALLINT col = 1; col <= numColumns; ++col) {
        SQLCHAR columnNameHeader[1024];
        SQLSMALLINT columnNameLen;
        SQLDescribeCol(sqlStmtHandle, col, columnNameHeader, sizeof(columnNameHeader), &columnNameLen, NULL, NULL, NULL, NULL);
        std::cout << std::setw(15) << columnNameHeader << "  ";
    }
    std::cout << std::endl;

    // Fetch and display rows with formatted width
    std::vector<std::vector<std::string>> rows;
    SQLLEN rowCount = 0;
    while (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) {
        std::vector<std::string> row;
        for (SQLSMALLINT col = 1; col <= numColumns; ++col) {
            SQLCHAR columnValue[1024];
            SQLLEN len;
            SQLGetData(sqlStmtHandle, col, SQL_C_CHAR, columnValue, sizeof(columnValue), &len);
            row.push_back(reinterpret_cast<char*>(columnValue));
        }
        rows.push_back(row);

        std::cout << "Row " << rowCount + 1 << ": ";
        for (const auto& value : row) {
            std::cout << std::setw(15) << value << "  ";
        }
        std::cout << std::endl;

        rowCount++;
    }

    if (rowCount == 0) {
        std::cout << "No matching rows found." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        return;
    }

    // Get user input for selecting a row
    int selectedRow;
    std::cout << "Select a row by entering the row number (1-" << rowCount << "): ";
    std::cin >> selectedRow;

    // Check if the selected row number is valid
    if (selectedRow < 1 || selectedRow > rowCount) {
        std::cout << "Invalid row number. Aborting check-in." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        return;
    }

    // Get the selected row data from the vector
    const std::vector<std::string>& selectedRowData = rows[selectedRow - 1];

    // Extract the values for fname, lname, item, and keep_or_return
    std::string selectedItem = selectedRowData[2];
    std::string keepOrReturn = selectedRowData[3];

    // Construct the SQL query to insert the selected row into the "checkin" table
    std::string insertQuery = "INSERT INTO checkin (fname, lname, item, keep_or_return) VALUES ('" + firstName + "', '" + lastName + "', '" + selectedItem + "', '" + keepOrReturn + "');";

    SQLHANDLE insertStatementHandle;
    SQLAllocHandle(SQL_HANDLE_STMT, sqlConnectionHandle, &insertStatementHandle);

    if (SQLExecDirect(insertStatementHandle, (SQLCHAR*)insertQuery.c_str(), SQL_NTS) != SQL_SUCCESS) {
        showSQLError(insertStatementHandle, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, insertStatementHandle);
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        return;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, insertStatementHandle);

    std::cout << "Check In completed successfully." << std::endl;
}

// Print the entire checkout log
void showCheckoutLog(SQLHANDLE sqlConnectionHandle, SQLHANDLE& sqlStmtHandle) {
    std::cout << "Checkout Log:" << std::endl;

    // Construct the SQL query to fetch all rows from the "checkout" table
    std::string query = "SELECT * FROM checkout;";

    // Execute the query and display the results
    SQLAllocHandle(SQL_HANDLE_STMT, sqlConnectionHandle, &sqlStmtHandle);

    if (SQLExecDirect(sqlStmtHandle, (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS) {
        showSQLError(sqlStmtHandle, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        return;
    }

    SQLSMALLINT numColumns = 0;
    SQLNumResultCols(sqlStmtHandle, &numColumns);

    // Display column headers with formatted width
    for (SQLSMALLINT col = 1; col <= numColumns; ++col) {
        SQLCHAR columnNameHeader[1024];
        SQLSMALLINT columnNameLen;
        SQLDescribeCol(sqlStmtHandle, col, columnNameHeader, sizeof(columnNameHeader), &columnNameLen, NULL, NULL, NULL, NULL);
        std::cout << std::setw(15) << columnNameHeader << "  ";
    }
    std::cout << std::endl;

    // Fetch and display rows with formatted width
    while (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) {
        for (SQLSMALLINT col = 1; col <= numColumns; ++col) {
            SQLCHAR columnValue[1024];
            SQLLEN len;
            SQLGetData(sqlStmtHandle, col, SQL_C_CHAR, columnValue, sizeof(columnValue), &len);
            std::cout << std::setw(15) << columnValue << "  ";
        }
        std::cout << std::endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
}

// Print the entire checkin log
void showCheckinLog(SQLHANDLE sqlConnectionHandle, SQLHANDLE& sqlStmtHandle) {
    std::cout << "Checkin Log:" << std::endl;

    // Construct the SQL query to fetch all rows from the "checkin" table
    std::string query = "SELECT * FROM checkin;";

    // Execute the query and display the results
    SQLAllocHandle(SQL_HANDLE_STMT, sqlConnectionHandle, &sqlStmtHandle);

    if (SQLExecDirect(sqlStmtHandle, (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS) {
        showSQLError(sqlStmtHandle, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        return;
    }

    SQLSMALLINT numColumns = 0;
    SQLNumResultCols(sqlStmtHandle, &numColumns);

    // Display column headers with formatted width
    for (SQLSMALLINT col = 1; col <= numColumns; ++col) {
        SQLCHAR columnNameHeader[1024];
        SQLSMALLINT columnNameLen;
        SQLDescribeCol(sqlStmtHandle, col, columnNameHeader, sizeof(columnNameHeader), &columnNameLen, NULL, NULL, NULL, NULL);
        std::cout << std::setw(15) << columnNameHeader << "  ";
    }
    std::cout << std::endl;

    // Fetch and display rows with formatted width
    while (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) {
        for (SQLSMALLINT col = 1; col <= numColumns; ++col) {
            SQLCHAR columnValue[1024];
            SQLLEN len;
            SQLGetData(sqlStmtHandle, col, SQL_C_CHAR, columnValue, sizeof(columnValue), &len);
            std::cout << std::setw(15) << columnValue << "  ";
        }
        std::cout << std::endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
}

// Function to export SQL query result to a CSV file
void exportTableToCSV(SQLHANDLE sqlStmtHandle, const std::string& filename)
{
    csvMutex.lock(); // Lock the mutex before writing to CSV

    // Remove the existing CSV file if it exists
    std::remove(filename.c_str());

    SQLSMALLINT numColumns = 0;
    SQLNumResultCols(sqlStmtHandle, &numColumns);

    // Open a CSV file for writing
    std::ofstream csvFile(filename);

    // Fetch and write rows to the CSV file
    while (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) {
        for (SQLSMALLINT col = 1; col <= numColumns; ++col) {
            SQLCHAR columnValue[1024];
            SQLLEN len;
            SQLGetData(sqlStmtHandle, col, SQL_C_CHAR, columnValue, sizeof(columnValue), &len);

            // Write the data to the CSV file
            csvFile << reinterpret_cast<char*>(columnValue);

            if (col < numColumns) {
                csvFile << ",";
            }
        }
        csvFile << std::endl;
    }

    // Close the CSV file
    csvFile.close();

    csvMutex.unlock(); // Unlock the mutex after writing is complete
}

// Export tables as CSV files
void exportOption(SQLHANDLE sqlConnectionHandle, SQLHANDLE& sqlStmtHandle)
{
    std::cout << "Export option selected." << std::endl;

    // Construct the SQL queries for different tables
    std::string checkoutQuery = "SELECT * FROM checkout;";
    std::string invQuery = "SELECT * FROM Inventory;";
    std::string checkinQuery = "SELECT * FROM checkin;";

    SQLHANDLE checkoutStmtHandle;
    SQLHANDLE invStmtHandle;
    SQLHANDLE checkinStmtHandle;

    // Execute the query for checkout
    SQLAllocHandle(SQL_HANDLE_STMT, sqlConnectionHandle, &checkoutStmtHandle);
    if (SQLExecDirect(checkoutStmtHandle, (SQLCHAR*)checkoutQuery.c_str(), SQL_NTS) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, checkoutStmtHandle);
        return;
    }
    exportTableToCSV(checkoutStmtHandle, "checkout_data.csv");
    SQLFreeHandle(SQL_HANDLE_STMT, checkoutStmtHandle);

    // Execute the query for Inventory
    SQLAllocHandle(SQL_HANDLE_STMT, sqlConnectionHandle, &invStmtHandle);
    if (SQLExecDirect(invStmtHandle, (SQLCHAR*)invQuery.c_str(), SQL_NTS) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, invStmtHandle);
        return;
    }
    exportTableToCSV(invStmtHandle, "inventory_data.csv");
    SQLFreeHandle(SQL_HANDLE_STMT, invStmtHandle);

    // Execute the query for checkin
    SQLAllocHandle(SQL_HANDLE_STMT, sqlConnectionHandle, &checkinStmtHandle);
    if (SQLExecDirect(checkinStmtHandle, (SQLCHAR*)checkinQuery.c_str(), SQL_NTS) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, checkinStmtHandle);
        return;
    }
    exportTableToCSV(checkinStmtHandle, "checkin_data.csv");
    SQLFreeHandle(SQL_HANDLE_STMT, checkinStmtHandle);

    std::cout << "Export to CSV completed successfully." << std::endl;
}

////////////////////////////////////////////////
//    DO NOT TOUCH UNDER ANY CIRCUMSTANCES    //
////////////////////////////////////////////////
// Helper function to create a new instance of Excel Application
IDispatch* CreateExcelApplication() {
    std::wcout << L"Creating Excel application instance..." << std::endl;

    IDispatch* pExcelApp = NULL;
    CoCreateInstance(CLSID_Application, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void**)&pExcelApp);

    std::wcout << L"Excel application instance created." << std::endl;
    return pExcelApp;
}

// Helper function to get the Workbooks collection
IDispatch* GetWorkbooks(IDispatch* pExcelApp) {
    HRESULT hr;
    DISPID dispid;
    OLECHAR* oleWorkbookName = SysAllocString(L"Workbooks"); // Convert string to OLECHAR

    std::wcout << L"Retrieving Workbooks collection..." << std::endl;

    hr = pExcelApp->GetIDsOfNames(IID_NULL, &oleWorkbookName, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
    
    SysFreeString(oleWorkbookName); // Free allocated memory
    
    if (FAILED(hr)) {
        std::cerr << "Failed to get IDs of names for Workbooks: " << hr << std::endl;
        return nullptr;
    }

    DISPPARAMS dispParamsNoArgs = {NULL, NULL, 0, 0};
    VARIANT result;
    VariantInit(&result);

    hr = pExcelApp->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dispParamsNoArgs, &result, NULL, NULL);

    if (FAILED(hr)) {
        std::cerr << "Failed to invoke Workbooks property: " << hr << std::endl;
        return nullptr;
    }

    std::wcout << L"Workbooks collection retrieved." << std::endl;
    return result.pdispVal;
}

// Helper function to add a new Workbook
IDispatch* AddWorkbook(IDispatch* pWorkbooks) {
    HRESULT hr;
    DISPID dispid;
    OLECHAR* oleMethodName = SysAllocString(L"Add"); // Convert string to OLECHAR

    std::wcout << L"Adding a new Workbook..." << std::endl;

    hr = pWorkbooks->GetIDsOfNames(IID_NULL, &oleMethodName, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
    
    SysFreeString(oleMethodName); // Free allocated memory
    
    if (FAILED(hr)) {
        std::cerr << "Failed to get IDs of names for Add method: " << hr << std::endl;
        return nullptr;
    }

    DISPPARAMS dispParamsNoArgs = {NULL, NULL, 0, 0};
    VARIANT result;
    VariantInit(&result);

    hr = pWorkbooks->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispParamsNoArgs, &result, NULL, NULL);

    if (FAILED(hr)) {
        std::cerr << "Failed to invoke Add method: " << hr << std::endl;
        return nullptr;
    }

    std::wcout << L"Workbook added." << std::endl;
    return result.pdispVal;
}

// Helper function to release COM interface
void ReleaseInterface(IDispatch* pInterface) {
    if (pInterface) {
        pInterface->Release();
        std::wcout << L"COM interface released." << std::endl;
    }
}

////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////

IDispatch* GetSheets(IDispatch* pWorkbook) {
    // Initialize COM for the function (if not already done)
    std::wcout << L"Initializing COM for GetSheets..." << std::endl;

    // Initialize variables
    DISPID dispid;
    OLECHAR* oleSheetsName = L"Sheets";
    
    // Get the DISPID of the "Sheets" property
    HRESULT hr = pWorkbook->GetIDsOfNames(IID_NULL, &oleSheetsName, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
    if (FAILED(hr)) {
        std::cerr << "Failed to get DISPID of 'Sheets' property: " << hr << std::endl;
        return nullptr;
    }
    std::wcout << L"DISPID of 'Sheets' property obtained: " << dispid << std::endl;
    
    // Prepare parameters for the invocation
    DISPPARAMS dispParamsNoArgs = { NULL, NULL, 0, 0 };
    VARIANT result;
    VariantInit(&result);

    // Invoke the "Sheets" property to get the Sheets collection
    std::wcout << L"Invoking 'Sheets' property..." << std::endl;
    hr = pWorkbook->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dispParamsNoArgs, &result, NULL, NULL);
    if (FAILED(hr)) {
        std::cerr << "Failed to invoke 'Sheets' property: " << hr << std::endl;
        return nullptr;
    }
    std::wcout << L"'Sheets' property invoked successfully." << std::endl;

    // Return the Sheets collection
    return result.pdispVal;
    
    // Release COM objects and clean up

    // Uninitialize COM for the function (if initialized)
    std::wcout << L"COM uninitialized for GetSheets." << std::endl;
}

IDispatch* AddSheet(IDispatch* pSheets) {
    // Initialize COM for the function (if not already done)
    std::wcout << L"Initializing COM for AddSheet..." << std::endl;

    // Initialize variables
    DISPID dispid;
    OLECHAR* oleAddSheetName = L"Add";
    
    // Get the DISPID of the "Add" method for adding a new sheet
    HRESULT hr = pSheets->GetIDsOfNames(IID_NULL, &oleAddSheetName, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
    if (FAILED(hr)) {
        std::cerr << "Failed to get DISPID of 'Add' method for adding a sheet: " << hr << std::endl;
        return nullptr;
    }
    std::wcout << L"DISPID of 'Add' method for adding a sheet obtained: " << dispid << std::endl;
    
    // Prepare parameters for the method invocation
    DISPPARAMS dispParamsNoArgs = { NULL, NULL, 0, 0 };
    VARIANT result;
    VariantInit(&result);

    // Invoke the "Add" method to add a new sheet
    std::wcout << L"Invoking 'Add' method to add a new sheet..." << std::endl;
    hr = pSheets->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispParamsNoArgs, &result, NULL, NULL);
    if (FAILED(hr)) {
        std::cerr << "Failed to invoke 'Add' method for adding a sheet: " << hr << std::endl;
        return nullptr;
    }
    std::wcout << L"'Add' method invoked successfully. New sheet added." << std::endl;

    // Return the IDispatch interface to the newly added sheet
    return result.pdispVal;
    
    // Release COM objects and clean up

    // Uninitialize COM for the function (if initialized)
    std::wcout << L"COM uninitialized for AddSheet." << std::endl;
}

bool OpenCSVInSheet(IDispatch* pSheet, const std::wstring& csvFilename) {
    DISPID dispid;
    OLECHAR* oleQueryTablesName = L"QueryTables";

    HRESULT hr = pSheet->GetIDsOfNames(IID_NULL, &oleQueryTablesName, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
    if (FAILED(hr)) {
        std::cerr << "Failed to get DISPID of 'QueryTables' property: " << hr << std::endl;
        return false;
    }

    VARIANTARG args[1];
    args[0].vt = VT_BSTR;
    std::wstring query = L"TEXT;" + csvFilename + L";FMT=Delimited,Local,Format=CSV";
    args[0].bstrVal = SysAllocString(query.c_str());

    DISPPARAMS dispParams = { args, NULL, 1, 0 };

    std::wcerr << L"Query String: " << query << std::endl;

    VARIANT result;
    VariantInit(&result);
    hr = pSheet->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dispParams, &result, NULL, NULL);
    if (FAILED(hr)) {
        std::cerr << "Failed to invoke 'QueryTables' property: " << hr << std::endl;
        VariantClear(args);
        return false;
    }

    IDispatch* pQueryTables = result.pdispVal;

    OLECHAR* oleAddQueryTableName = L"Add";
    hr = pQueryTables->GetIDsOfNames(IID_NULL, &oleAddQueryTableName, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
    if (FAILED(hr)) {
        std::cerr << "Failed to get DISPID of 'Add' method: " << hr << std::endl;
        pQueryTables->Release();
        VariantClear(args);
        return false;
    }

    VariantClear(&result);
    hr = pQueryTables->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispParams, &result, NULL, NULL);
    if (FAILED(hr)) {
        std::cerr << "Failed to invoke 'Add' method to open CSV in sheet: " << hr << std::endl;
        pQueryTables->Release();
        VariantClear(args);
        return false;
    }

    pQueryTables->Release();
    VariantClear(args);

    std::wcerr << L"CSV opened successfully!" << std::endl;
    return true;
}

void SaveWorkbook(IDispatch* pWorkbook, const std::wstring& excelFilename) {
    // Implement the necessary COM interactions to save the workbook to the specified file.
    DISPID dispid;
    OLECHAR* oleSaveAsName = L"SaveAs";

    VARIANTARG args[1];
    args[0].vt = VT_BSTR;
    args[0].bstrVal = SysAllocString(excelFilename.c_str());
    DISPPARAMS dispParams = { args, NULL, 1, 0 };

    HRESULT hr = pWorkbook->GetIDsOfNames(IID_NULL, &oleSaveAsName, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
    if (FAILED(hr)) {
        std::cerr << "Failed to get DISPID of 'SaveAs' method: " << hr << std::endl;
        VariantClear(args);
        return;
    }

    // Invoke the "SaveAs" method to save the workbook
    hr = pWorkbook->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispParams, NULL, NULL, NULL);
    VariantClear(args);

    if (FAILED(hr)) {
        std::cerr << "Failed to invoke 'SaveAs' method to save workbook: " << hr << std::endl;
    }
}

void CloseWorkbook(IDispatch* pWorkbook) {
    // Implement the necessary COM interactions to close the workbook.
    DISPID dispid;
    OLECHAR* oleCloseName = L"Close";

    HRESULT hr = pWorkbook->GetIDsOfNames(IID_NULL, &oleCloseName, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
    if (FAILED(hr)) {
        std::cerr << "Failed to get DISPID of 'Close' method: " << hr << std::endl;
        return;
    }

    // Invoke the "Close" method to close the workbook
    DISPPARAMS dispParamsNoArgs = { NULL, NULL, 0, 0 };
    hr = pWorkbook->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispParamsNoArgs, NULL, NULL, NULL);

    if (FAILED(hr)) {
        std::cerr << "Failed to invoke 'Close' method to close workbook: " << hr << std::endl;
    }
}

// Function to combine CSV files into an Excel workbook
void combineCSVToExcel(const std::vector<std::wstring>& csvFilenames, const std::wstring& excelFilename) {
    CoInitialize(NULL);
    
    std::wcout << L"Initializing COM..." << std::endl;

    IDispatch* pExcelApp = NULL;
    CLSID clsid;
    HRESULT hr = CLSIDFromProgID(L"Excel.Application", &clsid);
    if (SUCCEEDED(hr)) {
        std::wcout << L"Creating Excel application instance..." << std::endl;
        hr = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void**)&pExcelApp);
        if (SUCCEEDED(hr)) {
            std::wcout << L"Excel application instance created." << std::endl;

            IDispatch* pWorkbooks = GetWorkbooks(pExcelApp);
            if (pWorkbooks) {
                std::wcout << L"Retrieving Workbooks collection..." << std::endl;

                IDispatch* pWorkbook = AddWorkbook(pWorkbooks);
                if (pWorkbook) {
                    std::wcout << L"Workbook added." << std::endl;

                    for (const std::wstring& csvFilename : csvFilenames) {
                        IDispatch* pSheets = GetSheets(pWorkbook);
                        if (pSheets) {
                            std::wcout << L"Retrieving Sheets collection..." << std::endl;

                            IDispatch* pSheet = AddSheet(pSheets);
                            if (pSheet) {
                                std::wcout << L"Sheet added." << std::endl;

                                bool success = OpenCSVInSheet(pSheet, csvFilename);
                                if (success) {
                                    std::wcout << L"CSV opened successfully." << std::endl;

                                    // Perform other operations if needed
                                } else {
                                    std::wcerr << L"Failed to open CSV in sheet." << std::endl;
                                }

                                pSheet->Release();
                            } else {
                                std::wcerr << L"Failed to add sheet." << std::endl;
                            }

                            pSheets->Release();
                        } else {
                            std::wcerr << L"Failed to retrieve Sheets collection." << std::endl;
                        }
                    }

                    SaveWorkbook(pWorkbook, excelFilename);
                    CloseWorkbook(pWorkbook);
                    pWorkbook->Release();
                } else {
                    std::wcerr << L"Failed to add workbook." << std::endl;
                }

                pWorkbooks->Release();
            } else {
                std::wcerr << L"Failed to retrieve Workbooks collection." << std::endl;
            }

            pExcelApp->Release();
        } else {
            std::wcerr << L"Failed to create Excel application instance." << std::endl;
        }
    } else {
        std::wcerr << L"Failed to retrieve CLSID for Excel.Application." << std::endl;
    }

    CoUninitialize();
    std::wcout << L"COM uninitialized." << std::endl;
}

int main()
{
    SQLHANDLE sqlEnvHandle;
    SQLHANDLE sqlConnectionHandle;
    SQLHANDLE sqlStatementHandle;
    SQLRETURN retCode;

    //Draw script A
    int height = 27; // Adjust the height of the "A"
    int width = 50;  // Adjust the width of the "A"
    int offset = 10; // Adjust the offset of the "A"

    printUniversityAsciiArt();

    // Initialize the SQL environment
    if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle) != SQL_SUCCESS)
    {
        showSQLError(sqlEnvHandle, SQL_HANDLE_ENV);
        return 1;
    }

    if (SQLSetEnvAttr(sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0) != SQL_SUCCESS)
    {
        showSQLError(sqlEnvHandle, SQL_HANDLE_ENV);
        return 1;
    }

    // Allocate a connection handle
    if (SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnectionHandle) != SQL_SUCCESS)
    {
        showSQLError(sqlConnectionHandle, SQL_HANDLE_DBC);
        return 1;
    }

    // Connect to the local SQL Server (replace "YOUR_SERVER_NAME" with your SQL Server instance name)
    std::string connectionString = "DRIVER={SQL Server};SERVER=STUD01WRKR-LVTR\\SQLEXPRESS;DATABASE=Key Inventory;Trusted_Connection=yes;";
    retCode = SQLDriverConnect(sqlConnectionHandle, NULL, (SQLCHAR*)connectionString.c_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    if (retCode != SQL_SUCCESS && retCode != SQL_SUCCESS_WITH_INFO)
    {
        showSQLError(sqlConnectionHandle, SQL_HANDLE_DBC);
        return 1;
    }

    std::cout << "Connected to SQL Server successfully!" << std::endl;

    // Allocate a statement handle
    if (SQLAllocHandle(SQL_HANDLE_STMT, sqlConnectionHandle, &sqlStatementHandle) != SQL_SUCCESS)
    {
        showSQLError(sqlStatementHandle, SQL_HANDLE_STMT);
        return 1;
    }

    int choice;
     std::vector<std::wstring> csvFilenames = {
        L"C:\\Users\\student\\projects\\SQLConnect\\checkout_data.csv",
        L"C:\\Users\\student\\projects\\SQLConnect\\inventory_data.csv",
        L"C:\\Users\\student\\projects\\SQLConnect\\checkin_data.csv"
    };
    std::wstring excelFilename = L"C:\\Users\\student\\projects\\SQLConnect\\combined_data.xlsx";
    do
    {
        //Display menu options
        std::cout << "Menu:" << std::endl;
        std::cout << "1. Demo" << std::endl;
        std::cout << "2. Search" << std::endl;
        std::cout << "3. Check out" << std::endl;
        std::cout << "4. Check in" << std::endl;
        std::cout << "5. Show check out log" << std::endl;
        std::cout << "6. Show check in log" << std::endl;
        std::cout << "7. Export to Excel" <<std::endl;
        std::cout << "0. Exit" << std::endl;

        // Prompt user for choice
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        // Call the corresponding function based on user choice
        //std::string choice;
       // switch (choice);
        switch (choice)
        {
        case 1:
            demoOption(sqlConnectionHandle, sqlStatementHandle);
            break;
        case 2:
            searchOption(sqlConnectionHandle, sqlStatementHandle);
            break;
        case 3:
            checkOutOption(sqlConnectionHandle, sqlStatementHandle);
            break;
        case 4:
            checkInOption(sqlConnectionHandle, sqlStatementHandle);
            break;
        case 5:
            showCheckoutLog(sqlConnectionHandle, sqlStatementHandle);
            break;
        case 6:
            showCheckinLog(sqlConnectionHandle, sqlStatementHandle);
            break;
        case 7:
            exportOption(sqlConnectionHandle, sqlStatementHandle);
            combineCSVToExcel(csvFilenames, excelFilename);
            break;
        case 0:
            std::cout << "Exiting..." << std::endl;
            break;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
        }

    } while (choice != 0);
    
    // Cleanup and disconnect
    SQLFreeHandle(SQL_HANDLE_STMT, sqlStatementHandle);
    SQLDisconnect(sqlConnectionHandle);
    SQLFreeHandle(SQL_HANDLE_DBC, sqlConnectionHandle);
    SQLFreeHandle(SQL_HANDLE_ENV, sqlEnvHandle);

    return 0;
}