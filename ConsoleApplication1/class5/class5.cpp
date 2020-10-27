#include <iostream>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
using namespace std;

class A {
private:
	list<int> msgRecvQueue;//������ר��������ҷ��͹���������
	mutex my_mutex1;	//������һ����������һ��������һ����ͷ��
	mutex my_mutex2;	//��������һ����������һ��������һ����ͷ��

public:
	//���յ�����Ϣ���������뵽һ�����е��߳�
	void inMsgRecvQueue() {
		for (int i = 0; i < 10000; ++i) {
			cout << "inMsgRecvQueue()ִ�У�����һ��Ԫ��" << i << endl;
			{
				//lock_guard<mutex> myguard1(my_mutex1);//����ʹ��lock_guard����������������������ڻ�����������λ��֮�⣬���ܶ������������ݣ��ڱ��������ڿ��Բ�����													
				//lock_guard<mutex> myguard2(my_mutex2);


				//�����ķ������߳�A����mutex1��������mutex2
				//my_mutex1.lock();	//��סһ��Ԫ�صĲ��룬
				////������������������
				//my_mutex2.lock();	//ʵ�ʹ����У���������ͷ��һ�������ŵģ����ǿ�����Ҫ������ͬ�����ݹ����


				//�������ⷽ������ʹ��std::lock()����
				std::lock(my_mutex1, my_mutex2);	//ʹ��std::lock()�������������������������滹��Ҫ�ӽ�������

				//˼�����ⷽ���������ڷ�������ʡȥunlock
				lock_guard<mutex> myguard1(my_mutex1, std::adopt_lock);
				lock_guard<mutex> myguard2(my_mutex2, std::adopt_lock);

				msgRecvQueue.push_back(i);//��������i������ҷ��͵����ݣ�ѹ�������

				//my_mutex2.unlock();	//��Ӧ�Ľ�������
				//my_mutex1.unlock();	//������ʱ������ν�Ƚ����ĸ���
			}
		}
		return;
	}



	bool outMsgLULProc(int &command) {
		
		//lock_guard����ģ�壬���԰���ģ���д����������my_mutex��Ϊ��ģ�����myguard���β�
		//��ִ��lock_guard�Ķ�����ʱ�ͻ������ģ��Ĺ��캯��mutex::lock()�������ģ�������һ���ֲ�����
		//�ھֲ�������֮��ͻ������������mutex::unlock()

		// //�����ķ������߳�B����mutex2����mutex1
		// //�߳�A��inMsgRecvQueue������mutex1������mutex2�������ķ�����Ҫ��֮ǰ���߳�A����˳���෴������mutex2������mutex1
		//lock_guard<mutex> myguard2(my_mutex2);
		//lock_guard<mutex> myguard1(my_mutex1);																							
		
		//my_mutex2.lock();	
		//my_mutex1.lock();	//��������סһ�����ݵ�ȡ��

		
		//�������ⷽ��һ���ı�����˳��ʹ�������߳�������˳��һ��
		//my_mutex1.lock();	
		//my_mutex2.lock();	

		//�������ⷽ������ʹ��std::lock()����
		std::lock(my_mutex1, my_mutex2);	//ʹ��std::lock()�������������������������滹��Ҫ�ӽ�������
		
		//�������ⷽ���������ڷ�������ʡȥunlock
		lock_guard<mutex> myguard1(my_mutex1, std::adopt_lock);
		lock_guard<mutex> myguard2(my_mutex2, std::adopt_lock);


		if (!msgRecvQueue.empty()) {	   
			//��Ϣ���в�Ϊ��
			command = msgRecvQueue.front();//���ص�һ��Ԫ��
			msgRecvQueue.pop_front();//�Ƴ���һ��Ԫ�أ�

			//my_mutex1.unlock();	//�����unlock��Ӧif(true)�Ľ������������ں����˳�֮ǰ�������
			//my_mutex2.unlock();

			return true;
		}

		//my_mutex1.unlock();	//�����unlock��Ӧelse�Ľ������������ں����˳�֮ǰ�������
		//my_mutex2.unlock();

		return false;
	}


