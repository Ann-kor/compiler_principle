
#ifndef  IR_H_
#define IR_H_

#include<cstdio>  
#include<cstring>  
#include<algorithm>  
#include<iostream>  
#include <string>
#include<vector>  
#include<stack>  
#include<bitset>  
#include<cstdlib>  
#include<cmath>  
#include<set>  
#include<list>  
#include<deque>  
#include<map>  
#include<queue> 
#include<iomanip>
#include<fstream>
#include "yuyi.h"

using namespace std;
typedef struct loc {
	string self_name;
	int self_loc[2];
	string parent_name;
	int parent_loc[2];
	vector<vector<loc>> sub;
}loc;
class Ir {

public:
	const string key_list[50] = {"begin""case","do","downto","else","end","for","function","if","of","procedure",
								"repeat","then","to","until","while" };
	map<string, int> BinopPrecedence;

	loc* present;

	int key_num = 30;
	Ir() = default;
	void backpath(int p, int t);												//回填函数
	int merge();															//并链函数
	string newtemp();														//回送一个新的临时变量名
	int emit(string op, string arg2, string arg1, string res);	//四元式生成
	int error();//int error(int err_num, int n);
	SIdTable* lookup(string name);//查符号表
	int getToken(string str);
	void bexp(char *token);
	void bt();
	int reserve(string token);
	int isRelop(string op);
	void handleExpression(vector<vector<string>> token, int start, int end);
	int handleIf(int begin);
	void handleWhile(vector<vector<string>> token, int start, int end, vector<loc> root);
	void handleFor(vector<vector<string>> token, int start, int end, vector<loc> root);
	void handleRepeat(vector<vector<string>> token, int start, int end, vector<loc> root);
	void handleCase(vector<vector<string>> token, int start, int end, vector<loc> root);
	int handleExpression(int begin,vector<string> s);
	int handleSimpleExpression(int begin, vector<string> s);
	int handleFactor( vector<string> s);
	int handleTerm(vector<string> s);
	int handleAssign();
	void handleWhile();
	void handleCase();
	void handleFor();
	void handleRepeat();
	void handleBegin();
	void handleProcedure();
	void handleFunction();
	void setRelopPrecedence();
	void generate();
	void getNextToken();
	void setIDValue(string name);
	int getIDValue(string name);
	void searchDown(int i);
	void searchUp();
	int* newlabel();
	int getTokPrecedence(string s);
	float getNum(string num);
	//rhs expressionRHS(int ExprPrec, string LHS,vector<string> s);
	//rhs expressionOneRHS();
	int getConstNum(string s);
	int getFunctionLoc(string name);
	int getProcedureLoc(string name);
};
enum tokenNum {
	token_begin = -1,
	token_num = -2,
	token_if = -3,
	token_while = -4,
	token_end = -5,
	token_case = -6,
	token_for = -7,
	token_repeat = -8,
	token_then = -9,
	token_do = -10,
	token_downto = -11,
	token_of = -12,
	token_until = -13,
	token_to = -14,
	token_id = -15,
	token_procedure = -16,
	token_function = -17,
	token_over = -18
};
typedef struct
{
	string res;
	string arg1;
	string op;
	string arg2;
}D_quad;				//四元式结构体

extern const int END;						//四元式结束地址
extern int pGString;							//语法输入串指针
extern D_quad quad[500];						//四元式数组
extern int pQuad;								//四元式数组指针
extern void generateIR();
extern TreePoint *curtok;
extern int quad_tok;
extern int start_number, end_number, present_number;//记录现在分析的开始
extern bool begin_token , if_token , case_token,
	while_token, repeat_token , for_token;
extern vector<loc> block;

struct yuju {
	int next;
	int right; //true
	int left;//false
	int code;
	int begin;
};
struct block {
	int begin;
	int next;
};
typedef struct rhs {
	int end_loc;
	string temp_id;
}rhs; 
typedef struct func {
	string name;
	int begin;
	int end;
}func;

#endif