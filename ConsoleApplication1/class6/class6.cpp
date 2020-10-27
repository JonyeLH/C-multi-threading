#include <iostream>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
using namespace std;

class A {
private:
	list<int> msgRecvQueue;
	mutex my_mutex1;	
	mutex my_mutex2;	

public:
	// Ҳ���Խ�unique_lock��Ϊ������������
	std::unique_lock<std::mutex> rnt_unnique_lock() {
		std::unique_lock<std::mutex> tmpguard(my_mutex1);
		return tmpguard;//�Ӻ�������һ���ֲ���unique_lock�����ǿ��Եġ��������־ֲ�����tmpguard�ᵼ��ϵͳ������ʱ��unique_lock���󣬲�����unique_lock���ƶ����캯��
	}

	void inMsgRecvQueue() {
		for (int i = 0; i < 10000; ++i) {
			cout << "inMsgRecvQueue()ִ�У�����һ��Ԫ��" << i << endl;
			{
				// //����Ȩ��ת�Ʒ���һ��
				//unique_lock<mutex> myguard1(my_mutex1);	//lock_guard
				//unique_lock<mutex> myguard2(myguard1);		//���������ǷǷ���
				//unique_lock<mutex> myguard2(std::move(myguard1));		//�ƶ����壬�൱��myguard1��myguard2����һ����
																		//����myguard1ָ���˿գ�myguard2ָ����my_mutex1

				// //����Ȩ��ת�Ʒ�����������unique_lock��Ϊ�����������͵ĺ�����
				//unique_lock<mutex> myguard2 = rnt_unnique_lock();


				//my_mutex1.lock();
				//unique_lock<mutex> myguard1(my_mutex1,std::adopt_lock);	//ʹ��std::adopt_lock����ǰ����뽫����������
				//msgRecvQueue.push_back(i);

				//unique_lock<mutex> myguard1(my_mutex1, std::try_to_lock);	//ʹ��std::try_to_lock����ǰ�治�ܽ����������������������������
				//if (myguard1.owns_lock()) {
				//	//�õ�����
				//	msgRecvQueue.push_back(i);
				//	//��������
				//	//��������ز���
				//}
				//else {
				//	cout << "inMsgRecvQueue()ִ�У�����û���õ�����ֻ�ܲ������" << i << endl;
				//}


				unique_lock<mutex> myguard1(my_mutex1, std::defer_lock);	
				//���defer_lockʹ�õ�һЩ��Ա����

				//lock()��unlock()
				//myguard1.lock();	//������õ���unique_lock��ģ���е�lock��������lock_guard����һ���������ֶ��Ľ���
				////����һЩ�������
				//myguard1.unlock();
				////����һЩ�ǹ������
				//myguard1.lock();
				////����һЩ�������
				////������
				//msgRecvQueue.push_back(i);
				//myguard1.unlock();	//��������

				////try_lock()
				//if (myguard1.try_lock() == true) {//��ʾ�õ�����
				//	msgRecvQueue.push_back(i);
				//	//��������
				//	//��������ز���
				//}
				//else {
				//	cout << "inMsgRecvQueue()ִ�У�����û���õ�����ֻ�ܲ������" << i << endl;							
				//}

				//release()
				std::mutex *ptr = myguard1.release();	//ͨ��unique_lock����myguard1����release�����mutex�Ĺ��������ҽ�ָ��ptr�ӹ�mutex
				msgRecvQueue.push_back(i);
				ptr->unlock();	//�ӹ�֮��Ҫ����mutex��unlock
			}
		}
		return;
	}



	bool outMsgLULProc(int &command) {	
		my_mutex1.lock();
		unique_lock<mutex> myguard1(my_mutex1,std::adopt_lock);	//ʹ�õڶ����������Ȱѻ�������lock��

		//std::chrono::milliseconds dura(10);	//1�� = 1000���룬����Ϊ20��	��������ʱ��Խ���������̵߳õ����ļ���ԽС
		//std::this_thread::sleep_for(dura);	//��Ϣһ����ʱ��

		if (!msgRecvQueue.empty()) {			
			command = msgRecvQueue.front();
			msgRecvQueue.pop_front();
			return true;
		}
		return false;
	}


