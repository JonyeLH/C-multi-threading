#include <iostream>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
using namespace std;

class A {
private:
	list<int> msgRecvQueue;//容器，专门用于玩家发送过来的命令
	mutex my_mutex1;	//创建了一个互斥量（一个互斥量一个锁头）
	mutex my_mutex2;	//创建了另一个互斥量（一个互斥量一个锁头）

public:
	//把收到的消息（玩家命令）入到一个队列的线程
	void inMsgRecvQueue() {
		for (int i = 0; i < 10000; ++i) {
			cout << "inMsgRecvQueue()执行，插入一个元素" << i << endl;
			{
				//lock_guard<mutex> myguard1(my_mutex1);//在这使用lock_guard，在作用域类进行锁管理。在互斥量包裹的位置之外，不能动被保护的数据，在保护区域内可以操作。													
				//lock_guard<mutex> myguard2(my_mutex2);


				//死锁的发生：线程A先锁mutex1，接着锁mutex2
				//my_mutex1.lock();	//锁住一次元素的插入，
				////。。。。。处理数据
				//my_mutex2.lock();	//实际工程中，这两个锁头不一定是连着的，他们可能需要保护不同的数据共享块


				//死锁避免方法二：使用std::lock()函数
				std::lock(my_mutex1, my_mutex2);	//使用std::lock()函数进行锁的死锁发生，后面还是要接解锁函数

				//思索避免方法三：基于方法二上省去unlock
				lock_guard<mutex> myguard1(my_mutex1, std::adopt_lock);
				lock_guard<mutex> myguard2(my_mutex2, std::adopt_lock);

				msgRecvQueue.push_back(i);//假设数字i就是玩家发送的数据，压入队列中

				//my_mutex2.unlock();	//对应的解锁操作
				//my_mutex1.unlock();	//解锁的时候无所谓先解锁哪个锁
			}
		}
		return;
	}



	bool outMsgLULProc(int &command) {
		
		//lock_guard是类模板，所以按照模板的写法声明，把my_mutex作为类模板对象myguard的形参
		//在执行lock_guard的对象函数时就会调用类模板的构造函数mutex::lock()，这个类模板对象是一个局部对象，
		//在局部作用域之后就会调用析构函数mutex::unlock()

		// //死锁的发生：线程B先锁mutex2再锁mutex1
		// //线程A是inMsgRecvQueue中先锁mutex1，再锁mutex2，死锁的发生是要与之前的线程A上锁顺序相反，先锁mutex2，再锁mutex1
		//lock_guard<mutex> myguard2(my_mutex2);
		//lock_guard<mutex> myguard1(my_mutex1);																							
		
		//my_mutex2.lock();	
		//my_mutex1.lock();	//加锁，锁住一次数据的取出

		
		//死锁避免方法一：改变上锁顺序使得两个线程上锁的顺序一致
		//my_mutex1.lock();	
		//my_mutex2.lock();	

		//死锁避免方法二：使用std::lock()函数
		std::lock(my_mutex1, my_mutex2);	//使用std::lock()函数进行锁的死锁发生，后面还是要接解锁函数
		
		//死锁避免方法三：基于方法二上省去unlock
		lock_guard<mutex> myguard1(my_mutex1, std::adopt_lock);
		lock_guard<mutex> myguard2(my_mutex2, std::adopt_lock);


		if (!msgRecvQueue.empty()) {	   
			//消息队列不为空
			command = msgRecvQueue.front();//返回第一个元素
			msgRecvQueue.pop_front();//移除第一个元素，

			//my_mutex1.unlock();	//这里的unlock对应if(true)的解锁，可以想在函数退出之前必须解锁
			//my_mutex2.unlock();

			return true;
		}

		//my_mutex1.unlock();	//这里的unlock对应else的解锁，可以想在函数退出之前必须解锁
		//my_mutex2.unlock();

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

	（2.2）lock_guard类模板，直接取代lock和unlock，即使用了lock_guard之后，就不能使用lock和unlock
	为了防止大家忘记unlock()，引入了lock_guard的类模板，忘记unlock不要紧，系统替你unlock();   智能指针，忘记释放替你释放



	三：死锁
	 C++中，比如说我有两个锁(死锁必须是要满足两个锁（互斥量）才能发生死锁)；金锁、银锁
	 两个线程A,B 
	 （1）线程A执行的时候，这个线程先把 金锁 上锁lock()成功了，然后再去lock 银锁 

	 出现上下文切换
	 （2）线程B执行了，线程先锁 银锁，因为银锁没有被锁，所以因所以可以被锁成功，线程B接着去锁 金锁

	 此时此刻，死锁发生
	 （3）线程A因为拿不到 银锁 ，流程走不下去（解锁 金锁 的代码走不下去） 
	      线程B因为拿不到 金锁 ，流程走不下去（解锁 银锁 的代码走不下去） 
	
	（3.1）死锁演示

	（3.2）死锁的一般解决方法一：
			避免死锁，上锁顺序不能互斥，要一致：线程A怎么上锁的，线程B也怎么上锁

	（3.3）方法二：std::lock()函数模板，用于处理多互斥量的场景	要记得使用unlock()
			一次锁住两个或则两个以上的互斥量（至少两个，多了不限，1个不行）
			不存在因为在多个线程中 因为锁的顺序问题导致死锁的风险的问题
			如果互斥量中有一个没锁住，他就在那里等待，等待所有互斥量都锁住，他才会往下走（返回）
			要么两个互斥量都锁住，要么都没锁上；一旦发现有没锁住的互斥量，就会把已锁住的互斥量释放，再去锁另外一个
	 
	 （3.4）基于方法二，还需要自己进行unlock，比较不人性化
			因此引出方法三：在方法二基础上添加lock_guard()函数，形参中再添加std::adopt_lock，
			std::adopt_lock是一个结构体对象，表示这个互斥量已经lock过了，不需要在lock_guard<mutex> 构造函数里面中再对mutex对象进行lock了
			在调用析构函数是正常执行，也就是不需要自己手动去unlock了
			例如：lock_guard<mutex> myguard1(my_mutex1, std::adopt_lock);

		总结：std::lock()一次锁定多个互斥量，谨慎使用

	*/
	return 0;
}









