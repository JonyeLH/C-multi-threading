/*	README
һ�����ģʽ���̸

����C++�������ģʽʵ��
	���캯������private
	����һ����̬��Աָ���ʼ��λNULL
	���ݾ�̬��Ա״̬�ھ�̬���������ɵ�������
	����������ɾ����ƣ�������


�������̵߳��������ģʽ������������������������һ
	���̴߳���ͬʱ���ʵ�����������
	ʹ�û�������˫������

�ģ����̵߳��������ģʽ����������������������������
	ʹ��std::call_once()
	
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <map>
using namespace std;

std::mutex resource_mutex;
std::once_flag g_flag;	//����һ��ϵͳ����ı��

class MyCAS {	//����һ��������

	static void CreateInstance() {//ֻ����һ��
		std::chrono::milliseconds dura(10000);//��Ϣ20��۲�ڶ����߳��Ƿ���ס
		std::this_thread::sleep_for(dura);
		cout << "CreateInstance()ִ����" << endl;
		m_instance = new MyCAS();	//���ֻϣ��ִ��һ�Ρ���ֻ����һ�ζ���
		static CGarhuishou cl;
	}

private:
	MyCAS(){}	//˽�л����캯�����������Ͳ���ʵ������Ķ���

private:
	static MyCAS *m_instance;	//��̬��Ա������ָ�룩
	
public:
	static MyCAS *GetInstance() {
		//std::unique_lock<std::mutex> mymutex(resource_mutex);//����������жϣ�����һ���д������߳���Ҫ���õ�GetInstance���������
															   //�Ͷ��Ῠ���⣬����Ч�ʵ��¡���˽���������Ż�
		//if (m_instance == NULL) {
		//	m_instance = new MyCAS();	//���ֻϣ��ִ��һ�Ρ���ֻ����һ�ζ���
		//	static CGarhuishou cl;	//�����������ľ�̬����cl����̬��������������ǳ�����˳�ʱ�򣬾ͻ����cl�������������ͻ��ͷ�֮ǰnew�Ķ���
		//}


		////���Ч��
		////a)���if (m_instance != NULL) ������������ʾm_instance�Ѿ���new�ˣ�
		////b�����if (m_instance == NULL) ������������ʾm_instanceû��new��
		//if (m_instance == NULL) {	//˫���������߽���˫�ؼ��
		//	std::unique_lock<std::mutex> mymutex(resource_mutex);//
		//	if (m_instance == NULL) {
		//		m_instance = new MyCAS();	//���ֻϣ��ִ��һ�Ρ���ֻ����һ�ζ���
		//		static CGarhuishou cl;	//�����������ľ�̬����cl����̬��������������ǳ�����˳�ʱ�򣬾ͻ����cl�������������ͻ��ͷ�֮ǰnew�Ķ���
		//	}
		//}



		//������԰�g_flag����һ����
		std::call_once(g_flag,CreateInstance);	//���������߳�ͬʱִ�е�������е�һ���̳߳ɹ�����call_once������
												//����һ��Ҫ�ȴ���һ���߳�ִ����Ϻ󷵻صı�ǣ�g_flag���ھ����Ƿ�Ҫ�ٴ�ִ��
		cout << "call_once()ִ�����" << endl;

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

//�߳���ں���
void mythread() {
	cout << "�ҵ��߳̿�ʼִ����" << endl;
	MyCAS *p_a = MyCAS::GetInstance();
	cout << "�ҵ��߳�ִ�������" << endl;
	return;
}

int main() {
	/*
	һ�����ģʽ���̸
		�����ģʽ���������һЩд������Щд���ͳ��治һ������������ά���������㣬���Ǳ��˽ӹܡ��Ķ��������ʹ��
		�á����ģʽ������д�����Ĵ���ܻ�ɬ
		Ӧ���ر�����Ŀʱ���ѿ������顢ģ�黮�־��飬�ܽ�����ģʽ�����ݿ������󣬺����ܽ������
		Ҫ��ѧ���ã���Ҫ����Ӳ��

	�����������ģʽ��ʹ�õ�Ƶ�ʱȽϸߣ���������Ŀ�У���ĳ������ĳЩ������࣬���ڸ���Ķ���ֻ����һ�������ܴ������
		


				���ж��߳�ͬʱ���õ�����������ʱ������ķ����ͽ������û���ʹ��������
	�������������ģʽ��������������������	
			���ٵ����⣺��Ҫ���Լ��������̣߳������̣߳��д���MyCAS���������Ķ��������߳̿��ܲ�ֹһ����
			��������GetInstance�����������л���
			ʹ�û�������˫�����������

	�ģ�std::call_once()	C++11����ĺ������ú����ĵڶ������� ��һ��������a()
		���ܣ��ܹ���֤����a ֻ����һ��
		�߱���������������������Ч���ϣ��Ȼ�����������Դ���١��ڶ��߳���ʹ��call_once()������ֻ����һ��ĳһ������
		call_once()��Ҫ��һ����ǽ��ʹ�ã���������std::once_flag  ��ʵonce_flag��һ���ṹ��
		call_once()����ͨ����Ǿ�������a�Ƿ�ִ�У�����call_once()�ɹ���call_once()�Ͱ�����������Ϊһ���ѵ��õ�״̬����ô����a�Ͳ��ᱻ��ִ����
		
		�����
		�ҵ��߳̿�ʼִ����
		�ҵ��߳̿�ʼִ����
		CreateInstance()ִ����	//��һ���߳�ִ����
		call_once()ִ�����
		�ҵ��߳�ִ�������
		
		//�ڶ����̵߳ȴ���10s��������/

		call_once()ִ�����
		�ҵ��߳�ִ�������
		�밴���������. . .
	*/	


	////��û��������߳��аѵ������󴴽�������Ȼ����г�ʼ���Ȳ���
	////��Ϊ�ѹ��캯��д�����е�private��ֱ��ʹ����������������ʧ�ܵ�
	////MyCAS a1;

	//MyCAS *p_a = MyCAS::GetInstance();	//����һ�����󣬷��ظ��ࣨMyCAS�������ָ��
	////MyCAS *p_b = MyCAS::GetInstance();	//��ʱ��������ʧ�ܣ���Ϊ������һ������ʱ�������Ǿ�̬��������NULL���������˵�һ����ʱ�������������㣬���Դ���ʧ��,�����صĻ��ǵ�һ�δ����Ķ���
	//p_a->fun();
	//MyCAS::GetInstance()->fun();	//�����MyCAS::GetInstance()���ص���һ��ָ�룬���Կ���ʹ�� -> ���ó�Ա����
	

	////��û��������߳��аѵ������󴴽�������Ȼ����г�ʼ���Ȳ������ٴ��������߳̽��е���
	////�����߳���ͬһ����ں������������������̣߳���ζ���������������ͬʱִ�к������ͻ�������⣺�����߳�ͬʱִ�е������������ɶ����ж����
	//std::thread mytobj1(mythread);
	//std::thread mytobj2(mythread);
	//mytobj1.join();
	//mytobj2.join();

	return 0;
}