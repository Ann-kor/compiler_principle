#include<stdio.h>
#include<iostream>
#include<cstring>
#include<string>
#include<fstream>
#include<sstream>
#include "yuyi.h"
#include "ir.h"

using namespace std;

//��

string TYPE[10] = { "default","char","real","integer","Boolean","array","const","function","procedure","record" };

int scopenum = 0;//��������������͹��̵�������0��ʾֻ��main��
int SCOPE[10];//ÿ����ķ�Χ��Ĭ��ȫ�ֱ���ֻ���ڿ�ͷ���塣2020/7/16

int Main_num = 0;//ȫ�ֱ����ĸ��� ��Ҳ��֪����û����

SIdTable idTable[100];//���ű�
int idTable_num = 0;//��������

TreePoint *root = new TreePoint;//���ĸ��ڵ�
int broot = 0;
TreePoint *nowroot;


string word[1001];
int word_num = 0;
ofstream tree("tree.txt");
ofstream listid("idlist.txt");
ofstream bt("analyze.txt");


int endflag = 0;

/*
�������ƣ�BuildTree
�������ܣ�����word�����е����ݺ͸��׽ڵ�,ͨ���ݹ�ķ�ʽ�����﷨�������������õ��������tree.txt
��ǰ�汾��1.0
�汾���ߣ�������
���ڣ�2020/6/1
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
		tree << orderNum<<" :  ����:" << father->text << "->����" << i << ":" << a->text << endl;
		//int b;
		//if (orderNum == 660)>> b;
		//cout << word_num << endl;
		BuildTree(nowroot, orderNum + 1);
	}

	return;
}

/*
�������ƣ�readandmaketree
�������ܣ����ļ��ж�ȡ�ַ���������һ���﷨������
��ǰ�汾��1.0
�汾���ߣ�������
���ڣ�2020/6/1
*/