	//�����ݴӶ�����ȡ�����߳�
	void outMsgQueue() {
		int command = 0;
		for (int i = 0; i < 10000; ++i) {
			//my_mutex.lock();	//ֱ���������ĵ��ã���Ӱ��Ч��
			bool result = outMsgLULProc(command);
			//my_mutex.unlock();
			if (result == true) {
				cout << "outMsgQueue()ִ���ˣ�ȡ����һ��Ԫ��" << command << endl;
				//....��������
			}
			else {
				cout << "outMsgQueue()ִ�У���Ŀǰ��Ϣ����Ϊ��" << i << endl;
			}
		}
		cout << "end" << endl;
	}
};


int main() {

	A myobj;
	thread myOutMsgobj(&A::outMsgQueue, &myobj);	//�ڶ����������ã����ܱ�֤�߳���ʹ��ͬһ������
	thread myInMsgobj(&A::inMsgRecvQueue, &myobj);
	myOutMsgobj.join();
	myInMsgobj.join();

	/*
	�����������ݣ�����ʱ���ô���ѹ���������ס���ݡ��������ݡ��������ݣ�
		����������������ݵ��̱߳���ȴ����ݽ�������ס���ݡ��������ݡ���������

	һ����������Mutex��
	�������Ǹ���������Ϊһ����������̳߳�����lock()��Ա�����������������ֻ��һ���߳̿��������ɹ����ɹ��ı�־��lock()�������أ���
		���û���ɹ�����ô�̵߳ľͻῨ��lock()���ﲻ�ϵĳ���ȥ��

	������������ҪС�ĺ�׼ȷ��������ס��������Ҫ�����ݣ�Ӱ��Ч�ʣ���Ҳ����©����Ҫ������


	
	������������ʹ��
	(2.1)lock() unlock()
	��lock�������������ݣ����֮��unlock
	lock��unlockҪ�ɶ�ʹ�ã����ܵ���һ��lockȴ����������unlock��Ҳ���ܵ�������lock����һ��unlock

	��2.2��lock_guard��ģ�壬ֱ��ȡ��lock��unlock����ʹ����lock_guard֮�󣬾Ͳ���ʹ��lock��unlock
	Ϊ�˷�ֹ�������unlock()��������lock_guard����ģ�壬����unlock��Ҫ����ϵͳ����unlock();   ����ָ�룬�����ͷ������ͷ�



	��������
	 C++�У�����˵����������(����������Ҫ�����������������������ܷ�������)������������
	 �����߳�A,B 
	 ��1���߳�Aִ�е�ʱ������߳��Ȱ� ���� ����lock()�ɹ��ˣ�Ȼ����ȥlock ���� 

	 �����������л�
	 ��2���߳�Bִ���ˣ��߳����� ��������Ϊ����û�б��������������Կ��Ա����ɹ����߳�B����ȥ�� ����

	 ��ʱ�˿̣���������
	 ��3���߳�A��Ϊ�ò��� ���� �������߲���ȥ������ ���� �Ĵ����߲���ȥ�� 
	      �߳�B��Ϊ�ò��� ���� �������߲���ȥ������ ���� �Ĵ����߲���ȥ�� 
	
	��3.1��������ʾ

	��3.2��������һ��������һ��
			��������������˳���ܻ��⣬Ҫһ�£��߳�A��ô�����ģ��߳�BҲ��ô����

	��3.3����������std::lock()����ģ�壬���ڴ���໥�����ĳ���	Ҫ�ǵ�ʹ��unlock()
			һ����ס���������������ϵĻ��������������������˲��ޣ�1�����У�
			��������Ϊ�ڶ���߳��� ��Ϊ����˳�����⵼�������ķ��յ�����
			�������������һ��û��ס������������ȴ����ȴ����л���������ס�����Ż������ߣ����أ�
			Ҫô��������������ס��Ҫô��û���ϣ�һ��������û��ס�Ļ��������ͻ������ס�Ļ������ͷţ���ȥ������һ��
	 
	 ��3.4�����ڷ�����������Ҫ�Լ�����unlock���Ƚϲ����Ի�
			����������������ڷ��������������lock_guard()�������β��������std::adopt_lock��
			std::adopt_lock��һ���ṹ����󣬱�ʾ����������Ѿ�lock���ˣ�����Ҫ��lock_guard<mutex> ���캯���������ٶ�mutex�������lock��
			�ڵ�����������������ִ�У�Ҳ���ǲ���Ҫ�Լ��ֶ�ȥunlock��
			���磺lock_guard<mutex> myguard1(my_mutex1, std::adopt_lock);

		�ܽ᣺std::lock()һ���������������������ʹ��

	*/
	return 0;
}









