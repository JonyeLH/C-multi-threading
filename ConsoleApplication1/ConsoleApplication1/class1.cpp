#include <iostream>
#include <string>
#include <vector>
#include <thread>
using namespace std;

//自己创建的线程从一个函数(初始函数)开始运行:
void myprint() {
	cout << "我的线程开始了" << endl;

	cout << "我的线程结束了" << endl;

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


	1.1、thread是一个标准库里的类
		join是让主线程阻塞，等待子线程执行完毕，然后子线程和主线程汇合，这个join函数执行完毕，主线程继续往下走
	*/

	//myprint是可调用对象
	thread mytobj(myprint);	//使用thread类创建了线程mytobi，线程的执行起点（入口）myprint();  myprint线程开始执行
	
	//阻塞主线程并等待子线程执行完毕
	mytobj.join();			

	cout << "主线程结束了" << endl;		//


	//结果输出
	/*我的线程开始了
	  我的线程结束了
	  主线程结束了
	*/
	return 0;
}