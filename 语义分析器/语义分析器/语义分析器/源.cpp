#include<stdio.h>
#include<iostream>
#include<cstring>
#include<string>
#include<fstream>
#include<sstream>
#include "yuyi.h"
#include "ir.h"

using namespace std;

//树

string TYPE[10] = { "default","char","real","integer","Boolean","array","const","function","procedure","record" };

int scopenum = 0;//域的数量（函数和过程的数量，0表示只有main）
int SCOPE[10];//每个域的范围。默认全局变量只能在开头定义。2020/7/16

int Main_num = 0;//全局变量的个数 我也不知道有没有用

SIdTable idTable[100];//符号表
int idTable_num = 0;//符号数量

TreePoint *root = new TreePoint;//树的根节点
int broot = 0;
TreePoint *nowroot;


string word[1001];
int word_num = 0;
ofstream tree("tree.txt");
ofstream listid("idlist.txt");
ofstream bt("analyze.txt");


int endflag = 0;

/*
函数名称：BuildTree
函数功能：根据word数组中的内容和父亲节点,通过递归的方式构建语法分析树。建立好的树输出于tree.txt
当前版本：1.0
版本作者：流星雨
日期：2020/6/1
*/

void BuildTree(TreePoint *father,int orderNum) {
	//TreePoint *nowRoot;
	//nowRoot = root;
	//int left = 0, right = 0;
	//cout << "in"<<endl;
	if (endflag) {
		//cout << "return" << endl;
		return;
	}
	if (word_num == 1001) {
		if (orderNum >= 1000) {
			endflag = 1;
			return;
		}
	}
	else if (orderNum >= word_num) {
		//cout << "endflag=1" << endl;
		endflag = 1;
		return;
	}
	if (word[orderNum].length() == 1) {
		char *b= (char*)word[orderNum].c_str();
		if (b[0] == '\'' ||b[0]==':') {
			//cout << "l=1 return" << endl;
			BuildTree(nowroot, orderNum + 1);
			return;
		}
	}
	if (word[orderNum].compare("->") == 0) {
		//cout<<orderNum<< "-> return" << endl;
		BuildTree(nowroot, orderNum + 1);
		return;
	}

	//cout << "not return" << endl;
	int i;
	i = father->sonnumber;
	int matched = 0;

	if (word[orderNum + 1].compare("->") == 0) {
		//cout << "in1:" << orderNum << " : " << word[orderNum] << endl;
		while (1) {
			for (int j = 0; j < father->sonnumber; j++) {
				if ((word[orderNum].compare(father->sonPoint[j]->text)==0)
					&& (father->sonBranch[j] == 0)) 
				{
					//cout << "matched:" << j << endl;
					matched = 1;
					nowroot = father->sonPoint[j];
					father->sonBranch[j] = 1;
					for (int k = 0; k < j; k++)father->sonBranch[k] = 1;
					BuildTree(nowroot, orderNum + 1);
					if (endflag)return;
					matched = 0;
					break;
				}
				else {
					//cout << "not matched:" << j << endl;
					//cout << word[orderNum] << endl;
					//cout << father->sonPoint[j]->text << endl;
					//cout << father->sonBranch[j] << endl;
				}
			}
			if (matched == 0) {
				if (nowroot->fatherPoint == NULL) {
					//cout << "nofather!" << endl;
					break;
				}
				else {
					nowroot = nowroot->fatherPoint;
					father = father->fatherPoint;
					//cout << "wrong father! and back to grandfather" << endl;
					//cout << nowroot->text << endl;
				}
				//else cout << "no father!" << endl;
			}
		}
	}
	else {
		//cout << "in2" << endl;
		TreePoint *a = new TreePoint;
		a->text = word[orderNum];
		father->sonPoint[i] = a;
		father->sonnumber++;
		a->fatherPoint = father;
		tree << orderNum<<" :  父亲:" << father->text << "->儿子" << i << ":" << a->text << endl;
		//int b;
		//if (orderNum == 660)>> b;
		//cout << word_num << endl;
		BuildTree(nowroot, orderNum + 1);
	}

	return;
}

/*
函数名称：readandmaketree
函数功能：从文件中读取字符串并构建一棵语法分析树
当前版本：1.0
版本作者：流星雨
日期：2020/6/1
*/

void ReadinAndMakeTree() {
	ifstream in("result.txt");
	ofstream out("word.txt");
	string line;
	if (!in)cout << "打开文件失败\n" << endl;
	else {
		while (getline(in, line)) {
			stringstream ss(line);
			string token;
		
			while (ss >> token) {
				word[word_num] = token;
				word_num++;
				//cout << word_num - 1 << ":" << token << endl;
				while (word_num == 688)
					break;
				out << word_num-1<<":"<<token << endl;
				if (word_num == 1001) {
					if (broot == 0) {
						root->text = word[0];
						nowroot = root;
						broot = 1;
						BuildTree(nowroot, 1);
					}
					else {
						BuildTree(nowroot, 0);
					}
					word[0] = word[1000];
					for (int i = 1; i < 1001; i++)word[i].clear();
					word_num = 1;
				}
			}		
		}
		//cout << "dy4  " << dy << endl;
		if (broot == 0) {
			root->text = word[0];
			nowroot = root;
			broot = 1;
			BuildTree(nowroot, 1);
		}
		else {
			BuildTree(nowroot, 0);
		}
	}
	in.close();
	out.close();
	tree.close();
}

/*
函数名称：check
函数功能：根据id名称和域判断是否重命名。重名返回重名id的位置，未重名返回-1。
应该是不能和main中的重名？还是可以呢？先设定不可以吧。
版本1.0
版本作者：流星雨
日期：2020/7/16
*/

int checkID(string idd, int scope) {
	int i;
	if (scope == 0) {
		for (i = 0; i <= SCOPE[scope]; i++) {
			if (idd.compare(idTable[i].id) == 0)return(i);
		}
		return -1;
	}
	else {
		for (i = 0; i <= SCOPE[scope]; i++) {
			if (idd.compare(idTable[i].id) == 0)return i;
		}
		for (i = SCOPE[scope - 1]; i <= SCOPE[scope]; i++) {
			if (idd.compare(idTable[i].id) == 0)return i;
		}
		return -1;
	}
}

/*
函数名称：analyze
函数功能：对语法分析树的每个节点进行遍历，匹配。
版本：1.0
版本作者：流星雨
日期：2020/6/2

最新版本：2.0
版本作者：流星雨
日期：2020/7/16
更改内容：添加注释，完善关于作用域的内容。
*/

int error = 0;

