#include <iostream>
#include <vector>
#include <thread>
using namespace std;


class A {
public:
	int m_i;
	A(int a) :m_i(a) {	//����ת�����캯�������԰�һ��intת��Ϊһ����A����
		cout << "���캯����ʼִ��" << endl;
	}
	A(const A &a) :m_i(a.m_i) {
		cout << "�������캯��ִ��" << endl;
	}
	~A() {
		cout << "��������ִ��" << endl;
	}
};
void myprint2(const int i, const A &pmybuf) {
	//cout << i << endl;
	cout << &pmybuf << endl;
}


//void myprint(const int &i, char *pmybuf) {		//�鿴i�ĵ�ַ	0x00080004 {1}	�������ﲻ�������ã�����һ�����Ƶ�ֵ ,���������߳�ʹ��detach  �����̺߳�����ʹ�õ�i�ǰ�ȫ��
//	cout << i << endl;			//i������mvar�����ã�ʵ������ֵ����	  �����Ƽ���Ҫ������
//	cout << pmybuf << endl;		//ָ����detach���߳�ʱ��������������ģ���Ϊ��������ָ���ԭ���ľֲ�������ͬһ���ַ��������߳̽����ˣ��ֲ��������ڴ�ͻᱻ�ջأ����ӽ���ͬ��ַ��ָ�����������ֲ���Ԥ֪�Ĵ���
//}

/*	//�������µ��޸ģ���i���趨Ϊ���ã���ν��β��趨Ϊconst string & ���ͣ���Ϊ��������pmybuf��һ����ʱ������const���ÿ��Խ�����ֵ��
˼���ǽ�ԭ��char���͵��ַ�������תΪstring���˹��̾ͻᵼ�´������Ĳ�����ԭ�����ڴ��ַ��һ����
���߳��ٽ��б�̲��������߳��˳�������ԭ�����ڴ���պ�Ĳ���Ԥ֪����
*/
void myprint(const int i, const string &pmybuf) {
	cout << i << endl; 
	cout << pmybuf.c_str() << endl;
}

int main() {
	////һ��������ʱ������Ϊ�̲߳���
	//	//(1.1)Ҫ���������
	//int mvar = 1;	//��debug���趨�ϵ�  ѡ�б���mvar��  shift+F9  �ڱ���mvarǰ����  &  ���س���  ���ڲ鿴����mvar��ַ	0x00e5f9f4 {1}
	//int &mvary = mvar;		//ͬ��ѡ�б���mvary��  shift+F9  �ڱ���mvaryǰ����  &   ���ڲ鿴����mvary��ַ	�۲췢������������ַһ��	0x00e5f9f4 {1}
	//char mybuf[] = "this is a test1";

	////�ú�������ʽ�����̣߳����Ҹ����������������� mvar �� mybuf �ֱ��Ӧ�����������β�
	////thread mytobj1(myprint, mvar, mybuf);	//������debug���Խ����߳�myprint()����,�鿴���ݽ�ȥ�����ò���mvar��Ӧi�ĵ�ַ���ٲ鿴�ڶ�����mybuf��Ӧ��pmybuf��ַ
	////(1.2)Ҫ���������		�ڽ������һʱ����ʵ�Ͽ��ܴ��ڣ�mybuf���������ˣ����߳�ִ����ϣ���ϵͳ��ȥ��char���͵�mybufת��Ϊstring���ʹ��ݸ����߳�

	////��������������
	//thread mytobj1(myprint, mvar, string(mybuf));	//������ֱ�ӽ�mybufת��Ϊstring���󣬿��Ա�֤���߳����ÿ϶���Ч�Ķ���
	//mytobj1.detach();
	//cout << "���߳̽���" << endl;


	//��֤������������
	int mvar1 = 1;
	int mysecon2=12;
	thread mytobj2(myprint2, mvar1, mysecon2);		//����ϣ��mysecond2ת��A���Ͷ��󴫵ݸ�myprint2����		�������߳�ִ���꣬��ת��δִ��
	thread mytobj2(myprint2, mvar1, A(mysecon2));		//ʹ����������ת���Ϳ��������߳��˳�֮ǰ�������ת�����Ӷ����̵߳��԰�ȫִ��

	mytobj2.detach();
	//cout << "���߳̽���" << endl;


	return 0;
}