#include<cstdio>  
#include<cstring>  
#include<algorithm>  
#include<iostream>  
#include<string>
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

using namespace std;
#pragma warning(disable : 4996)
const int MAX = 1000;
int count1 = 0, count2 = 0;
vector<vector<vector<string>>> G;
list < list<string>> grammar;
map<string, set<string>> FIRST, FOLLOW;
vector<vector<vector<string>>> final_grammar;	//处理后的LL(1)文法
map<pair<string, string>, vector<string>> predictive_table;//预测分析表,结构为		predictive_table[非终结符号，终结符号] = 产生式
vector<vector<string>> token;

void get_grammar();
bool judge_LL1_grammar();
set<string> cal_first(string Vn,int flag);
bool cmp_set(const set<string> s1, const set<string> s2);
void string_to_vector(string & s, vector<string>& v);
void vector_to_string(string & s, vector<string>& v);
bool has_common_factor(vector<string>& gm1, vector<string>& gm2);
set<string> get_left(vector<string> & tmp);
void delete_left_digui();
void get_all_Vn();
void get_left_common_factor();
void get_FIRST();
void get_FOLLOW();
set<string> cal_follow(string Vn);
void print_FIRST();
void print_FOLLOW();
void print_grammar1();
void trim(string &s);

void get_grammar()
{
	FILE * f;
	f = fopen("grammar_file.txt", "r");
	std::ifstream infile("grammar.txt", ios::in);
	char *buffer = (char *)malloc(MAX * sizeof(char));
	string line;
	G.clear();
	while (infile.getline(buffer,1000))
	{
		//cout << buffer << endl;
		int j = 0;
		list<string> temp;
		vector<string> s;
		vector<vector<string>> s1;
		int len = strlen(buffer);
		string str;
		char *left, *end;
		left = strtok(buffer, "→");

		char order[10][100];
		int order_index = 0;

		while (left != NULL)
		{
			//cout << left << " ----- " << endl;
			strcpy(order[order_index++], left);
			left = strtok(NULL, "→|");
		}

		for (int i = 0; i < order_index; i++)
		{
			end = strtok(order[i], " ");
			while (end != NULL)
			{
				string st(end);
				trim(st);
				s.push_back(st);
				//cout << st << "\t";
				end = strtok(NULL, " ");
			}
			s1.push_back(s);
			s.clear();
		}
		//cout << endl;
		G.push_back(s1);
		s1.clear();
		
	}
	infile.close();
}

