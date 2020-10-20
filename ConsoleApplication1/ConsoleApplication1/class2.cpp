#include <iostream>
#include <string>
#include <vector>
#include <thread>
using namespace std;


class TA {

public:

	//void operator() () {	//仿函数   也可以叫做重载() 不带参数
	//	cout << "我的子线程2开始了" << endl;
	//	cout << "我的子线程2结束了" << endl;
	//}


	//如果这里传参是引用，使用detach则会出现不可预料的结果。因为这里传进来的引用是主函数（主线程）中的一个局部变量，
	//在主线程结束时，局部变量内存就会被系统回收，所以在子线程中继续使用这个局部变量的引用就会出现不可预知的结果。
	//int &m_i;	
	//TA(int &i) :m_i(i) {
	//	cout << "TA()构造函数被执行" << endl;
	//}
	//TA(const TA &ta) :m_i(ta.m_i) {
	//	cout << "TA()拷贝构造函数被执行" << endl;
	//}
	//~TA() {
	//	cout << "TA()析构函数被执行" << endl;
	//}

	/*
	所以以下参数中不要使用引用，就不会出现问题，都是之前复制进来的值
	*/
	int m_i;
	TA(int i) :m_i(i) {
		cout << "TA()构造函数被执行" << endl;
	}
	TA(const TA &ta) :m_i(ta.m_i) {
		cout << "TA()拷贝构造函数被执行" << endl;
	}
	~TA() {
		cout << "TA()析构函数被执行" << endl;
	}
	void operator() () {	//仿函数   也可以叫做重载() 不带参数
		cout << "m_i1的值为：" << m_i << endl;
		cout << "m_i2的值为：" << m_i << endl;
		cout << "m_i3的值为：" << m_i << endl;
		cout << "m_i4的值为：" << m_i << endl;
		cout << "m_i5的值为：" << m_i << endl;
	}
};




//自己创建的线程从一个函数(初始函数)开始运行:
void myprint() {
	cout << "我的子线程1开始了" << endl;
	cout << "我的子线程1结束了" << endl;

	// //detach test
	//cout << "我的线程结束了1" << endl;
	//cout << "我的线程结束了2" << endl;
	//cout << "我的线程结束了3" << endl;
	//cout << "我的线程结束了4" << endl;
	//cout << "我的线程结束了5" << endl; 
	//cout << "我的线程结束了6" << endl; 
	//cout << "我的线程结束了7" << endl;
	//cout << "我的线程结束了8" << endl;
	//cout << "我的线程结束了9" << endl;
}


int main() {
	
	/*范例演示线程运行和结束
	主线程从main()开始执行，那么我们自己创建的线程，也需要从一个函数开始运行（初始函数）。一旦这个函数运行完毕，就代表着这个线程运行结束
	
	整个进程时候执行完毕的标志是：主线程是否执行完，如果主线程执行完毕了，就代表着整个进程执行完毕了
	此时，一般情况，如果还有其他子线程还没有执行完毕，那么这些子线程也会被操作系统强行终止。
	所以，一般情况下，如果想保持子线程（自己用代码创建的线程）的运行状态，那么要让主线程一直保持，不要让主线程运行结束
	这个规律有例外!!!

	包含头文件thread
	初始函数
	主函数中编写代码

	两个线程在跑，是相当于程序有两条线在同时走，即是有一个线程阻塞了，另外一条线程还会继续运行




	1.1、若干个线程API
		thread:是一个标准库里的类，用于生成线程对象

		join():是让主线程阻塞，等待子线程执行完毕，然后子线程和主线程汇合，这个join函数执行完毕，主线程继续往下走

		detach():分离，主线程不和子线程不汇合，各自分开执行。但是最好还是让主线程等待所有子线程结束，这样更安全。
				一旦detach之后，与这个主线程关联的thread对象（子线程）就会失去与这个主线程的关联，此时这个子线程就会驻留在后台运行（被C++系统接管），
				当子线程运行结束后，由运行时库负责清理该线程的相关资源（守护线程）
				detach会使myprint失去控制
				一旦调用了detach就不能使用join，否则异常

		joinable():判断是否成功使用join或则detach，返回true（可以join或者detach） 或者 false（不可以join或者detach）	在使用join或则detach之前使用joinable（）
	


	！！！！！！！总之：能不用detach就不要用！！！！！！！

	*/


	// // 创建线程方法一：使用thread类创建线程
	// //myprint()函数是可调用对象
	//thread mytobj(myprint);	//使用thread类创建了线程mytobi，线程的执行起点（入口）myprint()函数;  myprint线程开始执行	
	//
	//if (mytobj.joinable()) {
	//	cout << "1:joinable()==true" << endl;
	//}
	//else {
	//	cout << "1:joinable()==false" << endl;
	//}

	////阻塞主线程并等待子线程执行完毕
	//mytobj.join();			
	//cout << "主线程结束了" << endl;		
	/////结果输出//////
	///*我的线程开始了
	//  我的线程结束了
	//  主线程结束了
	//*/

	//if (mytobj.joinable()) {
	//	cout << "2:joinable()==true" << endl;
	//}
	//else {
	//	cout << "2:joinable()==false" << endl;
	//}

	//mytobj.detach();	//detach调用后随意输出，一旦主线程结束，整个程序结束
	//cout << "main thread finish1" << endl;
	//cout << "main thread finish2" << endl;
	//cout << "main thread finish3" << endl;
	//cout << "main thread finish4" << endl;
	//cout << "main thread finish5" << endl;






	// //创建线程方法二：使用类，对象完成线程创建方法
	//TA ta;
	//thread mytobj3(ta);		//ta是可调用对象
	//mytobj3.join();

	//cout << "主线程结束了" << endl;
	/////结果输出//////
	///*我的线程开始了
	//  我的线程结束了
	//  主线程结束了
	//*/

	
	//int myi = 6;
	//TA ta(myi);
	//thread mytobj3(ta);		//ta是可调用对象
	////这里的对象实际上是被  复制  到线程中去，所以执行完主线程后，ta会被销毁，但是所复制的TA对象（ta）依旧存在，
	//mytobj3.join();
	////mytobj3.detach();
	//cout << "主线程结束" << endl;
	////////输出结果/////////
	/*
	TA()构造函数被执行				TA生成ta时调用构造函数
	TA()拷贝构造函数被执行			生成线程时，对象ta作为参数调用的拷贝构造函数，就是被复制到线程中的对象
	m_i1的值为：6
	m_i2的值为：6
	m_i3的值为：6
	m_i4的值为：6
	m_i5的值为：6
	TA()析构函数被执行				被复制进程中对象调用的析构函数
	主线程结束
	TA()析构函数被执行				主线程结束，对象ta生命周期结束调用的析构函数
	*/




	//创建线程方法三：lambda表达式
	auto mylambda_thread = [] {
		cout << "我的子线程3开始执行" << endl;

		cout << "我的子线程3结束" << endl;
	};

	thread mytobj4(mylambda_thread);
	mytobj4.join();
	cout << "主线程结束" << endl;

	return 0;
}