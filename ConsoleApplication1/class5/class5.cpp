#include <iostream>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
using namespace std;

class A {
public:
	//把收到的消息（玩家命令）入到一个队列的线程
	void inMsgRecvQueue() {
		for (int i = 0; i < 10000; ++i) {
			cout << "inMsgRecvQueue()执行，插入一个元素" << i << endl;
			my_mutex.lock();	//锁住一次元素的插入
			msgRecvQueue.push_back(i);//假设数字i就是玩家发送的数据，压入队列中
			my_mutex.unlock();	//解锁
		}
		return;
	}

	bool outMsgLULProc(int &command) {
		my_mutex.lock();	//加锁，锁住一次数据的取出
		if (!msgRecvQueue.empty()) {	   
			//消息队列不为空
			int command = msgRecvQueue.front();//返回第一个元素
			msgRecvQueue.pop_front();//移除第一个元素，
			my_mutex.unlock();	//这里的unlock对应if(true)的解锁，可以想在函数退出之前必须解锁
			return true;
		}
		my_mutex.unlock();	//这里的unlock对应else的解锁，可以想在函数退出之前必须解锁
		return false;
	}

	//把数据从队列中取出的线程
	void outMsgQueue() {
		int command = 0;
		for (int i = 0; i < 10000; ++i) {
			//my_mutex.lock();	//直接锁函数的调用，会影响效率
			bool result = outMsgLULProc(command);
			//my_mutex.unlock();
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

private:
	list<int> msgRecvQueue;//容器，专门用于玩家发送过来的命令
	mutex my_mutex;
};


int main() {

	A myobj;
	thread myOutMsgobj(&A::outMsgQueue, &myobj);	//第二参数是引用，才能保证线程中使用同一个对象
	thread myInMsgobj(&A::inMsgRecvQueue, &myobj);
	myOutMsgobj.join();
	myInMsgobj.join();

	/*
	保护共享数据，操作时，用代码把共享数据锁住数据、操作数据、解锁数据，
		其他想操作共享数据的线程必须等待数据解锁、锁住数据、操作数据、解锁数据

	一：互斥量（Mutex）
	互斥量是个类对象，理解为一把锁，多个线程尝试用lock()成员函数来加锁这个锁，只有一个线程可以锁定成功（成功的标志是lock()函数返回），
		如果没锁成功，那么线程的就会卡到lock()这里不断的尝试去锁

	上锁公共数据要小心和准确，不能锁住其他不重要的数据（影响效率），也不能漏锁重要的数据


	二：互斥量的使用
	(2.1)lock() unlock()
	先lock，操作共享数据，完成之后unlock
	lock和unlock要成对使用，不能调用一次lock却调用了两次unlock，也不能调用两次lock调用一次unlock

	为了防止大家忘记unlock()，引入了lock_guard的类模板，忘记unlock不要紧，系统替你unlock();   智能指针，忘记释放替你释放
	*/

}









