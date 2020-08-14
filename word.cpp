#include<iostream>
#include<stdio.h>
#include<cstring>
#include<string>
#include<stdlib.h>
#include<fstream>
using namespace std;
#pragma warning(disable : 4244)
int state = 0;
char C = ' ';
int iskey = 0;
char token[20];
int token_l = 0;
int identry = 0;
char *lexemebegin = NULL;
char *pforward = NULL;
char buffer[100] = {'\0'};
const char *key_list[50] = {"","and","array","begin","boolean","case","const","div","do","downto","else",
"end","for","function","if","integer","mod","not","of","or","procedure","program",
"real","record","repeat","then","to","type","until","var","while"};
int key_num = 30;

string id_list[100] ;
int id_num = 0;

ifstream ifile;
ofstream ofile;

void get_char() {
	ifile >> noskipws;
	char cc;
	if (pforward == &buffer[49]) {
		C = *pforward;
		//从文件中读入
		for (int i = 50; i <= 99; i++)buffer[i] = '\0';
		for (int i = 50; i <= 99; i++) {
			if (!ifile.eof()) {
				ifile >> cc;
				//ofile << cc ;
				buffer[i] = cc;
			}
			else break;
		}
		//ofile << endl;
		pforward = &buffer[50];
		return;
	}
	if (pforward == &buffer[99]) {
		C = *pforward;
		for (int i = 0; i <= 49; i++)buffer[i] = '\0';
		for (int i = 0; i <= 49; i++) {
			if (!ifile.eof()) {
				ifile >> cc;
				//ofile << cc ;
				buffer[i] = cc;
			}
			else break;
		}
		//ofile << endl;
		pforward = &buffer[0];
		return;
	}
	if (pforward == NULL) {//最开始是空 不赋值
		//wenjian
		//cout << "NULL" << endl;
		ifile.open("test.txt", ios::in | ios::app);
		if (!ifile.is_open()) {
			cout << "open test.txt file fail!" << endl;
			//getchar();
			exit(1);
		}
		for (int i = 0; i <= 49; i++) {
			if (!ifile.eof()) {
				ifile >> cc;
				//ofile << cc ;
				buffer[i] = cc;
			}
			else break;
		}
		//ofile << endl;
		pforward = &buffer[0];
		return;
	}
	C = *pforward;
	pforward = pforward + 1;
	return;
}

void get_nbc() {
	get_char();
	while (C == ' ') {
		get_char();
	}
	return;
}

void cat() {
	token[token_l] = C;
	token_l++;
}

bool letter() {
	if ((C >= 'a'&&C <= 'z') || (C >= 'A') && (C <= 'Z')) return true;
	return false;
}

bool digit() {
	if (C >= '0'&&C <= '9')return true;
	return false;
}

void retract() {
	if(pforward != buffer)
		pforward--;
}

int reserve() {
	for (int i = 1; i <= key_num; i++) {
		if (strcmp(token, key_list[i]) == 0)
			return i;
	}
	//char error[]="-1";
	return -1;
}

int SToI() {
	return atoi(token);
}

float SToF() {
	return atof(token);
}

void table_insert() {
	if (id_num >= 1) {
		//cout <<"before: "<< id_list[1] << endl;
		for (int i = 1; i <= id_num; i++) {
			if (strcmp( id_list[i].c_str(),token) == 0) {
				//cout << i << ":" << token << "  " << id_list[i] << endl;
				identry = i;
				return;
			}
		}
	}
	id_num++;
	//cout << id_num << "  ";
	id_list[id_num] =token;
	//cout << id_list[1] << endl;
	identry = id_num;
	
}
void error() {
	cout << "出现错误：";
	while (C != ' '||C!='\0'||C!='\n') {
		get_char();
	}
	char *p;
	for (p = lexemebegin; p < pforward; p++) {
		cout << *p;
	}
	lexemebegin = pforward;
	return;
}

