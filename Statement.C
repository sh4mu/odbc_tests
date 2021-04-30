#include <sql.h>
#include <string>
#include <string.h>
#include <sqlext.h>

#include "Statement.h"
#include "print_error.h"
#include "Environment.h"

void Statement::init(Environment* env)
{
    SQLRETURN rc = SQL_SUCCESS;

    mEnvironment = env;
    hstmt = SQL_NULL_HSTMT;
    mState = IDLE;

    rc = SQLAllocStmt(mEnvironment->m_hdbc, &hstmt);
    if (printException(rc, hstmt, SQL_HANDLE_STMT)) {
//    if(chkReturnCode(rc, mEnvironment->henv, mEnvironment->hdbc, SQL_NULL_HSTMT, "Unable to allocate a statement handle\n", __FILE__, __LINE__, 1)) {            
        return;
    }

    printf("SQLAllocStmt %d \n", rc);            

    rc = SQLSetStmtAttr(hstmt, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)2, SQL_IS_UINTEGER);
    if (printException(rc, hstmt, SQL_HANDLE_STMT)) {
    //if(chkReturnCode(rc, mEnvironment->henv, mEnvironment->hdbc, SQL_NULL_HSTMT, "Unable to allocate a statement handle\n", __FILE__, __LINE__, 1)) {            
        return;
    }

    rc = SQLPrepare(hstmt,(UCHAR*) mSql.c_str(), SQL_NTS);
    if (printException(rc, hstmt, SQL_HANDLE_STMT)) {
    //if(chkReturnCode(rc, mEnvironment->henv, mEnvironment->hdbc, hstmt, "Unable to SQLPrepare\n", __FILE__, __LINE__, 1)) {
        return;
    }
    
    printf("SQLPrepare %d \n", rc);
    mState = Statement::READY;
}

void Statement::executeStatement()
{
    SQLRETURN rc = SQL_SUCCESS;

    /* Your application code here */
    rc = SQLExecute(hstmt);
    printf("SQLExecute %d \n", rc);     

    if (printException(rc, hstmt, SQL_HANDLE_STMT)) 
    {  
        mState = Statement::READY_NOK;
        return;
    }

 //   if(chkReturnCode(rc, mEnvironment->henv, mEnvironment->hdbc, hstmt,"Unable to Exec Direct\n", __FILE__, __LINE__, 1)) 
  //  {  
   // }

    if (hstmt != SQL_NULL_HSTMT) {
        rc = SQLFreeStmt(hstmt, SQL_CLOSE);
        printf("SQLFreeStmt %d \n", rc);        
        //chkReturnCode(rc, henv, hdbc, hstmt,"Unable to free the statement handle\n",  __FILE__, __LINE__, 0);
    }
} 