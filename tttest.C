#include <sql.h>
#include <sqlext.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

using namespace std;

static void chkReturnCode(SQLRETURN rc, SQLHENV henv, SQLHDBC hdbc, SQLHSTMT hstmt, char* msg, char* filename, int lineno, BOOL err_is_fatal);

#define DEFAULT_CONNSTR "DSN=ttdatasvr64;TTC_SERVER=0-0-2;TCP_PORT=14502"
#define MAXCOLS 100
#define COL_LEN_MAX 1000

int main(int ac, char** av)
{
    SQLRETURN rc = SQL_SUCCESS;
    /* General return code for the API */
    SQLHENV henv = SQL_NULL_HENV;
    /* Environment handle */
    SQLHDBC hdbc = SQL_NULL_HDBC;
    /* Connection handle */
    SQLHSTMT hstmt = SQL_NULL_HSTMT;
    /* Statement handle */
    SQLCHAR connOut[1024];
    /* Buffer for completed connection string */
    SQLSMALLINT connOutLen;
    /* Number of bytes returned in ConnOut */
    SQLCHAR *connStr = (SQLCHAR*)DEFAULT_CONNSTR;
    /* Connection string */
    
    // Shared by all connections
    rc = SQLAllocEnv(&henv);
    if (rc != SQL_SUCCESS) {
        fprintf(stderr, "Unable to allocate an environment handle\n");
        exit(1);
    }
    printf("SQLAllocEnv %d\n", rc);

    rc = SQLAllocConnect(henv, &hdbc);
    chkReturnCode(rc, henv, SQL_NULL_HDBC, SQL_NULL_HSTMT,  "Unable to allocate a ""connection handle\n", __FILE__, __LINE__, 1);
    printf("SQLAllocConnect %d\n", rc);

    string mConnectionString="DSN=ttdatasvr64;TTC_SERVER=0-0-2;TCP_PORT=14502";
    rc = SQLDriverConnect(hdbc, 0, (SQLCHAR *)mConnectionString.c_str(), mConnectionString.length(), connOut, sizeof(connOut), &connOutLen, SQL_DRIVER_NOPROMPT);
    chkReturnCode(rc, henv, hdbc, SQL_NULL_HSTMT, "Error in connecting to the database\n", __FILE__, __LINE__, 1);
    printf("SQLDriverConnect %d\n", rc);

    if(!(rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)) {
        printf("SQLDriverConnect Error\n");
        SQLFreeConnect(hdbc);
        return 0;
    }

    rc = SQLAllocStmt(hdbc, &hstmt);
    chkReturnCode(rc, henv, hdbc, SQL_NULL_HSTMT, "Unable to allocate a statement handle\n", __FILE__, __LINE__, 1);
    printf("SQLAllocStmt %d \n", rc);

    if(!(rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)) {
        printf("SQLAllocStmt Error\n");
        SQLFreeConnect(hdbc);
        return 0;
    }

    string sql = "SELECT count(*) FROM UCDF_UserDN";
    rc = SQLPrepare(hstmt,(UCHAR*) sql.c_str(), SQL_NTS);
    chkReturnCode(rc, henv, hdbc, hstmt, "Unable to SQLPrepare\n", __FILE__, __LINE__, 1);
    printf("SQLPrepare %d \n", rc);

    SQLSMALLINT numCols;
    rc = SQLNumResultCols(hstmt, &numCols);
    printf("SQLNumResultCols %d \n", rc);

    SQLCHAR colname[32];
    SQLLEN outlen [MAXCOLS];
    SQLULEN collen [MAXCOLS];
    SQLCHAR* data [MAXCOLS];

    SQLSMALLINT colnamelen, coltype, scale, nullable;
    for (int i = 0; i < numCols; i++) {
        rc = SQLDescribeCol(hstmt, (SQLSMALLINT) (i + 1),
            colname,(SQLSMALLINT)sizeof(colname), &colnamelen, &coltype, &collen[i], &scale, &nullable);
        
        data[i] = (SQLCHAR*) malloc (collen[i] +1); //Allocate space for column data.
        rc = SQLBindCol(hstmt, (SQLSMALLINT) (i + 1), SQL_C_CHAR, data[i], COL_LEN_MAX, &outlen[i]);
        
        printf("SQLBindCol %d \n", rc);
    }

    int attempts = 0;
    bool failed = false;
    while(attempts < 10)
    {
        /* Your application code here */
        rc = SQLExecute(hstmt);
        chkReturnCode(rc, henv, hdbc, hstmt,"Unable to Exec Direct\n", __FILE__, __LINE__, 1);
        printf("SQLExecute %d \n", rc);
        
        if (hstmt != SQL_NULL_HSTMT) {
            rc = SQLFreeStmt(hstmt, SQL_CLOSE);
            //chkReturnCode(rc, henv, hdbc, hstmt,"Unable to free the statement handle\n",  __FILE__, __LINE__, 0);
        }

        ++attempts;

        if(!(rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)) {
            printf("Connection failed Error\n");
            failed = true;
            attempts = 0;
            break;
        }
        
        sleep(1);
    }
   
    //Recover statement


    //printf("ERROR %d, %d, %d, %d, %d, %d, %d", SQL_SUCCESS, SQL_SUCCESS_WITH_INFO, SQL_ERROR, SQL_INVALID_HANDLE, 
    //    SQL_NO_DATA, SQL_NEED_DATA, SQL_STILL_EXECUTING);

   

    rc = SQLDisconnect(hdbc);
    chkReturnCode(rc, henv, hdbc,
    SQL_NULL_HSTMT,
    "Unable to close the "
    "connection\n",
    __FILE__, __LINE__, 0);

    rc = SQLFreeConnect(hdbc);
    chkReturnCode(rc, henv, hdbc,
    SQL_NULL_HSTMT,
    "Unable to free the "
    "connection handle\n",
    __FILE__, __LINE__, 0);

    rc = SQLFreeEnv(henv);
    chkReturnCode(rc, henv, SQL_NULL_HDBC,
    SQL_NULL_HSTMT,
    "Unable to free the "
    "environment handle\n",
    __FILE__, __LINE__, 0);

    return 0;
}

