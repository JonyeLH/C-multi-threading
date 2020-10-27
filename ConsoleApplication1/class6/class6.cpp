#include <iostream>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
using namespace std;

class A {
private:
	list<int> msgRecvQueue;
	mutex my_mutex1;	
	mutex my_mutex2;	

public:
	// 也可以将unique_lock作为函数返回类型
	std::unique_lock<std::mutex> rnt_unnique_lock() {
		std::unique_lock<std::mutex> tmpguard(my_mutex1);
		return tmpguard;//从函数返回一个局部的unique_lock对象是可以的。返回这种局部对象tmpguard会导致系统生成临时的unique_lock对象，并调用unique_lock的移动构造函数
	}

	void inMsgRecvQueue() {
		for (int i = 0; i < 10000; ++i) {
			cout << "inMsgRecvQueue()执行，插入一个元素" << i << endl;
			{
				// //所有权的转移方法一：
				//unique_lock<mutex> myguard1(my_mutex1);	//lock_guard
				//unique_lock<mutex> myguard2(myguard1);		//这样操作是非法的
				//unique_lock<mutex> myguard2(std::move(myguard1));		//移动语义，相当于myguard1和myguard2绑定在一起了
																		//现在myguard1指向了空，myguard2指向了my_mutex1

				// //所有权的转移方法二：调用unique_lock作为函数返回类型的函数，
				//unique_lock<mutex> myguard2 = rnt_unnique_lock();


				//my_mutex1.lock();
				//unique_lock<mutex> myguard1(my_mutex1,std::adopt_lock);	//使用std::adopt_lock参数前面必须将互斥量上锁
				//msgRecvQueue.push_back(i);

				//unique_lock<mutex> myguard1(my_mutex1, std::try_to_lock);	//使用std::try_to_lock参数前面不能将互斥量上锁，否则就是上锁两次
				//if (myguard1.owns_lock()) {
				//	//拿到了锁
				//	msgRecvQueue.push_back(i);
				//	//。。。。
				//	//其他的相关操作
				//}
				//else {
				//	cout << "inMsgRecvQueue()执行，但是没有拿到锁，只能操作别的" << i << endl;
				//}


				unique_lock<mutex> myguard1(my_mutex1, std::defer_lock);	
				//配合defer_lock使用的一些成员函数

				//lock()和unlock()
				//myguard1.lock();	//这里调用的是unique_lock类模板中的lock函数，和lock_guard（）一样，不用手动的解锁
				////处理一些共享代码
				//myguard1.unlock();
				////处理一些非共享代码
				//myguard1.lock();
				////处理一些共享代码
				////。。。
				//msgRecvQueue.push_back(i);
				//myguard1.unlock();	//画蛇添足

				////try_lock()
				//if (myguard1.try_lock() == true) {//表示拿到了锁
				//	msgRecvQueue.push_back(i);
				//	//。。。。
				//	//其他的相关操作
				//}
				//else {
				//	cout << "inMsgRecvQueue()执行，但是没有拿到锁，只能操作别的" << i << endl;							
				//}

				//release()
				std::mutex *ptr = myguard1.release();	//通过unique_lock对象myguard1调用release解除与mutex的关联，并且将指针ptr接管mutex
				msgRecvQueue.push_back(i);
				ptr->unlock();	//接管之后要负责mutex的unlock
			}
		}
		return;
	}



	bool outMsgLULProc(int &command) {	
		my_mutex1.lock();
		unique_lock<mutex> myguard1(my_mutex1,std::adopt_lock);	//使用第二参数必须先把互斥量给lock了

		//std::chrono::milliseconds dura(10);	//1秒 = 1000毫秒，这里为20秒	这里锁的时间越长，其他线程得到锁的几率越小
		//std::this_thread::sleep_for(dura);	//休息一定的时常

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

	A myobj;
	thread myOutMsgobj(&A::outMsgQueue, &myobj);	//第二参数是引用，才能保证线程中使用同一个对象
	thread myInMsgobj(&A::inMsgRecvQueue, &myobj);
	myOutMsgobj.join();
	myInMsgobj.join();

	/*
	一：unique_lock取代lock_guard
		unique_lock是一个类模板，工作中，一般lock_guard(推荐使用)；lock_guard取代了mutex的lock和unlock
		unique_lock比lock_guard更灵活；效率低点，内存占用多点


	二：unique_lock可以带的第二参数：
		（2.1）std::adopt_lock		
			lock_guard可以带第二个参数：std::lock_guard<mutex> myguard1(my_mutex1, std::adopt_lock);
				adopt_lock起标记作用表示这个互斥量已经被lock了（必须先把互斥量lock了，否则会报异常）
				adopt_lock标记的效果，“假设调用方 线程已经拥有了互斥量的所有权（已经lock成功）”，通知lock_guard不需要在构造函数中lock这个互斥量了
			unique_lock也可以带这个参数，含义相同。

		（2.2）std::try_to_lock
			尝试使用mutex的lock去锁定mutex，但是如果没有锁成功，也会立即返回，并不会阻塞在那里
			用这个互斥量的前提是不能先去lock

		（2.3）std::defer_lock
			使用这个参数的前提是不能先lock，否则异常
			defer_lock的作用，不给mutex加锁：初始化一个没有锁的互斥量，没加锁也能体现unique_lock的灵活性，可以方便地调用一些unique_lock的一些成员函数


	三：借着defer_lock的话题，引出unique_lock一些重要成员函数
		（3.1）lock()，加锁,对应defer使用
		（3.2）unlock(),虽然在unique_lock中可以不使用unlock()，unique_lock类模板会自动调用unloc的析构函数。但是可以灵活地处理一些非共享资源的操作，这也是体现unique_lock的灵活性
				在共享操作执行完毕之后，也可以加上unlock，只是有点画蛇添足，因为类模板会检测有没有unlock，如果没有就会自动调用，如果有的话，就不会调用。
				锁住的代码段越少，执行越快，整个程序运行效率越高。要学会尽量选择合适的代码量进行上锁，准确上锁！！！
		（3.3）try_lock(),尝试给互斥量加锁，如果拿不到，则返回false；如果拿到了锁，返回true。这个函数是不阻塞的

		（3.4）release(),返回unique_lock所管理的mutex对象指针，并释放所有权；也就是说这个unique_lock和mutex不再有关系。要严格区分release 和 unlock区别
			 如果原来mutex对象处于加锁状态，接管之后就有责任负责解锁（release返回的是原始mutex的指针）

	
	四：unique_lock所有权的传递 mutex
		lock_guard<mutex> myguard1(my_mutex1);	所有权概念
		myguard1拥有my_mutex1的所有权，myguard1可以把my_mutex1的所有权转移给别的unique_lock对象，可以转移不能复制
		方法一：std::move()
		方法二：std::unique_lock<std::mutex>

	*/
	


	return 0;
}