void Analyze(TreePoint *father) {

	if (error == 1)return;

	if (father->text.compare("program") == 0) {
		bt << endl;
		bt << "program->program_1" << endl;
		father->scope = 0;
		father->sonPoint[0]->scope = father->scope;
		Analyze(father->sonPoint[0]);//program->program_1。必定为树的根。
		return;//分析完直接return，不再往下进行判断。
	}

	else if (father->text.compare("program_1") == 0) {
		if (father->sonnumber == 1) {
			bt << endl;
			bt << "prgram_1->enmpy" << endl;
			return;//program_1->empty
		}

		/*以下为program->id(identifier_list);program_body.program_1*/
		/*创建最开始的program项 默认为第0项*/
		/*啥意思？↑大概是位于main里面的意思？*/
		/*这个地方把program的名称也作为符号存入符号表中了*/
		idTable[idTable_num].id = father->sonPoint[0]->sonPoint[0]->text;
		idTable[idTable_num].scope = father->scope;
		bt << "创建program：" << idTable_num << endl;
		bt << "program的名称为：" << idTable[idTable_num].id << endl;
		father->idnumber = idTable_num;
		idTable_num++;

		bt<<endl << "program_1->id(identifier_list);program_body.program_1,1" << endl;
		father->sonPoint[1]->scope = father->scope;
		Analyze(father->sonPoint[1]);
		//先将identifier_list分析完，并把list中的变量数存到兄弟节点的idnum值中

		/*7.16：把创建id表项的内容移动到分析过程中了，所以这一段没用了
		int main_idlist = father->sonPoint[0]->idnum;
		TreePoint*a = father->sonPoint[1];
		for (int j = 1; j <= main_idlist; j++) {
			idTable[idTable_num].id = a->sonPoint[0]->sonPoint[0]->text;
			bt << "创建形参项：" << idTable_num << " 名称为:" 
				<< a->sonPoint[0]->sonPoint[0]->text << endl;
			idTable_num++;
		}
		*/
		father->sonPoint[2]->scope = father->scope;//继承作用域属性，表示在main中
		Analyze(father->sonPoint[2]);

		Analyze(father->sonPoint[3]);//第三个子节点为program_1，肯定是空的。
		return;
	}

	else if (father->text.compare("program_body")==0) {

		/*全部赋值 作为main的内容，其中subprogram会被另外赋值为子函数在表中的位置。*/
		for (int i = 0; i <= 4; i++) {
			father->sonPoint[i]->scope = father->scope;
			father->sonPoint[i]->idnumber = father->idnumber;//忘记拿这个变量干啥了

		}
		bt << endl << "program_body → const_declarations type_declarations var_declarations subprogram_declarations compound_statement" << endl;

		Analyze(father->sonPoint[0]);

		cout << "const" << endl;
		Analyze(father->sonPoint[1]);
		cout << "type" << endl; 
		Analyze(father->sonPoint[2]);
		cout << "var" << endl;
		Analyze(father->sonPoint[3]);
		cout << "sub" << endl;
		Analyze(father->sonPoint[4]);
		cout << "compound" << endl;
		return;
	}

	else if (father->text.compare("identifier_list") == 0) {
		bt << endl << "identifier_list->id identifier_list_1" << endl;
		for (int i = 0; i <= 1; i++) {
			father->sonPoint[i]->scope = father->scope;
			father->sonPoint[i]->in = father->in;//该节点的儿子0继承该节点的类型属性
		}

		Analyze(father->sonPoint[1]);//分析儿子1,即为identifier_list_1
		father->idnum = father->sonPoint[1]->idnum + 1;
		
		Analyze(father->sonPoint[0]);
		return;
	}

	/*7.16*/
	else if (father->text.compare("id") == 0) {
		bt << endl << "id->" << father->sonPoint[0]->text << endl;

		string id = father->sonPoint[0]->text;
		if (checkID(id, father->scope) != -1) {
			bt << "变量重命名！！" << endl;
			return;
		}
		else {
			idTable[idTable_num].id = father->sonPoint[0]->text;
			idTable[idTable_num].type = father->in;
			if (father->in == 5) {
				idTable[idTable_num].array_type = idTable[99].array_type;
				idTable[idTable_num].dimention = idTable[99].dimention;
			}
			idTable[idTable_num].scope = father->scope;
			SCOPE[father->scope] = idTable_num;
			idTable_num++;
		}
		bt << "在id表中创建id  :"<<idTable_num-1 << endl;
		bt << "该项的类型为：" << father->in << endl;
		return;
	}

	else if (father->text.compare("identifier_list_1")==0) {
		if (father->sonPoint[0]->text.compare("empty")==0) {
			bt << endl << "identifier_list_1 →  empty" << endl;
			father->idnum = 0;//该节点中id的数量为0
			//节点为空，没有类型属性
			return;
		}
		else {
			bt << endl << "identifier_list_1 → , id identifier_list_1 " << endl;
			Analyze(father->sonPoint[1]);
			father->idnum = father->sonPoint[1]->idnum + 1;//该节点中id的数量等于儿子1的id数量+1
			
			for (int i = 0; i <= 1; i++) {
				father->sonPoint[i]->scope = father->scope;
				father->sonPoint[i]->in = father->in;//该节点的儿子0继承该节点的类型属性
			}
			Analyze(father->sonPoint[0]);
		}
		return;
	}

	else if (father->text.compare("const_declarations") == 0) {
	cout << "const_1" << endl;
		if (father->sonPoint[0]->text.compare("empty") == 0) {
			bt << endl << "const_declarations →  empty" << endl;
			return;
		}
		
		bt << endl << "const_declarations → const const_declaration" << endl;
		father->sonPoint[1]->scope = father->scope;
		Analyze(father->sonPoint[1]);
		return;
	}

	else if (father->text.compare("const_declaration") == 0||father->text.compare("const_declaration_1") == 0) {
		bt << endl << "const_declaration → id = const_variable ; const_declaration_1" << endl;
		bt << "或者:const_declaration_1 → ; id = const_variable ; const_declaration_1" << endl;
		if (father->sonPoint[0]->text.compare("empty") == 0)return;
		
		father->sonPoint[0]->in = 6;//类型为常量。
		//father->sonPoint[0]->constent = 1;//类型为常量。

		father->sonPoint[0]->idnumber = idTable_num;//标记其在符号表中的位置,以便于记录信息。
		father->sonPoint[2]->idnumber = idTable_num;
		//father->sonPoint[2]->idnumber = idTable_num;
		father->idnumber = idTable_num;

		string id = father->sonPoint[0]->sonPoint[0]->text;
		if (checkID(id, father->scope) != -1) {
			bt << "const变量名重复！" << endl;
			return;
		}
		else {
			//进行符号表搭建
			idTable[idTable_num].type = 6;
			idTable[idTable_num].idnumber = idTable_num;

			bt << "新建const: " << idTable_num;
			//Analyze(father->sonPoint[1]);

			idTable[idTable_num].id = father->sonPoint[0]->sonPoint[0]->text;
			bt << "const:表中第" << father->idnumber << "项const的名称为"
				<< idTable[idTable_num].id << endl;
			idTable[idTable_num].scope = father->scope;
			SCOPE[father->scope] = idTable_num;
			idTable_num++;
		}
		//第二个子节点好像是等号。
		father->sonPoint[2]->scope = father->scope;
		father->sonPoint[3]->scope = father->scope;
		Analyze(father->sonPoint[2]);
		Analyze(father->sonPoint[3]);
		return;
	}

	else if (father->text.compare("const_variable") == 0) {
		father->sonPoint[0]->idnumber = father->idnumber;
		if (father->sonPoint[0]->text.compare("letter") == 0) {
			bt << endl << "const_variable → 'letter'" << endl;
			idTable[father->idnumber].const_type = 6;
			//idTable
			bt << "const:表中第" << father->idnumber << "项const的值为letter" << endl;
		}

		else {
			bt << endl << "const_variable → + const_variable_1 | - const_variable_1 | const_variable_1 " << endl;
			father->sonPoint[1]->idnumber = father->idnumber;
			father->sonPoint[1]->scope = father->scope;
			Analyze(father->sonPoint[1]);
			return;
		}
		return;
	}

	else if (father->text.compare("const_variable_1") == 0) {
		bt << endl << "const_variable_1 → id | num" << endl;
		if (father->sonPoint[0]->text.compare("id") == 0) {
			string id = father->sonPoint[0]->sonPoint[0]->text;
			//判断id是否已定义，未定义则出错。
			int id_wz = checkID(id, father->scope);
			if (id_wz == -1) {
				bt << "const引用id不存在！" << endl;
			}
			else {
				idTable[father->idnumber].const_id_type = idTable[id_wz].type;
				bt << "const:表中第" << father->idnumber << "项const引用的值为"<<id << endl;
			}
		}
		else if (father->sonPoint[0]->text.compare("num") == 0) {
			idTable[father->idnumber].const_id_type = 5;
			bt << "const:表中第" << father->idnumber << "项const的值为num" << endl;
		}
		return;
	}

	/*
	//7.16合并至const_declaration中
	else if (father->text.compare("const_declaration_1") == 0) {
		bt << endl << "const_declaration_1 → ; id = const_variable ; const_declaration_1 " << endl;
		if (father->sonPoint[0]->text.compare("empty") == 0)return;
		else {
			father->sonPoint[0]->in = 6;//类型为常量。
			father->sonPoint[0]->idnumber = idTable_num;//标记其在符号表中的位置
			father->sonPoint[1]->idnumber = idTable_num;
			father->sonPoint[0]->scope = father->scope;
			father->sonPoint[1]->scope = father->scope;
			father->idnumber = idTable_num;

			//进行符号表搭建
			idTable[idTable_num].type = 6;
			idTable[idTable_num].idnumber = idTable_num;
			idTable[idTable_num].id = father->sonPoint[0]->sonPoint[0]->text;

			bt << "新建const: "<<idTable_num ;			
			bt << "const:表中第" << father->idnumber << "项const的名称为"
				<< idTable[idTable_num].id << endl;

			idTable_num++;
			Analyze(father->sonPoint[1]);
			Analyze(father->sonPoint[2]);
		}
	}

	else if (father->text.compare("const_variable_1")==0) {
		if (father->sonPoint[0]->text.compare("num") == 0) {
			idTable[father->idnumber].const_type = 2;
			bt << "const:表中第" << father->idnumber << "项const的值为num" << endl;
		}
		else if (father->sonPoint[0]->text.compare("id") == 0) {
			idTable[father->idnumber].const_type = 3;
			bt << "const:表中第" << father->idnumber << "项const的值为id" << endl;
			//for(int )
			//此处关于id的处理还需要进一步细化
		}
	}
	*/
	
	else if (father->text.compare("type_declarations") == 0) {
		if (father->sonPoint[0]->text.compare("empty") == 0) {
			bt << endl << "type_declarations →  empty" << endl;
			return;
		}

		bt << endl << "type_declarations → type type_declaration" << endl;
		father->sonPoint[1]->scope = father->scope;
		Analyze(father->sonPoint[1]);
		return;
	}

	else if (father->text.compare("type_declaration") == 0|| father->text.compare("type_declaration_1") == 0) {
		bt << "type_declaration → id = type ; type_declaration_1" << endl;
		bt << "或者：type_declaration_1 → ; id = type ; type_declaration_1 | empty" << endl;
		if (father->sonPoint[0]->text.compare("empty") == 0)return;
		
		father->sonPoint[2]->idnumber = idTable_num;
		father->idnumber = idTable_num;

		string id= father->sonPoint[0]->sonPoint[0]->text;
		if (checkID(id, father->scope) != -1) {
			bt << "出现重名变量！" << endl;
			return;
		}

		else {
			idTable[idTable_num].id = father->sonPoint[0]->sonPoint[0]->text;
			bt << "新建type: " << idTable_num;
			bt << "type:表中第" << father->idnumber << "项type的名称为"
				<< idTable[idTable_num].id << endl;
			idTable[idTable_num].idnumber = idTable_num;
			SCOPE[father->scope] = idTable_num;
			idTable_num++;

			father->sonPoint[2]->scope = father->scope;
			father->sonPoint[3]->scope = father->scope;
			Analyze(father->sonPoint[2]);
			Analyze(father->sonPoint[3]);
		}
		return;
	}

	
	else if (father->text.compare("type") == 0) {
		bt << endl << "type → standard_type | record record_body end | array [periods] of type" << endl;
		if (father->sonPoint[0]->text.compare("standard_type") == 0) {
			if (father->sonPoint[0]->sonPoint[0]->text.compare("integer")==0) {
				idTable[father->idnumber].type = 3;
			}
			else if (father->sonPoint[0]->sonPoint[0]->text.compare("real")==0) {
				idTable[father->idnumber].type = 2;
			}
			else if (father->sonPoint[0]->sonPoint[0]->text.compare("Boolean")==0) {
				idTable[father->idnumber].type = 4;
			}
			else if (father->sonPoint[0]->sonPoint[0]->text.compare("char")==0) {
				idTable[father->idnumber].type = 1;
			}
			bt << "type:表中第" << father->idnumber << "项的类型为"
				<< idTable[father->idnumber].type << endl;
		}
		else if (father->sonPoint[0]->text.compare("record") == 0) {
			idTable[father->idnumber].type = 9;
			bt << "type:表中第" << father->idnumber << "项的类型为"
				<< idTable[father->idnumber].type << endl;
			father->sonPoint[1]->idnumber = father->idnumber;
			//这个地方的record_body还是没分析！！！！！！！！
			Analyze(father->sonPoint[1]);
		}
		else if (father->sonPoint[0]->text.compare("array") == 0) {
			idTable[father->idnumber].type = 5;
			bt << "type:表中第" << father->idnumber << "项的类型为"
				<< idTable[father->idnumber].type << endl;
			father->sonPoint[1]->idnumber = father->idnumber;
			//分析array中的periods
			Analyze(father->sonPoint[1]);
			//分析array中的type
			if (father->sonPoint[3]->sonPoint[0]->text.compare("standard_type") == 0) {
				if (father->sonPoint[3]->sonPoint[0]->sonPoint[0]->text.compare("integer")) {
					idTable[father->idnumber].array_type = 3;
				}
				else if (father->sonPoint[3]->sonPoint[0]->sonPoint[0]->text.compare("real")) {
					idTable[father->idnumber].array_type = 2;
				}
				else if (father->sonPoint[3]->sonPoint[0]->sonPoint[0]->text.compare("Boolean")) {
					idTable[father->idnumber].array_type = 4;
				}
				else if (father->sonPoint[3]->sonPoint[0]->sonPoint[0]->text.compare("char")) {
					idTable[father->idnumber].array_type = 1;
				}
				bt << "array:表中第" << father->idnumber << "项的数组类型为"
					<< idTable[father->idnumber].array_type << endl;
			}
			else {
			/*如果数组中存放的不是标准数据类型，以现有的符号表结构无法记录..*/
			}
		}	
		return;
	}

	else if (father->text.compare("record_body") == 0) {
		if (father->sonPoint[0]->text.compare("empty") == 0) {
			return;
		}
		else {
			father->sonPoint[0]->idnumber = father->idnumber;
			//Analyze(father->sonPoint[0]);
			//虽然知道应该分析record内容 但是似乎很复杂..
			//这个地方！！没想好怎么处理

		}
		return;
	}

	else if (father->text.compare("periods") == 0) {
		bt << endl << "periods → period periods_1" << endl;
		father->sonPoint[0]->idnumber = father->idnumber;
		father->sonPoint[1]->idnumber = father->idnumber;
		father->sonPoint[1]->idnum = 1;

		Analyze(father->sonPoint[1]);
		return;
	}

	else if (father->text.compare("periods_1") == 0) {
		bt << endl << "periods_1 → , period periods_1 | empty" << endl;
		if (father->sonPoint[0]->text.compare("empty") == 0) {
			idTable[father->idnumber].dimention = father->idnum;
			bt << "array 第" << father->idnumber << "项array中维度为" << father->idnum << endl;
			return;
		}
		else {
			father->sonPoint[1]->idnum = father->idnum + 1;
			father->sonPoint[1]->idnumber = father->idnumber;
			Analyze(father->sonPoint[1]);
		}
		return;
	}

	else if (father->text.compare("var_declarations") == 0) {
		if (father->sonPoint[0]->text.compare("empty") == 0) {
			bt << endl << "var_declarations →  empty" << endl;
			return;
		}

		bt << endl << "var_declarations → var var_declaration" << endl;
		father->sonPoint[1]->scope = father->scope;
		Analyze(father->sonPoint[1]);
		return;
	}

	else if (father->text.compare("var_declaration") == 0 || father->text.compare("var_declaration_1") == 0) {
		bt << endl << "var_declaration → identifier_list : type ; var_declaration_1" << endl;
		if (father->sonPoint[0]->text.compare("empty") == 0)return;

		cout << "var非空" << endl;
		father->sonPoint[0]->scope = father->scope;
		father->sonPoint[1]->scope = father->scope;
		father->sonPoint[1]->idnumber = 99;
		Analyze(father->sonPoint[1]);//把type的类型存放到99号表项中，分析前半部分时直接赋值。

		father->in = idTable[99].type;
		father->sonPoint[0]->in = father->in;
		father->sonPoint[0]->scope = father->scope;

		Analyze(father->sonPoint[0]);//分析identifier_list

		return;
	}

	/*
	else if (father->text.compare("var_declaration") == 0|| father->text.compare("var_declaration_1") == 0) {
		bt << endl << "var_declaration → identifier_list : type ; var_declaration_1" << endl;
		if (father->sonPoint[0]->text.compare("empty") == 0)return;
	
		
		father->sonPoint[0]->scope = father->scope;
		father->sonPoint[1]->scope = father->scope;
		Analyze(father->sonPoint[1]);

		Analyze(father->sonPoint[0]);
		//根据identifier_list中的id个数来统一构建符号表。在表中新增多少多少项。
		int idlist_idnum = father->sonPoint[0]->idnum;

		//令父亲节点变为type
		father = father->sonPoint[1];
		for (int j = 1; j <= idlist_idnum; j++) {		
			if (father->sonPoint[0]->text.compare("standard_type") == 0) {
				if (father->sonPoint[0]->sonPoint[0]->text.compare("integer")) {					
					idTable[idTable_num].type = 3;
					idTable_num++;
				}
				else if (father->sonPoint[0]->sonPoint[0]->text.compare("real")) {
					idTable[idTable_num].type = 2;
					idTable_num++;
				}
				else if (father->sonPoint[0]->sonPoint[0]->text.compare("Boolean")) {
					idTable[idTable_num].type = 4;
					idTable_num++;
				}
				else if (father->sonPoint[0]->sonPoint[0]->text.compare("char")) {
					idTable[idTable_num].type = 1;
					idTable_num++;
				}
				bt << "type:表中第" << idTable_num-1 << "项的类型为"
					<< idTable[idTable_num - 1].type << endl;
			}
			else if (father->sonPoint[0]->text.compare("record") == 0) {
				idTable[idTable_num].type = 9;
				idTable_num++;
				bt << "type:表中第" << idTable_num - 1 << "项的类型为"
					<< idTable[idTable_num - 1].type << endl;
				//father->sonPoint[1]->idnumber = father->idnumber;
				//Analyze(father->sonPoint[1]);
				//需要学习关于pascal—s的record类型的有关知识后再进行补充
			}
			else if (father->sonPoint[0]->text.compare("array") == 0) {
				idTable[idTable_num].type = 5;
				idTable_num++;
				bt << "type:表中第" << idTable_num - 1 << "项的类型为"
					<< idTable[idTable_num - 1].type << endl;
				father->sonPoint[1]->idnumber = idTable_num-1;
				//分析array中的periods
				Analyze(father->sonPoint[1]);
				//分析array中的type
				if (father->sonPoint[3]->sonPoint[0]->text.compare("standard_type") == 0) {
					if (father->sonPoint[3]->sonPoint[0]->sonPoint[0]->text.compare("integer")) {
						idTable[idTable_num-1].array_type = 3;
					}
					else if (father->sonPoint[3]->sonPoint[0]->sonPoint[0]->text.compare("real")) {
						idTable[idTable_num - 1].array_type = 2;
					}
					else if (father->sonPoint[3]->sonPoint[0]->sonPoint[0]->text.compare("Boolean")) {
						idTable[idTable_num - 1].array_type = 4;
					}
					else if (father->sonPoint[3]->sonPoint[0]->sonPoint[0]->text.compare("char")) {
						idTable[idTable_num - 1].array_type = 1;
					}
					bt << "array:表中第" << idTable_num - 1 << "项的数组类型为"
						<< idTable[idTable_num - 1].array_type << endl;
				}
				else {
					//*如果数组中存放的不是标准数据类型，以现有的符号表结构无法记录..
				}
			}
		}

		//令父亲节点变回var_declaration
		father = father->fatherPoint;
		TreePoint *a = father->sonPoint[0];
		for (int j = 1; j <= idlist_idnum; j++) {
			idTable[idTable_num - j].id = a->sonPoint[0]->sonPoint[0]->text;
			bt << "var:第" << idTable_num - j << "项的名称为" << idTable[idTable_num - j].id << endl;
			a = a->sonPoint[1];
		}

	}

	else if (father->text.compare("var_declaration_1") == 0) {
		if (father->sonnumber == 1)return;//为empty
		else {
			Analyze(father->sonPoint[0]);
			//根据identifier_list中的id个数来统一构建符号表。在表中新增多少多少项。
			int idlist_idnum = father->sonPoint[0]->idnum;

			//令父亲节点变为type
			father = father->sonPoint[1];
			for (int j = 1; j <= idlist_idnum; j++) {
				if (father->sonPoint[0]->text.compare("standard_type") == 0) {
					if (father->sonPoint[0]->sonPoint[0]->text.compare("integer")) {
						idTable[idTable_num].type = 3;
						idTable_num++;
					}
					else if (father->sonPoint[0]->sonPoint[0]->text.compare("real")) {
						idTable[idTable_num].type = 2;
						idTable_num++;
					}
					else if (father->sonPoint[0]->sonPoint[0]->text.compare("Boolean")) {
						idTable[idTable_num].type = 4;
						idTable_num++;
					}
					else if (father->sonPoint[0]->sonPoint[0]->text.compare("char")) {
						idTable[idTable_num].type = 1;
						idTable_num++;
					}
					bt << "type:表中第" << idTable_num - 1 << "项的类型为"
						<< idTable[idTable_num - 1].type << endl;
				}
				else if (father->sonPoint[0]->text.compare("record") == 0) {
					idTable[idTable_num].type = 9;
					idTable_num++;
					bt << "type:表中第" << idTable_num - 1 << "项的类型为"
						<< idTable[idTable_num - 1].type << endl;
					//father->sonPoint[1]->idnumber = father->idnumber;
					//Analyze(father->sonPoint[1]);
					//需要学习关于pascal—s的record类型的有关知识后再进行补充
				}
				else if (father->sonPoint[0]->text.compare("array") == 0) {
					idTable[idTable_num].type = 5;
					idTable_num++;
					bt << "type:表中第" << idTable_num - 1 << "项的类型为"
						<< idTable[idTable_num - 1].type << endl;
					father->sonPoint[1]->idnumber = idTable_num - 1;
					//分析array中的periods
					Analyze(father->sonPoint[1]);
					//分析array中的type
					if (father->sonPoint[3]->sonPoint[0]->text.compare("standard_type") == 0) {
						if (father->sonPoint[3]->sonPoint[0]->sonPoint[0]->text.compare("integer")) {
							idTable[idTable_num - 1].array_type = 3;
						}
						else if (father->sonPoint[3]->sonPoint[0]->sonPoint[0]->text.compare("real")) {
							idTable[idTable_num - 1].array_type = 2;
						}
						else if (father->sonPoint[3]->sonPoint[0]->sonPoint[0]->text.compare("Boolean")) {
							idTable[idTable_num - 1].array_type = 4;
						}
						else if (father->sonPoint[3]->sonPoint[0]->sonPoint[0]->text.compare("char")) {
							idTable[idTable_num - 1].array_type = 1;
						}
						bt << "array:表中第" << idTable_num - 1 << "项的数组类型为"
							<< idTable[idTable_num - 1].array_type << endl;
					}
					else {
						//如果数组中存放的不是标准数据类型，以现有的符号表结构无法记录..
					}
				}
			}

			//令父亲节点变回var_declaration
			father = father->fatherPoint;
			TreePoint *a = f
ather->sonPoint[0];
			for (int j = 1; j <= idlist_idnum; j++) {
				idTable[idTable_num - j].id = a->sonPoint[0]->sonPoint[0]->text;
				bt << "var:第" << idTable_num - j << "项的名称为" << idTable[idTable_num - j].id << endl;
				a = a->sonPoint[1];
			}

		}
	}
	*/
	
	else if (father->text.compare("subprogram_declarations") == 0) {
		bt << endl << "subprogram_declarations → subprogram_declarations_1" << endl;
		Analyze(father->sonPoint[0]);
		return;
	}

	else if (father->text.compare("subprogram_declarations_1") == 0) {
		bt << endl << "subprogram_declarations_1 → subprogram_declaration ; subprogram_declarations_1 | empty" << endl;
		if (father->sonPoint[0]->text.compare("empty") == 0)return;

		Analyze(father->sonPoint[0]);
		Analyze(father->sonPoint[1]);

		return;
	}

	else if (father->text.compare("subprogram_declaration") == 0) {

	//bt << endl << "subprogram_declarations_1 → subprogram_declaration ; subprogram_declarations_1 | empty" << endl;

		if (father->sonPoint[0]->text.compare("function") == 0) {

			bt << endl << "subprogram_declaration → function id formal_parameter : standard_type ; program_body" << endl;

			if (Main_num == 0)Main_num = idTable_num - 1;//wangle
			scopenum++;//位于新的域中了。
			father->scope = scopenum;


			/*在此处子函数/过程被重新赋值*/
			for (int i = 1; i <= 4; i++) {
				father->sonPoint[i]->idnumber = idTable_num;
				father->sonPoint[i]->scope = father->scope;//这一句使得program——body位于新的scope中了。
			}

			string id= father->sonPoint[1]->sonPoint[0]->text;
			if (checkID(id, 0) != -1) {
				bt << "函数名重命名！" << endl;
				return;
			}

			idTable[idTable_num].type = 7;
			bt << "新建function:" << idTable_num<<endl;
			father->idnumber = idTable_num;
			idTable_num++;

			idTable[idTable_num - 1].id = father->sonPoint[1]->sonPoint[0]->text;
			bt << "function:第" << idTable_num - 1 << "项的名称为" 
				<< idTable[idTable_num - 1].id << endl;

			Analyze(father->sonPoint[2]);

			cout << "return444" << endl;
			//Analyze(father->sonPoint[3]);
			cout << father->sonPoint[3]->sonPoint[0]->text<<endl;
			if (father->sonPoint[3]->sonPoint[0]->text.compare("integer")==0) {
				idTable[father->idnumber].returntype = 3;
				cout << "integer!" << endl;
			}
			else if (father->sonPoint[3]->sonPoint[0]->text.compare("char")==0) {
				idTable[father->idnumber].returntype = 1;
			}
			else if (father->sonPoint[3]->sonPoint[0]->text.compare("real")==0) {
				idTable[father->idnumber].returntype = 2;
			}
			else if (father->sonPoint[3]->sonPoint[0]->text.compare("Boolean")==0) {
				idTable[father->idnumber].returntype = 4;
			}
			cout << "standard_type！" << endl;

			Analyze(father->sonPoint[4]);
			
			cout << "program_body!" << endl;
			return;
		}

		else if (father->sonPoint[0]->text.compare("procedure") == 0) {

			bt << endl << "subprogram_declaration → procedure id formal_parameter; program_body" << endl;

			if (Main_num == 0)Main_num = idTable_num - 1;

			scopenum++;//位于新的域中了。
			father->scope = scopenum;


			/*在此处子函数/过程被重新赋值*/
			for (int i = 1; i <= 3; i++) {
				father->sonPoint[i]->idnumber = idTable_num;
				father->sonPoint[i]->scope = father->scope;
			}

			string id = father->sonPoint[1]->sonPoint[0]->text;
			if (checkID(id, 0) != -1) {
				bt << "过程名重命名！" << endl;
				return;
			}

			idTable[idTable_num].type = 8;
			bt << "新建procedure:" << idTable_num<<endl;
			idTable_num++;

			idTable[idTable_num - 1].id = father->sonPoint[1]->sonPoint[0]->text;
			bt << "procedure:第" << idTable_num - 1 << "项的名称为"
				<< idTable[idTable_num - 1].id << endl;

			Analyze(father->sonPoint[2]);
			Analyze(father->sonPoint[3]);
			return;
		}
		return;
	}
	
	else if (father->text.compare("formal_parameter") == 0) {

		bt << endl << "formal_parameter → ( parameter_lists ) | empty" << endl;
		if (father->sonPoint[0]->text.compare("empty") == 0) {
			return;
		}
		else {
			father->sonPoint[0]->idnumber = father->idnumber;
			father->sonPoint[0]->scope = father->scope;
			Analyze(father->sonPoint[0]);
		}
		cout << "return" << endl;

		return;
	}

	else if (father->text.compare("parameter_lists") == 0) {

		bt << endl << "parameter_lists → parameter_list parameter_lists_1" << endl;

		father->sonPoint[0]->idnum = 1;
		father->sonPoint[1]->idnum = 2;//表示为函数的第几种形参
		father->sonPoint[0]->idnumber = father->idnumber;
		father->sonPoint[1]->idnumber = father->idnumber;
		father->sonPoint[0]->scope = father->scope;
		father->sonPoint[1]->scope = father->scope;

		cout << "analyze" << endl;
		Analyze(father->sonPoint[0]);
		Analyze(father->sonPoint[1]);

		cout << "return" << endl;
		return;
	}

	else if (father->text.compare("parameter_lists_1") == 0) {

		
		if (father->sonPoint[0]->text.compare("empty")==0){
			bt << endl << "parameter_lists_1 → empty" << endl;

			idTable[father->idnumber].dimention = father->idnum-1;
			bt << "function/procedure：第" << father->idnumber << "项，的形参种类为(种)：" << father->idnum - 1 << endl;
			cout << "return" << endl;
			return;
		}
		else {
			bt << endl << "parameter_lists_1 → ; parameter_list parameter_lists_1 " << endl;


			father->sonPoint[0]->idnum = father->idnum;
			father->sonPoint[1]->idnum = father->idnum + 1;
			father->sonPoint[0]->idnumber = father->idnumber;
			father->sonPoint[1]->idnumber = father->idnumber;
			father->sonPoint[0]->scope = father->scope;
			father->sonPoint[1]->scope = father->scope;
			Analyze(father->sonPoint[0]);
			Analyze(father->sonPoint[1]);
		}
		return;
	}

	else if (father->text.compare("parameter_list") == 0) {
		
		cout << "parameter_list" << endl;
		

		if (father->sonPoint[0]->text.compare("var_parameter") == 0) {
			bt << endl << "parameter_list → var_parameter " << endl;
			father->sonPoint[0]->idnumber = father->idnumber;
			father->sonPoint[0]->sonPoint[1]->idnumber = father->idnumber;
			father->sonPoint[0]->sonPoint[1]->idnum = father->idnum;
			father->sonPoint[0]->sonPoint[1]->scope = father->scope;
			bt << endl << "var_parameter -> var value_parameter" << endl;

			Analyze(father->sonPoint[0]->sonPoint[1]);
		}
		else  if (father->sonPoint[0]->text.compare("value_parameter") == 0) {
			bt << endl << "parameter_list →  value_parameter" << endl;
			father->sonPoint[0]->idnumber = father->idnumber;
			father->sonPoint[0]->idnum = father->idnum;
			father->sonPoint[0]->scope = father->scope;
			Analyze(father->sonPoint[0]);

		}
		return;
	}

	else if (father->text.compare("value_parameter") == 0) {

		bt << endl << "value_parameter → identifier_list : standard_type" << endl;

		int type;
		if (father->sonPoint[1]->sonPoint[0]->text.compare("integer") == 0)type=3;
		else if (father->sonPoint[1]->sonPoint[0]->text.compare("real") == 0)type = 2;
		else if (father->sonPoint[1]->sonPoint[0]->text.compare("Boolean") == 0)type = 4;
		else if (father->sonPoint[1]->sonPoint[0]->text.compare("char") == 0)type = 1;

		idTable[father->idnumber].parameter_type[father->idnum] = type;
		bt << "function/procedure:第" << father->idnumber << "项的第" << father->idnum
			<< "种形参类型为" << type;

		father->sonPoint[0]->in = type;
		father->sonPoint[0]->scope = father->scope;
		father->sonPoint[0]->call_p_s = father->idnumber;//用于表示是哪个函数or过程的形参
		father->sonPoint[0]->idnum = father->idnum;

		Analyze(father->sonPoint[0]);//分析 identifier_list
		
		idTable[father->idnumber].parameter_num[father->idnum] = father->sonPoint[0]->idnum;
		bt << "function/procedure:第" << father->idnumber << "项的第" << father->idnum
			<< "种形参个数为" << father->sonPoint[0]->idnum;
		
		return;
	}

	else if (father->text.compare("compound_statement") == 0) {

		bt << endl << "compound_statement → begin statement_list end" << endl;

		father->sonPoint[1]->idnumber = father->idnumber;
		father->sonPoint[1]->scope = father->scope;
		Analyze(father->sonPoint[1]);

		return;
	}

	else if (father->text.compare("statement_list") == 0) {

		bt << endl << "statement_list → statement statement_list_1" << endl;

		father->sonPoint[0]->idnumber = father->idnumber;
		father->sonPoint[1]->idnumber = father->idnumber;
		father->sonPoint[0]->scope = father->scope;
		father->sonPoint[1]->scope = father->scope;
		Analyze(father->sonPoint[0]);
		Analyze(father->sonPoint[1]);

		return;
	}

	else if (father->text.compare("statement_list_1") == 0) {

		bt << endl << "statement_list_1 → ; statement statement_list_1 | empty" << endl;

		if (father->sonnumber == 1)return;

		else {
			father->sonPoint[0]->idnumber = father->idnumber;
			father->sonPoint[1]->idnumber = father->idnumber;
			father->sonPoint[0]->scope = father->scope;
			father->sonPoint[1]->scope = father->scope;
			Analyze(father->sonPoint[0]);
			Analyze(father->sonPoint[1]);
		}

		return;
	}

	else if (father->text.compare("statement") == 0) {
		int finded = 0;

		if (father->sonPoint[0]->text.compare("empty") == 0) {
			bt << endl << "statement -> empty" << endl;
			return;
		}

		else if (father->sonPoint[0]->text.compare("id") == 0) {
			bt << endl << "statement → id statement_1 " << endl;
			int id_wz;
			//call_procedure_statement 找到过程或函数在程序中的位置
			if (father->sonPoint[1]->sonnumber == 1) {			
				for (int j = 0; j < idTable_num; j++) {
					if (idTable[j].type == 7 || idTable[j].type == 8) {
						if (idTable[j].id.compare(father->sonPoint[0]->sonPoint[0]->text) == 0) {
							finded = 1;
							id_wz = j;
							father->sonPoint[1]->idnum = j;
							father->sonPoint[1]->sonPoint[0]->idnum = j;
							//用idnum保存我们找到的f/p的位置
							
						}
					}
				}
				if (finded == 0) {
					bt << "找不到函数或者过程!" << endl;
					return;
				}
			}

			//id_varparts assignop expression 找到id在程序中的位置
			else {
				string id = father->sonPoint[0]->sonPoint[0]->text;
				id_wz = checkID(id, father->scope);
				if (id_wz == -1) {
					bt << "符号表中无此id" << endl;
					return;
				}
				else {
					bt << "符号表中有此id" << endl;
				}
			}
			father->sonPoint[1]->scope = father->scope;
			father->sonPoint[1]->idnumber = id_wz;//这个地方记录的是非函数调用过程时，前面那个id的位置。

			Analyze(father->sonPoint[1]);//进一步分析statement_1

			return;
		}
		
		else if (father->sonPoint[0]->text.compare("if") == 0) {
			bt << endl << "statement -> if expression then statement else_part" << endl;
			father->sonPoint[1]->scope = father->scope;
			father->sonPoint[1]->in = 20;//不能简单的当作布尔型
			Analyze(father->sonPoint[1]);

			cout << "if expression" << endl;

			father->sonPoint[3]->scope = father->scope;
			Analyze(father->sonPoint[3]);

			father->sonPoint[4]->scope = father->scope;
			Analyze(father->sonPoint[4]);
		}
		
		else if (father->sonPoint[0]->text.compare("case") == 0) {
			bt << endl << "statement -> case expression of case_body end" << endl;
			father->sonPoint[1]->scope = father->scope;
			father->sonPoint[1]->in = 20;//boolean;好像没什么用
			Analyze(father->sonPoint[1]);

			father->sonPoint[3]->scope = father->scope;
			Analyze(father->sonPoint[3]);
		}

		else if (father->sonPoint[0]->text.compare("while") == 0) {
			bt << endl << "statement ->  while expression do statement" << endl;
			father->sonPoint[1]->scope = father->scope;
			father->sonPoint[1]->in = 20;//boolean;
			Analyze(father->sonPoint[1]);

			father->sonPoint[3]->scope = father->scope;
			Analyze(father->sonPoint[3]);
		}

		else if (father->sonPoint[0]->text.compare("repeat") == 0) {
			bt << endl << "statement ->  repeat statement_list until expression" << endl;
			father->sonPoint[1]->scope = father->scope;
			//
			Analyze(father->sonPoint[1]);

			father->sonPoint[3]->in = 20;//boolean;
			father->sonPoint[3]->scope = father->scope;
			Analyze(father->sonPoint[3]);
		}

		else if (father->sonPoint[0]->text.compare("for") == 0) {
			bt << endl << "statement ->  for id assignop expression updown expression do statement" << endl;
			
			int id_wz;
			string id;
			id = father->sonPoint[1]->sonPoint[0]->text;
			id_wz = checkID(id, father->scope);
			if (id_wz == -1) {
				bt << "该变量不存在！" << id << endl;
				return;
			}
			else {
				bt << "for循环中的变量存在：" << id << endl;
				father->in = idTable[id_wz].type;//需要把类型记录下来。
			}
			
			father->sonPoint[3]->scope = father->scope;
			father->sonPoint[3]->in = father->in;//id的类型
			Analyze(father->sonPoint[3]);

			father->sonPoint[5]->scope = father->scope;
			father->sonPoint[5]->in = father->in;//id的类型
			Analyze(father->sonPoint[5]);

			father->sonPoint[7]->scope = father->scope;
			Analyze(father->sonPoint[7]);
		}
		
		return;
	}

	else if (father->text.compare("else_part") == 0) {
		bt << endl << "else_part → else statement | empty" << endl;
		if (father->sonnumber == 1)return;

		father->sonPoint[1]->scope = father->scope;
		Analyze(father->sonPoint[1]);

		return;
	}

	else if (father->text.compare("case_body") == 0) {
		bt << endl << "case_body → branch_list | empty" << endl;
		if (father->sonPoint[0]->text.compare("empty")==0)return;

		father->sonPoint[0]->scope = father->scope;
		Analyze(father->sonPoint[0]);

		return;
	}

	else if (father->text.compare("branch_list") == 0|| father->text.compare("branch_list_1") == 0
	|| father->text.compare("branch") == 0|| father->text.compare("const_list") == 0
	|| father->text.compare("branch_list_1") == 0) {
		bt << endl << "branch_list → branch branch_list_1" << endl;
		if (father->sonPoint[0]->text.compare("empty") == 0)return;

		father->sonPoint[0]->scope = father->scope;
		Analyze(father->sonPoint[0]);

		father->sonPoint[1]->scope = father->scope;
		Analyze(father->sonPoint[1]);
		return;
	}

	

	else if (father->text.compare("statement_1") == 0) {

		/*call_proceduer_statement_1*/
		if (father->sonnumber == 1) {
			bt << endl << "statement_1 → call_procedure_statement_1" << endl;
			father->sonPoint[0]->call_p_s = 1;
			father->sonPoint[0]->scope = father->scope;//如果可以在函数里调用子函数的话emm，想不明白惹
			father->sonPoint[0]->idnum = father->idnum;//记录是哪个函数/过程。

			Analyze(father->sonPoint[0]);
		}
		else {
			bt << endl << "statement_1 → id_varparts assignop expression " << endl;
			//id_varparts应该是id是数组或者record时需要的内容，以后写！！！
			int l_type = idTable[father->idnum].type;
			//father->sonPoint[2]->idnumber = father->idnumber;
			father->sonPoint[2]->in = idTable[father->idnumber].type;//需要与id的类型一致才可以赋值。
			father->sonPoint[2]->idnum = father->idnum;
			father->sonPoint[2]->scope = father->scope;
			Analyze(father->sonPoint[2]);
		}
		return;
	}

	else if (father->text.compare("call_procedure_statement_1") == 0) {
		if (father->sonPoint[0]->text.compare("empty") == 0) {
			bt << endl << "call_procedure_statement_1 → empty" << endl;

			return;
		}
		else {
			bt << endl << "call_procedure_statement_1 → ( expression_list ) " << endl;

			father->sonPoint[0]->idnum = father->idnum;
			father->sonPoint[0]->call_p_s = 1;
			father->sonPoint[0]->scope = father->scope;
			father->call_p_s = father->idnum;//试试干脆把函数序号放到新定义变量中
			//这样的话空出idnum可以用来存储别的内容，比如当前变量为expression?_list中的第几个变量。

			bt << "调用第" << father->idnum << "项函数/过程,进行形参判断..." << endl;

			Analyze(father->sonPoint[0]);
			//father = father->sonPoint[0];
			/*应该是基于形参进行类型检查的，但是不想写了 嗯*/
			/*明天把类型检测补上！！！！*/
		}	
		return;
	}

	else if (father->text.compare("expression_list") == 0) {
		bt << endl << "expression_list → expression expression_list_1" << endl;
		father->sonPoint[0]->idnum = 1;//expression就是第一个形参
		father->sonPoint[1]->idnum = 2;
		father->sonPoint[0]->scope = father->scope;
		father->sonPoint[1]->scope = father->scope;
		father->sonPoint[0]->in = father->in;
		father->sonPoint[1]->in = father->in;
		father->sonPoint[0]->call_p_s = father->call_p_s;
		father->sonPoint[1]->call_p_s = father->call_p_s;

		Analyze(father->sonPoint[0]);
		Analyze(father->sonPoint[1]);

		return;

	}

	else if (father->text.compare("expression_list_1") == 0) {
		bt << endl << "expression_list_1 → , expression expression_list_1 | empty" << endl;

		if (father->sonnumber == 1) {
			bt << endl << "expression_list_1 → empty" << endl;

			return;
		}
		father->sonPoint[0]->idnum = father->idnum;
		father->sonPoint[1]->idnum = father->idnum + 1;
		father->sonPoint[0]->scope = father->scope;
		father->sonPoint[1]->scope = father->scope;
		father->sonPoint[0]->in = father->in;
		father->sonPoint[1]->in = father->in;
		father->sonPoint[0]->call_p_s = father->call_p_s;
		father->sonPoint[1]->call_p_s = father->call_p_s;

		Analyze(father->sonPoint[0]);
		Analyze(father->sonPoint[1]);

		return;

	}

	else if (father->text.compare("expression") == 0) {

		bt << endl << "expression → simple_expression expression_1" << endl;
		father->sonPoint[0]->idnum = father->idnum;
		father->sonPoint[1]->idnum = father->idnum;//记录为第几个形参（如果是形参的话）
		father->sonPoint[0]->idnumber = father->idnumber;
		father->sonPoint[1]->idnumber = father->idnumber;
		father->sonPoint[0]->scope = father->scope;
		father->sonPoint[1]->scope = father->scope;
		father->sonPoint[0]->in = father->in;
		father->sonPoint[1]->in = father->in;
		father->sonPoint[0]->call_p_s = father->call_p_s;
		father->sonPoint[1]->call_p_s = father->call_p_s;

		Analyze(father->sonPoint[0]);

		father->in = father->sonPoint[0]->in;
		father->sonPoint[1]->in = father->sonPoint[0]->in;


		cout << "进入expression1" << endl;
		//int a;
		//cin >> a;
		Analyze(father->sonPoint[1]);

		cout << "expression_1 return" << endl;

		return;
	}

	else if (father->text.compare("expression_1") == 0) {
		if (father->sonnumber == 1) {
			bt << endl << "expression_1 →  empty" << endl;
			return;

		}
		cout << "进入expression1" << endl;
		
		bt << endl << "expression_1 → relop simple_expression " << endl;//作为函数的形参的话，这一部分肯定是empty
		bt << "!!!!!!!!!!!!" << endl;
		//father->sonPoint[0]->idnum = father->idnum;
		father->sonPoint[1]->idnum = father->idnum;
		//father->sonPoint[0]->idnumber = father->idnumber;
		father->sonPoint[1]->idnumber = father->idnumber;
		father->sonPoint[1]->scope = father->scope;
		father->sonPoint[1]->in = father->in;

		Analyze(father->sonPoint[1]);

		return;
	}

	else if (father->text.compare("simple_expression") == 0) {
	
		if (father->sonnumber == 2) {
			bt << endl << "simple_expression → term simple_expression_1  " << endl;

			father->sonPoint[0]->idnum = father->idnum;
			father->sonPoint[1]->idnum = father->idnum;
			father->sonPoint[0]->idnumber = father->idnumber;
			father->sonPoint[1]->idnumber = father->idnumber;
			father->sonPoint[0]->scope = father->scope;
			father->sonPoint[1]->scope = father->scope;
			father->sonPoint[0]->in = father->in;
			father->sonPoint[1]->in = father->in;
			father->sonPoint[0]->call_p_s = father->call_p_s;
			father->sonPoint[1]->call_p_s = father->call_p_s;

			Analyze(father->sonPoint[0]);

			cout << "return" << endl;

			father->in = father->sonPoint[0]->in;
			father->sonPoint[1]->in = father->sonPoint[0]->in;

			Analyze(father->sonPoint[1]);

			cout << "simple return" << endl;

		}
		if (father->sonnumber == 3) {
			bt << endl << "simple_expression → +/- term simple_expression_1  " << endl;

			father->sonPoint[2]->idnum = father->idnum;
			father->sonPoint[1]->idnum = father->idnum;
			father->sonPoint[2]->idnumber = father->idnumber;
			father->sonPoint[1]->idnumber = father->idnumber;
			father->sonPoint[2]->scope = father->scope;
			father->sonPoint[1]->scope = father->scope;
			father->sonPoint[2]->in = father->in;
			father->sonPoint[1]->in = father->in;
			father->sonPoint[2]->call_p_s = father->call_p_s;
			father->sonPoint[1]->call_p_s = father->call_p_s;

			Analyze(father->sonPoint[1]);
			Analyze(father->sonPoint[2]);
		}
		return;
	}

	else if (father->text.compare("simple_expression_1") == 0) {
		if (father->sonnumber == 1)return;
		else {
			bt << endl << "simple_expression_1 → addop term simple_expression_1   " << endl;

			father->sonPoint[1]->idnum = father->idnum;
			father->sonPoint[2]->idnum = father->idnum;
			father->sonPoint[1]->idnumber = father->idnumber;
			father->sonPoint[2]->idnumber = father->idnumber;
			father->sonPoint[2]->scope = father->scope;
			father->sonPoint[1]->scope = father->scope;
			father->sonPoint[2]->in = father->in;
			father->sonPoint[1]->in = father->in;

			Analyze(father->sonPoint[1]);

			father->idnumber = father->sonPoint[1]->idnumber;
			father->sonPoint[2]->in = father->sonPoint[1]->idnumber;

			Analyze(father->sonPoint[2]);
		}

		return;
	}

	else if (father->text.compare("term") == 0) {
		bt << endl << "term → factor term_1  " << endl;

		cout << "1" << endl;

		father->sonPoint[0]->idnum = father->idnum;
		father->sonPoint[1]->idnum = father->idnum;
		father->sonPoint[0]->idnumber = father->idnumber;
		father->sonPoint[1]->idnumber = father->idnumber;
		father->sonPoint[0]->scope = father->scope;
		father->sonPoint[1]->scope = father->scope;
		father->sonPoint[0]->in = father->in;
		father->sonPoint[1]->in = father->in;
		father->sonPoint[0]->call_p_s = father->call_p_s;
		father->sonPoint[1]->call_p_s = father->call_p_s;

		cout << "2" << endl;

		Analyze(father->sonPoint[0]);

		cout << "return" << endl;

		father->in = father->sonPoint[0]->in;//把第一个子节点的类型带回来了。
		father->sonPoint[1]->in = father->in;

		cout << "term_1" << endl;

		Analyze(father->sonPoint[1]);

		cout << "return" << endl;

		return;
	}

	else if (father->text.compare("term_1") == 0) {
		if (father->sonnumber == 1)return;
		else if (father->sonnumber == 0) {
			bt << endl << "term_1->mulop factor term_1出错！！" << endl;
			error = 1;
			return;
		}

		else {
			bt << endl << "term_1 → mulop factor term_1  " << endl;

			

			cout << father->sonnumber << endl;

			father->sonPoint[1]->idnum = father->idnum;
			father->sonPoint[2]->idnum = father->idnum;
			father->sonPoint[1]->idnumber = father->idnumber;
			father->sonPoint[2]->idnumber = father->idnumber;
			father->sonPoint[2]->scope = father->scope;
			father->sonPoint[1]->scope = father->scope;
			
			//father->idnumber = father->sonPoint[1]->idnumber;//把factor的类型带回来了。

			father->sonPoint[2]->in = father->in;//mulop后面的类型需要与前面的factor保持一致
			father->sonPoint[1]->in = father->in;

			//这个地方数组应该是除外的，but俺不会写
			//！！！！！！！！！！！
			cout << "term_1->factor1" << endl;
			Analyze(father->sonPoint[1]);
			cout << "term_1->factor2" << endl;

			Analyze(father->sonPoint[2]);

			cout << "term_1_end" << endl;
		}

		return;
	}

	else if (father->text.compare("factor") == 0) {

		//bt << endl << "term_1 → mulop factor term_1  " << endl;
		cout << "factor" << endl;
		//如果为函数的形参的话
		if (father->call_p_s != 0) {
			cout << "形参" << endl;

			//是函数的参数，应该是第idnum个参数，是call_p_s的
			int type;
			int f_type;//形参类型
			int f_wz;//函数位置
			int wz;//形参位置
			wz = father->idnum;
			f_wz = father->call_p_s;
			int j = 0;
			for (int i = 1; i <= idTable[f_wz].dimention; i++) {
				j = j + idTable[f_wz].parameter_num[i];
				if (wz <= j) {
					f_type = idTable[f_wz].parameter_type[i];
					//求出应该的类型。
				}
			}
			if (father->sonPoint[0]->text.compare("num") == 0) {
				type = 3;//integer
				bt << endl << "factor → num " << endl;
			}
			if (father->sonPoint[0]->text.compare("letter") == 0) {
				bt << endl << "factor → 'letter' " << endl;
				type = 1;//char
			}
			if (father->sonPoint[0]->text.compare("not") == 0) {
				bt << endl << "factor → not factor" << endl;
				type = 4;//boolean;
				//check factor
				father->sonPoint[1]->in = 4;//后面的必须也是Boolean才行。
				father->sonPoint[1]->scope = father->scope;
				Analyze(father->sonPoint[1]);
			}
			if (father->sonPoint[0]->text.compare("id") == 0) {
				bt << endl << "factor → id factor_1" << endl;
				string id;
				id= father->sonPoint[0]->sonPoint[0]->text;
				int idwz;
				idwz = checkID(id, 0);//应该就是得全局变量吧，如果可以嵌套调用的话就麻烦了呀
				if (idwz == -1) {
					bt << "未定义变量:" << id << endl;
					
				}
				else {
					bt << "已定义变量：" << id << endl;
					type = idTable[idwz].type;
				}
				
			}
			if (type == f_type||(type==3&&f_type==4)) {
				bt << "该变量与形参匹配,类型为：" << f_type << endl;
			}
			else {
				bt << "该变量与形参类型不匹配，变量和形参类型为：" << type << " " << f_type << endl;
			}
			return;
			
		}
		//如果不是函数的形参的话
		else {
			bt << endl << "factor → id factor_1 | num | ' letter ' | ( expression ) | not factor" << endl;
			cout << "非形参" << endl;

			if (father->in == 4) {

				cout << "not factor" << endl;

				if (father->sonPoint[0]->text.compare("num") == 0) {
					bt << "变量类型为数字，可以放到not后面" << endl;
				}

				else if (father->sonPoint[0]->text.compare("letter") == 0) {
					bt << "变量类型为字母，不可以放到not后面！" << endl;
				}

				else if (father->sonPoint[0]->text.compare("not") == 0) {
					father->sonPoint[1]->in = 4;//后面的必须也是Boolean才行。
					father->sonPoint[1]->scope = father->scope;
					Analyze(father->sonPoint[1]);

				}

				else if (father->sonPoint[0]->text.compare("id") == 0) {
					int id_type;
					string id;
					id = father->sonPoint[0]->sonPoint[0]->text;
					if (checkID(id, father->scope) == -1) {
						bt << "此id不存在！" << id << endl;
					}
					else {
						id_type = idTable[checkID(id, father->scope)].type;
						if (id_type == 2 || id_type == 4) {
							bt << "此id类型为数字或布尔，可以放在not后面" << endl;
						}
						else {
							bt << "此id不可以放在not后面！" << endl;
						}
					}
				}
				return;
			}

			int type;
			if (father->sonPoint[0]->text.compare("num") == 0) {
				type = 3;
			}
			else if (father->sonPoint[0]->text.compare("letter") == 0) {
				type = 1;
			}
			else if (father->sonPoint[0]->text.compare("not") == 0) {
				type = 4;

				father->sonPoint[1]->in = 4;
				father->sonPoint[1]->scope = father->scope;
				Analyze(father->sonPoint[1]);

			}
			else if (father->sonPoint[0]->text.compare("id") == 0) {
				cout << "id" << endl;

				string id;
				id = father->sonPoint[0]->sonPoint[0]->text;
				if (checkID(id, father->scope) == -1) {
					bt << "此id不存在！" << id << endl;
				}
				else {
					type = idTable[checkID(id, father->scope)].type;
				}
			}

			if (father->in != 20 && father->in != 0 && father->in != 4
				&&father->in != 7 &&father->in != 8) {//这样写的意思是，需要它是一个确定的类型了
				if (type != father->in) {
					bt << "id类型不符合，需要类型为：" << father->in << " 实际类型为：" << type << endl;
				}

				else {
					bt << "id类型符合。" << type << endl;
				}
			}
			else if (father->in == 7 || father->in == 8) {
				//if(type==)
				//这个地方需要进行函数和过程的判断
			}
			else {
				//这里就不需要
				cout << "get type   " << type << endl;
				father->in = type;//用idnumber把type带回去。
			}

		}
		
		return;
		
	}
	else {
		return;
	}

}