static void chkReturnCode(SQLRETURN rc, SQLHENV henv, SQLHDBC hdbc, SQLHSTMT hstmt, char* msg, char* filename, int lineno, BOOL err_is_fatal)
{
    #define MSG_LNG 512
    SQLCHAR sqlState[MSG_LNG];
    /* SQL state string */SQLINTEGER nativeErr;
    /* Native error code */
    SQLCHAR errMsg[MSG_LNG];
    /* Error msg text buffer pointer */
    SQLSMALLINT errMsgLen;
    /* Error msg text Available bytes */
    SQLRETURN ret = SQL_SUCCESS;
    if (rc != SQL_SUCCESS && rc != SQL_NO_DATA_FOUND ) 
    {
        if (rc != SQL_SUCCESS_WITH_INFO) {
            /*
            * It's not just a warning
            */
            fprintf(stderr, "*** ERROR in %s, line %d:"
            " %s\n",
            filename, lineno, msg);
        }
        /*
        * Now see why the error/warning occurred
        */
        while (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) 
        {
            ret = SQLError(henv, hdbc, hstmt,
            sqlState, &nativeErr,
            errMsg, MSG_LNG,
            &errMsgLen);
            switch (ret) {
                case SQL_SUCCESS:
                fprintf(stderr, "*** %s\n"
                "*** ODBC Error/Warning = %s, "
                "TimesTen Error/Warning "
                " = %d\n",
                errMsg, sqlState,
                nativeErr);
                break;
                case SQL_SUCCESS_WITH_INFO:
                fprintf(stderr, "*** Call to SQLError"
                " failed with return code of "
                "SQL_SUCCESS_WITH_INFO.\n "
                "*** Need to increase size of"
                " message buffer.\n");
                break;
                case SQL_INVALID_HANDLE:
                fprintf(stderr, "*** Call to SQLError"
                " failed with return code of "
                "SQL_INVALID_HANDLE.\n");
                break;
                case SQL_ERROR:
                fprintf(stderr, "*** Call to SQLError"
                " failed with return code of "
                "SQL_ERROR.\n");
                break;
                case SQL_NO_DATA_FOUND:
                break;
            } /* switch */
        } /* while */

        if (rc != SQL_SUCCESS_WITH_INFO && err_is_fatal) {
            fprintf(stderr, "Exiting.\n");
            //exit(-1);
        }
    }
}  