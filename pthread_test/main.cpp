/*=============================================================================
#
# Author: jianjun.yu
#
# mail: yujianjun1025@126.com
#
# Last modified: 2015-07-17 18:46
#
# Filename: main.cpp
#
# Description: 本来想测试C++中的读写锁的，结果写了一个消费者生成者例子，
写消费者生产者不能用读写锁， 只用了写锁， 同理互斥器的实现 
#
=============================================================================*/
#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <vector>
using namespace std;


class threadSafeContainer{

    public:
        threadSafeContainer(){
            pthread_rwlock_init(&rwlock, NULL);
        }
        ~threadSafeContainer(){
             pthread_rwlock_destroy(&rwlock);
        }
    private:
        pthread_rwlock_t rwlock;
        vector<int> m_vec;
    public:
        int getValue(){

l1:
            while(m_vec.size() == 0){
                sleep(1);
                //cout << "consumer wait ()"<< endl;
                continue;
            }
            
            pthread_rwlock_wrlock(&rwlock);
            if(m_vec.size() == 0){
                pthread_rwlock_unlock(&rwlock);
                goto l1;

            }

            int tmp = m_vec[0];
            m_vec.pop_back();
            pthread_rwlock_unlock(&rwlock);
            return tmp;
        }
        
        void setValue(int value){
            
l1:
            while(m_vec.size() == 1){
                sleep(1);
                //cout << "provider wait ()" << endl;
                continue;
            }

            pthread_rwlock_wrlock(&rwlock);
            if(m_vec.size() == 1) {
                pthread_rwlock_unlock(&rwlock);
                goto l1;
            }

            m_vec.push_back(value);
            pthread_rwlock_unlock(&rwlock);
        }
};

void *read_call_back(void* p){

    threadSafeContainer* container = static_cast<threadSafeContainer*>(p);
    while(1){
        int int_value = container->getValue();  
        printf("consumer:%d\n", int_value);
        sleep(1);
    }
}

void *write_call_back(void* p){
    
    threadSafeContainer* container = static_cast<threadSafeContainer*>(p);
    static char thread_num = 0;
    char char_value = thread_num++;
    while(1){
        int int_value = static_cast<int> (char_value); 
        container->setValue(int_value); 
        printf("provider:%d\n", int_value);
     //   sleep(1);
    }
}
int main(){

    threadSafeContainer container;

    for(int i = 0; i < 20; i++){

        pthread_t  pid_r;
        pthread_create(&pid_r,NULL, read_call_back, &container);
    }
    //pthread_detach(pid_r);

    for(int i =0; i < 20; i++) {
        pthread_t pid_w;
        pthread_create(&pid_w,NULL, write_call_back, &container);
    }
   // pthread_detach(pid_w);
    cout<< "sleep " << 50 << endl;
    sleep(50);
    cout <<"over"<<endl;
    return 0;
}


