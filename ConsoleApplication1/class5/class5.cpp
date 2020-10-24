#include <iostream>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
using namespace std;

class A {
public:
	//���յ�����Ϣ���������뵽һ�����е��߳�
	void inMsgRecvQueue() {
		for (int i = 0; i < 10000; ++i) {
			cout << "inMsgRecvQueue()ִ�У�����һ��Ԫ��" << i << endl;
			my_mutex.lock();	//��סһ��Ԫ�صĲ���
			msgRecvQueue.push_back(i);//��������i������ҷ��͵����ݣ�ѹ�������
			my_mutex.unlock();	//����
		}
		return;
	}

	bool outMsgLULProc(int &command) {
		my_mutex.lock();	//��������סһ�����ݵ�ȡ��
		if (!msgRecvQueue.empty()) {	   
			//��Ϣ���в�Ϊ��
			int command = msgRecvQueue.front();//���ص�һ��Ԫ��
			msgRecvQueue.pop_front();//�Ƴ���һ��Ԫ�أ�
			my_mutex.unlock();	//�����unlock��Ӧif(true)�Ľ������������ں����˳�֮ǰ�������
			return true;
		}
		my_mutex.unlock();	//�����unlock��Ӧelse�Ľ������������ں����˳�֮ǰ�������
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

private:
	list<int> msgRecvQueue;//������ר��������ҷ��͹���������
	mutex my_mutex;
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

	Ϊ�˷�ֹ�������unlock()��������lock_guard����ģ�壬����unlock��Ҫ����ϵͳ����unlock();   ����ָ�룬�����ͷ������ͷ�
	*/

}









