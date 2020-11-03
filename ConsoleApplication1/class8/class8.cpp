/*README
��ĳ���߳��У���Щ�ж����᲻�ϵĽ����жϣ�Ч�ʵͣ����ʹ����������������Ч���ж�

һ����������std::condition_variable��wait()��notify_one()

����wait()��notify_one()���ڵ�һЩ����

����notify_all()
*/


#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <map>
using namespace std;

class A {
private:
	list<int> msgRecvQueue;
	mutex my_mutex1;
	mutex my_mutex2;
	std::condition_variable my_cond;//����һ��������������


public:
	void inMsgRecvQueue() {
		for (int i = 0; i < 10000; ++i) {
			//cout << "inMsgRecvQueue()ִ�У�����һ��Ԫ��" << i << endl;
			{
				
				std::unique_lock<std::mutex> myguard(my_mutex1);
				cout << "inMsgRecvQueueִ�У�����һ��Ԫ��" << i << endl;
				msgRecvQueue.push_back(i);

				//����outMsgQueue()���ڴ���һ��������Ҫһ��ʱ�䣬�����ǿ���wait����ȴ��㻽�ѣ���ô��ʱ���notify_one()�������Ҳ��û��Ч��
				my_cond.notify_one();	//���ǳ��԰�wait�������ѡ�ִ�������У���ôoutMsgQueue() ��wait()�ͻᱻ���ѣ�����֮�����������о�

				//������������������
			}
		}
		return;
	}


	void outMsgQueue() {
		int command = 0;
		while (true) {
			unique_lock<mutex> myguard1(my_mutex1);

			//wait() ���ڵȴ�һ��������������������ĳ�Ա������
				//����һ�ǻ�������
				//��������һ��lambda���ʽ(����һ����lambda���ʽ)����lambda����ֵ��false����ôwait()�������������������������У�����������ĳ���̵߳���notify_one()��Ա����Ϊֹ��
					//������ֵ��true����ôwait()ֱ�ӷ��أ����̽�������ȥ
				//���wait()û�еڶ�������my_cond.wait(myguard1):��ô�͸��ڶ�������lambda���ʽ����falseһ��

			//�������߳�ʹ��notify_one()�󣬽���wait��״̬�ı䣬wait�ͻָ��ɻ
				//a��wait���ϳ������»�ȡ�����������������ȡ���������̻��ǻῨ�����ﲻ�ϵػ�ȡ�������õ�����������������ִ��b)
				//b)
					//b.1)���wait�еڶ���������lambda�������ж�lambda���ʽΪfalse����wait�ֶԻ������������ֽ���˯�ߵȴ�notify_one����
					//b.2)���lambdaΪtrue����wait���أ���������ȥ����ʱ���������Ǳ���ס����
					//b.3)���waitû�еڶ�����������wait���أ���������ȥ
			my_cond.wait(myguard1, [this] {	//һ��lambda����һ���ɵ����󣨺�����
				if (!msgRecvQueue.empty())
					return true;
				return false;
			});

			//ֻҪ�����ߵ��⣬�������������ס�ģ�ͬʱmsgRecvQueue������һ������
			command = msgRecvQueue.front();
			msgRecvQueue.pop_front();
			myguard1.unlock();	//unique_lock������ԣ���ʱ�⿪�����ñ���̻߳��������������ʱ��̫��
			cout << "outMsgQueue()ִ���ˣ�ȡ����һ��Ԫ��" << command << "threadid = "<< std::this_thread::get_id()<< endl;
		}
	}
};

int main(){

	/*
	һ����������std::condition_variable��wait()��notify_one():ֻ֪ͨһ���߳�

	�߳�A���ȴ�һ����������
	�߳�B��ר������Ϣ�����������Ϣ�����ݣ�
	���⣺��ĳһ�߳��д�����һֱ�жϵ���������������ȴ��δ�����С�������outMsgLULProc()�е�˫������������if (!msgRecvQueue.empty())

	std::condition_variableʵ������һ����������ص��࣬˵���˾��ǵȴ�һ��������ɡ����������Ҫ�ͻ�������Ϲ������õ�ʱ��Ҫ���������Ķ���


	����������������˼��
		wait()��notify_one()���ڵ�һЩ���⣺
			a��notify_one���ú���������ѭ����wait��������wait������һ���ܻ������������Ϣ�����д����Ŷ�������δ��ʱ����������Ҫ��������
			b��wait�ͷ���֮������̿�����Ҫ�ϾõĴ���ʱ�䣬 �������̻߳����������notify_one������δ������wait�ȴ�ʱ�ڣ����»�����Ч
			c��������ܻ���ִ��notify_one��������û��ִ��wait��������ʱnotify_one����ɶЧ����û��


	����notify_all():֪ͨ����̻߳���wait���������ǻ����߳̾��������õ������̼������ò����Ļ��ÿ��ţ����ſ�������˯�ſ���
			���notify_all��notify_one�������
		


	*/

	A myobj;
	thread myOutMsgobj(&A::outMsgQueue, &myobj);	//�ڶ����������ã����ܱ�֤�߳���ʹ��ͬһ������
	thread myOutMsgobj2(&A::outMsgQueue, &myobj);
	thread myInMsgobj(&A::inMsgRecvQueue, &myobj);
	myOutMsgobj.join();
	myInMsgobj.join();

	return 0;
}