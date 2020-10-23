#include <iostream>
#include <vector>
#include <thread>
using namespace std;

/*
只要用临时构造的A类对象作为参数传递给线程，那么一定能够在主线程执行结束之前把线程函数的第二个参数给构造出来，即便使用detach()也能安全的运行
*/

// 陷阱二解决方法的验证
class A {
public:
	mutable int m_i;	//nutable是使参数可修改，即便是在const类型也可以修改，见myprint3函数
	A(int a) :m_i(a) {	//类型转换构造函数，可以把一个int转换为一个类A对象
		cout << "构造函数开始执行" << this << "\t" <<"thread_id=" << this_thread::get_id() << endl;
	}
	A(const A &a) :m_i(a.m_i) {
		cout << "拷贝构造函数执行" << this << "\t" << "thread_id=" << this_thread::get_id() << endl;
	}
	~A() {
		cout << "析构函数执行" << this << "\t" << "thread_id=" << this_thread::get_id() << endl;
	}

	void thread_work(int num) {
		cout << "子线程thread_work开始执行" << this << "\t" << "thread_id=" << this_thread::get_id() << endl;
	}
	
	void operator()(int num) {
		cout << "子线程()开始执行" << this << "\t" << "thread_id=" << this_thread::get_id() << endl;
	}
};
void myprint2(const int i, const A &pmybuf) {
	//cout << i << endl;
	cout << &pmybuf << endl;
}

void myprint3(const A &pmybuf) {
	pmybuf.m_i = 199;	//在这里修改了变量m_i ，不会影响主函数 
	cout <<"子线程myprint3的参数地址是：" << &pmybuf << " thread_id=" << this_thread::get_id() << endl;
}


//传递智能指针
void myprint4(unique_ptr<int> pzn) {
	//pmybuf.m_i = 199;	//在这里修改了变量m_i ，不会影响主函数 
	//cout << "子线程myprint3的参数地址是：" << &pmybuf << " thread_id=" << this_thread::get_id() << endl;

}



// //陷阱一写法，形参是引用和指针
//void myprint(const int &i, char *pmybuf) {		//查看i的地址	0x00080004 {1}	所以这里不是真引用，而是一个复制的值 ,所以在主线程使用detach  而子线程函数中使用的i是安全的
//	cout << i << endl;			//i并不是mvar的引用，实际上是值传递	  但是推荐不要传引用
//	cout << pmybuf << endl;		//指针在detach子线程时，绝对是有问题的，因为传进来的指针和原来的局部变量是同一块地址，如果主线程结束了，局部变量的内存就会被收回，而子进程同地址的指针输出将会出现不可预知的错误
//}

// //陷阱一的解决
/*	//进行如下的修改：将i不设定为引用，其次将形参设定为const string & 类型（因为传进来的pmybuf是一个临时变量，const引用可以接收右值）
思想是将原来char类型的字符串数组转为string，此过程就会导致传进来的参数与原参数内存地址不一样，
子线程再进行编程不会因主线程退出而导致原参数内存回收后的不可预知错误
*/
void myprint(const int i, const string &pmybuf) {
	cout << i << endl; 
	cout << pmybuf.c_str() << endl;
}

