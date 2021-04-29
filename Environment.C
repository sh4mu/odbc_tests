#include <sql.h>
#include <string>
#include <sqlext.h>
#include <unistd.h>

#include "Environment.h"
#include "Statement.h"
#include "print_error.h"

Environment::~Environment() {
    printf("~Environment\n");
    disableConnect();
    disableEnv();
}

void Environment::init()
{
    initEnv();
    initConnect();
}

void Environment::initEnv()
{
    printf("Environment::initEnv()\n");
    
    SQLRETURN rc = SQL_SUCCESS;
    henv = SQL_NULL_HENV;

    // Shared by all connections
    rc = SQLAllocEnv(&henv);
    if (rc != SQL_SUCCESS) {
        fprintf(stderr, "Unable to allocate an environment handle\n");
        return;   
    }
    printf("SQLAllocEnv %d\n", rc);

    mState = Environment::ENV;
}

void Environment::initConnect()
{
    printf("Environment::initConnect()\n");

    SQLRETURN rc = SQL_SUCCESS;
    hdbc = SQL_NULL_HDBC;
    
    rc = SQLAllocConnect(henv, &hdbc);
    if(chkReturnCode(rc, henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, "Unable to allocate a connection handle\n", __FILE__, __LINE__, 1)) {
        return;
    }
    printf("SQLAllocConnect %d\n", rc);

    rc = SQLDriverConnect(hdbc, 0, (SQLCHAR *)mConnectionString.c_str(), mConnectionString.length(), connOut, sizeof(connOut), &connOutLen, SQL_DRIVER_NOPROMPT);
    if(chkReturnCode(rc, henv, hdbc, SQL_NULL_HSTMT, "Error in connecting to the database\n", __FILE__, __LINE__, 1)) {
        return;
    }
    printf("SQLDriverConnect %d\n", rc);

    rc = SQLSetConnectOption(hdbc, SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_ON);
    if(chkReturnCode(rc, henv, hdbc, SQL_NULL_HSTMT, "Error in connecting to the database\n", __FILE__, __LINE__, 1)) {
        return;
    }

    mState = Environment::CONNECTION;
}

void Environment::disableEnv() 
{
    printf("Environment::disableEnv()\n");

    SQLRETURN rc = SQL_SUCCESS;

    rc = SQLFreeEnv(henv);
    chkReturnCode(rc, henv, SQL_NULL_HDBC, SQL_NULL_HSTMT,"Unable to free the environment handle\n", __FILE__, __LINE__, 0);

    mState = Environment::CLOSED;
}

void Environment::disableConnect() 
{
    printf("Environment::disableConnect()\n");

    SQLRETURN rc = SQL_SUCCESS;

    rc = SQLDisconnect(hdbc);
    chkReturnCode(rc, henv, hdbc, SQL_NULL_HSTMT, "Unable to close the connection\n", __FILE__, __LINE__, 0);
    
    rc = SQLFreeConnect(hdbc);
    chkReturnCode(rc, henv, hdbc, SQL_NULL_HSTMT,"Unable to free the connection handle\n",  __FILE__, __LINE__, 0);

    mState = Environment::ENV;
}

void Environment::addStatement(string sql) 
{
    mStatement = new Statement(sql);

    mStatement->init(this);

    if(mStatement->mState != Statement::READY) {
        printf("Environment::addStatement() SQLFreeConnect\n");
        SQLFreeConnect(hdbc);
    }
}

void Environment::executeStatement() 
{
    //string sql
    mStatement->executeStatement();

    if(mStatement->mState == Statement::READY_NOK)
    {
        printf("Environment::executeStatement() Statement::READY_NOK\n");

        disableConnect();

        while(mState != Environment::CONNECTION) {
            printf("Try Connecting\n");
            initConnect();
            sleep(1);
        }
        
        printf("Environment Connected\n");
        mStatement->init(this);

        if(mStatement->mState != Statement::READY) {
            printf("ERROR Cannot Prepare Statement\n");
            return;
        }

        mStatement->executeStatement();
    }
}
