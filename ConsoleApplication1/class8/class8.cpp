/*README
在某个线程中，有些判断语句会不断的进行判断，效率低；因此使用条件变量进行有效的判断

一：条件变量std::condition_variable、wait()、notify_one()

二：wait()和notify_one()存在的一些问题

三：notify_all()
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
	std::condition_variable my_cond;//生成一个条件变量对象


public:
	void inMsgRecvQueue() {
		for (int i = 0; i < 10000; ++i) {
			//cout << "inMsgRecvQueue()执行，插入一个元素" << i << endl;
			{
				
				std::unique_lock<std::mutex> myguard(my_mutex1);
				cout << "inMsgRecvQueue执行，插入一个元素" << i << endl;
				msgRecvQueue.push_back(i);

				//加入outMsgQueue()正在处理一个事务，需要一段时间，而不是卡在wait那里等待你唤醒，那么此时这个notify_one()这个调用也许没有效果
				my_cond.notify_one();	//我们尝试把wait（）唤醒。执行完这行，那么outMsgQueue() 的wait()就会被唤醒，唤醒之后在做后续研究

				//。。。。其他处理函数
			}
		}
		return;
	}


	void outMsgQueue() {
		int command = 0;
		while (true) {
			unique_lock<mutex> myguard1(my_mutex1);

			//wait() 用于等待一个东西是条件变量对象的成员函数。
				//参数一是互斥量；
				//参数二是一个lambda表达式(但不一定是lambda表达式)，若lambda返回值是false，那么wait()将解锁互斥量，并堵塞到本行，堵塞到其他某个线程调用notify_one()成员函数为止；
					//若返回值是true，那么wait()直接返回，流程接着走下去
				//如果wait()没有第二个参数my_cond.wait(myguard1):那么就跟第二个参数lambda表达式返回false一样

			//当其他线程使用notify_one()后，将本wait的状态改变，wait就恢复干活：
				//a）wait不断尝试重新获取互斥量的锁，如果获取不到，流程还是会卡到这里不断地获取；如果获得到了锁（上锁），就执行b)
				//b)
					//b.1)如果wait有第二个参数（lambda），就判断lambda表达式为false，那wait又对互斥量解锁，又进入睡眠等待notify_one唤醒
					//b.2)如果lambda为true，那wait返回，流程走下去（此时互斥锁还是被锁住）。
					//b.3)如果wait没有第二个参数，则wait返回，流程走下去
			my_cond.wait(myguard1, [this] {	//一个lambda就是一个可调对象（函数）
				if (!msgRecvQueue.empty())
					return true;
				return false;
			});

			//只要流程走到这，这个互斥锁是锁住的，同时msgRecvQueue至少有一条数据
			command = msgRecvQueue.front();
			msgRecvQueue.pop_front();
			myguard1.unlock();	//unique_lock的灵活性，随时解开锁，让别的线程获得锁，以免锁的时间太长
			cout << "outMsgQueue()执行了，取出了一个元素" << command << "threadid = "<< std::this_thread::get_id()<< endl;
		}
	}
};

int main(){

	/*
	一：条件变量std::condition_variable、wait()、notify_one():只通知一个线程

	线程A：等待一个条件满足
	线程B：专门往消息队列中添加消息（数据）
	问题：在某一线程中存在着一直判断的条件，而此条件却总未能命中。例如在outMsgLULProc()中的双重锁定条件：if (!msgRecvQueue.empty())

	std::condition_variable实际上是一个和条件相关的类，说白了就是等待一个条件达成。这个类是需要和互斥量配合工作，用的时候要生成这个类的对象


	二：上述代码深入思考
		wait()和notify_one()存在的一些问题：
			a）notify_one调用后立马跳出循环与wait竞争锁，wait不见得一定能获得锁，所以消息队列中存在着多组数据未及时处理，可能需要限流操作
			b）wait释放锁之后的流程可能需要较久的处理时间， 而另外线程获得锁并调用notify_one函数，未能命中wait等待时期，导致唤醒无效
			c）程序可能会先执行notify_one函数，而没有执行wait函数，此时notify_one函数啥效果都没有


	三：notify_all():通知多个线程唤醒wait函数，但是唤醒线程竞争锁，拿到的流程继续；拿不到的还得卡着（醒着卡，不是睡着卡）
			因此notify_all与notify_one差不多意义
		


	*/

	A myobj;
	thread myOutMsgobj(&A::outMsgQueue, &myobj);	//第二参数是引用，才能保证线程中使用同一个对象
	thread myOutMsgobj2(&A::outMsgQueue, &myobj);
	thread myInMsgobj(&A::inMsgRecvQueue, &myobj);
	myOutMsgobj.join();
	myInMsgobj.join();

	return 0;
}