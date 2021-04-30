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

/* 
* Return true when SQL RC is not SQL_SUCCESS
*/
static bool printException(SQLRETURN rc, SQLHANDLE handle, SQLSMALLINT handleType)
{
    if (rc != SQL_SUCCESS && rc != SQL_NO_DATA_FOUND ) 
    {
            SQLLEN numRecs = 0;
            SQLGetDiagField(handleType, handle, 0, SQL_DIAG_NUMBER, &numRecs, 0, 0);
            printf("printException numRecs %d\n", numRecs);

            //SQLCHAR* theDiagState=new SQLCHAR[50];
            //theDiagState[0]='\0';
            SQLCHAR theDiagState[6], Msg[SQL_MAX_MESSAGE_LENGTH];
            SQLINTEGER nativeError=0;
            //SQLCHAR* theMessageText = new SQLCHAR[1000];
            SQLSMALLINT   i, MsgLen;

            // Get the status records.
            i = 1;  

            //printf("printException rc %d, %d, %d, %d, %d\n", SQL_SUCCESS, SQL_SUCCESS_WITH_INFO, SQL_NO_DATA, SQL_ERROR, SQL_INVALID_HANDLE);
            //printf("printException rc %d\n", rc);

            while (i <= numRecs && (rc = SQLGetDiagRec(handleType, handle, i, theDiagState, &nativeError, Msg, sizeof(Msg), &MsgLen)) != SQL_NO_DATA) 
            {   
                char buf[1024]; sprintf(buf, "[%5.5s]", theDiagState);
                printf("printException nativeError %d\n", nativeError);
                printf("printException theDiagState %s\n", buf);

                std::string theMessageTextAsString = std::string((char *)Msg,sizeof(Msg));
                    printf("printException MessageText %s\n", theMessageTextAsString.c_str());                

                 i++;  
            }  

            //delete [] Msg;
            //delete [] theDiagState;

        return true;                
    }

    return false;
}