//标记所有非终结符与终结符
bool get_all_Vn(string Vn)
{
	for (int i = 0; i < (int)G.size(); i++)
	{
		if (G[i][0][0] == Vn)
		{
			//cout << "V[0]->" << G[i][0][0] <<"__________"<< endl;
			return true;
		}
	}
	return false;
}
//判断是不是LL(1)文法
bool judge_LL1_grammar()
{
	bool res = true;
	for (int j = 0; j < (int)G.size(); j++)
	{
		string Vn = G[j][0][0];
		if (FIRST[Vn].find("empty") != FIRST[Vn].end())
		{
			//first(Vn)里有empty，需要比较follow(Vn)与first(A)，A是first集里没有empty的产生式
			for (int k = 1; k < (int)G[j].size(); k++)
			{
				set<string> s;
				s.insert(FOLLOW[Vn].begin(), FOLLOW[Vn].end());
				s.insert(FIRST[G[j][k][0]].begin(), FIRST[G[j][k][0]].end());
				if (s.size() < FOLLOW[Vn].size() + FIRST[G[j][k][0]].size())
				{
					cout << "no LL(1)!结束位置1为" << Vn << "\t" << G[j][k][0] << endl;
					return false;
				}
			}
		}
		else
		{
			for (int k = 1; k < (int)G[j].size(); k++)
			{
				for (int h = k + 1; h < (int)G[j].size(); h++)
				{
					set<string> s;
					s.insert(FIRST[G[j][h][0]].begin(), FIRST[G[j][h][0]].end());
					s.insert(FIRST[G[j][k][0]].begin(), FIRST[G[j][k][0]].end());
					if (s.size() < FIRST[G[j][h][0]].size() + FIRST[G[j][k][0]].size())
					{
						cout << "no LL(1)!结束位置2为" << Vn << "\t" << G[j][k][0] << "\t" << G[j][h][0] << endl;
						return false;
					}
				}
			}
		}
		for (int i = 1; i < G[j].size(); i++)
		{
			for (int k = i + 1; k < G[j].size(); k++)
			{
				if (get_all_Vn(G[j][i][0]) && get_all_Vn(G[j][k][0]))//非终结符
				{
					if (FIRST[G[j][i][0]].find("empty") == FIRST[G[j][i][0]].end() && FIRST[G[j][k][0]].find("empty") == FIRST[G[j][k][0]].end())
					{
						set<string> s;
						s.insert(FIRST[G[j][i][0]].begin(), FIRST[G[j][i][0]].end());
						s.insert(FIRST[G[j][k][0]].begin(), FIRST[G[j][k][0]].end());
						if (s.size() < FIRST[G[j][i][0]].size() + FIRST[G[j][k][0]].size())
						{
							cout << "no LL(1)!结束位置3为" << Vn << "\t" << G[j][i][0] << "\t" << G[j][k][0] << endl;
							return false;
						}
					}
				}
				else if(get_all_Vn(G[j][i][0]) && !get_all_Vn(G[j][k][0]))
				{
					if (FIRST[G[j][i][0]].find(G[j][k][0]) != FIRST[G[j][i][0]].end() && FIRST[G[j][i][0]].find("empty") == FIRST[G[j][i][0]].end())
					{
						cout << "no LL(1)!结束位置4为" << Vn << "\t" << G[j][i][0] << "\t" << G[j][k][0] << endl;
						return false;
					}
				}
				else if (!get_all_Vn(G[j][i][0]) && get_all_Vn(G[j][k][0]))
				{
					if (FIRST[G[j][k][0]].find(G[j][i][0]) != FIRST[G[j][k][0]].end() && FIRST[G[j][k][0]].find("empty") == FIRST[G[j][k][0]].end())
					{
						cout << "no LL(1)!结束位置5为" << Vn << "\t" << G[j][i][0] << "\t" << G[j][k][0] << endl;
						return false;
					}
				}
				else
				{
					if (G[j][k][0] == G[j][i][0])
					{
						cout << "no LL(1)!结束位置6为" << Vn << "\t" << G[j][i][0] << "\t" << G[j][k][0] << endl;
						return false;
					}
				}
			}
		}
	}
	cout << "is LL(1)!" << endl;
	return true;
}

//比较两个集合有没有交集
bool cmp_set(const set<string> s1, const set<string> s2)
{
	int l1 = s1.size();
	int l2 = s2.size();
	set<string> s;
	string t1, t2;
	s.insert(s1.begin(), s1.end());
	s.insert(s2.begin(), s2.end());
	int l = s.size();
	if (l1 + l2 != l) {
		//cout << "error" << endl;
		return false;
	}
	return true;
}

void delete_left_digui()//消除左递归
{
	for (auto p = grammar.begin(); p != grammar.end(); p++)//将间接左递归转换成直接左递归
	{
		list<string> &A = *p;
		//auto j = p;
		//for (j++; j != grammar.end(); j++)
		for (auto j = grammar.begin(); j != p; j++)
		{
			list<string> &B = *j;
			string &Vn = B.front();
			auto pA = A.begin(); pA++;
			for (; pA != A.end(); pA++)
			{
				string &production = *pA;
				string item(production);//返回产生式当前项
				string temp;
				for (char c : production)//遍历产生式
				{
					if(c != ' ')
						temp.push_back(c);
					if (c == ' ')//提取出了一个完整的非终结符
					{
						//cout << "\t" << Vn << "Vn\t" << temp << "temp" << endl << endl;
						//if(!temp_s.empty())
							//temp_s.pop_back();
						if (Vn == temp)//该非终结符是前面的开始符合
						{
							//cout << "begin" << endl << endl;
							pA = A.erase(pA);
							auto pB = B.begin(); //pB++;
							for (; pB != B.end(); pB++)//将production替换为B中的产生式
							{
								string newproduction(item);
								newproduction.replace(0, Vn.length(), *pB);
								A.insert(pA, newproduction);//加入替换后的产生式
								//pA = A.begin();
							}
							break;
						}
						break;
					}
				}
				break;
			}
		}
	}
	for (auto p = grammar.begin(); p != grammar.end(); p++)
	{
		list<string> &A = *p;
		list<string>::iterator pA = A.begin();
		string &Vn = A.front();
		string temp_s = Vn;
			//pA++;
		vector<string> v1, v2;
		for (; pA != A.end(); pA++)
		{
			string &production = *pA;
			string first;
			bool flag = true;
			for (char c : production)
			{
				if (c == ' ')
				{
					//temp_s.pop_back();
					//cout << first << "first\t" << Vn << "Vn" << endl;
					if (first == temp_s)
					{
						v1.push_back(production.substr(Vn.length() + 1));
					}
					else {
						v2.push_back(production);
					}
					flag = false;
			break;
				}
				else
				{
					first.push_back(c);
				}
			}
			if (flag)
			{
				v2.push_back(production);
			}
		}
		if (v1.empty())
			continue;
		pA = A.begin(); pA++;
		while (pA != A.end())
			pA = A.erase(pA);
		if (v2[0] == "empty")
		{
			for (string s : v1)
			{
				A.push_back(s + ' ' + Vn);
			}
			A.push_back("empty");
			continue;
		}
		string newpro(Vn + "_1");
		for (string s : v2)
		{
			A.push_back(s + ' ' + newpro);
		}
		list<string> new_grammar;
		new_grammar.push_back(newpro);
		for (string s : v1)
		{
			new_grammar.push_back(s + ' ' + newpro);
		}
		new_grammar.push_back("empty");
		p++;
		p = grammar.insert(p, new_grammar);
	}
	//cout << "delete finish!" << endl;
}


