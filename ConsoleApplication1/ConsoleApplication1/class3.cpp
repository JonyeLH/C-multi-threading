#include <iostream>
#include <vector>
#include <thread>
using namespace std;


class A {
public:
	int m_i;
	A(int a) :m_i(a) {	//类型转换构造函数，可以把一个int转换为一个类A对象
		cout << "构造函数开始执行" << endl;
	}
	A(const A &a) :m_i(a.m_i) {
		cout << "拷贝构造函数执行" << endl;
	}
	~A() {
		cout << "析构函数执行" << endl;
	}
};
void myprint2(const int i, const A &pmybuf) {
	//cout << i << endl;
	cout << &pmybuf << endl;
}


//void myprint(const int &i, char *pmybuf) {		//查看i的地址	0x00080004 {1}	所以这里不是真引用，而是一个复制的值 ,所以在主线程使用detach  而子线程函数中使用的i是安全的
//	cout << i << endl;			//i并不是mvar的引用，实际上是值传递	  但是推荐不要传引用
//	cout << pmybuf << endl;		//指针在detach子线程时，绝对是有问题的，因为传进来的指针和原来的局部变量是同一块地址，如果主线程结束了，局部变量的内存就会被收回，而子进程同地址的指针输出将会出现不可预知的错误
//}

/*	//进行如下的修改：将i不设定为引用，其次将形参设定为const string & 类型（因为传进来的pmybuf是一个临时变量，const引用可以接收右值）
思想是将原来char类型的字符串数组转为string，此过程就会导致传进来的参数与原参数内存地址不一样，
子线程再进行编程不会因主线程退出而导致原参数内存回收后的不可预知错误
*/
void myprint(const int i, const string &pmybuf) {
	cout << i << endl; 
	cout << pmybuf.c_str() << endl;
}

int main() {
	////一：传递临时对象作为线程参数
	//	//(1.1)要避免的陷阱
	//int mvar = 1;	//在debug中设定断点  选中变量mvar按  shift+F9  在变量mvar前加上  &  按回车后  用于查看变量mvar地址	0x00e5f9f4 {1}
	//int &mvary = mvar;		//同理选中变量mvary按  shift+F9  在变量mvary前加上  &   用于查看变量mvary地址	观察发现两个变量地址一致	0x00e5f9f4 {1}
	//char mybuf[] = "this is a test1";

	////用函数的形式创建线程，并且给函数传入两个参数 mvar 和 mybuf 分别对应函数的两个形参
	////thread mytobj1(myprint, mvar, mybuf);	//接着在debug调试进入线程myprint()函数,查看传递进去的引用参数mvar对应i的地址，再查看第二参数mybuf对应的pmybuf地址
	////(1.2)要避免的陷阱		在解决陷阱一时，事实上可能存在，mybuf都被回收了（主线程执行完毕），系统才去把char类型的mybuf转换为string类型传递给子线程

	////陷阱二解决方法：
	//thread mytobj1(myprint, mvar, string(mybuf));	//在这里直接将mybuf转换为string对象，可以保证子线程中用肯定有效的对象
	//mytobj1.detach();
	//cout << "主线程结束" << endl;


	//验证陷阱二解决方法
	int mvar1 = 1;
	int mysecon2=12;
	thread mytobj2(myprint2, mvar1, mysecon2);		//我们希望mysecond2转成A类型对象传递给myprint2函数		这里主线程执行完，而转换未执行
	thread mytobj2(myprint2, mvar1, A(mysecon2));		//使用这种类型转换就可以在主线程退出之前完成类型转换，从而子线程得以安全执行

	mytobj2.detach();
	//cout << "主线程结束" << endl;


	return 0;
}