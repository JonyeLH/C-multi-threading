#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <map>
using namespace std;

class MyCAS {	//这是一个单例类
private:
	MyCAS(){}	//私有化构造函数，主函数就不能实例化类的对象

private:
	static MyCAS *m_instance;	//静态成员变量（指针）
	
public:
	static MyCAS *GetInstance() {
		if (m_instance == NULL) {
			m_instance = new MyCAS();
			static CGarhuishou cl;	//建立了清除类的静态对象cl，静态对象的生命周期是程序的退出时候，就会调用cl的析构函数，就会释放之前new的对象
		}
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

int main() {
	/*
	一：设计模式大概谈
		“设计模式”：代码的一些写法（这些写法和常规不一样）：程序灵活，维护起来方便，但是别人接管、阅读代码代码痛苦
		用“设计模式”理念写出来的代码很晦涩
		应付特别大的项目时，把开发经验、模块划分经验，总结成设计模式（根据开发需求，后有总结和整理）
		要活学活用，不要生搬硬套

	二：单例设计模式，使用的频率比较高，在整个项目中，由某个或则某些特殊的类，属于该类的对象，只创建一个，不能创建多个
		

		

	*/	


	//因为把构造函数写进类中的private，直接使用类名创建对象是失败的
	//MyCAS a1;

	MyCAS *p_a = MyCAS::GetInstance();	//创建一个对象，返回该类（MyCAS）对象的指针
	//MyCAS *p_b = MyCAS::GetInstance();	//此时创建对象失败，因为创建第一个对象时的条件是静态变量等于NULL，当创建了第一对象时，该条件不满足，所以创建失败,而返回的还是第一次创建的对象
	p_a->fun();
	MyCAS::GetInstance()->fun();	//这里的MyCAS::GetInstance()返回的是一个指针，所以可以使用 -> 调用成员函数
	 
	return 0;
}