//提取左公因子
void get_left_common_factor()
{
	for (auto &gm : grammar)
	{
		int sign = 2;
	loop:
		auto p = gm.begin();
		p++;
		//两层循环比较该非终结符所有的产生式
		for (; p != gm.end(); p++)
		{
			auto p2 = p;
			for (p2++; p2 != gm.end(); p2++)
			{
				//把产生式内每个标识符存到vector里方便访问
				vector<string> pro1, pro2;
				string_to_vector(*p, pro1);
				string_to_vector(*p2, pro2);
				//判断两个产生式有没有左公因式
				if (!has_common_factor(pro1, pro2))
				{
					//cout << "flag" << endl;
					continue;
				}
				else
				{
					//产生式相同，提取出来
					if (pro1[0] == pro2[0])
					{
						vector<string> common;
						int i = 0;
						while (i < (int)pro1.size() && i < (int)pro2.size() && pro1[i] == pro2[i])
						{
							common.push_back(pro1[i]);
							pro1[i] = "";
							pro2[i] = "";
							i++;
						}
						string common_fact;
						vector_to_string(common_fact, common);
						string npro1, npro2;
						vector_to_string(npro1, pro1);
						vector_to_string(npro2, pro2);
						string nVn(gm.front());
						nVn = nVn + '_' + (char)(sign + 48);
						sign++;
						//将有公因子的产生式删除
						gm.erase(p);
						gm.erase(p2);
						//替换
						gm.push_back(common_fact + ' ' + nVn);
						//新的文法规则
						list<string> tmp;
						tmp.push_back(nVn);
						if (npro1 != "")
						{
							tmp.push_back(npro1);
						}
						else
						{
							tmp.push_back("empty");
						}
						if (npro2 != "")
						{
							tmp.push_back(npro2);
						}
						else
						{
							tmp.push_back("empty");
						}
						grammar.push_back(tmp);
						//get_all_Vn(nVn) = true;
					}
					//说明是该产生式的产生式有公因子,先展开
					else
					{
						//展开p指向的产生式
						if (get_all_Vn(pro1[0]))
						{
							for (auto & g : grammar)
							{
								if (g.front() == pro1[0])
								{
									string npro;
									pro1[0] = "";
									vector_to_string(npro, pro1);
									gm.erase(p);
									auto it = g.begin(); it++;
									for (; it != g.end(); it++)
									{
										if (npro != "")
										{
											gm.push_back(*it + ' ' + npro);
										}
										else
										{
											gm.push_back(*it);
										}
									}
									//cout << "break1" << endl << endl;
									break;
								}
							}
						}
						//展开p2指向的产生式
						if (get_all_Vn(pro2[0]))
						{
							for (auto & g : grammar)
							{
								if (g.front() == pro2[0])
								{
									string npro;
									pro2[0] = "";
									vector_to_string(npro, pro2);
									gm.erase(p2);
									auto it = g.begin(); it++;
									for (; it != g.end(); it++)
									{
										if (npro != "")
										{
											gm.push_back(*it + ' ' + npro);
										}
										else
										{
											gm.push_back(*it);
										}
									}
									//cout << "break2" << endl << endl;
									break;
								}
							}
						}
					}
					goto loop;
				}
			}
		}
	}
	for (auto p = grammar.begin(); p != grammar.end(); p++)
	{
		list<string> &A = *p;
		auto pA = A.begin(); pA++;
		for (; pA != A.end(); pA++)
		{
			string &production = *pA;
			string temp;
			string &Vn = A.front();
			vector<string> v;
			int pos = 0;
			int count = 0;
			int k = 0;
			string s[20] ,t[20];
			auto pB = pA;
			for (char c : production)//遍历产生式
			{
				if (c != ' ')
					temp.push_back(c);
				if (c == ' ')//提取出了一个完整的非终结符
				{
					s[pos] = temp.substr(0, 1);
					t[pos] = temp.substr(1);
					//cout << s[pos] << "\t" << t[pos] << endl << endl;
					for (; k < pos; k++)
					{
						for (int j = k + 1; j < pos; j++)
						{
							if (s[k] == s[j])
							{
								//cout << "flag" << endl << endl;
								pA = A.erase(pA);
								string newproduction;
								list<string> new_grammar;
								string newpro(Vn + "_");
								newproduction.replace(0, Vn.length() + 2, Vn + "_2");
								A.insert(pA, newproduction);//加入替换后的产生式
								new_grammar.push_back(Vn + "_2");
								new_grammar.push_back(t[k]);
								new_grammar.push_back(t[j]);
								new_grammar.push_back("empty");
								p = grammar.insert(p, new_grammar);
							}
						}
					}
					pos++;
					temp.clear();
				}
			}
		}
	}
}

