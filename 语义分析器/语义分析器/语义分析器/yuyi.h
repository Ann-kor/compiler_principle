#ifndef  YUYI_H_
#define YUYI_H_
#include<stdio.h>
#include<iostream>
#include<cstring>
#include<string>
#include<fstream>
#include<sstream>

using namespace std;

//��
struct TreePoint {
	int idnumber;//���ԣ��ýڵ���ص�id�ڷ��ű��е����
	int sonnumber = 0;
	int in = 0;//�̳�����in��һ���Ƿ��ű��typeֵ��
	int idnum = 0;//���ԣ�id
	int scope = 0;//��id�����scope��˼��ͬ
	int call_p_s = 0;//�Ƿ�Ϊ�������β�

	string text;//id������
	TreePoint* sonPoint[11] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };//���ȹ̶���tai'duan'l
	TreePoint* fatherPoint = NULL;
	int sonBranch[7] = { 0 };
};

//extern string TYPE[10] = { "default","char","real","integer","Boolean","array","const","function","procedure","record" };

//���ű����
struct SIdTable {
	int idnumber;
	string id;
	int type;//1 char 2 real 3 integer 4 boolean 5 array 6 const 7 function 8 procedure 9 record
	int dimention;//ά��or��������
	int array_type;//��������
	int scope;//�÷��������ĸ�������main�������Ӻ����ȵȡ���0��n

	/*function��procedure�����б���*/
	int parameter_type[10];//ÿ���βε����͡�
	int parameter_num[10] = { 0 };//ÿ���βεĸ���
	int returntype;//����ֵ���͡�

	/*���������б���*/
	int const_type;//���������� 1 char 2 real 3 integer 4 boolean (����id�����) 5 num 6 letter
	int const_id_type;
};

/*�˶�������������ʶ����ŵ�������*/
//int FandP_num=0;//�����е�function��procedure����
//int FandPinTable[10];//function��procedure�ڷ��ű��е�λ��-���������ڷ��ű��еķֲ�

extern int scopenum;//��������������͹��̵�������0��ʾֻ��main��
extern int SCOPE[10];//ÿ����ķ�Χ��Ĭ��ȫ�ֱ���ֻ���ڿ�ͷ���塣2020/7/16

extern int Main_num;//ȫ�ֱ����ĸ��� ��Ҳ��֪����û����

extern SIdTable idTable[100];//���ű�
extern int idTable_num;//��������

extern TreePoint *root;//���ĸ��ڵ�
extern int broot;
extern TreePoint *nowroot;

extern void generate();
#endif