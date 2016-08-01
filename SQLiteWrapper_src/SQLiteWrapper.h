/* 
   SQLiteWrapper.h

   Copyright (C) 2004 Ren¨¦ Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   Ren¨¦ Nyffenegger rene.nyffenegger@adp-gmbh.ch
*/

#ifndef __SQLITE_WRAPPER_H__
#define __SQLITE_WRAPPER_H__

#pragma warning(disable: 4786)

#include <string>
#include <vector>

#include "sqlite3.h"

class SQLiteStatement 
{
    friend class SQLiteWrapper;
	
public:
    SQLiteStatement();
	virtual ~SQLiteStatement();

private:
    SQLiteStatement(const std::string& statement, sqlite3* db);

public:
    enum dataType
	{
		INT = SQLITE_INTEGER,
		FLT = SQLITE_FLOAT  ,
		TXT = SQLITE_TEXT   ,
		BLB = SQLITE_BLOB   ,
		NUL = SQLITE_NULL   ,
    };

public:
    int         ValueInt(int pos_zero_indexed);
    dataType	DataType(int pos_zero_indexed);
    std::string ValueString(int pos_zero_indexed);
	
	
    bool BindNull(int pos_zero_indexed);
    bool Bind    (int pos_zero_indexed, int value);
    bool Bind    (int pos_zero_indexed, double value);
    bool Bind    (int pos_zero_indexed, const std::string& value);
	
    bool Execute();
    bool NextRow();
	
    /* Call Reset if not depending on the NextRow cleaning up.
	   For example for select count(*) statements */
    bool Reset();
    bool RestartSelect();
	
private:
    sqlite3_stmt* stmt_;
};

class SQLiteWrapper
{
public:
    SQLiteWrapper();
	virtual ~SQLiteWrapper();

public:
    bool Open(const std::string& db_file);
    int  Close();
	
    class ResultRecord 
	{
    public:
        std::vector<std::string> fields_;
    };
	
    class ResultTable
	{
		friend class SQLiteWrapper;
	public:
        ResultTable() : ptr_cur_record_(0) { }
		
        std::vector<ResultRecord> records_;
		
        ResultRecord* next()
		{
			if (ptr_cur_record_ < records_.size())
			{
				return &(records_[ptr_cur_record_++]);
			}
			return 0;
        }
		
	private:
		void reset()
		{
			records_.clear();
			ptr_cur_record_ = 0;
		}
		
	private:
        unsigned int ptr_cur_record_;
    };

public:
    SQLiteStatement* Statement(const std::string& stmt);
    bool DirectStatement      (const std::string& stmt);
    bool SelectStmt           (const std::string& stmt, ResultTable& res);
	
    bool Begin   ();
    bool Commit  ();
    bool Rollback();
    
    std::string LastError();
	
private:
	static int SelectCallback(void *p_data, int num_fields, char **p_fields, char **p_col_names);
	sqlite3* db_;
};

#endif // !defined(__SQLITE_WRAPPER_H__)