void string_to_vector(string & s, vector<string>& v)
{
	string tmp;
	for (auto &c : s)
	{
		if (c == ' ')
		{
			v.push_back(tmp);
			tmp.clear();
		}
		else
		{
			tmp.push_back(c);
		}
	}
	if (tmp.length()) v.push_back(tmp);
}

void vector_to_string(string & s, vector<string>& v)
{
	for (int i = 0; i < (int)v.size(); i++)
	{
		if (v[i] == "") continue;
		s += v[i];
		if (i != (int)v.size() - 1)
		{
			s.push_back(' ');
		}
	}
}

//获取FIRST集合
void get_FIRST()
{
	FIRST.clear();
	for (auto &gm : G)
	{
		string &Vn = gm[0][0];
		FIRST[Vn] = cal_first(Vn,0);
	}
}

//递归计算当前非终结符的first集合
void trim(string &s)
{
	if (!s.empty())
	{
		s.erase(0, s.find_first_not_of(" "));
		s.erase(s.find_last_not_of(" ") + 1);
	}
}
set<string> cal_first(string Vn, int flag)
{
	set<string> res;

	int count = flag;
	count++;
	for (int i = 0; i < (int)G.size(); i++)//一条产生式
	{
		//cout << "start_1_" << i << endl;
		if (G[i][0][0] == Vn)
		{
			if (FIRST[G[i][0][0]].empty())
			{
				for (int j = 1; j < (int)G[i].size(); j++)//产生式的符号
				{
					//cout << "flaggggggggggggg" << endl;
					for (int k = 0; k < (int)G[i][j].size(); k++)
					{
						if (get_all_Vn(G[i][j][k]))//非终结符
						{
							FIRST[G[i][j][k]] = cal_first(G[i][j][k], count);
							res.insert(FIRST[G[i][j][k]].begin(), FIRST[G[i][j][k]].end());
							if (FIRST[G[i][j][k]].find("empty") == FIRST[G[i][j][k]].end())//没找到
							{
								break;
							}
						}
						else
						{
							res.insert(G[i][j][k]);
							break;
						}

					}
					if (j == G[i].size() - 1)//判断找完
					{
						//cout << G[i].size() << "\tflagggggggggggggg\t" << G[i][j][0] << endl;
						count--;
						return res;

					}
				}
			}
			else
			{
				//cout << " return " << endl;
				for (auto i : FIRST[Vn])
				{
					//cout << i << " -- ";
					res.insert(i);
				}
				//cout << endl;
			}
		}

	}
	count--;
	return res;
}

