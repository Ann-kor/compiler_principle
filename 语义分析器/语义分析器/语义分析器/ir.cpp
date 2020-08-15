#include "ir.h"
#include "yuyi.h"
int start_number = 0, end_number = 0, present_num = 0;
bool begin_token = false, if_token = false, case_token = false,
	while_token = false, repeat_token = false, for_token = false;
int quad_tok = 0;//该block的最后一条三地址语句的标号
TreePoint* curtok = root;

bool end_getnexttoken_flag = false;
bool expression_flag = false;
stack<int> tiers; //层
const int END = 1000;						//四元式结束地址
int pGString = 0;							//语法输入串指针
D_quad quad[500] = {};						//四元式数组
int pQuad = 0;								//四元式数组指针
vector<func> func_table;
vector<func> procedureTable;
int func_table_num;
int procedureNum;
int tempNum = 0;

void Ir::backpath(int p, int t) {
	quad[p].res = to_string(t);
	cout << p << ": " << "(" << quad[p].op << "," << quad[p].arg1 << "," << quad[p].arg2 << "," << quad[p].res << ")" << endl;
}											//回填函数
/*int Ir::merge() {}	*/														//并链函数
string Ir::newtemp() {
	string name = "t" + to_string(tempNum);
	tempNum++;
	return name;
}														//回送一个新的临时变量名
int Ir::emit(string op, string arg1, string arg2, string res) {
	quad[quad_tok].arg1 = arg1;
	quad[quad_tok].arg2 = arg2;
	quad[quad_tok].op = op;
	quad[quad_tok].res = res;
	cout << quad_tok << ": "<< "(" << quad[quad_tok].op << "," << quad[quad_tok].arg1 << "," << quad[quad_tok].arg2 << "," << quad[quad_tok].res << ")" << endl;
	quad_tok++;
	return quad_tok - 1;
}	//四元式生成，返回的是最后插入的下标地址
//SIdTable* Ir::lookup(string name) {}
int Ir::getIDValue(string name) { return 0; }
float Ir::getNum(string num) { 
	return 0; }
int Ir::getToken(string str) {
	if (str == "id")
		return token_id;
	if (str == "num") 
		return token_num;
	if (str == "if") 
		return token_if;
	if (str == "while") 
		return token_while;
	if (str == "case") 
		return token_case;
	if (str == "for") 
		return token_for;
	if (str == "repeat") 
		return token_repeat;
	if (str == "begin") 
		return token_begin;
	if (str == "end") 
		return token_end;
	if (str == "procedure")
		return token_procedure;
	if (str == "function")
		return token_function;
	return isRelop(str);
}
void Ir::bt() {}
void Ir::bexp(char *token) {
	bt();
}
int Ir::reserve(string token) {
	for (int i = 1; i <= key_num; i++) {
		if (token == key_list[i])
			return i;
	}
	//char error[]="-1";
	return -1;
}
int Ir::isRelop(string op) {
	if (op == ":=")
		return 1;
	if (op == "+" || op == "-" || op == "or")
		return 2;
	if (op == ">" || op == ">=" || op == "<" || op == "<=" || op == "<>" || op == "=")
		return 3;
	if (op == "[")
		return 4;
	if (op == ";")
		return 5;
	if (op == ".")
		return token_over;
	if (op == "(")
		return 6;
	if (op == "and" || op == "*" || op == "/" || op == "mod" || op == "div") //mulop
		return 7;
	if (op == "not")
		return 8;
	return 0;
}
int Ir::getFunctionLoc(string name){
	for (int i = 0; i < func_table_num; i++) {
		if (func_table[i].name == name)
			return func_table[i].begin;
	}
	return -1;
}

