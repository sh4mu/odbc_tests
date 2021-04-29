#include <sql.h>
#include <sqlext.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

#include "Environment.h"
#include "Statement.h"

using namespace std;

//static void chkReturnCode(SQLRETURN rc, SQLHENV henv, SQLHDBC hdbc, SQLHSTMT hstmt, char* msg, char* filename, int lineno, BOOL err_is_fatal);

#define DEFAULT_CONNSTR "DSN=ttdatasvr64;TTC_SERVER=0-0-2;TCP_PORT=14502"
#define MAXCOLS 100
#define COL_LEN_MAX 1000

int main(int ac, char** av)
{
    Environment env;
    env.init();

    env.addStatement("select count(*) from UCDF_UserDN;");

    env.executeStatement();

    printf("Sleep for 10\n");
    sleep(10);

    env.executeStatement();

    return 0;
}