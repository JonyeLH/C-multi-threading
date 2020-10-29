#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <map>
using namespace std;

class MyCAS {	//����һ��������
private:
	MyCAS(){}	//˽�л����캯�����������Ͳ���ʵ������Ķ���

private:
	static MyCAS *m_instance;	//��̬��Ա������ָ�룩
	
public:
	static MyCAS *GetInstance() {
		if (m_instance == NULL) {
			m_instance = new MyCAS();
			static CGarhuishou cl;	//�����������ľ�̬����cl����̬��������������ǳ�����˳�ʱ�򣬾ͻ����cl�������������ͻ��ͷ�֮ǰnew�Ķ���
		}
		return m_instance;
	}

	class CGarhuishou {	//�������࣬�����ͷ���ǰnew�Ķ���
	public:
		CGarhuishou() {
			if (MyCAS::m_instance) {
				delete MyCAS::m_instance;
				MyCAS::m_instance = NULL;
			}
		}
	};

	void fun() {
		cout << "����" << endl;
	}

};

//�ྲ̬�����ĳ�ʼ��
MyCAS *MyCAS::m_instance = NULL;

int main() {
	/*
	һ�����ģʽ���̸
		�����ģʽ���������һЩд������Щд���ͳ��治һ������������ά���������㣬���Ǳ��˽ӹܡ��Ķ��������ʹ��
		�á����ģʽ������д�����Ĵ���ܻ�ɬ
		Ӧ���ر�����Ŀʱ���ѿ������顢ģ�黮�־��飬�ܽ�����ģʽ�����ݿ������󣬺����ܽ������
		Ҫ��ѧ���ã���Ҫ����Ӳ��

	�����������ģʽ��ʹ�õ�Ƶ�ʱȽϸߣ���������Ŀ�У���ĳ������ĳЩ������࣬���ڸ���Ķ���ֻ����һ�������ܴ������
		

		

	*/	


	//��Ϊ�ѹ��캯��д�����е�private��ֱ��ʹ����������������ʧ�ܵ�
	//MyCAS a1;

	MyCAS *p_a = MyCAS::GetInstance();	//����һ�����󣬷��ظ��ࣨMyCAS�������ָ��
	//MyCAS *p_b = MyCAS::GetInstance();	//��ʱ��������ʧ�ܣ���Ϊ������һ������ʱ�������Ǿ�̬��������NULL���������˵�һ����ʱ�������������㣬���Դ���ʧ��,�����صĻ��ǵ�һ�δ����Ķ���
	p_a->fun();
	MyCAS::GetInstance()->fun();	//�����MyCAS::GetInstance()���ص���һ��ָ�룬���Կ���ʹ�� -> ���ó�Ա����
	 
	return 0;
}