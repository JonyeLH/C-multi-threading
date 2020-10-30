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
			cout << "inMsgRecvQueue()执行，插入一个元素" << i << endl;
			{
				cout << "inMsgRecvQueue执行，插入一个元素" << i << endl;
				std::unique_lock<std::mutex> myguard(my_mutex1);
				msgRecvQueue.push_back(i);
			}
		}
		return;
	}

	bool outMsgLULProc(int &command) {
		//my_mutex1.lock();
		unique_lock<mutex> myguard1(my_mutex1);	//使用第二参数必须先把互斥量给lock了

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
				cout << "outMsgQueue()执行了，取出了一个元素" << command << endl;
				//....处理命令
			}
			else {
				cout << "outMsgQueue()执行，但目前消息队列为空" << i << endl;
			}
		}
		cout << "end" << endl;
	}
};

int main() {
	/*
	一：条件变量std::condition_variable、wait()、notify_one()
		线程A：等待一个条件满足
		线程B：专门往消息队列中添加消息（数据）





	*/

	A myobj;
	thread myOutMsgobj(&A::outMsgQueue, &myobj);	//第二参数是引用，才能保证线程中使用同一个对象
	thread myInMsgobj(&A::inMsgRecvQueue, &myobj);
	myOutMsgobj.join();
	myInMsgobj.join();

	return 0;
}