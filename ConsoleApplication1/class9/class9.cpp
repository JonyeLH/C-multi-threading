/*
怎么用，什么时候用：

*/

#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <map>
#include <future>
using namespace std;

class A {


public:
	int mythread(int mypar) {

		cout << "mythread() start" << "threadid = " << std::this_thread::get_id() << endl;//打印新线程id

		std::chrono::milliseconds dura(5000);//定义一段时间
		std::this_thread::sleep_for(dura);//休息时长
		cout << "mythread() end" << "threadid = " << std::this_thread::get_id() << endl;//打印新线程id
		return 5;
	}

};

int mythread() {
	cout << "mythread() start" << "threadid = " << std::this_thread::get_id() << endl;//打印新线程id

	std::chrono::milliseconds dura(5000);//定义一段时间
	std::this_thread::sleep_for(dura);//休息时长
	cout << "mythread() end" << "threadid = " << std::this_thread::get_id() << endl;//打印新线程id
	return 5;
}

int mythread2(int mypar) {
	cout << mypar << endl;
	cout << "mythread() start" << "threadid = " << std::this_thread::get_id() << endl;//打印新线程id

	std::chrono::milliseconds dura(5000);//定义一段时间
	std::this_thread::sleep_for(dura);//休息时长
	cout << "mythread() end" << "threadid = " << std::this_thread::get_id() << endl;//打印新线程id
	return 5;
}

vector<std::packaged_task<int(int)> > mytasks;

void mythread3(std::promise<int> &temp,int clac) {
	//做一系列操作
	clac++;
	clac += 10;
	std::chrono::milliseconds dura(5000);//定义一段时间
	std::this_thread::sleep_for(dura);//休息时长

	//结果
	int result = clac;	//保存结果
	temp.set_value(result);//结果保存到了temp对象中
}

void mythread4(std::future<int> &tempp) {
	auto result = tempp.get();
	cout << "mythread4 result = " << endl;
	return;
}

