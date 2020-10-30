/*	README
一：设计模式大概谈

二：C++单例设计模式实现
	构造函数放在private
	声明一个静态成员指针初始化位NULL
	根据静态成员状态在静态函数中生成单例对象
	单例类对象的删除设计，类种类


三：多线程单例类设计模式共享数据问题分析、解决方法一
	多线程存在同时访问单例对象生成
	使用互斥量和双重锁定

四：多线程单例类设计模式共享数据问题分析、解决方法二：
	使用std::call_once()
	
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <map>
using namespace std;

std::mutex resource_mutex;
std::once_flag g_flag;	//这是一个系统定义的标记

class MyCAS {	//这是一个单例类

	static void CreateInstance() {//只调用一次
		std::chrono::milliseconds dura(10000);//休息20秒观察第二个线程是否被锁住
		std::this_thread::sleep_for(dura);
		cout << "CreateInstance()执行了" << endl;
		m_instance = new MyCAS();	//这句只希望执行一次。即只生成一次对象
		static CGarhuishou cl;
	}

private:
	MyCAS(){}	//私有化构造函数，主函数就不能实例化类的对象

private:
	static MyCAS *m_instance;	//静态成员变量（指针）
	
public:
	static MyCAS *GetInstance() {
		//std::unique_lock<std::mutex> mymutex(resource_mutex);//上锁下面的判断，但是一旦有大量的线程需要调用到GetInstance这个函数，
															   //就都会卡到这，导致效率低下。因此进行下面的优化
		//if (m_instance == NULL) {
		//	m_instance = new MyCAS();	//这句只希望执行一次。即只生成一次对象
		//	static CGarhuishou cl;	//建立了清除类的静态对象cl，静态对象的生命周期是程序的退出时候，就会调用cl的析构函数，就会释放之前new的对象
		//}


		////提高效率
		////a)如果if (m_instance != NULL) 条件成立，表示m_instance已经被new了；
		////b）如果if (m_instance == NULL) 条件成立，表示m_instance没被new了
		//if (m_instance == NULL) {	//双重锁定或者叫做双重检查
		//	std::unique_lock<std::mutex> mymutex(resource_mutex);//
		//	if (m_instance == NULL) {
		//		m_instance = new MyCAS();	//这句只希望执行一次。即只生成一次对象
		//		static CGarhuishou cl;	//建立了清除类的静态对象cl，静态对象的生命周期是程序的退出时候，就会调用cl的析构函数，就会释放之前new的对象
		//	}
		//}



		//这里可以把g_flag看成一个锁
		std::call_once(g_flag,CreateInstance);	//假设两个线程同时执行到这里，其中第一个线程成功调用call_once（）后，
												//另外一个要等待第一个线程执行完毕后返回的标记（g_flag）在决定是否要再次执行
		cout << "call_once()执行完毕" << endl;

		return m_instance;
	}

	class CGarhuishou {	//类中套类，用来释放先前new的对象
	public:
		CGarhuishou() {
			if (MyCAS::m_instance) {
				delete MyCAS::m_instance;
				MyCAS::m_instance = NULL;
			}
		}
	};

	void fun() {
		cout << "测试" << endl;
	}

};

//类静态变量的初始化
MyCAS *MyCAS::m_instance = NULL;

//线程入口函数
void mythread() {
	cout << "我的线程开始执行了" << endl;
	MyCAS *p_a = MyCAS::GetInstance();
	cout << "我的线程执行完毕了" << endl;
	return;
}

int main() {
	/*
	一：设计模式大概谈
		“设计模式”：代码的一些写法（这些写法和常规不一样）：程序灵活，维护起来方便，但是别人接管、阅读代码代码痛苦
		用“设计模式”理念写出来的代码很晦涩
		应付特别大的项目时，把开发经验、模块划分经验，总结成设计模式（根据开发需求，后有总结和整理）
		要活学活用，不要生搬硬套

	二：单例设计模式，使用的频率比较高，在整个项目中，由某个或则某些特殊的类，属于该类的对象，只创建一个，不能创建多个
		


				当有多线程同时调用单例对象生成时，问题的分析和解决，最好还是使用三方法
	三：单例类设计模式共享数据问题分析、解决	
			面临的问题：需要在自己创建的线程（非主线程）中创建MyCAS这个单例类的对象，这种线程可能不止一个。
			可能面临GetInstance（）函数进行互斥
			使用互斥量和双重锁定来解决

	四：std::call_once()	C++11引入的函数，该函数的第二个参数 是一个函数名a()
		功能：能够保证函数a 只调用一次
		具备互斥量的能力，而且在效率上，比互斥量消耗资源更少。在多线程中使用call_once()函数，只调用一次某一个函数
		call_once()需要与一个标记结合使用，这个标记是std::once_flag  其实once_flag是一个结构。
		call_once()就是通过标记决定函数a是否执行，调用call_once()成功后，call_once()就把这个标记设置为一种已调用的状态，那么函数a就不会被再执行了
		
		输出：
		我的线程开始执行了
		我的线程开始执行了
		CreateInstance()执行了	//第一个线程执行了
		call_once()执行完毕
		我的线程执行完毕了
		
		//第二个线程等待了10s。。。。/

		call_once()执行完毕
		我的线程执行完毕了
		请按任意键继续. . .
	*/	


	////最好还是在主线程中把单例对象创建出来，然后进行初始化等操作
	////因为把构造函数写进类中的private，直接使用类名创建对象是失败的
	////MyCAS a1;

	//MyCAS *p_a = MyCAS::GetInstance();	//创建一个对象，返回该类（MyCAS）对象的指针
	////MyCAS *p_b = MyCAS::GetInstance();	//此时创建对象失败，因为创建第一个对象时的条件是静态变量等于NULL，当创建了第一对象时，该条件不满足，所以创建失败,而返回的还是第一次创建的对象
	//p_a->fun();
	//MyCAS::GetInstance()->fun();	//这里的MyCAS::GetInstance()返回的是一个指针，所以可以使用 -> 调用成员函数
	

	////最好还是在主线程中把单例对象创建出来，然后进行初始化等操作，再创建其他线程进行调用
	////两个线程是同一个入口函数，但是这是两个线程，意味着这会有两个流程同时执行函数，就会出现问题：两个线程同时执行到单例类中生成对象判断语句
	//std::thread mytobj1(mythread);
	//std::thread mytobj2(mythread);
	//mytobj1.join();
	//mytobj2.join();

	return 0;
}