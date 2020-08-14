#ifndef  YUYI_H_
#define YUYI_H_
#include<stdio.h>
#include<iostream>
#include<cstring>
#include<string>
#include<fstream>
#include<sstream>

using namespace std;

//树
struct TreePoint {
	int idnumber;//属性，该节点相关的id在符号表中的序号
	int sonnumber = 0;
	int in = 0;//继承属性in，一般是符号表的type值。
	int idnum = 0;//属性，id
	int scope = 0;//和id表里的scope意思相同
	int call_p_s = 0;//是否为函数的形参

	string text;//id的名称
	TreePoint* sonPoint[11] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };//长度固定，tai'duan'l
	TreePoint* fatherPoint = NULL;
	int sonBranch[7] = { 0 };
};

//extern string TYPE[10] = { "default","char","real","integer","Boolean","array","const","function","procedure","record" };

//符号表表项
struct SIdTable {
	int idnumber;
	string id;
	int type;//1 char 2 real 3 integer 4 boolean 5 array 6 const 7 function 8 procedure 9 record
	int dimention;//维度or参数种类
	int array_type;//数组类型
	int scope;//该符号属于哪个作用域（main、或者子函数等等。从0到n

	/*function和procedure中特有变量*/
	int parameter_type[10];//每种形参的类型。
	int parameter_num[10] = { 0 };//每种形参的个数
	int returntype;//返回值类型。

	/*常量中特有变量*/
	int const_type;//常量的类型 1 char 2 real 3 integer 4 boolean (引用id的情况) 5 num 6 letter
	int const_id_type;
};

/*此二变量的作用是识别符号的作用域*/
//int FandP_num=0;//程序中的function和procedure数量
//int FandPinTable[10];//function和procedure在符号表中的位置-》作用域在符号表中的分布

extern int scopenum;//域的数量（函数和过程的数量，0表示只有main）
extern int SCOPE[10];//每个域的范围。默认全局变量只能在开头定义。2020/7/16

extern int Main_num;//全局变量的个数 我也不知道有没有用

extern SIdTable idTable[100];//符号表
extern int idTable_num;//符号数量

extern TreePoint *root;//树的根节点
extern int broot;
extern TreePoint *nowroot;

extern void generate();
#endif