int main() {
	
	//一：std::async、std::future创建后台任务并返回值
	//	std::async是一个函数模板，用来启动一个异步任务，启动之后返回一个std::future对象，std::future是一个类模板
	//	启动一个异步任务：就是自动创建一个线程并开始执行对应的线程入口函数，返回一个std::future对象
	//		这个std::future对象里面就含有线程入口函数锁返回的结果（线程返回结果），可以通过future对象的成员函数get()来获取结果；
	//		“future”：将来的意思，有人称呼std::future提供一种访问异步操作结果的机制，就是说这个结果可能没办法马上得到，在线程执行完毕时，就能拿到结果，future保存一个值，这个值是在将来某个时刻拿到


	// //下列程序通过所提std::future对象的get函数等待线程执行结束并返回结果，这个get函数必须拿到值，否则一直卡着等待拿到



	//cout << "main" << std::this_thread::get_id() << endl;
	//std::future<int> result = std::async(mythread);//创建一个线程并开始执行
	////std::future<int> result = std::async(mythread,paramter);	//如果线程函数含有参数，则是这样写，传入参数
	//cout << "continue:" << endl;
	//int def = 0;
	//cout << result.get() << endl;	//卡到这里等待mythread()执行完毕，拿到结果
	//// //cout << result.get() << endl;//get()不能调用多次

	////result.wait();//等待线程返回，但是本身不返回结果，相当于join()
	//cout << "End!";



	// //用类成员函数作为async()的参数
	//A a;
	//int tmppar = 12;
	//cout << "main" << std::this_thread::get_id() << endl;
	//std::future<int> result = std::async(&A::mythread, &a, tmppar);//参数一是传入类A的成员函数，参数二是对象a的引用（必须是引用，那么线程里使用的是同一个对象），参数三是成员函数的参数
	//cout << "continue:" << endl;
	//int def = 0;
	//cout << result.get() << endl;	//卡到这里等待mythread()执行完毕，拿到结果
	//cout << "End!";



	////通过向async()传递一个std::launch参数(枚举类型)，来达到一些特殊的目标
	////a）std::launch::deferred,表示线程入口函数调用被延迟到std::future的wait()和get()函数调用才执行	线程的延迟调用
	//	//那如果wait或get没有调用，线程还会执行吗?	没执行线程，实际上线程都没创建出来
	//	//如果有调用wait或get，然而并没有实际创建线程，而是在主线程中调用的线程入口函数

	////b)std::launch::async,在调用async函数的时候就开始创建线程，不需要等到调用wait或get
	//	//async()函数，默认使用的是std::launch::async标记
	//A a;
	//int tmppar = 12;
	//cout << "main" << std::this_thread::get_id() << endl;
	////std::future<int> result = std::async(std::launch::deferred, &A::mythread, &a, tmppar);//参数一是传入类A的成员函数，参数二是对象a的引用（必须是引用，那么线程里使用的是同一个对象），参数三是成员函数的参数
	//std::future<int> result = std::async(std::launch::async, &A::mythread, &a, tmppar);
	//cout << "continue:" << endl;
	//int def = 0;
	//cout << result.get() << endl;	//卡到这里等待mythread()执行完毕，拿到结果
	//cout << "End!";





	//////二:std::packaged_task:打包任务，把任务包装起来
	//////a）	是一个类模板，它的模板参数是各种可调用对象；通过std::packaged_task 来把各种可调用对象包装起来，方便将来作为线程入口函数来调用
	//////b)	packaged_task包装起来的可调用对象还可以直接调用，

	////cout << "main" << "threadid = " << std::this_thread::get_id() << endl;
	////std::packaged_task<int(int)> mypt(mythread2);	//把函数mythread通过packaged_task包装起来
	////std::thread t1(std::ref(mypt), 1);	//线程直接开始执行，第二参数作为线程入口函数的参数
	////t1.join();	//等待线程执行完毕
	////std::future<int> result = mypt.get_future();	//std::future对象里包含线程入口函数的返回结果，这里的result保存mythread返回结果
	////cout << result.get() << endl;
	////cout << "END" << endl;



	////线程里面是一个lambda表达式
	//cout << "main" << "threadid = " << std::this_thread::get_id() << endl;
	//std::packaged_task<int(int)> mypt2([](int mypar) {
	//	cout << mypar << endl;
	//	cout << "mythread() start" << "threadid = " << std::this_thread::get_id() << endl;//打印新线程id

	//	std::chrono::milliseconds dura(5000);//定义一段时间
	//	std::this_thread::sleep_for(dura);//休息时长
	//	cout << "mythread() end" << "threadid = " << std::this_thread::get_id() << endl;//打印新线程id
	//	return 5;
	//});
	////std::thread t1(std::ref(mypt2), 1);	//线程直接开始执行，第二参数作为线程入口函数的参数
	////t1.join();	//等待线程执行完毕
	////std::future<int> result = mypt2.get_future();	//std::future对象里包含线程入口函数的返回结果，这里的result保存mythread返回结果
	////cout << result.get() << endl;
	////cout << "END" << endl;

	//////b）
	////mypt2(105);//直接调用,相当于函数调用
	////std::future<int> result = mypt2.get_future();
	////cout << result.get() << endl;
	////cout << "END" << endl;



	////还可以使用容器存储
	//mytasks.push_back(std::move(mypt2));	//入容器，这里使用了移动语义，入进去后mypt2就为空

	////取出
	//std::packaged_task<int(int)> mypt3;
	//auto  iter = mytasks.begin();
	//mypt3 = std::move(*iter);	//移动语义
	//mytasks.erase(iter);//删除第一个元素，迭代已经失效了，所以后续代码不可以再使用iter
	//std::future<int> result = mypt2.get_future();
	//cout << result.get() << endl;



	//三：：std::promise,类模板
		//能够在某个线程中给他赋值，然后可以在其他线程中，取出这个值
		//总结：通过promise保存一个值，在将来某个时刻通过一个future绑定到这个promise获得这个绑定的值
	//std::promise<int> myprom;
	//std::thread t2(mythread3, std::ref(myprom), 180);
	//t2.join();
	//std::future<int> fu1 = myprom.get_future();	//promise和future绑定，用于获取线程返回值

	//auto result = fu1.get();	//get只能调用一次
	//cout << "result = " << result<<endl;
	//cout << "END" << endl;

	//future获得其他线程的结果
	std::promise<int> myprom;
	std::thread t2(mythread3, std::ref(myprom), 180);
	t2.join();
	std::future<int> fu1 = myprom.get_future();	//promise和future绑定，用于获取线程返回值
	std::thread t2(mythread4, std::ref(fu1));
	t2.join();

	return 0;
}