int Ir::getProcedureLoc(string name) {
	for (int i = 0; i < procedureNum; i++) {
		if (procedureTable[i].name == name)
			return procedureTable[i].begin;
	}
	return -1;
}
int Ir::handleAssign() {
	int res,attri_count = 0;
	string p = curtok -> text;//目标
	string temp;
	getNextToken();
	if (curtok->text == ":=") {
		getNextToken();
			string primary;
			primary = curtok->text;
		
			vector<string> s;
			s.push_back(primary);
			while (curtok->text != ";" && reserve(curtok ->text) == -1) {
				getNextToken();
				/*if (curtok->fatherPoint->text == "num" && curtok->fatherPoint->text != "id")
					s.push_back(to_string(getNum(curtok->text)));
				else*/
					s.push_back(curtok->text);

			}//读到了expression的后一个token
			s.pop_back();
			expression_flag = false;
			int res = handleExpression(quad_tok,s); 
			if (quad[res].op == "")//a:= b,覆盖
				quad_tok--;
			return emit(":=", quad[res].res, "", p);
		}
	else if(curtok->text == "("){
		getNextToken();
		vector<string> part;
		while (curtok != NULL && curtok ->text != ")" && reserve(curtok->text) == -1) {
				part.clear();
				while (curtok != NULL && curtok->text != "," && curtok->text != ")" && reserve(curtok->text) == -1) {//括号
					if(part.size() != 0)
						temp = part.back();
					if (curtok->text == "(" && getToken(temp) != 0) {
						vector<string> part2;
						stack<string> temp_stack;

						temp_stack.push("(");
						while (!temp_stack.empty() && curtok != NULL) {
							if(curtok->text == "(")
								temp_stack.push(curtok->text);
							else if(curtok->text == ")"){
								if (!temp_stack.empty())
									temp_stack.pop();
							}
							part2.push_back(curtok->text);
							getNextToken();
						}
						part2.pop_back();

						int res = handleExpression(quad_tok, part2);
						if (quad[res].op == "")
							quad_tok--;
						part.push_back(quad[res].res);
						getNextToken();//")",i=j
					}
					else if (curtok->text == "(") {//排除调用参数内遇到)
						part.push_back(curtok->text);
						stack<string> temp_stack;

						temp_stack.push("(");
						getNextToken();
						while(!temp_stack.empty()) {
							if (curtok->text == "(") {
								temp_stack.push("(");
							}
							else if (curtok->text == ")") {
								if (!temp_stack.empty())
									temp_stack.pop();
							}
							part.push_back(curtok->text);//")"
							getNextToken();
						}
						getNextToken();//")"
					}
					else {
						part.push_back(curtok->text);
						getNextToken();
					}
				}
				res = handleExpression(quad_tok, part);//不减1,保留
				if (quad[res].op == "")
					quad_tok--;
				emit("exp_list", "", "", quad[res].res);//插入参数
				attri_count++;
				if(curtok->text == ",")
					getNextToken();
			}
		return emit("procedure", to_string(attri_count), to_string(getProcedureLoc(p)), p);//返回函数名，先是参数才是函数调用
	}
	else {
		cout << "error" << endl;
		return -1;
	}
}
//int Ir::getTokPrecedence(string s) {
//	int flag = isRelop(s);
//	if (flag != 0) {
//		if (BinopPrecedence.count(s) == 1)
//			return  BinopPrecedence[s];
//		else
//			return 0;
//	}
//}
//rhs Ir::expressionRHS(int ExprPrec, string LHS,vector<string> s) {
//	int i = 0;
//	while (1) {
//		int tokPrec = getTokPrecedence(s[i]);
//		if (tokPrec < ExprPrec)
//			return LHS;
//		string binop = s[i];
//		i++;
//		string RHS ;
//		if (s[i] == "(") {
//			vector<string> part;
//			stack<string> temp;
//			i++;
//			temp.push("(");
//			int j;
//			for (j = i; !temp.empty() && j < length; j++) {
//				if (s[j] == "(") {
//					temp.push(s[j]);
//				}
//				else if (s[j] == ")") {
//					if (!temp.empty())
//						temp.pop();
//				}
//				part.push_back(s[j]);
//			}
//			part.pop_back();// ")"
//		
//			int res = handleExpression(quad_tok, part);
//			s[i] = quad[res].res; //新建的变量
//			i++;
//			for (;j < length; i++,j++)
//				s[i] = s[j];
//		}
//		else if (s[i] == "not"){
//			string L = newtemp();
//			
//		}
//	}
//}
int Ir::handleFactor(vector<string> s) {
	int length = s.size();
	if (length == 1) {
		return emit("", "", "", s[0]);
	}
	string temp;
	int res;
//	int funloc;
	int i = 0;
	int attri_count = 0;
	vector<string> part;
	if (s[0] != "not" and s[0] != "(")
	{
		//函数调用
		if (s[1] == "(") {
			//参数处理
			i = 2;
			while (i < length) {
				part.clear();
				while (s[i] != "," && i < length - 1) {//括号
					if (s[i] == "(" && getToken(s[i - 1]) != 0) {
						vector<string> part2;
						stack<string> temp_stack;

						temp_stack.push("(");
						int j;
						for (j = i+1; !temp_stack.empty() && j < length; j++) {
							if (s[j] == "(") {
								temp_stack.push(s[j]);
							}
							else if (s[j] == ")") {
								if (!temp_stack.empty())
									temp_stack.pop();
							}
							part2.push_back(s[j]);
						}
						part2.pop_back();

						int res = handleExpression(quad_tok, part2);
						if (quad[res].op == "")
							quad_tok--;
						part.push_back(quad[res].res);
						i = j;
					}
					else if (s[i] == "(") {//排除调用参数内遇到,
						part.push_back(s[i]);
						stack<string> temp_stack;

						temp_stack.push("(");
						int j;
						for (j = i + 1; !temp_stack.empty() && j < length; j++) {
							if (s[j] == "(") {
								temp_stack.push(s[j]);
							}
							else if (s[j] == ")") {
								if (!temp_stack.empty())
									temp_stack.pop();
							}
							part.push_back(s[j]);//")"
						}
						i = j;
					}
					else {
						part.push_back(s[i]);
						i++;
					}
				}
				res = handleExpression(quad_tok,part);//不减1,保留
				if (quad[res].op == "")
					quad_tok--;
				emit("exp_list", "", "", quad[res].res);
				attri_count++;
				i++;//，下一个
			}
			return emit("function", to_string(attri_count), to_string(getFunctionLoc(s[0])), s[0]);//返回函数名，先是参数才是函数调用
		}
		else {
			return emit("letter", "", "", s[1]);//letter
		}	
	}
	else if (s[0] == "(") { //(expression),meiyong
		vector<string> part;
		stack<string> temp_stack;

		temp_stack.push("(");
		int j;
		for (j = 1; !temp_stack.empty() && j < length; j++) {
			if (s[j] == "(") {
				temp_stack.push(s[j]);
			}
			else if (s[j] == ")") {
				if (!temp_stack.empty())
					temp_stack.pop();
			}
			part.push_back(s[j]);
		}
		part.pop_back();// ")"

		return handleExpression(quad_tok, part);
	}
	else {//not
		for (i = 1; i < length;i++)
			part.push_back(s[i]);
		res = handleFactor(part);
		if (quad[res].op == "")
			quad_tok--;
		temp = newtemp();
		return emit("not", quad[res].res, "", temp);
	}
}
int Ir::handleTerm(vector<string> s){
	string primary;
	string temp;
	string mulop;
	int i = 0;
	vector<string> part;
	int LHS, RHS;
	string temp_res;
	int length = s.size();

	while (i < length && getToken(s[i]) != 7 ) {
		if (s[i] == "(" && getToken(s[i - 1]) != 0) {
			vector<string> part2;
			stack<string> temp_stack;

			temp_stack.push("(");
			int j;
			for (j = i+1; !temp_stack.empty() && j < length; j++) {
				if (s[j] == "(") {
					temp_stack.push(s[j]);
				}
				else if (s[j] == ")") {
					if (!temp_stack.empty())
						temp_stack.pop();
				}
				part2.push_back(s[j]);
			}
			part2.pop_back();

			int res = handleExpression(quad_tok, part2);
			if (quad[res].op == "")
				quad_tok--;
			part.push_back(quad[res].res);
			i = j;
		}
		else if (s[i] == "(") {//排除调用参数内遇到mulop
			part.push_back(s[i]);
			stack<string> temp_stack;

			temp_stack.push("(");
			int j;
			for (j = i + 1; !temp_stack.empty() && j < length; j++) {
				if (s[j] == "(") {
					temp_stack.push(s[j]);
				}
				else if (s[j] == ")") {
					if (!temp_stack.empty())
						temp_stack.pop();
				}
				part.push_back(s[j]);//")"
			}
			i = j;
		}
		else {
			part.push_back(s[i]);
			i++;
		}
	}
	LHS = handleFactor(part);//不能-1，否则LHS就被覆盖了
	temp = newtemp();
	if (i >= length)
		return LHS;
	else {
		for (; i < length; i++) {
			mulop = s[i++];
			part.clear();
			while (i < length && getToken(s[i]) != 7) {
				if (s[i] == "(" && getToken(s[i - 1]) != 0) {
					vector<string> part2;
					stack<string> temp_stack;

					temp_stack.push("(");
					int j;
					for (j = i+1; !temp_stack.empty() && j < length; j++) {
						if (s[j] == "(") {
							temp_stack.push(s[j]);
						}
						else if (s[j] == ")") {
							if (!temp_stack.empty())
								temp_stack.pop();
						}
						part2.push_back(s[j]);
					}
					part2.pop_back();

					int res = handleExpression(quad_tok, part2);
					if (quad[res].op == "")
						quad_tok--;
					part.push_back(quad[res].res);
					i = j;
				}
				else if (s[i] == "(") {//排除调用参数内遇到addop
					part.push_back(s[i]);
					stack<string> temp_stack;

					temp_stack.push("(");
					int j;
					for (j = i + 1; !temp_stack.empty() && j < length; j++) {
						if (s[j] == "(") {
							temp_stack.push(s[j]);
						}
						else if (s[j] == ")") {
							if (!temp_stack.empty())
								temp_stack.pop();
						}
						part.push_back(s[j]);//")"
					}
					i = j;
				}
				else {
					part.push_back(s[i]);
					i++;
				}
			}
			RHS = handleFactor(part);
			if (quad[RHS].op == "")
				quad_tok--;
			if (quad[LHS].op == "")
				quad_tok--;
			string temp_res = newtemp();
			LHS = emit(mulop, quad[LHS].res, quad[RHS].res, temp_res);//返回的是插入的下标
		}
		return LHS;
	}
}
int Ir::handleSimpleExpression(int begin, vector<string> s){
	//id
	int length = s.size();
	if (length == 1) {
		return emit("", "", "", s[0]);
	}

	string primary;
	string temp;
	string addop;
	int i = 0;
	vector<string> part;
	int variety = 0;
	int LHS,RHS;
	string temp_res;

	if (s[0] == "-") {
		variety = 1;
		i = 1;
	}
	else if (s[0] == "+") {
		variety = 2;
		i = 1;
	}

	while (i < length && getToken(s[i]) != 2 ) {
		//括号
		if (s[i] == "(" && getToken(s[i - 1]) != 0) {
			vector<string> part2;
			stack<string> temp_stack;

			temp_stack.push("(");
			int j;
			for (j = i+1; !temp_stack.empty() && j < length; j++) {
				if (s[j] == "(") {
					temp_stack.push(s[j]);
				}
				else if (s[j] == ")") {
					if (!temp_stack.empty())
						temp_stack.pop();
				}
				part2.push_back(s[j]);
			}
			part2.pop_back();

			int res = handleExpression(quad_tok, part2);
			if (quad[res].op == "")
				quad_tok--;
			part.push_back(quad[res].res);
			i = j;
		}
		else if (s[i] == "("){//排除调用参数内遇到addop
			part.push_back(s[i]);
			stack<string> temp_stack;

			temp_stack.push("(");
			int j;
			for (j = i + 1; !temp_stack.empty() && j < length; j++) {
				if (s[j] == "(") {
					temp_stack.push(s[j]);
				}
				else if (s[j] == ")") {
					if (!temp_stack.empty())
						temp_stack.pop();
				}
				part.push_back(s[j]);//")"
			}
			i = j;
		}
		else {
			part.push_back(s[i]);
			i++;
		}
	}
	LHS = handleTerm(part);
	/*if (quad[LHS].op == "")
		quad_tok--;*/
	temp = newtemp();
	if (i >= length) {
		if (variety == 0)
			return LHS;
		else if (variety == 1) {
			return emit("uminus", quad[LHS].res, "", temp);
		}
		else
			return emit("pos", quad[LHS].res, "", quad[LHS].res);
	}
	else {
		if (variety == 1)
			emit("uminus", quad[LHS].res, "", temp);
		else
			emit("pos", quad[LHS].res, "", quad[LHS].res);
		for (; i < length; i++) {
			addop = s[i++];
			part.clear();
			while (i < length && getToken(s[i]) != 2) {
				if (s[i] == "(" && getToken(s[i - 1]) != 0) {
					vector<string> part2;
					stack<string> temp_stack;

					temp_stack.push("(");
					int j;
					for (j = i+1; !temp_stack.empty() && j < length ; j++) {
						if (s[j] == "(") {
							temp_stack.push(s[j]);
						}
						else if (s[j] == ")") {
							if (!temp_stack.empty())
								temp_stack.pop();
						}
						part2.push_back(s[j]);
					}
					part2.pop_back();

					int res = handleExpression(quad_tok, part2);
					if (quad[res].op == "")
						quad_tok--;
					part.push_back(quad[res].res);
					i = j;
				}
				else if (s[i] == "(") {//排除调用参数内遇到addop
					part.push_back(s[i]);
					stack<string> temp_stack;

					temp_stack.push("(");
					int j;
					for (j = i + 1; !temp_stack.empty() && j < length; j++) {
						if (s[j] == "(") {
							temp_stack.push(s[j]);
						}
						else if (s[j] == ")") {
							if (!temp_stack.empty())
								temp_stack.pop();
						}
						part.push_back(s[j]);//")"
					}
					i = j;
				}
				else {
					part.push_back(s[i]);
					i++;
				}
			}
			RHS = handleTerm(part);
			if (quad[RHS].op == "")
				quad_tok--;
			if (quad[LHS].op == "")
				quad_tok--;
			string temp_res = newtemp();
			LHS = emit(addop, quad[LHS].res, quad[RHS].res, temp_res);//返回的是插入的下标
		}
		return LHS;
		
	}

}
int Ir::handleExpression(int begin,vector<string> s) {//返回最终结果的res，
	vector<string> part;
	int i = 0;
	int LHS,RHS;
	string relop;
	int length = s.size();
	while (i < length && getToken(s[i]) != 3) {
		if (s[i] == "(" && getToken(s[i-1]) != 0) {
			vector<string> part2;
			stack<string> temp_stack;

			temp_stack.push("(");
			int j;
			for (j = i+1; !temp_stack.empty() && j < length; j++) {
				if (s[j] == "(") {
					temp_stack.push(s[j]);
				}
				else if (s[j] == ")") {
					if (!temp_stack.empty())
						temp_stack.pop();
				}
				part2.push_back(s[j]);
			}
			part2.pop_back();

			int res = handleExpression(quad_tok, part2);
			if (quad[res].op == "")
				quad_tok--;
			part.push_back(quad[res].res);
			i = j;
		}
		else if (s[i] == "(") {//排除调用参数内遇到relop
			part.push_back(s[i]);
			stack<string> temp_stack;

			temp_stack.push("(");
			int j;
			for (j = i + 1; !temp_stack.empty() && j < length; j++) {
				if (s[j] == "(") {
					temp_stack.push(s[j]);
				}
				else if (s[j] == ")") {
					if (!temp_stack.empty())
						temp_stack.pop();
				}
				part.push_back(s[j]);//")"
			}
			i = j;
		}
		else {
			part.push_back(s[i]);
			i++;
		}	
	}
	LHS = handleSimpleExpression(quad_tok, part);
	
	if (i >= length)
		return LHS;
	else
	{
		for (; i < length; i++) {
			relop = s[i++];
			part.clear();
			while (i < length && getToken(s[i]) != 3) {
				if (s[i] == "("  && getToken(s[i - 1]) != 0) {
					vector<string> part2;
					stack<string> temp_stack;

					temp_stack.push("(");
					int j;
					for (j = i+1; !temp_stack.empty() && j < length; j++) {
						if (s[j] == "(") {
							temp_stack.push(s[j]);
						}
						else if (s[j] == ")") {
							if (!temp_stack.empty())
								temp_stack.pop();
						}
						part2.push_back(s[j]);
					}
					part2.pop_back();

					int res = handleExpression(quad_tok, part2);
					if (quad[res].op == "")
						quad_tok--;
					part.push_back(quad[res].res);
					i = j;
				}
				else if (s[i] == "(") {//排除调用参数内遇到addop
					part.push_back(s[i]);
					stack<string> temp_stack;

					temp_stack.push("(");
					int j;
					for (j = i + 1; !temp_stack.empty() && j < length; j++) {
						if (s[j] == "(") {
							temp_stack.push(s[j]);
						}
						else if (s[j] == ")") {
							if (!temp_stack.empty())
								temp_stack.pop();
						}
						part.push_back(s[j]);//")"
					}
					i = j;
				}
				else {
					part.push_back(s[i]);
					i++;
				}
			}
			RHS = handleSimpleExpression(quad_tok, part);
			if (quad[RHS].op == "")
				quad_tok--;
			if (quad[LHS].op == "")
				quad_tok--;
			string temp_res = newtemp();
			LHS = emit(relop, quad[LHS].res, quad[RHS].res, temp_res);//返回的是插入的下标

			/*if (i >= length) {
				return LHS;
			}*/
		}
		return LHS;
	}
		
}
void Ir::setRelopPrecedence() {
	BinopPrecedence["<"] = 10;
	BinopPrecedence["<="] = 10;
	BinopPrecedence[">"] = 10;
	BinopPrecedence[">="] = 10;
	BinopPrecedence["<>"] = 10;
	BinopPrecedence["+"] = 20;
	BinopPrecedence["-"] = 20;
	BinopPrecedence["or"] = 20;
	BinopPrecedence["*"] = 30;
	BinopPrecedence["/"] = 30;
	BinopPrecedence["mod"] = 30;
	BinopPrecedence["div"] = 30;
	BinopPrecedence["and"] = 30;
	BinopPrecedence["not"] = -1;
	//BinopPrecedence["("] = 5;
}
void Ir::searchUp() {
	int i = tiers.top();
	while (curtok->fatherPoint != NULL && i == curtok->fatherPoint->sonnumber && i != 0) { //找到最近的有兄弟的结点
			tiers.pop();
			if (!tiers.empty())
				i = tiers.top();
			else
				i = 0;
			curtok = curtok->fatherPoint;
			if (curtok->text == "expression")
				expression_flag = true;
		}
	if(curtok->fatherPoint == NULL)
		i = 0;
	//i按最小为1，不是0，0是没有
	if (i != 0) {
		tiers.pop();
		i++;
		tiers.push(i);
		curtok = curtok->fatherPoint->sonPoint[i - 1];
	}
}
void Ir::getNextToken() {
	
	searchUp();
		
	while ((curtok -> sonnumber != 0 || curtok -> text == "empty" ) && curtok != NULL && !tiers.empty()){
		
		if (curtok->sonnumber != 0) { //"empty”算孩子
			curtok = curtok->sonPoint[0];
			tiers.push(1);
		}
		else
			searchUp();
		
	}
	if (tiers.empty())
		curtok = NULL;
}
void print(TreePoint* r) {
	if (r != NULL) {
		for (TreePoint* sub : r->sonPoint) {
			if (sub != NULL)
				cout << "父: " << r->text << "->子" << sub->text << endl;
			
		}
		if (r->fatherPoint != NULL)
			cout << "2父: " << r->fatherPoint->text << "->子" << r->text << endl;
		for (TreePoint* sub : r->sonPoint)
			print(sub);
	}
}
int Ir::handleIf(int begin) {
	struct block S;
	S.begin = quad_tok;
	struct yuju B;
	struct yuju S1;
	vector<string> s;
	/*
	expression.right = newlabel();
	expression.left = newlabel();
	*/
	while (curtok->text != "then") {
		getNextToken();
		/*if (curtok->fatherPoint->text == "num" && curtok->fatherPoint->text != "id")
			s.push_back(to_string(getNum(curtok->text)));
		else*/
			s.push_back(curtok->text);
	}
	s.pop_back();
	B.code = handleExpression(S.begin,s); //表达式的结果地址，这时候存的还是临时变量
	B.left = emit("goto", "", "", to_string(quad_tok));//假设返回的是该四元式的标号
	
	if (curtok->text == "then") {
		B.right = quad_tok;
		getNextToken();
		if (curtok->fatherPoint->text == "id")
			handleAssign();
		else {
			int flag = getToken(curtok->text);
			switch (flag) {
				//分情况处理
			//case token_id: handleAssign(); break;
			case token_if:handleIf(quad_tok); break;
			case token_begin: handleBegin(); break;
			case token_case:case_token = true;  handleCase(); break;
			case token_while: while_token = true; handleWhile(); break;
			case token_for:for_token = true; handleFor(); break;
			case token_repeat:repeat_token = true; handleRepeat(); break;
				/*******/
			case token_procedure: getNextToken(); break;
			case token_function: getNextToken(); break;
				/******/
			default:break;
			}
		}
		
		S1.code = emit("goto", "", "", to_string(quad_tok));
	}
	//getNextToken();//不一定有，看怎么分析的
	if (curtok->text == "else") {
		B.left = quad_tok;
//		struct yuju S2;
		getNextToken();
		if (curtok->fatherPoint->text == "id")
			handleAssign();
		else {
			int flag = getToken(curtok->text);
			switch (flag) {
				//分情况处理
			//case token_id:quad_tok = handleAssign(); break;
			case token_if:
				 handleIf(quad_tok); break;
			case token_begin: handleBegin(); break;
			case token_case:case_token = true;  handleCase(); break;
			case token_while: while_token = true; handleWhile(); break;
			case token_for:for_token = true; handleFor(); break;
			case token_repeat:repeat_token = true; handleRepeat(); break;
				/*******/
			case token_procedure: getNextToken(); break;
			case token_function: getNextToken(); break;
				/******/
			default:break;
			}
		}
		//S2.next = quad_tok;
		S1.next = quad_tok;
	}
	else {
		B.left = quad_tok;
		S1.next = quad_tok;
	}
	backpath(B.code, B.right);
	backpath(B.code + 1, B.left);
	backpath(S1.code, S1.next);
	return S1.code;
}
void Ir::handleWhile(){
	struct block S;
	struct yuju B, S1;
	vector<string> s;
	S.begin = quad_tok;
	while (curtok -> text != "do") { // 词法分析记得改";"
		getNextToken();
		/*if (curtok->fatherPoint->text == "num" && curtok->fatherPoint->text != "id")
			s.push_back(to_string(getNum(curtok->text)));
		else*/
			s.push_back(curtok->text);

	}//读到了expression的后一个token
	s.pop_back();
	B.code = handleExpression(S.begin,s);//表达式最后一条成功
	B.left = emit("goto", "", "", "" + quad_tok);//失败
	if (curtok->text == "do") {
		B.right = quad_tok;
		getNextToken();
		if (curtok->fatherPoint->text == "id")
			handleAssign();
		else {
			int flag = getToken(curtok->text);
			switch (flag) {
				//分情况处理
			case token_id: handleAssign(); break;
			case token_if:handleIf(quad_tok);
				//backpath(handleIf(quad_tok), S.begin);
				break;//if的then,也需要回填
			case token_begin: handleBegin(); break;
			case token_case:case_token = true;  handleCase(); break;
			case token_while: while_token = true; handleWhile(); break;
			case token_for:for_token = true; handleFor(); break;
			case token_repeat:repeat_token = true; handleRepeat(); break;
				/*******/
			case token_procedure: getNextToken(); break;
			case token_function: getNextToken(); break;
				/******/
			default:break;
			}
		}
		
		S1.code = emit("goto", "", "", "" + S.begin);
		backpath(B.left, quad_tok);
		backpath(B.code, B.right);
	}
	
}
void Ir::handleCase(){
	struct block S;
	S.begin = quad_tok;
	struct yuju B;
	struct yuju S1;
	vector<string> s;
	vector<int> mulS1;
	string id ;
	//float num;

	while (curtok->text != "of") {
		getNextToken();
		/*if (curtok->fatherPoint->text == "num" && curtok->fatherPoint->text != "id")
			s.push_back(to_string(getNum(curtok->text)));
		else*/
			s.push_back(curtok->text);		
	}
	s.pop_back();
	B.code = handleExpression(S.begin,s);
	id = quad[B.code].res;
	getNextToken();
	while (curtok->text != "end") {
		S1.begin = quad_tok;
		while (curtok->text != ":") {
			if(curtok->text == ",")
				getNextToken();
			/*if (curtok->fatherPoint->text == "num" && curtok->fatherPoint->text != "id")
				num = getNum(curtok->text);
			else */
				//num = getIDValue(curtok->text);
			S1.code = emit("=", id, curtok->text, to_string(quad_tok));
			getNextToken();
		}
		for (int i = S1.begin; i <= S1.code; i++) {
			backpath(i, quad_tok);
		}
		getNextToken();
		if (curtok->fatherPoint->text == "id")
			handleAssign();
		else {
			int flag = getToken(curtok->text);
			switch (flag) {
				//分情况处理
			case token_if:handleIf(quad_tok); break;
			case token_begin: handleBegin(); break;
			case token_case:case_token = true;  handleCase(); break;
			case token_while: while_token = true; handleWhile(); break;
			case token_for:for_token = true; handleFor(); break;
			case token_repeat:repeat_token = true; handleRepeat(); break;
				/*******/
			case token_procedure: getNextToken(); break;
			case token_function: getNextToken(); break;
				/******/
			default:break;
			}
		}
		
		mulS1.push_back(emit("goto", "", "", to_string(quad_tok)));
		//getNextToken();//不一定有
	}
	for (int i = 0; i < mulS1.size(); i++)
		backpath(i, quad_tok);
}
void Ir::handleFor(){
	struct block S;
	S.begin = quad_tok;
	struct yuju B1,B2;
	struct yuju S1;
	vector<string> s;
	vector<int> mulS1;
	getNextToken();
	string id = curtok->text;
	string addop;

	getNextToken();
	while (curtok->text != "to" && curtok ->text != "downto") {
		getNextToken();
		/*if (curtok->fatherPoint->text == "num" && curtok->fatherPoint->text != "id")
			s.push_back(to_string(getNum(curtok->text)));
		else*/
			s.push_back(curtok->text);
	}
	s.pop_back();
	B1.code = handleExpression(S.begin, s);

	if (curtok->text == "to") {
		addop = "+";
	}
	else {
		addop = "-";
	}
	emit(":=", quad[B1.code].res, "", id);
	S1.begin = emit("goto", "", "", to_string(quad_tok));//jump到for的开始
	S1.code = emit(addop, id, "1", id);//+/- 1

	while (curtok->text != "do") {
		getNextToken();
		/*if (curtok->fatherPoint->text == "num" && curtok->fatherPoint->text != "id")
			s.push_back(to_string(getNum(curtok->text)));
		else*/
			s.push_back(curtok->text);
	}
	s.pop_back();
	B2.code = handleExpression(S.begin, s);
	S1.right = emit("<", quad[B1.code].res, quad[B2.code].res, to_string(quad_tok));
	backpath(S1.begin, S1.right);
	S1.next = emit("goto", "", "", to_string(quad_tok));//end
	backpath(S1.right, quad_tok);

	getNextToken();
	if (curtok->fatherPoint->text == "id")
		handleAssign();
	else {
		int flag = getToken(curtok->text);
		switch (flag) {
			//分情况处理
		case token_id: handleAssign(); break;
		case token_if:handleIf(quad_tok); break;//为什么不回填
		case token_begin: handleBegin(); break;
		case token_case:case_token = true;  handleCase(); break;
		case token_while: while_token = true; handleWhile(); break;
		case token_for:for_token = true; handleFor(); break;
		case token_repeat:repeat_token = true; handleRepeat(); break;
			/*******/
		case token_procedure: getNextToken(); break;
		case token_function: getNextToken(); break;
			/******/
		default:break;
		}
	}

	emit("goto", "", "", to_string(S1.code));
	backpath(S1.next, quad_tok);//S1.right + 1
}
void Ir::handleRepeat(){
	struct block S;
	struct yuju B, S1;
	vector<string> s;
	S1.begin = quad_tok;
	S.begin = quad_tok;

	while (curtok->text != "until") {
		getNextToken();

		if (curtok->fatherPoint->text == "id")
			handleAssign();
		else {
			int flag = getToken(curtok->text);
			switch (flag) {
				//分情况处理
			case token_if:handleIf(quad_tok);
				//backpath(handleIf(quad_tok), S.begin);
				break;//if的then,也需要回填？
			case token_begin: handleBegin(); break;
			case token_case:case_token = true;  handleCase(); break;
			case token_while: while_token = true; handleWhile(); break;
			case token_for:for_token = true; handleFor(); break;
			case token_repeat:repeat_token = true; handleRepeat(); break;
				/*******/
			case token_procedure: getNextToken(); break;
			case token_function: getNextToken(); break;
				/******/
			default:break;
			}
		}
		//getNextToken();//";"不一定有
	}

	while (curtok->text != "end" && curtok->text != ";") { //
		getNextToken();
		/*if (curtok->fatherPoint->text == "num" && curtok->fatherPoint->text != "id")
			s.push_back(to_string(getNum(curtok->text)));
		else*/
			s.push_back(curtok->text);

	}//读到了expression的后一个token
	s.pop_back();
	B.code = handleExpression(S.begin, s);//表达式最后一条成功
	backpath(B.code, S1.begin);
}
void Ir::handleBegin() {
	bool begin_token = true, if_token = false, case_token = false,
		while_token = false, repeat_token = false, for_token = false;
	emit("begin", "", "", "");
	getNextToken();
	while (begin_token) {

		if (curtok->fatherPoint->text == "id") {
			handleAssign();
			//getNextToken();
		}
		else {
			int flag = getToken(curtok->text);
			switch (flag) {
				//分情况处理
			case token_if:if_token = true;
				 handleIf(quad_tok); break;
			case token_begin:handleBegin(); break;
			case token_end:
				if (case_token)
					case_token = false; 
				else {
					begin_token = false;
					emit("end", "", "", "");
				}
				break;
			case token_case:case_token = true; handleCase(); break;
			case token_while: while_token = true; handleWhile(); break;
			case token_for:for_token = true; handleFor(); break;
			case token_repeat:repeat_token = true;  handleRepeat(); break;
			default:getNextToken(); break;
				//ir.root.self_loc["begin"] = make_pair(start_number, present_number);  break;
			}
		}
		
	}
}
void Ir::handleProcedure() {
	getNextToken();
	func temp;
	temp.name = curtok->text;
	temp.begin = quad_tok;
	cout << temp.name << ":" << endl;
	getNextToken();
	int flag;
	bool proce_end = false;
	while (!proce_end) {
		flag = getToken(curtok->text);

		switch (flag)
		{
		case token_procedure:handleProcedure(); getNextToken(); break;//要标识
		case token_function:handleFunction(); getNextToken(); break;
		case token_begin:handleBegin(); temp.end = quad_tok - 1; procedureTable.push_back(temp); procedureNum++; getNextToken(); break;
		case 5:
			getNextToken();
			if (curtok->text == ";" && (curtok->fatherPoint->text == "end" || curtok->fatherPoint->text == ";")) {
				proce_end = true;
				emit("proce_end", "", "", temp.name);
			}
			break;
		default:
			getNextToken();
			break;
		}
	}
}
void Ir::handleFunction() {
	getNextToken();
	func temp;
	temp.name = curtok->text;
	temp.begin = quad_tok;
	func_table.push_back(temp); 
	func_table_num++;
	cout << temp.name << ":" << endl;
	getNextToken();
	int flag;
	bool func_end = false;
	while (!func_end) {
		flag = getToken(curtok->text);

		switch (flag)
		{
			case token_procedure:handleProcedure(); getNextToken(); break;//要标识
			case token_function:handleFunction(); getNextToken(); break;
			case token_begin://end;
				handleBegin(); 
				temp.end = quad_tok - 1; 
				func_end = true;
				emit("fun_end", "", "", temp.name); 
				getNextToken(); break;
			case 5://;;
				getNextToken();
				if (curtok->text == ";" && curtok->fatherPoint->text == ";") {
					temp.end = quad_tok - 1;
					func_end = true;
					emit("fun_end", "", "", temp.name);
				}
				break;
			default:
				getNextToken();
				break;
		}
	}
}
void Ir::generate() {
	curtok = root;
	curtok = curtok->sonPoint[0];
	tiers.push(1);
	int i = tiers.top();
	while ((curtok->sonnumber != 0 || curtok->text == "empty") && curtok != NULL && !tiers.empty()) {
		curtok = curtok->sonPoint[0];
		tiers.push(1);
		if (curtok->text == "empty") { //"empty”算孩子
			searchUp();
		}
	}

	int flag;
	while (curtok != NULL) {
		flag = getToken(curtok->text);
			
		switch (flag)
		{
			case token_procedure:handleProcedure(); break;//要标识
			case token_function:handleFunction(); break;
			case token_begin:handleBegin(); break;
			default:
				break;
		}
		getNextToken();
	}
	for (i = 0; i < quad_tok; i++)
		cout <<i<< ": /(" << quad[i].op << "," << quad[i].arg1 << "," << quad[i].arg2 << "," << quad[i].res << ")" << endl;
	/*if (root != NULL) {
		print(root);
	}*/
}