//获取FOLLOW集合
void get_FOLLOW()
{
	FOLLOW.clear();
	FOLLOW[G[0][0][0]].insert("$");
	for (auto &gm : G)
	{
		string &Vn = gm[0][0];
		FOLLOW[Vn] = cal_follow(Vn);
	}
}
set<string> cal_follow(string Vn)
{	
	int t = G.size();
	set<string> res;
	for (int i = 0; i < (int)G.size(); i++)
	{
		if (Vn != G[i][0][0])
		{
			for (int j = 1; j < G[i].size(); j++)
			{
				for (int k = 0; k < G[i][j].size(); k++)
				{
					if (G[i][j][k] == Vn)
					{
						if (k == G[i][j].size() - 1)//S=>A
						{
							FOLLOW[Vn].insert("$");
							//FOLLOW[G[i][0][0]] = cal_follow(G[i][0][0]);
							FOLLOW[Vn].insert(FOLLOW[G[i][0][0]].begin(), FOLLOW[G[i][0][0]].end());
						}
						else//A->αBβ
						{
							if (!get_all_Vn(G[i][j][k + 1]))//终结符
							{
								FOLLOW[Vn].insert(G[i][j][k + 1]);
							}
							else//非终结符
							{
								if (FIRST[G[i][j][k + 1]].find("empty") == FIRST[G[i][j][k + 1]].end())//没找到
								{
									FOLLOW[Vn].insert(FIRST[G[i][j][k + 1]].begin(), FIRST[G[i][j][k + 1]].end());
								}
								else//包含empty
								{
									FOLLOW[Vn].insert(FIRST[G[i][j][k + 1]].begin(), FIRST[G[i][j][k + 1]].end());
									FOLLOW[Vn].erase("empty");
									FOLLOW[G[i][0][0]] = cal_follow(G[i][0][0]);
									FOLLOW[Vn].insert(FOLLOW[G[i][0][0]].begin(), FOLLOW[G[i][0][0]].end());
								}
							}
						}
					}
				}
			}
		}
	}
	return FOLLOW[Vn];
}

//打印各个文法的FIRST集合
void print_FIRST()
{
	ofstream outfile("FIRST.txt", ios::out | ios::trunc);
	for (const auto &gm : G)
	{
		const auto &Vn = gm[0][0];
		outfile << Vn << " :{ ";
		for (auto &str : FIRST[Vn])
		{
			outfile << str << " | ";
		}
		outfile << "}" << endl;
	}
	outfile.close();
}

//打印各个文法的FOLLOW集合
void print_FOLLOW()
{
	ofstream outfile("FOLLOW.txt", ios::out | ios::trunc);
	for (const auto &gm : G)
	{
		const auto &Vn = gm[0][0];
		outfile << Vn << " :{ ";
		for (auto &str : FOLLOW[Vn])
		{
			outfile << str << " | ";
		}
		outfile << "}" << endl;
	}
	outfile.close();
}
void print()
{
	cout << "————————————————————————————————————————————————" << endl;
	for (int i = 0; i < (int)G.size(); i++)
	{
		string Vn = G[i][0][0];
		for (auto &str : FIRST[Vn])
		{
			if (FOLLOW[Vn].find(str) != FOLLOW[Vn].end())
			{
				cout << Vn << "\t" << str << endl;
			}
		}
	}
	cout << "————————————————————————————————————————————————" << endl;
}
//打印去除左递归后的文法
void print_grammar1()//
{
	const char *grammar_file;
	grammar_file = "new_grammar1.txt";
	ofstream outfile(grammar_file, ios::out | ios::trunc);
	for (auto p = grammar.begin(); p != grammar.end(); p++)
	{
		list<string> &A = *p;
		list<string>::iterator pA = A.begin();
		string &Vn = A.front();
		for (; pA != A.end(); pA++)
		{
			string &production = *pA;
			string first;
			bool flag = true;
			for (char c : production)
			{
				if (c == ' ')
				{
					//temp_s.pop_back();
					//cout << first << "first\t" << Vn << "Vn" << endl;
					if (first == Vn)
					{
						pA = A.erase(pA);
						break;
					}
				}
				else
				{
					first.push_back(c);
				}
			}
		}
	}
	for (const auto &l : grammar)
	{
		bool first = true;
		for (const auto &t : l)
		{
			//cout << t;
			if (first)
			{

				outfile << t << " → ";
				first = false;
			}

			else outfile << t << " | ";
		}
		//cout << endl;
		outfile << endl << endl;
	}
	outfile.close();

}

//打印提取公因子后的文法
void print_grammar2()
{
	ofstream outfile("new_grammar2.txt", ios::out | ios::trunc);
	for (const auto &l : grammar)
	{
		bool first = true;
		for (const auto &t : l)
		{
			if (first)
			{
				outfile << t << " -> ";
				first = false;
			}
			else outfile << t << " | ";
		}
		outfile << endl << endl;
	}
	outfile.close();
}