	void outMsgQueue() {
		int command = 0;
		for (int i = 0; i < 10000; ++i) {
			bool result = outMsgLULProc(command);
			if (result == true) {
				cout << "outMsgQueue()ִ���ˣ�ȡ����һ��Ԫ��" << command << endl;
				//....��������
			}
			else {
				cout << "outMsgQueue()ִ�У���Ŀǰ��Ϣ����Ϊ��" << i << endl;
			}
		}
		cout << "end" << endl;
	}
};


int main() {

	A myobj;
	thread myOutMsgobj(&A::outMsgQueue, &myobj);	//�ڶ����������ã����ܱ�֤�߳���ʹ��ͬһ������
	thread myInMsgobj(&A::inMsgRecvQueue, &myobj);
	myOutMsgobj.join();
	myInMsgobj.join();

	/*
	һ��unique_lockȡ��lock_guard
		unique_lock��һ����ģ�壬�����У�һ��lock_guard(�Ƽ�ʹ��)��lock_guardȡ����mutex��lock��unlock
		unique_lock��lock_guard����Ч�ʵ͵㣬�ڴ�ռ�ö��


	����unique_lock���Դ��ĵڶ�������
		��2.1��std::adopt_lock		
			lock_guard���Դ��ڶ���������std::lock_guard<mutex> myguard1(my_mutex1, std::adopt_lock);
				adopt_lock�������ñ�ʾ����������Ѿ���lock�ˣ������Ȱѻ�����lock�ˣ�����ᱨ�쳣��
				adopt_lock��ǵ�Ч������������÷� �߳��Ѿ�ӵ���˻�����������Ȩ���Ѿ�lock�ɹ�������֪ͨlock_guard����Ҫ�ڹ��캯����lock�����������
			unique_lockҲ���Դ����������������ͬ��

		��2.2��std::try_to_lock
			����ʹ��mutex��lockȥ����mutex���������û�����ɹ���Ҳ���������أ�����������������
			�������������ǰ���ǲ�����ȥlock

		��2.3��std::defer_lock
			ʹ�����������ǰ���ǲ�����lock�������쳣
			defer_lock�����ã�����mutex��������ʼ��һ��û�����Ļ�������û����Ҳ������unique_lock������ԣ����Է���ص���һЩunique_lock��һЩ��Ա����


	��������defer_lock�Ļ��⣬����unique_lockһЩ��Ҫ��Ա����
		��3.1��lock()������,��Ӧdeferʹ��
		��3.2��unlock(),��Ȼ��unique_lock�п��Բ�ʹ��unlock()��unique_lock��ģ����Զ�����unloc���������������ǿ������ش���һЩ�ǹ�����Դ�Ĳ�������Ҳ������unique_lock�������
				�ڹ������ִ�����֮��Ҳ���Լ���unlock��ֻ���е㻭�����㣬��Ϊ��ģ�������û��unlock�����û�оͻ��Զ����ã�����еĻ����Ͳ�����á�
				��ס�Ĵ����Խ�٣�ִ��Խ�죬������������Ч��Խ�ߡ�Ҫѧ�ᾡ��ѡ����ʵĴ���������������׼ȷ����������
		��3.3��try_lock(),���Ը�����������������ò������򷵻�false������õ�����������true����������ǲ�������

		��3.4��release(),����unique_lock�������mutex����ָ�룬���ͷ�����Ȩ��Ҳ����˵���unique_lock��mutex�����й�ϵ��Ҫ�ϸ�����release �� unlock����
			 ���ԭ��mutex�����ڼ���״̬���ӹ�֮��������θ��������release���ص���ԭʼmutex��ָ�룩

	
	�ģ�unique_lock����Ȩ�Ĵ��� mutex
		lock_guard<mutex> myguard1(my_mutex1);	����Ȩ����
		myguard1ӵ��my_mutex1������Ȩ��myguard1���԰�my_mutex1������Ȩת�Ƹ����unique_lock���󣬿���ת�Ʋ��ܸ���
		����һ��std::move()
		��������std::unique_lock<std::mutex>

	*/
	


	return 0;
}









