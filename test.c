/* Standard C/C++ Headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// sudo apt install unixodbc-dev
#include <sql.h>
#include <sqlext.h>

//#include <locale.h> // setlocale()

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

	/*
	// 効果見られず
	char *old_locale = setlocale (LC_ALL, NULL);
	printf("old_locale: %s\n",old_locale);

	char *new_locale = setlocale (LC_ALL, "ja_JP.eucJP");
	printf("new_locale: %s\n",new_locale);
	*/

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

	char value[]="日本語";
	/*
	// 文字コードの切り替えは-fexec-charsetで可能
	//char value[] = {0xE6, 0x97, 0xA5, 0xE6, 0x9C, 0xAC, 0xE8, 0xAA, 0x9E,'\0'};  //utf8
	//char value[] = {0xC6, 0xFC, 0xCB, 0xDC, 0xB8, 0xEC, '\0'}; //euc-jp
	//char value[] = {0x93, 0xFA, 0x96, 0x7B, 0x8C, 0xEA, '\0'}; //SJIS
	*/
	printf("value %s. strlen of value: %ld\n",value,strlen(value));

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

	rc = SQLRowCount(hstmt1,&rowcount);
	printf("SQLRowCount:%ld\n",rowcount);

	SQLPOINTER attr;
	SQLHDESC   hdesc = NULL;
	rc = SQLGetStmtAttr( hstmt1,SQL_ATTR_CURSOR_SCROLLABLE, &attr,-6,NULL);
	check("SQLGetStmtAttr",rc);
	printf("SQLGetStmtAttr:%d\n",(int)attr); //cause warning

	SQLSMALLINT ColumnCount;
	rc = SQLNumResultCols( hstmt1, &ColumnCount);
	check("SQLNumResultCols",rc);
	printf("SQLNumResultCols:%d\n",(int)ColumnCount);

	#define MAXCOLS 2
	SQLCHAR         colname[32];
	SQLSMALLINT     coltype;
	SQLSMALLINT     colnamelen;
	SQLSMALLINT     nullable;
	SQLULEN      collen[MAXCOLS];
	SQLSMALLINT     scale;

 	for (int i = 0; i < ColumnCount; i++) {
		rc = SQLDescribeCol (hstmt1, i+1, colname, sizeof (colname), &colnamelen, &coltype, &collen[i], &scale, &nullable);
		check("SQLDescribeCol",rc);
		printf("SQLDescribeCol/ColumnName:%s ColumnSizePtr:%ld DecimalDigitsPtr: %d\n", colname,collen[i],scale);

		//rc = SQLColAttribute(hstmt1, i+1,SQL_COLUMN_UNSIGNED);
	}

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