int main() {
	////////////////////////////////////////////一：传递临时对象作为线程参数////////////////////////////////////////////////////////


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



	////验证陷阱二解决方法
	//int mvar1 = 1;
	//int mysecon2=12;
	////thread mytobj2(myprint2, mvar1, mysecon2);		//我们希望mysecond2转成A类型对象传递给myprint2函数		但是这里主线程执行完，转换却未执行
	//thread mytobj2(myprint2, mvar1, A(mysecon2));		//使用这种类型转换就可以在主线程退出之前完成类型转换，从而子线程得以安全执行
	//												//在创建线程的同时构造临时对象的方法传递参数是可行的
	//mytobj2.detach();
	//cout << "主线程结束" << endl;
	///*输出打印
	//	构造函数开始执行
	//	拷贝构造函数执行
	//	0051BC50
	//	析构函数执行
	//	析构函数执行
	//	主线程结束	
	//*/



	/*
	(1.3)总结
	（a）若是传递int这种简单的类型参数，建议直接使用值传递，不用使用引用，防止节外生枝
	（b）如果传递是类对象，避免隐式转换（就类似陷阱二，又如在（1.1）中将char类型转换成一个strin类型）。全部都在创建线程的地方就构造出临时对象，然后再函数参数里，用引用来接；否则会多出调用构造函数
	！！！！！！！终极结论：不使用detach(),只使用join()，这样就不存在局部变量失效导致线程对内存的非法引用问题！！！！！！
	*/

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////(2.1)线程id：每一个线程实际上都对应一个独自的id，线程id可以使用c++标准库里的函数来获取，std::this_thread::get_id()来获取
	////(2.2)临时对象捕获
	//cout << "主线程id是：" << this_thread::get_id() << endl;
	////int mvar = 1;
	////thread mytobj3(myprint3, mvar);
	////mytobj3.join();
	///*输出打印
	//主线程id是：20968
	//构造函数开始执行00CAF764        thread_id=2188				从这可看出是子线程中构造A类对象（pmybuf），还依赖于主线程的参数地址
	//子线程myprint3的参数地址是：00CAF764 thread_id=2188
	//析构函数执行00CAF764    thread_id=2188
	//*/

	//int mvar = 1;
	//thread mytobj3(myprint3, A(mvar));
	////mytobj3.join();
	///*
	//主线程id是：20440
	//构造函数开始执行00B5F954        thread_id=20440				从这可以看出使用了临时对象后，所有的A类对象都在main()函数中就已经构造完毕
	//拷贝构造函数执行003AF880        thread_id=20440
	//析构函数执行00B5F954    thread_id=子线程myprint3的参数地址是：003AF880 thread_id=23572
	//析构函数执行003AF880    20440
	//thread_id=23572
	//*/

	//mytobj3.detach();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	///////////////////////////////////////////////////传递类对象、智能指针作为线程参数////////////////////////////////////////////////////////////

	//A mytobj(10);	//生成一个类对象	地址：0x0077f968 {m_i=10 }	经过拷贝构造后主线程和子线程拥有的地址是不一致的，在子线程中进行的修改不会影响主线程的内容
	////thread mytobj4(myprint3, mytobj);	//将类对象mytobj作为线程参数		所以这里mytobj的地址还是原主线程的地址，并未改变
	////mytobj4.join();

	////在子线程函数中，无论是使用引用接收参数，还是不用引用接收，又或者是使用值接收，编译器都会为参数进行拷贝构造函数进行拷贝，从而导致在子线程中进行的修改不会造成主线程的改变
	////如果有需求在子线程中修改能够导致主线程原始参数的改变，即把主线程的参数真正引用传递到子线程中，就要使用std::ref()函数,不能用detach()函数
	//thread mytobj4(myprint3, ref(mytobj));	//这时，mytobj运行之后变成了199，被修改了，如果这里写了ref()函数，那么在A类中的参数m_i就可以不使用mutable，而且在线程函数中可以不使用const关键字
	//mytobj4.join();
	/*输出打印
	构造函数开始执行0093F9EC        thread_id=21492			因为是对原始地址进行修改，所以这里也省略了拷贝构造函数的调用
	子线程myprint3的参数地址是：0093F9EC thread_id=17604
	析构函数执行0093F9EC    thread_id=21492
	*/


	////使用智能指针进行线程的传递，但是这里必须使用join()不能使用detach()，因为传给子线程函数的指针指向的主线程的内存，一旦主线执行完毕，那么子线程就是不可预知的错误
	//unique_ptr<int> myp(new int(100));		
	//thread mytobj5(myprint4, move(myp));
	//mytobj5.join();


	////////////////////////////////////////////////////用成员函数指针做线程函数////////////////////////////////////////////////////////////////
	A mytobj7(10);
	thread mytobj6(&A::thread_work, mytobj7, 15);
	mytobj6.join();
	/*
	构造函数开始执行00D4F7B0        thread_id=1700
	拷贝构造函数执行0125B7F4        thread_id=1700
	子线程thread_work开始执行0125B7F4       thread_id=20876
	析构函数执行0125B7F4    thread_id=20876
	析构函数执行00D4F7B0    thread_id=1700
	*/


	A mytobj8(10);
	thread mytobj6(ref(mytobj8),15);	//不调用拷贝构造，之后不能用detach
	mytobj6.join();


	return 0;
}