/*
函数名称：printlist
函数功能：打印当前的符号表
版本：1.0
版本作者:lxy
日期：2020/7/17
*/

void printlist() {
	int i,j;
	cout << idTable_num << endl;
	for (i = 0; i < idTable_num; i++) {
		listid << "序号：" << i << endl;
		listid << "名称：" << idTable[i].id << endl;
		listid << "类型：" <<TYPE[idTable[i].type] << endl;
		if (idTable[i].type == 7 || idTable[i].type == 8) {
			listid << "形参种类：" << idTable[i].dimention << endl;
			for (j = 1; j <= idTable[i].dimention; j++) {
				listid << "第" << j << "种形参个数为：" << idTable[i].parameter_num[j] << endl;
				listid << "第" << j << "种形参类型为：" << TYPE[idTable[i].parameter_type[j]] << endl;
			}
		}
		listid << endl;
	}
}
int main() {
	ReadinAndMakeTree();

	cout << "语法树搭建完成，输出为tree.txt..." << endl;

	tree.close();

	Analyze(root);

	if(error==0)
		cout << "类型检查已完成，输出为check.txt..." << endl;
	else cout << "过程中出错.." << endl;

	bt.close();

	printlist();
	cout << "符号表已建立，输出为idlist.txt..." << endl;

	listid.close();

	Ir ir;
	ir.generate();
	system("pause");
}