//打印修改后的LL(1)文法
void print_G()
{
	ofstream outfile("ll(1)grammar.txt", ios::out | ios::trunc);
	for (int i = 0; i < (int)G.size(); i++)
	{
		for (int j = 0; j < int(G[i].size()); j++)
		{
			for (int k = 0; k < (int)G[i][j].size(); k++)
				outfile << G[i][j][k] << " ";
			outfile << "| " << endl;
		}
		outfile << endl << endl;
	}
	outfile.close();
}

bool has_common_factor(vector<string>& gm1, vector<string>& gm2)
{
	set<string> S, s1, s2;
	//cout << "get_left flag" << endl;
	s1 = get_left(gm1);
	s2 = get_left(gm2);
	S.insert(s1.begin(), s1.end());
	S.insert(s2.begin(), s2.end());
	//cout << "check left common factor finish!" << endl << endl;
	return s1.size() + s2.size() != S.size();
}
//返回产生式的first集，查找左公因子用

set<string> get_left(vector<string> & tmp)
{
	set<string> res;
	if (tmp.size() == 0)
	{
		return res;
	}
	int i = 0;
	for (; i < int(tmp.size()); i++)
	{
		if (get_all_Vn(tmp[i]))//产生式左部
		{
			for (auto &gm : grammar)
			{
				auto p = gm.begin();
				if (tmp[i] == *p)
				{
					for (p++; p != gm.end(); p++)
					{
						vector<string> vs;
						string_to_vector(*p, vs);
						auto s = get_left(vs);
						//cout << "break flag" << endl;
						res.insert(s.begin(), s.end());
					}
					break;
				}
			}
		}
		else
		{
			//cout << "else flag" << endl;
			res.insert(tmp[i]);
		}
		if (i == tmp.size() - 1)
		{
			//cout << "empty flag" << endl;
			res.insert("empty");
		}
	}
	//cout << "finish" << endl;
	return res;
}

//文法不再改变，换个数据结构，方便后面使用
void change_ds()
{
	final_grammar.clear();
	vector<vector<string>> tmp;
	for (auto &gm : grammar)
	{
		tmp.clear();
		vector<string> vs;
		for (auto &pro : gm)
		{
			vs.clear();
			string sub;
			for (auto &c : pro)
			{
				if (c == ' ')
				{
					vs.push_back(sub);
					sub.clear();
				}
				else
				{
					sub.push_back(c);
				}
			}
			vs.push_back(sub);
			tmp.push_back(vs);
		}
		final_grammar.push_back(tmp);
	}
	print();
}

