/*
��ô�ã�ʲôʱ���ã�

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

		cout << "mythread() start" << "threadid = " << std::this_thread::get_id() << endl;//��ӡ���߳�id

		std::chrono::milliseconds dura(5000);//����һ��ʱ��
		std::this_thread::sleep_for(dura);//��Ϣʱ��
		cout << "mythread() end" << "threadid = " << std::this_thread::get_id() << endl;//��ӡ���߳�id
		return 5;
	}

};

int mythread() {
	cout << "mythread() start" << "threadid = " << std::this_thread::get_id() << endl;//��ӡ���߳�id

	std::chrono::milliseconds dura(5000);//����һ��ʱ��
	std::this_thread::sleep_for(dura);//��Ϣʱ��
	cout << "mythread() end" << "threadid = " << std::this_thread::get_id() << endl;//��ӡ���߳�id
	return 5;
}

int mythread2(int mypar) {
	cout << mypar << endl;
	cout << "mythread() start" << "threadid = " << std::this_thread::get_id() << endl;//��ӡ���߳�id

	std::chrono::milliseconds dura(5000);//����һ��ʱ��
	std::this_thread::sleep_for(dura);//��Ϣʱ��
	cout << "mythread() end" << "threadid = " << std::this_thread::get_id() << endl;//��ӡ���߳�id
	return 5;
}

vector<std::packaged_task<int(int)> > mytasks;

void mythread3(std::promise<int> &temp,int clac) {
	//��һϵ�в���
	clac++;
	clac += 10;
	std::chrono::milliseconds dura(5000);//����һ��ʱ��
	std::this_thread::sleep_for(dura);//��Ϣʱ��

	//���
	int result = clac;	//������
	temp.set_value(result);//������浽��temp������
}

void mythread4(std::future<int> &tempp) {
	auto result = tempp.get();
	cout << "mythread4 result = " << endl;
	return;
}

int main() {
	
	//һ��std::async��std::future������̨���񲢷���ֵ
	//	std::async��һ������ģ�壬��������һ���첽��������֮�󷵻�һ��std::future����std::future��һ����ģ��
	//	����һ���첽���񣺾����Զ�����һ���̲߳���ʼִ�ж�Ӧ���߳���ں���������һ��std::future����
	//		���std::future��������ͺ����߳���ں��������صĽ�����̷߳��ؽ����������ͨ��future����ĳ�Ա����get()����ȡ�����
	//		��future������������˼�����˳ƺ�std::future�ṩһ�ַ����첽��������Ļ��ƣ�����˵����������û�취���ϵõ������߳�ִ�����ʱ�������õ������future����һ��ֵ�����ֵ���ڽ���ĳ��ʱ���õ�


	// //���г���ͨ������std::future�����get�����ȴ��߳�ִ�н��������ؽ�������get���������õ�ֵ������һֱ���ŵȴ��õ�



	//cout << "main" << std::this_thread::get_id() << endl;
	//std::future<int> result = std::async(mythread);//����һ���̲߳���ʼִ��
	////std::future<int> result = std::async(mythread,paramter);	//����̺߳������в�������������д���������
	//cout << "continue:" << endl;
	//int def = 0;
	//cout << result.get() << endl;	//��������ȴ�mythread()ִ����ϣ��õ����
	//// //cout << result.get() << endl;//get()���ܵ��ö��

	////result.wait();//�ȴ��̷߳��أ����Ǳ������ؽ�����൱��join()
	//cout << "End!";



	// //�����Ա������Ϊasync()�Ĳ���
	//A a;
	//int tmppar = 12;
	//cout << "main" << std::this_thread::get_id() << endl;
	//std::future<int> result = std::async(&A::mythread, &a, tmppar);//����һ�Ǵ�����A�ĳ�Ա�������������Ƕ���a�����ã����������ã���ô�߳���ʹ�õ���ͬһ�����󣩣��������ǳ�Ա�����Ĳ���
	//cout << "continue:" << endl;
	//int def = 0;
	//cout << result.get() << endl;	//��������ȴ�mythread()ִ����ϣ��õ����
	//cout << "End!";



	////ͨ����async()����һ��std::launch����(ö������)�����ﵽһЩ�����Ŀ��
	////a��std::launch::deferred,��ʾ�߳���ں������ñ��ӳٵ�std::future��wait()��get()�������ò�ִ��	�̵߳��ӳٵ���
	//	//�����wait��getû�е��ã��̻߳���ִ����?	ûִ���̣߳�ʵ�����̶߳�û��������
	//	//����е���wait��get��Ȼ����û��ʵ�ʴ����̣߳����������߳��е��õ��߳���ں���

	////b)std::launch::async,�ڵ���async������ʱ��Ϳ�ʼ�����̣߳�����Ҫ�ȵ�����wait��get
	//	//async()������Ĭ��ʹ�õ���std::launch::async���
	//A a;
	//int tmppar = 12;
	//cout << "main" << std::this_thread::get_id() << endl;
	////std::future<int> result = std::async(std::launch::deferred, &A::mythread, &a, tmppar);//����һ�Ǵ�����A�ĳ�Ա�������������Ƕ���a�����ã����������ã���ô�߳���ʹ�õ���ͬһ�����󣩣��������ǳ�Ա�����Ĳ���
	//std::future<int> result = std::async(std::launch::async, &A::mythread, &a, tmppar);
	//cout << "continue:" << endl;
	//int def = 0;
	//cout << result.get() << endl;	//��������ȴ�mythread()ִ����ϣ��õ����
	//cout << "End!";





	//////��:std::packaged_task:������񣬰������װ����
	//////a��	��һ����ģ�壬����ģ������Ǹ��ֿɵ��ö���ͨ��std::packaged_task ���Ѹ��ֿɵ��ö����װ���������㽫����Ϊ�߳���ں���������
	//////b)	packaged_task��װ�����Ŀɵ��ö��󻹿���ֱ�ӵ��ã�

	////cout << "main" << "threadid = " << std::this_thread::get_id() << endl;
	////std::packaged_task<int(int)> mypt(mythread2);	//�Ѻ���mythreadͨ��packaged_task��װ����
	////std::thread t1(std::ref(mypt), 1);	//�߳�ֱ�ӿ�ʼִ�У��ڶ�������Ϊ�߳���ں����Ĳ���
	////t1.join();	//�ȴ��߳�ִ�����
	////std::future<int> result = mypt.get_future();	//std::future����������߳���ں����ķ��ؽ���������result����mythread���ؽ��
	////cout << result.get() << endl;
	////cout << "END" << endl;



	////�߳�������һ��lambda���ʽ
	//cout << "main" << "threadid = " << std::this_thread::get_id() << endl;
	//std::packaged_task<int(int)> mypt2([](int mypar) {
	//	cout << mypar << endl;
	//	cout << "mythread() start" << "threadid = " << std::this_thread::get_id() << endl;//��ӡ���߳�id

	//	std::chrono::milliseconds dura(5000);//����һ��ʱ��
	//	std::this_thread::sleep_for(dura);//��Ϣʱ��
	//	cout << "mythread() end" << "threadid = " << std::this_thread::get_id() << endl;//��ӡ���߳�id
	//	return 5;
	//});
	////std::thread t1(std::ref(mypt2), 1);	//�߳�ֱ�ӿ�ʼִ�У��ڶ�������Ϊ�߳���ں����Ĳ���
	////t1.join();	//�ȴ��߳�ִ�����
	////std::future<int> result = mypt2.get_future();	//std::future����������߳���ں����ķ��ؽ���������result����mythread���ؽ��
	////cout << result.get() << endl;
	////cout << "END" << endl;

	//////b��
	////mypt2(105);//ֱ�ӵ���,�൱�ں�������
	////std::future<int> result = mypt2.get_future();
	////cout << result.get() << endl;
	////cout << "END" << endl;



	////������ʹ�������洢
	//mytasks.push_back(std::move(mypt2));	//������������ʹ�����ƶ����壬���ȥ��mypt2��Ϊ��

	////ȡ��
	//std::packaged_task<int(int)> mypt3;
	//auto  iter = mytasks.begin();
	//mypt3 = std::move(*iter);	//�ƶ�����
	//mytasks.erase(iter);//ɾ����һ��Ԫ�أ������Ѿ�ʧЧ�ˣ����Ժ������벻������ʹ��iter
	//std::future<int> result = mypt2.get_future();
	//cout << result.get() << endl;



	//������std::promise,��ģ��
		//�ܹ���ĳ���߳��и�����ֵ��Ȼ������������߳��У�ȡ�����ֵ
		//�ܽ᣺ͨ��promise����һ��ֵ���ڽ���ĳ��ʱ��ͨ��һ��future�󶨵����promise�������󶨵�ֵ
	//std::promise<int> myprom;
	//std::thread t2(mythread3, std::ref(myprom), 180);
	//t2.join();
	//std::future<int> fu1 = myprom.get_future();	//promise��future�󶨣����ڻ�ȡ�̷߳���ֵ

	//auto result = fu1.get();	//getֻ�ܵ���һ��
	//cout << "result = " << result<<endl;
	//cout << "END" << endl;

	//future��������̵߳Ľ��
	std::promise<int> myprom;
	std::thread t2(mythread3, std::ref(myprom), 180);
	t2.join();
	std::future<int> fu1 = myprom.get_future();	//promise��future�󶨣����ڻ�ȡ�̷߳���ֵ
	std::thread t2(mythread4, std::ref(fu1));
	t2.join();

	return 0;
}