#include <iostream>
#include <vector>
#include <thread>
using namespace std;

void myprint(int num) {
	cout << "myprint�߳̿�ʼִ���ˣ��̱߳��=" << num << endl;

	cout << "myprint�߳̽���ִ���ˣ��̱߳��=" << num << endl;
}
int main() {
	//�����͵ȴ�����߳�
	//a)�����߳�ִ��˳�����ҵģ�������ϵͳ�ڲ����̵߳ĵ��Ȼ����й�
	//b)��ʹ��join��д�������̵߳ȴ��������߳����н�����������߳̽���
	//c)����thread������������й������������thread�������飬���һ�δ����������߳̽�����Ч�Ĺ���
	vector <thread> mythreads;
	//����10���̣߳��߳���ں���ͬһʹ��myprint����
	for (int i = 0; i < 10; i++) {
		mythreads.push_back(thread(myprint, i));//����10���߳�,ͬʱ��10���߳��Ѿ���ʼִ��
	}
	for (auto iter = mythreads.begin(); iter != mythreads.end(); ++iter) {
		iter->join();	//�ȴ�10���̶߳�����
	}
	cout << "���߳��˳�" << endl;

	return 0;
}