int main() {
	ofile.open("out2.txt", ios::out | ios::app);
	if (!ofile.is_open()) {
		cout << "open out.txt file fail!！" << endl;
		
		return 0;
	}

	//buffer[99] = '\0';
	//char null[] = "";
	//cout << &token<<endl;
	//cout << &id_list[1] << endl;
	get_char();//将pforward指针定位在buffer0
	while (1) {
		//cout << "1" << endl;		
		//cout << *pforward << endl;
		if (state == 0) {
			//cout << C << "  ";
			memset(token, '\0',sizeof(token));
			token_l = 0;
			lexemebegin = pforward;
			get_nbc();
			if (letter())state = 1;
			else if (digit())state = 2;
			else if (C == '<')state = 8;
			else if (C == '>')state = 9;
			else if (C == ':')state = 10;
			else if (C == '.')state = 11;
			else if (C == '{')state = 12;
			else if (C == '(')state = 13;
			else if (C == ')') {
				state = 0;
				ofile << "<SYMBOL, )>" << endl;
			}
			else if (C == '=') {
				state = 0;
				ofile << "<relop, EQ>" <<endl;//加空格 
			}
			else if (C == '+') {
				state = 0;
				ofile << "<SYMBOL, +>" << endl;
			}
			else if (C == '-') {
				state = 0;
				ofile << "<SYMBOL, ->" << endl;
			}
			else if (C == '*') {
				state = 0;
				ofile << "<SYMBOL, *>" << endl;
			}
			else if (C == '/') {
				state = 0;
				ofile << "<SYMBOL, />" << endl;
			}
			else if (C == '[') {
				state = 0;
				ofile << "<SYMBOL, [>" << endl;
			}
			else if (C == ']') {
				state = 0;
				ofile << "<SYMBOL, ]>" << endl;
			}
			else if (C == ',') {
				state = 0;
				ofile << "<SYMBOL, ,>" << endl;
			}
			else if (C == ';') {
				state = 0;
				ofile << "<SYMBOL, ;>" << endl;
			}
			else if (C == '\'') {
				state = 0;
				ofile << "<SYMBOL, '>" << endl;
			}
			else if (C == '\n') {
				state = 0;
			}

			else if (C == '\0')break;
			else error();
		}
		if (state == 1) {
			//if (id_num >= 1)cout << "0:" << id_list[1] <<"    "<<token<< endl;
			cat();
			//if (id_num >= 1)cout << "1:" << id_list[1] << "    " << token <<endl;
			get_char();
			if (letter() ==true|| digit()==true)state = 1;
			else {
				retract();
				state = 0;
				iskey = reserve();
				if (iskey != -1) {
					if(key_list[iskey] == "program")
						ofile << "<non-terminal, " << "program>" << endl;
					else
						ofile << "<keyword, " << key_list[iskey] << ">" << endl;
				}
				else {
					//if (id_num >= 1)cout << "bbefore:" << id_list[1] << endl;
					table_insert();					
					//cout << id_list[1] << endl;
					ofile << "<ID, " << id_list[identry] << ">" << endl;
					//if (id_num >= 1)cout << "1:" << id_list[1] << endl;
				}
			}
		}
		if (state == 2) {
			cat();
			get_char();
			if (digit()) {
				state = 2;
			}
			else if (C == '.') state = 3;
			else if (C == 'E')state = 5;
			else {
				retract();
				state = 0;
				ofile << "<NUM, " << SToI() << ">" << endl;
			}
		}
		if (state == 3) {
			cat();
			get_char();
			if (digit())state = 4;
			else {
				error();
				state = 0;
			}
		}
		if (state == 4) {
			cat();
			get_char();
			if (digit()) {
				state = 4;
			}
			else if (state = 'E')state = 5;
			else {
				retract();
				state = 0;
				ofile << "<NUM, " << SToF() << ">" << endl;
			}
		}
		if (state == 5) {
			cat();
			get_char();
			if (digit()) {
				state = 7;
			}
			if (C == '+' || C == '-') {
				state = 6;
			}
			else {
				retract();
				error();
				state = 0;
			}
		}
		if (state == 6) {
			cat();
			get_char();
			if (digit()) state = 7;
			else {
				retract();
				error();
				state = 0;
			}
		}
		if (state == 7) {
			cat();
			get_char();
			if (digit())state = 7;
			else {
				retract();
				state = 0;
				ofile << "<NUM, " << SToF() << ">" << endl;
			}
		}
		if (state == 8) {
			cat();
			get_char();
			if (C == '=') {
				state = 0;
				ofile << "<SYMBOL, <=>" << endl;
			}
			else if (C == '>') {
				state = 0;
				ofile << "<SYMBOL, <>>" << endl;
			}
			else {
				retract();
				state = 0;
				ofile << "<SYMBOL, <>" << endl;
			}
		}
		if (state == 9) {
			cat();
			get_char();
			if (C == '=') {
				state = 0;
				ofile << "<SYMBOL, >=>" << endl;
			}
			else {
				retract();
				state = 0;
				ofile << "<SYMBOL, >>" << endl;
			}
		}
		if (state == 10) {
			cat();
			get_char();
			if (C == '=') {
				cat();
				state = 0;
				ofile << "<SYMBOL, :=>" << endl;
			}
			else {
				retract();
				state = 0;
				ofile << "<SYMBOL, ;>" << endl;
			}
		}
		if (state == 11) {
			get_char();
			if (C == '.') {
				state = 0;
				ofile << "<SYMBOL, ..>" << endl;
			}
			else {				
				retract();
				state = 0;
				ofile << "<SYMBOL, .>" << endl;				
			}
		}
		if (state == 12) {
			get_char();
			if (C != '}') {
				state = 12;
			}
			else {
				state = 0;
				ofile << "<note, ->" << endl;
			}
		}
		if (state == 13) {
			get_char();
			if (C == '*') {
				state = 14;
			}
			else {
				retract();
				state = 0;
				ofile << "<SYMBOL, (>" << endl;
			}
		}
		if (state == 14) {
			get_char();
			if (C == '*') {
				state = 15;
			}
			else {
				state = 14;
			}
		}
		if (state == 15) {
			get_char();
			if (C == ')') {
				state = 0;
				ofile << "<note, ->" << endl;
			}
			else {
				state = 14;
			}
		}

		//ofile << C << endl;
	}
	//cout << "a";
	ifile.close();
	ofile.close();
	getchar();
}
