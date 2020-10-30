/*README


*/

#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <map>
using namespace std;

class A {
private:
	list<int> msgRecvQueue;
	mutex my_mutex1;
	mutex my_mutex2;

public:
	void inMsgRecvQueue() {
		for (int i = 0; i < 10000; ++i) {
			cout << "inMsgRecvQueue()ִ�У�����һ��Ԫ��" << i << endl;
			{
				cout << "inMsgRecvQueueִ�У�����һ��Ԫ��" << i << endl;
				std::unique_lock<std::mutex> myguard(my_mutex1);
				msgRecvQueue.push_back(i);
			}
		}
		return;
	}

	bool outMsgLULProc(int &command) {
		//my_mutex1.lock();
		unique_lock<mutex> myguard1(my_mutex1);	//ʹ�õڶ����������Ȱѻ�������lock��

		if (!msgRecvQueue.empty()) {
			command = msgRecvQueue.front();
			msgRecvQueue.pop_front();
			return true;
		}
		return false;
	}


	void outMsgQueue() {
		int command = 0;
		for (int i = 0; i < 10000; ++i) {
			bool result = outMsgLULProc(command);
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
	/*
	һ����������std::condition_variable��wait()��notify_one()
		�߳�A���ȴ�һ����������
		�߳�B��ר������Ϣ�����������Ϣ�����ݣ�





	*/

	A myobj;
	thread myOutMsgobj(&A::outMsgQueue, &myobj);	//�ڶ����������ã����ܱ�֤�߳���ʹ��ͬһ������
	thread myInMsgobj(&A::inMsgRecvQueue, &myobj);
	myOutMsgobj.join();
	myInMsgobj.join();

	return 0;
}