void ReadinAndMakeTree() {
	ifstream in("result.txt");
	ofstream out("word.txt");
	string line;
	if (!in)cout << "���ļ�ʧ��\n" << endl;
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
�������ƣ�check
�������ܣ�����id���ƺ����ж��Ƿ���������������������id��λ�ã�δ��������-1��
Ӧ���ǲ��ܺ�main�е����������ǿ����أ����趨�����԰ɡ�
�汾1.0
�汾���ߣ�������
���ڣ�2020/7/16
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
�������ƣ�analyze
�������ܣ����﷨��������ÿ���ڵ���б�����ƥ�䡣
�汾��1.0
�汾���ߣ�������
���ڣ�2020/6/2

���°汾��2.0
�汾���ߣ�������
���ڣ�2020/7/16
�������ݣ����ע�ͣ����ƹ�������������ݡ�
*/

int error = 0;

void Analyze(TreePoint *father) {

	if (error == 1)return;

	if (father->text.compare("program") == 0) {
		bt << endl;
		bt << "program->program_1" << endl;
		father->scope = 0;
		father->sonPoint[0]->scope = father->scope;
		Analyze(father->sonPoint[0]);//program->program_1���ض�Ϊ���ĸ���
		return;//������ֱ��return���������½����жϡ�
	}

	else if (father->text.compare("program_1") == 0) {
		if (father->sonnumber == 1) {
			bt << endl;
			bt << "prgram_1->enmpy" << endl;
			return;//program_1->empty
		}

		/*����Ϊprogram->id(identifier_list);program_body.program_1*/
		/*�����ʼ��program�� Ĭ��Ϊ��0��*/
		/*ɶ��˼���������λ��main�������˼��*/
		/*����ط���program������Ҳ��Ϊ���Ŵ�����ű�����*/
		idTable[idTable_num].id = father->sonPoint[0]->sonPoint[0]->text;
		idTable[idTable_num].scope = father->scope;
		bt << "����program��" << idTable_num << endl;
		bt << "program������Ϊ��" << idTable[idTable_num].id << endl;
		father->idnumber = idTable_num;
		idTable_num++;

		bt<<endl << "program_1->id(identifier_list);program_body.program_1,1" << endl;
		father->sonPoint[1]->scope = father->scope;
		Analyze(father->sonPoint[1]);
		//�Ƚ�identifier_list�����꣬����list�еı������浽�ֵܽڵ��idnumֵ��

		/*7.16���Ѵ���id����������ƶ��������������ˣ�������һ��û����
		int main_idlist = father->sonPoint[0]->idnum;
		TreePoint*a = father->sonPoint[1];
		for (int j = 1; j <= main_idlist; j++) {
			idTable[idTable_num].id = a->sonPoint[0]->sonPoint[0]->text;
			bt << "�����β��" << idTable_num << " ����Ϊ:" 
				<< a->sonPoint[0]->sonPoint[0]->text << endl;
			idTable_num++;
		}
		*/
		father->sonPoint[2]->scope = father->scope;//�̳����������ԣ���ʾ��main��
		Analyze(father->sonPoint[2]);

		Analyze(father->sonPoint[3]);//�������ӽڵ�Ϊprogram_1���϶��ǿյġ�
		return;
	}

	else if (father->text.compare("program_body")==0) {

		/*ȫ����ֵ ��Ϊmain�����ݣ�����subprogram�ᱻ���⸳ֵΪ�Ӻ����ڱ��е�λ�á�*/
		for (int i = 0; i <= 4; i++) {
			father->sonPoint[i]->scope = father->scope;
			father->sonPoint[i]->idnumber = father->idnumber;//���������������ɶ��

		}
		bt << endl << "program_body �� const_declarations type_declarations var_declarations subprogram_declarations compound_statement" << endl;

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
			father->sonPoint[i]->in = father->in;//�ýڵ�Ķ���0�̳иýڵ����������
		}

		Analyze(father->sonPoint[1]);//��������1,��Ϊidentifier_list_1
		father->idnum = father->sonPoint[1]->idnum + 1;
		
		Analyze(father->sonPoint[0]);
		return;
	}

	/*7.16*/
	else if (father->text.compare("id") == 0) {
		bt << endl << "id->" << father->sonPoint[0]->text << endl;

		string id = father->sonPoint[0]->text;
		if (checkID(id, father->scope) != -1) {
			bt << "��������������" << endl;
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
		bt << "��id���д���id  :"<<idTable_num-1 << endl;
		bt << "���������Ϊ��" << father->in << endl;
		return;
	}

	else if (father->text.compare("identifier_list_1")==0) {
		if (father->sonPoint[0]->text.compare("empty")==0) {
			bt << endl << "identifier_list_1 ��  empty" << endl;
			father->idnum = 0;//�ýڵ���id������Ϊ0
			//�ڵ�Ϊ�գ�û����������
			return;
		}
		else {
			bt << endl << "identifier_list_1 �� , id identifier_list_1 " << endl;
			Analyze(father->sonPoint[1]);
			father->idnum = father->sonPoint[1]->idnum + 1;//�ýڵ���id���������ڶ���1��id����+1
			
			for (int i = 0; i <= 1; i++) {
				father->sonPoint[i]->scope = father->scope;
				father->sonPoint[i]->in = father->in;//�ýڵ�Ķ���0�̳иýڵ����������
			}
			Analyze(father->sonPoint[0]);
		}
		return;
	}

	else if (father->text.compare("const_declarations") == 0) {
	cout << "const_1" << endl;
		if (father->sonPoint[0]->text.compare("empty") == 0) {
			bt << endl << "const_declarations ��  empty" << endl;
			return;
		}
		
		bt << endl << "const_declarations �� const const_declaration" << endl;
		father->sonPoint[1]->scope = father->scope;
		Analyze(father->sonPoint[1]);
		return;
	}

	else if (father->text.compare("const_declaration") == 0||father->text.compare("const_declaration_1") == 0) {
		bt << endl << "const_declaration �� id = const_variable ; const_declaration_1" << endl;
		bt << "����:const_declaration_1 �� ; id = const_variable ; const_declaration_1" << endl;
		if (father->sonPoint[0]->text.compare("empty") == 0)return;
		
		father->sonPoint[0]->in = 6;//����Ϊ������
		//father->sonPoint[0]->constent = 1;//����Ϊ������

		father->sonPoint[0]->idnumber = idTable_num;//������ڷ��ű��е�λ��,�Ա��ڼ�¼��Ϣ��
		father->sonPoint[2]->idnumber = idTable_num;
		//father->sonPoint[2]->idnumber = idTable_num;
		father->idnumber = idTable_num;

		string id = father->sonPoint[0]->sonPoint[0]->text;
		if (checkID(id, father->scope) != -1) {
			bt << "const�������ظ���" << endl;
			return;
		}
		else {
			//���з��ű�
			idTable[idTable_num].type = 6;
			idTable[idTable_num].idnumber = idTable_num;

			bt << "�½�const: " << idTable_num;
			//Analyze(father->sonPoint[1]);

			idTable[idTable_num].id = father->sonPoint[0]->sonPoint[0]->text;
			bt << "const:���е�" << father->idnumber << "��const������Ϊ"
				<< idTable[idTable_num].id << endl;
			idTable[idTable_num].scope = father->scope;
			SCOPE[father->scope] = idTable_num;
			idTable_num++;
		}
		//�ڶ����ӽڵ�����ǵȺš�
		father->sonPoint[2]->scope = father->scope;
		father->sonPoint[3]->scope = father->scope;
		Analyze(father->sonPoint[2]);
		Analyze(father->sonPoint[3]);
		return;
	}

	else if (father->text.compare("const_variable") == 0) {
		father->sonPoint[0]->idnumber = father->idnumber;
		if (father->sonPoint[0]->text.compare("letter") == 0) {
			bt << endl << "const_variable �� 'letter'" << endl;
			idTable[father->idnumber].const_type = 6;
			//idTable
			bt << "const:���е�" << father->idnumber << "��const��ֵΪletter" << endl;
		}

		else {
			bt << endl << "const_variable �� + const_variable_1 | - const_variable_1 | const_variable_1 " << endl;
			father->sonPoint[1]->idnumber = father->idnumber;
			father->sonPoint[1]->scope = father->scope;
			Analyze(father->sonPoint[1]);
			return;
		}
		return;
	}

	else if (father->text.compare("const_variable_1") == 0) {
		bt << endl << "const_variable_1 �� id | num" << endl;
		if (father->sonPoint[0]->text.compare("id") == 0) {
			string id = father->sonPoint[0]->sonPoint[0]->text;
			//�ж�id�Ƿ��Ѷ��壬δ���������
			int id_wz = checkID(id, father->scope);
			if (id_wz == -1) {
				bt << "const����id�����ڣ�" << endl;
			}
			else {
				idTable[father->idnumber].const_id_type = idTable[id_wz].type;
				bt << "const:���е�" << father->idnumber << "��const���õ�ֵΪ"<<id << endl;
			}
		}
		else if (father->sonPoint[0]->text.compare("num") == 0) {
			idTable[father->idnumber].const_id_type = 5;
			bt << "const:���е�" << father->idnumber << "��const��ֵΪnum" << endl;
		}
		return;
	}

	/*
	//7.16�ϲ���const_declaration��
	else if (father->text.compare("const_declaration_1") == 0) {
		bt << endl << "const_declaration_1 �� ; id = const_variable ; const_declaration_1 " << endl;
		if (father->sonPoint[0]->text.compare("empty") == 0)return;
		else {
			father->sonPoint[0]->in = 6;//����Ϊ������
			father->sonPoint[0]->idnumber = idTable_num;//������ڷ��ű��е�λ��
			father->sonPoint[1]->idnumber = idTable_num;
			father->sonPoint[0]->scope = father->scope;
			father->sonPoint[1]->scope = father->scope;
			father->idnumber = idTable_num;

			//���з��ű�
			idTable[idTable_num].type = 6;
			idTable[idTable_num].idnumber = idTable_num;
			idTable[idTable_num].id = father->sonPoint[0]->sonPoint[0]->text;

			bt << "�½�const: "<<idTable_num ;			
			bt << "const:���е�" << father->idnumber << "��const������Ϊ"
				<< idTable[idTable_num].id << endl;

			idTable_num++;
			Analyze(father->sonPoint[1]);
			Analyze(father->sonPoint[2]);
		}
	}

	else if (father->text.compare("const_variable_1")==0) {
		if (father->sonPoint[0]->text.compare("num") == 0) {
			idTable[father->idnumber].const_type = 2;
			bt << "const:���е�" << father->idnumber << "��const��ֵΪnum" << endl;
		}
		else if (father->sonPoint[0]->text.compare("id") == 0) {
			idTable[father->idnumber].const_type = 3;
			bt << "const:���е�" << father->idnumber << "��const��ֵΪid" << endl;
			//for(int )
			//�˴�����id�Ĵ�����Ҫ��һ��ϸ��
		}
	}
	*/
	
	else if (father->text.compare("type_declarations") == 0) {
		if (father->sonPoint[0]->text.compare("empty") == 0) {
			bt << endl << "type_declarations ��  empty" << endl;
			return;
		}

		bt << endl << "type_declarations �� type type_declaration" << endl;
		father->sonPoint[1]->scope = father->scope;
		Analyze(father->sonPoint[1]);
		return;
	}

	else if (father->text.compare("type_declaration") == 0|| father->text.compare("type_declaration_1") == 0) {
		bt << "type_declaration �� id = type ; type_declaration_1" << endl;
		bt << "���ߣ�type_declaration_1 �� ; id = type ; type_declaration_1 | empty" << endl;
		if (father->sonPoint[0]->text.compare("empty") == 0)return;
		
		father->sonPoint[2]->idnumber = idTable_num;
		father->idnumber = idTable_num;

		string id= father->sonPoint[0]->sonPoint[0]->text;
		if (checkID(id, father->scope) != -1) {
			bt << "��������������" << endl;
			return;
		}

		else {
			idTable[idTable_num].id = father->sonPoint[0]->sonPoint[0]->text;
			bt << "�½�type: " << idTable_num;
			bt << "type:���е�" << father->idnumber << "��type������Ϊ"
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
		bt << endl << "type �� standard_type | record record_body end | array [periods] of type" << endl;
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
			bt << "type:���е�" << father->idnumber << "�������Ϊ"
				<< idTable[father->idnumber].type << endl;
		}
		else if (father->sonPoint[0]->text.compare("record") == 0) {
			idTable[father->idnumber].type = 9;
			bt << "type:���е�" << father->idnumber << "�������Ϊ"
				<< idTable[father->idnumber].type << endl;
			father->sonPoint[1]->idnumber = father->idnumber;
			//����ط���record_body����û��������������������
			Analyze(father->sonPoint[1]);
		}
		else if (father->sonPoint[0]->text.compare("array") == 0) {
			idTable[father->idnumber].type = 5;
			bt << "type:���е�" << father->idnumber << "�������Ϊ"
				<< idTable[father->idnumber].type << endl;
			father->sonPoint[1]->idnumber = father->idnumber;
			//����array�е�periods
			Analyze(father->sonPoint[1]);
			//����array�е�type
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
				bt << "array:���е�" << father->idnumber << "�����������Ϊ"
					<< idTable[father->idnumber].array_type << endl;
			}
			else {
			/*��������д�ŵĲ��Ǳ�׼�������ͣ������еķ��ű�ṹ�޷���¼..*/
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
			//��Ȼ֪��Ӧ�÷���record���� �����ƺ��ܸ���..
			//����ط�����û�����ô����

		}
		return;
	}

	else if (father->text.compare("periods") == 0) {
		bt << endl << "periods �� period periods_1" << endl;
		father->sonPoint[0]->idnumber = father->idnumber;
		father->sonPoint[1]->idnumber = father->idnumber;
		father->sonPoint[1]->idnum = 1;

		Analyze(father->sonPoint[1]);
		return;
	}

	else if (father->text.compare("periods_1") == 0) {
		bt << endl << "periods_1 �� , period periods_1 | empty" << endl;
		if (father->sonPoint[0]->text.compare("empty") == 0) {
			idTable[father->idnumber].dimention = father->idnum;
			bt << "array ��" << father->idnumber << "��array��ά��Ϊ" << father->idnum << endl;
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
			bt << endl << "var_declarations ��  empty" << endl;
			return;
		}

		bt << endl << "var_declarations �� var var_declaration" << endl;
		father->sonPoint[1]->scope = father->scope;
		Analyze(father->sonPoint[1]);
		return;
	}

	else if (father->text.compare("var_declaration") == 0 || father->text.compare("var_declaration_1") == 0) {
		bt << endl << "var_declaration �� identifier_list : type ; var_declaration_1" << endl;
		if (father->sonPoint[0]->text.compare("empty") == 0)return;

		cout << "var�ǿ�" << endl;
		father->sonPoint[0]->scope = father->scope;
		father->sonPoint[1]->scope = father->scope;
		father->sonPoint[1]->idnumber = 99;
		Analyze(father->sonPoint[1]);//��type�����ʹ�ŵ�99�ű����У�����ǰ�벿��ʱֱ�Ӹ�ֵ��

		father->in = idTable[99].type;
		father->sonPoint[0]->in = father->in;
		father->sonPoint[0]->scope = father->scope;

		Analyze(father->sonPoint[0]);//����identifier_list

		return;
	}

	/*
	else if (father->text.compare("var_declaration") == 0|| father->text.compare("var_declaration_1") == 0) {
		bt << endl << "var_declaration �� identifier_list : type ; var_declaration_1" << endl;
		if (father->sonPoint[0]->text.compare("empty") == 0)return;
	
		
		father->sonPoint[0]->scope = father->scope;
		father->sonPoint[1]->scope = father->scope;
		Analyze(father->sonPoint[1]);

		Analyze(father->sonPoint[0]);
		//����identifier_list�е�id������ͳһ�������ű��ڱ����������ٶ����
		int idlist_idnum = father->sonPoint[0]->idnum;

		//��׽ڵ��Ϊtype
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
				bt << "type:���е�" << idTable_num-1 << "�������Ϊ"
					<< idTable[idTable_num - 1].type << endl;
			}
			else if (father->sonPoint[0]->text.compare("record") == 0) {
				idTable[idTable_num].type = 9;
				idTable_num++;
				bt << "type:���е�" << idTable_num - 1 << "�������Ϊ"
					<< idTable[idTable_num - 1].type << endl;
				//father->sonPoint[1]->idnumber = father->idnumber;
				//Analyze(father->sonPoint[1]);
				//��Ҫѧϰ����pascal��s��record���͵��й�֪ʶ���ٽ��в���
			}
			else if (father->sonPoint[0]->text.compare("array") == 0) {
				idTable[idTable_num].type = 5;
				idTable_num++;
				bt << "type:���е�" << idTable_num - 1 << "�������Ϊ"
					<< idTable[idTable_num - 1].type << endl;
				father->sonPoint[1]->idnumber = idTable_num-1;
				//����array�е�periods
				Analyze(father->sonPoint[1]);
				//����array�е�type
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
					bt << "array:���е�" << idTable_num - 1 << "�����������Ϊ"
						<< idTable[idTable_num - 1].array_type << endl;
				}
				else {
					//*��������д�ŵĲ��Ǳ�׼�������ͣ������еķ��ű�ṹ�޷���¼..
				}
			}
		}

		//��׽ڵ���var_declaration
		father = father->fatherPoint;
		TreePoint *a = father->sonPoint[0];
		for (int j = 1; j <= idlist_idnum; j++) {
			idTable[idTable_num - j].id = a->sonPoint[0]->sonPoint[0]->text;
			bt << "var:��" << idTable_num - j << "�������Ϊ" << idTable[idTable_num - j].id << endl;
			a = a->sonPoint[1];
		}

	}

	else if (father->text.compare("var_declaration_1") == 0) {
		if (father->sonnumber == 1)return;//Ϊempty
		else {
			Analyze(father->sonPoint[0]);
			//����identifier_list�е�id������ͳһ�������ű��ڱ����������ٶ����
			int idlist_idnum = father->sonPoint[0]->idnum;

			//��׽ڵ��Ϊtype
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
					bt << "type:���е�" << idTable_num - 1 << "�������Ϊ"
						<< idTable[idTable_num - 1].type << endl;
				}
				else if (father->sonPoint[0]->text.compare("record") == 0) {
					idTable[idTable_num].type = 9;
					idTable_num++;
					bt << "type:���е�" << idTable_num - 1 << "�������Ϊ"
						<< idTable[idTable_num - 1].type << endl;
					//father->sonPoint[1]->idnumber = father->idnumber;
					//Analyze(father->sonPoint[1]);
					//��Ҫѧϰ����pascal��s��record���͵��й�֪ʶ���ٽ��в���
				}
				else if (father->sonPoint[0]->text.compare("array") == 0) {
					idTable[idTable_num].type = 5;
					idTable_num++;
					bt << "type:���е�" << idTable_num - 1 << "�������Ϊ"
						<< idTable[idTable_num - 1].type << endl;
					father->sonPoint[1]->idnumber = idTable_num - 1;
					//����array�е�periods
					Analyze(father->sonPoint[1]);
					//����array�е�type
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
						bt << "array:���е�" << idTable_num - 1 << "�����������Ϊ"
							<< idTable[idTable_num - 1].array_type << endl;
					}
					else {
						//��������д�ŵĲ��Ǳ�׼�������ͣ������еķ��ű�ṹ�޷���¼..
					}
				}
			}

			//��׽ڵ���var_declaration
			father = father->fatherPoint;
			TreePoint *a = f
ather->sonPoint[0];
			for (int j = 1; j <= idlist_idnum; j++) {
				idTable[idTable_num - j].id = a->sonPoint[0]->sonPoint[0]->text;
				bt << "var:��" << idTable_num - j << "�������Ϊ" << idTable[idTable_num - j].id << endl;
				a = a->sonPoint[1];
			}

		}
	}
	*/
	
	else if (father->text.compare("subprogram_declarations") == 0) {
		bt << endl << "subprogram_declarations �� subprogram_declarations_1" << endl;
		Analyze(father->sonPoint[0]);
		return;
	}

	else if (father->text.compare("subprogram_declarations_1") == 0) {
		bt << endl << "subprogram_declarations_1 �� subprogram_declaration ; subprogram_declarations_1 | empty" << endl;
		if (father->sonPoint[0]->text.compare("empty") == 0)return;

		Analyze(father->sonPoint[0]);
		Analyze(father->sonPoint[1]);

		return;
	}

	else if (father->text.compare("subprogram_declaration") == 0) {

	//bt << endl << "subprogram_declarations_1 �� subprogram_declaration ; subprogram_declarations_1 | empty" << endl;

		if (father->sonPoint[0]->text.compare("function") == 0) {

			bt << endl << "subprogram_declaration �� function id formal_parameter : standard_type ; program_body" << endl;

			if (Main_num == 0)Main_num = idTable_num - 1;//wangle
			scopenum++;//λ���µ������ˡ�
			father->scope = scopenum;


			/*�ڴ˴��Ӻ���/���̱����¸�ֵ*/
			for (int i = 1; i <= 4; i++) {
				father->sonPoint[i]->idnumber = idTable_num;
				father->sonPoint[i]->scope = father->scope;//��һ��ʹ��program����bodyλ���µ�scope���ˡ�
			}

			string id= father->sonPoint[1]->sonPoint[0]->text;
			if (checkID(id, 0) != -1) {
				bt << "��������������" << endl;
				return;
			}

			idTable[idTable_num].type = 7;
			bt << "�½�function:" << idTable_num<<endl;
			father->idnumber = idTable_num;
			idTable_num++;

			idTable[idTable_num - 1].id = father->sonPoint[1]->sonPoint[0]->text;
			bt << "function:��" << idTable_num - 1 << "�������Ϊ" 
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
			cout << "standard_type��" << endl;

			Analyze(father->sonPoint[4]);
			
			cout << "program_body!" << endl;
			return;
		}

		else if (father->sonPoint[0]->text.compare("procedure") == 0) {

			bt << endl << "subprogram_declaration �� procedure id formal_parameter; program_body" << endl;

			if (Main_num == 0)Main_num = idTable_num - 1;

			scopenum++;//λ���µ������ˡ�
			father->scope = scopenum;


			/*�ڴ˴��Ӻ���/���̱����¸�ֵ*/
			for (int i = 1; i <= 3; i++) {
				father->sonPoint[i]->idnumber = idTable_num;
				father->sonPoint[i]->scope = father->scope;
			}

			string id = father->sonPoint[1]->sonPoint[0]->text;
			if (checkID(id, 0) != -1) {
				bt << "��������������" << endl;
				return;
			}

			idTable[idTable_num].type = 8;
			bt << "�½�procedure:" << idTable_num<<endl;
			idTable_num++;

			idTable[idTable_num - 1].id = father->sonPoint[1]->sonPoint[0]->text;
			bt << "procedure:��" << idTable_num - 1 << "�������Ϊ"
				<< idTable[idTable_num - 1].id << endl;

			Analyze(father->sonPoint[2]);
			Analyze(father->sonPoint[3]);
			return;
		}
		return;
	}
	
	else if (father->text.compare("formal_parameter") == 0) {

		bt << endl << "formal_parameter �� ( parameter_lists ) | empty" << endl;
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

		bt << endl << "parameter_lists �� parameter_list parameter_lists_1" << endl;

		father->sonPoint[0]->idnum = 1;
		father->sonPoint[1]->idnum = 2;//��ʾΪ�����ĵڼ����β�
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
			bt << endl << "parameter_lists_1 �� empty" << endl;

			idTable[father->idnumber].dimention = father->idnum-1;
			bt << "function/procedure����" << father->idnumber << "����β�����Ϊ(��)��" << father->idnum - 1 << endl;
			cout << "return" << endl;
			return;
		}
		else {
			bt << endl << "parameter_lists_1 �� ; parameter_list parameter_lists_1 " << endl;


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
			bt << endl << "parameter_list �� var_parameter " << endl;
			father->sonPoint[0]->idnumber = father->idnumber;
			father->sonPoint[0]->sonPoint[1]->idnumber = father->idnumber;
			father->sonPoint[0]->sonPoint[1]->idnum = father->idnum;
			father->sonPoint[0]->sonPoint[1]->scope = father->scope;
			bt << endl << "var_parameter -> var value_parameter" << endl;

			Analyze(father->sonPoint[0]->sonPoint[1]);
		}
		else  if (father->sonPoint[0]->text.compare("value_parameter") == 0) {
			bt << endl << "parameter_list ��  value_parameter" << endl;
			father->sonPoint[0]->idnumber = father->idnumber;
			father->sonPoint[0]->idnum = father->idnum;
			father->sonPoint[0]->scope = father->scope;
			Analyze(father->sonPoint[0]);

		}
		return;
	}

	else if (father->text.compare("value_parameter") == 0) {

		bt << endl << "value_parameter �� identifier_list : standard_type" << endl;

		int type;
		if (father->sonPoint[1]->sonPoint[0]->text.compare("integer") == 0)type=3;
		else if (father->sonPoint[1]->sonPoint[0]->text.compare("real") == 0)type = 2;
		else if (father->sonPoint[1]->sonPoint[0]->text.compare("Boolean") == 0)type = 4;
		else if (father->sonPoint[1]->sonPoint[0]->text.compare("char") == 0)type = 1;

		idTable[father->idnumber].parameter_type[father->idnum] = type;
		bt << "function/procedure:��" << father->idnumber << "��ĵ�" << father->idnum
			<< "���β�����Ϊ" << type;

		father->sonPoint[0]->in = type;
		father->sonPoint[0]->scope = father->scope;
		father->sonPoint[0]->call_p_s = father->idnumber;//���ڱ�ʾ���ĸ�����or���̵��β�
		father->sonPoint[0]->idnum = father->idnum;

		Analyze(father->sonPoint[0]);//���� identifier_list
		
		idTable[father->idnumber].parameter_num[father->idnum] = father->sonPoint[0]->idnum;
		bt << "function/procedure:��" << father->idnumber << "��ĵ�" << father->idnum
			<< "���βθ���Ϊ" << father->sonPoint[0]->idnum;
		
		return;
	}

	else if (father->text.compare("compound_statement") == 0) {

		bt << endl << "compound_statement �� begin statement_list end" << endl;

		father->sonPoint[1]->idnumber = father->idnumber;
		father->sonPoint[1]->scope = father->scope;
		Analyze(father->sonPoint[1]);

		return;
	}

	else if (father->text.compare("statement_list") == 0) {

		bt << endl << "statement_list �� statement statement_list_1" << endl;

		father->sonPoint[0]->idnumber = father->idnumber;
		father->sonPoint[1]->idnumber = father->idnumber;
		father->sonPoint[0]->scope = father->scope;
		father->sonPoint[1]->scope = father->scope;
		Analyze(father->sonPoint[0]);
		Analyze(father->sonPoint[1]);

		return;
	}

	else if (father->text.compare("statement_list_1") == 0) {

		bt << endl << "statement_list_1 �� ; statement statement_list_1 | empty" << endl;

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
			bt << endl << "statement �� id statement_1 " << endl;
			int id_wz;
			//call_procedure_statement �ҵ����̻����ڳ����е�λ��
			if (father->sonPoint[1]->sonnumber == 1) {			
				for (int j = 0; j < idTable_num; j++) {
					if (idTable[j].type == 7 || idTable[j].type == 8) {
						if (idTable[j].id.compare(father->sonPoint[0]->sonPoint[0]->text) == 0) {
							finded = 1;
							id_wz = j;
							father->sonPoint[1]->idnum = j;
							father->sonPoint[1]->sonPoint[0]->idnum = j;
							//��idnum���������ҵ���f/p��λ��
							
						}
					}
				}
				if (finded == 0) {
					bt << "�Ҳ����������߹���!" << endl;
					return;
				}
			}

			//id_varparts assignop expression �ҵ�id�ڳ����е�λ��
			else {
				string id = father->sonPoint[0]->sonPoint[0]->text;
				id_wz = checkID(id, father->scope);
				if (id_wz == -1) {
					bt << "���ű����޴�id" << endl;
					return;
				}
				else {
					bt << "���ű����д�id" << endl;
				}
			}
			father->sonPoint[1]->scope = father->scope;
			father->sonPoint[1]->idnumber = id_wz;//����ط���¼���ǷǺ������ù���ʱ��ǰ���Ǹ�id��λ�á�

			Analyze(father->sonPoint[1]);//��һ������statement_1

			return;
		}
		
		else if (father->sonPoint[0]->text.compare("if") == 0) {
			bt << endl << "statement -> if expression then statement else_part" << endl;
			father->sonPoint[1]->scope = father->scope;
			father->sonPoint[1]->in = 20;//���ܼ򵥵ĵ���������
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
			father->sonPoint[1]->in = 20;//boolean;����ûʲô��
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
				bt << "�ñ��������ڣ�" << id << endl;
				return;
			}
			else {
				bt << "forѭ���еı������ڣ�" << id << endl;
				father->in = idTable[id_wz].type;//��Ҫ�����ͼ�¼������
			}
			
			father->sonPoint[3]->scope = father->scope;
			father->sonPoint[3]->in = father->in;//id������
			Analyze(father->sonPoint[3]);

			father->sonPoint[5]->scope = father->scope;
			father->sonPoint[5]->in = father->in;//id������
			Analyze(father->sonPoint[5]);

			father->sonPoint[7]->scope = father->scope;
			Analyze(father->sonPoint[7]);
		}
		
		return;
	}

	else if (father->text.compare("else_part") == 0) {
		bt << endl << "else_part �� else statement | empty" << endl;
		if (father->sonnumber == 1)return;

		father->sonPoint[1]->scope = father->scope;
		Analyze(father->sonPoint[1]);

		return;
	}

	else if (father->text.compare("case_body") == 0) {
		bt << endl << "case_body �� branch_list | empty" << endl;
		if (father->sonPoint[0]->text.compare("empty")==0)return;

		father->sonPoint[0]->scope = father->scope;
		Analyze(father->sonPoint[0]);

		return;
	}

	else if (father->text.compare("branch_list") == 0|| father->text.compare("branch_list_1") == 0
	|| father->text.compare("branch") == 0|| father->text.compare("const_list") == 0
	|| father->text.compare("branch_list_1") == 0) {
		bt << endl << "branch_list �� branch branch_list_1" << endl;
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
			bt << endl << "statement_1 �� call_procedure_statement_1" << endl;
			father->sonPoint[0]->call_p_s = 1;
			father->sonPoint[0]->scope = father->scope;//��������ں���������Ӻ����Ļ�emm���벻������
			father->sonPoint[0]->idnum = father->idnum;//��¼���ĸ�����/���̡�

			Analyze(father->sonPoint[0]);
		}
		else {
			bt << endl << "statement_1 �� id_varparts assignop expression " << endl;
			//id_varpartsӦ����id���������recordʱ��Ҫ�����ݣ��Ժ�д������
			int l_type = idTable[father->idnum].type;
			//father->sonPoint[2]->idnumber = father->idnumber;
			father->sonPoint[2]->in = idTable[father->idnumber].type;//��Ҫ��id������һ�²ſ��Ը�ֵ��
			father->sonPoint[2]->idnum = father->idnum;
			father->sonPoint[2]->scope = father->scope;
			Analyze(father->sonPoint[2]);
		}
		return;
	}

	else if (father->text.compare("call_procedure_statement_1") == 0) {
		if (father->sonPoint[0]->text.compare("empty") == 0) {
			bt << endl << "call_procedure_statement_1 �� empty" << endl;

			return;
		}
		else {
			bt << endl << "call_procedure_statement_1 �� ( expression_list ) " << endl;

			father->sonPoint[0]->idnum = father->idnum;
			father->sonPoint[0]->call_p_s = 1;
			father->sonPoint[0]->scope = father->scope;
			father->call_p_s = father->idnum;//���Ըɴ�Ѻ�����ŷŵ��¶��������
			//�����Ļ��ճ�idnum���������洢������ݣ����統ǰ����Ϊexpression?_list�еĵڼ���������

			bt << "���õ�" << father->idnum << "���/����,�����β��ж�..." << endl;

			Analyze(father->sonPoint[0]);
			//father = father->sonPoint[0];
			/*Ӧ���ǻ����βν������ͼ��ģ����ǲ���д�� ��*/
			/*��������ͼ�ⲹ�ϣ�������*/
		}	
		return;
	}

	else if (father->text.compare("expression_list") == 0) {
		bt << endl << "expression_list �� expression expression_list_1" << endl;
		father->sonPoint[0]->idnum = 1;//expression���ǵ�һ���β�
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
		bt << endl << "expression_list_1 �� , expression expression_list_1 | empty" << endl;

		if (father->sonnumber == 1) {
			bt << endl << "expression_list_1 �� empty" << endl;

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

		bt << endl << "expression �� simple_expression expression_1" << endl;
		father->sonPoint[0]->idnum = father->idnum;
		father->sonPoint[1]->idnum = father->idnum;//��¼Ϊ�ڼ����βΣ�������βεĻ���
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


		cout << "����expression1" << endl;
		//int a;
		//cin >> a;
		Analyze(father->sonPoint[1]);

		cout << "expression_1 return" << endl;

		return;
	}

	else if (father->text.compare("expression_1") == 0) {
		if (father->sonnumber == 1) {
			bt << endl << "expression_1 ��  empty" << endl;
			return;

		}
		cout << "����expression1" << endl;
		
		bt << endl << "expression_1 �� relop simple_expression " << endl;//��Ϊ�������βεĻ�����һ���ֿ϶���empty
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
			bt << endl << "simple_expression �� term simple_expression_1  " << endl;

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
			bt << endl << "simple_expression �� +/- term simple_expression_1  " << endl;

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
			bt << endl << "simple_expression_1 �� addop term simple_expression_1   " << endl;

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
		bt << endl << "term �� factor term_1  " << endl;

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

		father->in = father->sonPoint[0]->in;//�ѵ�һ���ӽڵ�����ʹ������ˡ�
		father->sonPoint[1]->in = father->in;

		cout << "term_1" << endl;

		Analyze(father->sonPoint[1]);

		cout << "return" << endl;

		return;
	}

	else if (father->text.compare("term_1") == 0) {
		if (father->sonnumber == 1)return;
		else if (father->sonnumber == 0) {
			bt << endl << "term_1->mulop factor term_1������" << endl;
			error = 1;
			return;
		}

		else {
			bt << endl << "term_1 �� mulop factor term_1  " << endl;

			

			cout << father->sonnumber << endl;

			father->sonPoint[1]->idnum = father->idnum;
			father->sonPoint[2]->idnum = father->idnum;
			father->sonPoint[1]->idnumber = father->idnumber;
			father->sonPoint[2]->idnumber = father->idnumber;
			father->sonPoint[2]->scope = father->scope;
			father->sonPoint[1]->scope = father->scope;
			
			//father->idnumber = father->sonPoint[1]->idnumber;//��factor�����ʹ������ˡ�

			father->sonPoint[2]->in = father->in;//mulop�����������Ҫ��ǰ���factor����һ��
			father->sonPoint[1]->in = father->in;

			//����ط�����Ӧ���ǳ���ģ�but������д
			//����������������������
			cout << "term_1->factor1" << endl;
			Analyze(father->sonPoint[1]);
			cout << "term_1->factor2" << endl;

			Analyze(father->sonPoint[2]);

			cout << "term_1_end" << endl;
		}

		return;
	}

	else if (father->text.compare("factor") == 0) {

		//bt << endl << "term_1 �� mulop factor term_1  " << endl;
		cout << "factor" << endl;
		//���Ϊ�������βεĻ�
		if (father->call_p_s != 0) {
			cout << "�β�" << endl;

			//�Ǻ����Ĳ�����Ӧ���ǵ�idnum����������call_p_s��
			int type;
			int f_type;//�β�����
			int f_wz;//����λ��
			int wz;//�β�λ��
			wz = father->idnum;
			f_wz = father->call_p_s;
			int j = 0;
			for (int i = 1; i <= idTable[f_wz].dimention; i++) {
				j = j + idTable[f_wz].parameter_num[i];
				if (wz <= j) {
					f_type = idTable[f_wz].parameter_type[i];
					//���Ӧ�õ����͡�
				}
			}
			if (father->sonPoint[0]->text.compare("num") == 0) {
				type = 3;//integer
				bt << endl << "factor �� num " << endl;
			}
			if (father->sonPoint[0]->text.compare("letter") == 0) {
				bt << endl << "factor �� 'letter' " << endl;
				type = 1;//char
			}
			if (father->sonPoint[0]->text.compare("not") == 0) {
				bt << endl << "factor �� not factor" << endl;
				type = 4;//boolean;
				//check factor
				father->sonPoint[1]->in = 4;//����ı���Ҳ��Boolean���С�
				father->sonPoint[1]->scope = father->scope;
				Analyze(father->sonPoint[1]);
			}
			if (father->sonPoint[0]->text.compare("id") == 0) {
				bt << endl << "factor �� id factor_1" << endl;
				string id;
				id= father->sonPoint[0]->sonPoint[0]->text;
				int idwz;
				idwz = checkID(id, 0);//Ӧ�þ��ǵ�ȫ�ֱ����ɣ��������Ƕ�׵��õĻ����鷳��ѽ
				if (idwz == -1) {
					bt << "δ�������:" << id << endl;
					
				}
				else {
					bt << "�Ѷ��������" << id << endl;
					type = idTable[idwz].type;
				}
				
			}
			if (type == f_type||(type==3&&f_type==4)) {
				bt << "�ñ������β�ƥ��,����Ϊ��" << f_type << endl;
			}
			else {
				bt << "�ñ������β����Ͳ�ƥ�䣬�������β�����Ϊ��" << type << " " << f_type << endl;
			}
			return;
			
		}
		//������Ǻ������βεĻ�
		else {
			bt << endl << "factor �� id factor_1 | num | ' letter ' | ( expression ) | not factor" << endl;
			cout << "���β�" << endl;

			if (father->in == 4) {

				cout << "not factor" << endl;

				if (father->sonPoint[0]->text.compare("num") == 0) {
					bt << "��������Ϊ���֣����Էŵ�not����" << endl;
				}

				else if (father->sonPoint[0]->text.compare("letter") == 0) {
					bt << "��������Ϊ��ĸ�������Էŵ�not���棡" << endl;
				}

				else if (father->sonPoint[0]->text.compare("not") == 0) {
					father->sonPoint[1]->in = 4;//����ı���Ҳ��Boolean���С�
					father->sonPoint[1]->scope = father->scope;
					Analyze(father->sonPoint[1]);

				}

				else if (father->sonPoint[0]->text.compare("id") == 0) {
					int id_type;
					string id;
					id = father->sonPoint[0]->sonPoint[0]->text;
					if (checkID(id, father->scope) == -1) {
						bt << "��id�����ڣ�" << id << endl;
					}
					else {
						id_type = idTable[checkID(id, father->scope)].type;
						if (id_type == 2 || id_type == 4) {
							bt << "��id����Ϊ���ֻ򲼶������Է���not����" << endl;
						}
						else {
							bt << "��id�����Է���not���棡" << endl;
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
					bt << "��id�����ڣ�" << id << endl;
				}
				else {
					type = idTable[checkID(id, father->scope)].type;
				}
			}

			if (father->in != 20 && father->in != 0 && father->in != 4
				&&father->in != 7 &&father->in != 8) {//����д����˼�ǣ���Ҫ����һ��ȷ����������
				if (type != father->in) {
					bt << "id���Ͳ����ϣ���Ҫ����Ϊ��" << father->in << " ʵ������Ϊ��" << type << endl;
				}

				else {
					bt << "id���ͷ��ϡ�" << type << endl;
				}
			}
			else if (father->in == 7 || father->in == 8) {
				//if(type==)
				//����ط���Ҫ���к����͹��̵��ж�
			}
			else {
				//����Ͳ���Ҫ
				cout << "get type   " << type << endl;
				father->in = type;//��idnumber��type����ȥ��
			}

		}
		
		return;
		
	}
	else {
		return;
	}

}

/*
�������ƣ�printlist
�������ܣ���ӡ��ǰ�ķ��ű�
�汾��1.0
�汾����:lxy
���ڣ�2020/7/17
*/

void printlist() {
	int i,j;
	cout << idTable_num << endl;
	for (i = 0; i < idTable_num; i++) {
		listid << "��ţ�" << i << endl;
		listid << "���ƣ�" << idTable[i].id << endl;
		listid << "���ͣ�" <<TYPE[idTable[i].type] << endl;
		if (idTable[i].type == 7 || idTable[i].type == 8) {
			listid << "�β����ࣺ" << idTable[i].dimention << endl;
			for (j = 1; j <= idTable[i].dimention; j++) {
				listid << "��" << j << "���βθ���Ϊ��" << idTable[i].parameter_num[j] << endl;
				listid << "��" << j << "���β�����Ϊ��" << TYPE[idTable[i].parameter_type[j]] << endl;
			}
		}
		listid << endl;
	}
}
int main() {
	ReadinAndMakeTree();

	cout << "�﷨�����ɣ����Ϊtree.txt..." << endl;

	tree.close();

	Analyze(root);

	if(error==0)
		cout << "���ͼ������ɣ����Ϊcheck.txt..." << endl;
	else cout << "�����г���.." << endl;

	bt.close();

	printlist();
	cout << "���ű��ѽ��������Ϊidlist.txt..." << endl;

	listid.close();

	Ir ir;
	ir.generate();
	system("pause");
}
