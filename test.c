/* Standard C/C++ Headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// sudo apt install unixodbc-dev
#include <sql.h>
#include <sqlext.h>

void check(char *loc,RETCODE rc);
void check2(char *loc,RETCODE rc);

/* Handles */   
SQLHANDLE henv = SQL_NULL_HENV;;
SQLHANDLE hdbc = SQL_NULL_HDBC;
SQLHSTMT hstmt1 = SQL_NULL_HSTMT;

// gcc test.c -lodbc
int main(int argc, char** argv)
{
	RETCODE rc;
    long l_id=1;
	char name[100];
	SQLLEN name_len;
	SQLLEN rowcount;
	long id;
	SQLLEN id_len;

 	// SQLHANDLE 変数を用意して初期化する
	SQLAllocHandle (SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	SQLSetEnvAttr (henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
	SQLAllocHandle (SQL_HANDLE_DBC, henv, &hdbc);

	// データベースに接続
	rc = SQLConnect (hdbc, (SQLCHAR *)"irisdatasource", SQL_NTS, (SQLCHAR *)"_system", SQL_NTS, (SQLCHAR *)"SYS", SQL_NTS);

	SQLAllocHandle (SQL_HANDLE_STMT, hdbc, &hstmt1);
	
	// 変数をバインドする
	rc = SQLPrepare (hstmt1, (SQLCHAR *)"INSERT INTO TEST (P) VALUES (?)", SQL_NTS);
	check("prepare",rc);
	rc=SQLBindParameter (hstmt1, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(name), 0, name, 0, NULL);
	check("bindpara",rc);

	char value[]="日本語の文字列";
	printf("strlen value: %ld\n",strlen(value));

	//sprintf(value,"%d",100);
	strncpy(name, value, strlen(value));

	// SQL を実行する
	rc = SQLExecute (hstmt1);
	check("execute",rc);

	SQLRowCount(hstmt1,&rowcount);
	printf("SQLRowCount:%ld\n",rowcount);

	rc=SQLBindParameter (hstmt1, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &l_id, 0, NULL);
	check("SQLBindParameter",rc);

	// SQL を実行する(Direct)
	rc = SQLExecDirect( hstmt1, "SELECT ID,P FROM TEST WHERE ID=?",SQL_NTS );
	check("SQLExecDirect",rc);

	rc = SQLBindCol( hstmt1, 1, SQL_C_SLONG, &id, sizeof(id), &id_len);
	check("SQLBindCol",rc);

	rc = SQLBindCol( hstmt1, 2, SQL_C_CHAR, name, sizeof(name), &name_len);
	check("SQLBindCol",rc);

	rc = SQLFetch( hstmt1 );
	check("SQLFetch",rc);

	printf("name is %s. Length is %ld. ID is %ld.\n",name,name_len,id);

	rc = SQLCloseCursor( hstmt1 );
	check("SQLCloseCursor",rc);

	// 接続を切断し、ハンドルを開放する
	SQLFreeStmt(hstmt1,SQL_DROP);

	SQLDisconnect (hdbc);
	SQLFreeHandle (SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle (SQL_HANDLE_ENV, henv);

}
void check(char *loc,RETCODE rc) {

	SQLCHAR Sqlstate[10];
	SQLINTEGER NativeError;
    SQLCHAR MessageText[SQL_MAX_MESSAGE_LENGTH];
    SQLSMALLINT TextLength;
	RETCODE rcode2;

	if (rc==100) return;

	if (rc) {
		rcode2=SQLError(henv,
           hdbc, hstmt1,
           Sqlstate, &NativeError,
           MessageText, SQL_MAX_MESSAGE_LENGTH - 1,
           &TextLength);
		if (rcode2 == SQL_ERROR) {
			printf ("SQLError failed!\n");
		}
		else if(rcode2!= SQL_NO_DATA_FOUND) {
			printf("NativeError=%d\n",NativeError);
			printf("Sqlstate=%s\n",Sqlstate);
			printf("message=%s \n",MessageText);
		}
		printf("Error at %s, Hit Enter\n",loc);
		getchar();
	}
}
void check2(char *loc,RETCODE rc) {

	SQLCHAR Sqlstate[10];
	SQLINTEGER NativeError;
    SQLCHAR MessageText[SQL_MAX_MESSAGE_LENGTH];
    SQLSMALLINT TextLength;
	RETCODE rcode2;

	if (1) {
		rcode2=SQLError(henv,
           hdbc, hstmt1,
           Sqlstate, &NativeError,
           MessageText, SQL_MAX_MESSAGE_LENGTH - 1,
           &TextLength);
		if (rcode2 == SQL_ERROR) {
			printf ("SQLError failed!\n");
		}
		else if(rcode2!= SQL_NO_DATA_FOUND) {
			printf("NativeError=%d\n",NativeError);
			printf("Sqlstate=%s\n",Sqlstate);
			printf("message=%s \n",MessageText);
		}
		printf("Error at %s, Hit Enter\n",loc);
		getchar();
	}
}

