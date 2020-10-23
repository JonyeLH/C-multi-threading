#include <iostream>
#include <vector>
#include <thread>
using namespace std;

void myprint(int num) {
	cout << "myprint线程开始执行了，线程编号=" << num << endl;

	cout << "myprint线程结束执行了，线程编号=" << num << endl;
}
int main() {
	//创建和等待多个线程
	//a)：多线程执行顺序是乱的，跟操作系统内部对线程的调度机制有关
	//b)：使用join的写法，主线程等待所有子线程运行结束后，最后主线程结束
	//c)：把thread对象放在容器中管理，看起来像个thread对象数组，这对一次创建大量的线程进行有效的管理
	vector <thread> mythreads;
	//创建10个线程，线程入口函数同一使用myprint函数
	for (int i = 0; i < 10; i++) {
		mythreads.push_back(thread(myprint, i));//创建10个线程,同时这10个线程已经开始执行
	}
	for (auto iter = mythreads.begin(); iter != mythreads.end(); ++iter) {
		iter->join();	//等待10个线程都返回
	}
	cout << "主线程退出" << endl;

	return 0;
}