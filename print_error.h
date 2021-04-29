static bool chkReturnCode(SQLRETURN rc, SQLHENV henv, SQLHDBC hdbc, SQLHSTMT hstmt, char* msg, char* filename, int lineno, BOOL err_is_fatal)
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
            return (bool)1;
        }
    }

    return (bool)0;
}  