//计算预测分析表
void get_predict_table()
{
	for (int i = 0; i < (int)G.size(); i++)
	{
		string Vn = G[i][0][0];
		//cout << "vn->" << Vn << "\t";
		int flag[64] = { 0 };
		int t = 0;
		vector<string> s;
		for (int k = 1; k < (int)G[i].size(); k++)
		{
			for (int h = 0; h < (int)G[i][k].size(); h++)
			{
				if (get_all_Vn(G[i][k][h]))//如果是非终结符A->B
				{
					for (auto &vt : FIRST[G[i][k][h]])
					{
						s.push_back(Vn);
						s.push_back("->");
						for (int w = 0; w < (int)G[i][k].size(); w++)
						{
							s.push_back(G[i][k][w]);
						}
						predictive_table[make_pair(Vn, vt)] = s;
						s.clear();
						s.shrink_to_fit();
					}
					if (FIRST[G[i][k][h]].find("empty") != FIRST[G[i][k][h]].end())//包含empty
					{
						for (auto &vt : FOLLOW[Vn])
						{
							s.push_back(Vn);
							s.push_back("->");
							for (int w = 0; w < (int)G[i][k].size(); w++)
							{
								s.push_back(G[i][k][w]);
							}
							predictive_table[make_pair(Vn, vt)] = s;
							s.clear();
							s.shrink_to_fit();
						}
					}
					else
					{
						break;
					}
				}
				else
				{
					if (G[i][k][h] == "empty")//A->ε
					{
						//cout << "start" << endl;
						for (auto &vt : FOLLOW[Vn])
						{
							s.push_back(Vn);
							s.push_back("->");
							s.push_back("empty");
							//cout << Vn << "empty" << endl;
							predictive_table[make_pair(Vn, vt)] = s;
							s.clear();
							s.shrink_to_fit();
						}
					}
					else//A->αB
					{
						//cout << "!!!!!!!!!start" << endl;
						s.push_back(Vn);
						s.push_back("->");
						for (int w = 0; w < (int)G[i][k].size(); w++)
						{
							s.push_back(G[i][k][w]);
						}
						//cout << Vn << "!!!!!!!!!!!!!!!!111" << endl;
						predictive_table[make_pair(Vn, G[i][k][0])] = s;
						s.clear();
						s.shrink_to_fit();
					}
					break;
				}
			}
			//cout << "Vt->" << G[i][k][0] << endl;
		
		}
	}
	cout << "finish" << endl;
}
//打印预测分析表
void print_predict_table()
{
	ofstream outfile("predictive_table.txt", ios::out | ios::trunc);
	outfile << "Vn" << setw(30) << "Vt" << setw(30) << "process" << endl;
	//cout << "Vn" << setw(30) << "Vt" << setw(30) << "process" << endl;
	for (auto &p : predictive_table)
	{
		outfile << p.first.first << setw(30) << p.first.second << "\t";
		//cout << p.first.first << setw(30) << p.first.second << "\t";
		string pro;
		vector_to_string(pro, p.second);
		outfile << pro << endl;
		//cout << pro << endl << endl;
	}
	outfile.close();
}
//匹配token
void match_token()
{
	//ifstream infile("out1.txt", ios::in);
	ofstream outfile("matching_process.txt", ios::out | ios::trunc);
	ofstream outfile1("result.txt", ios::out | ios::trunc);
	char buffer[256];
	FILE * f;
	f = fopen("out1.txt", "r");
	while (fgets(buffer, 256, f))
	{
		vector<string>temp;
		string str;
		for (int i = 0; i < (int)strlen(buffer); i++)
		{
			if (buffer[i] == '<')	continue;
			else if (buffer[i] == ' ')
			{
				str.pop_back();
				if (str == "note")
					str.clear();
				else
				{
					temp.push_back(str);
					str.clear();
				}
			}
			else if (buffer[i] == '>')
			{
				temp.push_back(str);
				str.clear();
			}
			else str.push_back(buffer[i]);
		}
		token.push_back(temp);
		temp.clear();
	}
	stack<string> s, s1;
	outfile << setw(30) << "Analyse_Stack" << setw(20) << "Input_Stack" << setw(30) << "Action" << endl;
	s.push("$");
	s1.push("$");
	s1.push("program");
	int count = 0;
	for (int i = (int)token.size() - 1; i >= 0; i--)
	{
		//if (token[i][0] == "ID")
		if (token[i][0] == "ID" || token[i][0] == "NUM")
		{
			if (token[i][0] == "ID")
				s.push(token[i][1]);
			else
			{
				string ss = "num";
				ss.append(token[i][1]);
				s.push(ss);
			}
		}
		else s.push(token[i][1]);
	}
	int count1 = 0, count2 = 0;
	do
	{
		string str;
		str = s.top();//输入栈
		string value;
		string temp;
		temp = s1.top();
		if (str == "$" && temp == "$")
		{
			s.pop();
			s1.pop();
			outfile << setw(30) << "$" << setw(20) << "$" << setw(30) <<"$\tmatch" <<endl;
			outfile << setw(30) << "" << setw(20) << "" << setw(30) << "Accept!" << endl;
			break;
		}
		if (!get_all_Vn(temp))
		{
			if (temp == "id")
			{
				for (int i = (int)token.size() - 1; i >= 0; i--)
				{
					if (token[i][1] == str && token[i][0] == "ID")
					{
						string ss = str;
						if (token[i][0] == "ID")
							str = "id";
						if (str == temp)
						{
							cout <<"id" << setw(30) << temp << setw(20) << ss << setw(30) << "match" << endl;
							outfile << setw(30) << temp << setw(20) << ss << setw(30) << str << "match" << endl;
							outfile1 << temp << " -> " << ss << endl;
							s.pop();
							s1.pop();
							temp = s1.top();
							str = s.top();
							break;
						}
					}

				}
			}
			else if (temp == "num")
			{
				cout << "\t\t\t\t\t\t\t\t\t1"<< str.substr(0, 3)<<"2"<< str.substr(3, str.size()) << endl;
				string ss = str.substr(3, str.size());
				if (str.substr(0, 3) == "num")
				{
					cout << "num" << setw(30) << temp << setw(20) << ss << setw(30) << "match" << endl;
					outfile << setw(30) << str.substr(0, 3) << setw(20) << ss << setw(30) << temp << "match" << endl;
					outfile1 << temp << " -> " << ss << endl;
					s.pop();
					s1.pop();
					temp = s1.top();
					str = s.top();
					cout << temp << "\t\t\t\t" << endl;
					
				}
				//break;
			}
			else if (temp == str)
			{
				cout <<"temp == str" << setw(30) << temp << setw(20) << str << setw(30) << "match" << endl;
				outfile << setw(30) << temp << setw(20) << str << setw(30) << str << "match" << endl;

				s.pop();
				s1.pop();
				temp = s1.top();
				str = s.top();
			}
			if (temp == "empty")
			{
				s1.pop();
				temp = s1.top();
			}
		}
		else if (get_all_Vn(temp))
		{
			for (int i = (int)token.size() - 1; i >= 0; i--)
			{
				if (token[i][1] == str && token[i][0] == "ID")
				{
					string ss = str;
					if (token[i][0] == "ID")
						str = "id";
				}

			}
			int flag = 0;
			for (auto &p : predictive_table)//遍历预测分析表
			{
				if (str.substr(0, 3) == "num")
				{
					string ss = str.substr(0, 3);
					if (temp == p.first.first && ss == p.first.second)
					{
						//cout << "start" << endl;
						string pro;
						vector_to_string(pro, p.second);
						cout << setw(30) << p.first.first << setw(20) << p.first.second << setw(30) << pro << "match" << endl;
						outfile << setw(30) << p.first.first << setw(20) << p.first.second << setw(30) << pro << "\tmatch" << endl;
						outfile1 << pro << endl;
						s1.pop();
						//cout << "p.second->" << p.second[1];
						for (int i = p.second.size() - 1; i >= 2; i--)
						{
							//cout << "p.second->" << p.second[i]<<"\t";
							s1.push(p.second[i]);
						}
						flag = 1;
						break;
					}
				}
				if (temp == p.first.first && str == p.first.second)
				{
					string pro;
					vector_to_string(pro, p.second);
					cout << setw(30) << p.first.first << setw(20) << p.first.second << setw(30) << pro << "match" << endl;
					outfile << setw(30) << p.first.first << setw(20) << p.first.second << setw(30) << pro << "\tmatch" << endl;
					outfile1 << pro << endl;
					s1.pop();
					//cout << "p.second->" << p.second[1];
					for (int i = p.second.size() - 1; i >= 2; i--)
					{
						//cout << "p.second->" << p.second[i]<<"\t";
							s1.push(p.second[i]);
					}
					flag = 1;
					break;
				}
			}
			if (!flag)//没找到
			{
				if (temp == "program")
					s.pop();
				else if (FIRST[temp].find("empty") != FIRST[temp].end())//说明可以把temp置换为空
				{//改了
					cout << "first" << setw(30) << "" << setw(20) << "" << setw(30) << temp << " -> empty" << endl;
					outfile << setw(30) << "" << setw(20) << "" << setw(30) << temp << " -> empty" << endl;
					outfile1 << temp << " -> empty" << endl;
					s1.pop();
				}
				else
				{
					cout << " ————————————————————————————————————————————no match!!!!!!!!!!!!" << endl;
					s.pop();
				}
			}
		}
	} while (1);
}
int main()
{
	get_grammar();
	cout << "get_grammar finish!" << endl << endl;
	delete_left_digui();
	cout << "delete_left_digui finish!" << endl << endl;
	
	print_grammar1();
	//get_left_common_factor();
	cout << "print_grammar1 finish!" << endl;
	//get_all_Vn();
	//print();
	cout << "get_all_vn finish!" << endl << endl;
	//get_left_common_factor();
	cout << "get_left_common_factor finish!" << endl << endl;
	//print_grammar2();
	cout << "print_grammar2 finish！" << endl << endl;
	//change_ds();
	//cout << "change_ds finish!" << endl;
	print_G();
	cout << "print_G finish!" << endl;
	get_FIRST();
	cout << "get_first finish!" << endl << endl;
	print_FIRST();
	cout << "print_fisrt finish!" << endl << endl;
	get_FOLLOW();
	cout << "get_follow finish!" << endl << endl;
	print_FOLLOW();
	cout << "print_follow finish!" << endl << endl;
	print();
	judge_LL1_grammar();
	get_predict_table();
	cout << "get_predict_table finish!" << endl << endl;
	print_predict_table();
	cout << "print_predict_table finish!" << endl << endl;
	match_token();
	cout << "match_token finish!" << endl << endl;
	getchar();
	return 0;
}