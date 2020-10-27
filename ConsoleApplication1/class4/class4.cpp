#include <iostream>
#include <vector>
#include <list>
#include <thread>
using namespace std;


vector<int> num = { 1,2,3 };

void myprint(int num) {
	cout << "myprint线程开始执行了，线程编号=" << num << endl;

	cout << "myprint线程结束执行了，线程编号=" << num << endl;
}

class A {
public:
	//把收到的消息（玩家命令）入到一个队列的线程
	void inMsgRecvQueue() {
		for (int i = 0; i < 10000; ++i) {
			cout << "inMsgRecvQueue()执行，插入一个元素" << i << endl;
			msgRecvQueue.push_back(i);//假设数字i就是玩家发送的数据，压入队列中
		}
	}
	//把数据从队列中取出的线程
	void outMsgQueue() {
		for (int i = 0; i < 10000; ++i) {
			if (!msgRecvQueue.empty()) {
				//消息队列不为空
				int command = msgRecvQueue.front();//返回第一个元素
				msgRecvQueue.pop_front();//移除第一个元素，
				//进行处理命令。。。
			}
			else {
				cout << "outMsgQueue()执行，但目前消息队列为空" << i << endl;
			}
		}
		cout << "end" << endl;
	}

private:
	list<int> msgRecvQueue;//容器，专门用于玩家发送过来的命令
};
int main() {
	/*
	一、创建和等待多个线程
	a)：多线程执行顺序是乱的，跟操作系统内部对线程的调度机制有关
	b)：使用join的写法，主线程等待所有子线程运行结束后，最后主线程结束
	c)：把thread对象放在容器中管理，看起来像个thread对象数组，这对一次创建大量的线程进行有效的管理	
	*/

	vector <thread> mythreads;
	//创建10个线程，线程入口函数同一使用myprint函数
	for (int i = 0; i < 10; i++) {
		mythreads.push_back(thread(myprint, i));//创建10个线程,同时这10个线程已经开始执行
	}
	for (auto iter = mythreads.begin(); iter != mythreads.end(); ++iter) {
		iter->join();	//等待10个线程都返回
	}
	cout << "主线程退出" << endl; 


	/*
	二：数据共享问题分析
	（2.1）只读数据，是安全的，不需要特别的什么处理手段。直接读就可以
	
	(2.2)有读有写：2个线程写，8个线程读，如果没有特别的处理，程序肯定崩溃
	简单处理：，读的时候不能写，写的时候不能读。2个线程不能同时写，8个线程不能同时读
	写的动作分10小步；由于任务切换，导致各种诡异事情发生（崩溃）
	
	（2.3）其他案例
	数据共享：
	北京--深圳 火车 T123，10个售票窗口1 2，同时都要订 99号座位		互斥访问数据		
	*/


	/*
	三：共享数据的保护案例代码	
	网络游戏服务器。两个自己创建的线程，一个线程收集玩家命令（用一个数字代表玩家发来的命令），并把命令数据写在一个队列中。
										另外一个线程，从队列中取出玩家的命令，解析，然后执行玩家需要的动作；
	list,在频繁的按顺序插入和删除数据时效率高。vector容器随机的插入和删除数据效率高；
	准备用成员函数作为线程函数的方法来写线程
	引入 互斥量
	*/
	A myobj;
	thread myOutMsgobj(&A::outMsgQueue, &myobj);	//第二参数是引用，才能保证线程中使用同一个对象
	thread myInMsgobj(&A::inMsgRecvQueue, &myobj);	
	myOutMsgobj.join();
	myInMsgobj.join();
	//运行乱套，一个线程在写入数据，一个在读数据并进行删除数据，会导致程序内部崩溃。必须在写线程运行期间，限制读线程的执行。锁住共享数据，谁用谁锁，等数据者一直等待。



	return 0;
}