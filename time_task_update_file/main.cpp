#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

typedef bool (*update_file_fun_type)(string file_name);

class update_file_util{

    public :
        static bool addUpdateFile(string file_name,  update_file_fun_type p){

            //判断文件是否存在　
            ifstream f_input(file_name.c_str());
            if(!f_input.is_open()){
                return false;
            }
            f_input.close();

            //pair<std::map< string, update_file_fun_type>::iterator,bool >  ret = g_update_file_fun.insert(pair<string, update_file_fun_type>(file_name, p));

            for(vector<pair<string, update_file_fun_type> >::iterator ite = g_update_file_fun.begin(); ite != g_update_file_fun.end(); ++ite){
                if((*ite).first == file_name){
                    return false;
                }

            }

            pthread_mutex_lock(&g_mutex);
            g_update_file_fun.push_back(make_pair<string, update_file_fun_type>(file_name, p));
            pthread_mutex_unlock(&g_mutex);

            start();

            //判断是否插入成功
            return true;
        }

    private:
        static void  start(){ 

            if(g_start_flag){
                return ;
            }

            pthread_mutex_lock(&g_mutex);

            if(g_start_flag){
                goto finally;        
            }

            pthread_t pid;
            pthread_create(&pid, NULL, run, NULL);
            pthread_detach(pid);
            g_start_flag = true;
finally:
            pthread_mutex_unlock(&g_mutex);

        }

        static void* run(void* p){
            while(1){

                cout << "g_update_file_fun size "<< g_update_file_fun.size()<<endl;
                //map<string, update_file_fun_type>::iterator ite = g_update_file_fun.begin();
                vector<pair<string, update_file_fun_type> >::iterator ite = g_update_file_fun.begin();
               
                pthread_mutex_lock(&g_mutex);

                for(; ite != g_update_file_fun.end(); ++ite){

                    //文件未发生变化，直接进行下一个文件判断
                    struct  stat st; 
                    if(!isNewVersion(ite->first, st)){
                        //         continue;
                    }

                    cout << "文件发生变化"<<endl;

                    cout << "ite->first:"<< ite->first << endl;
                    cout << "ite->second:"<< ite->second << endl;
                    //文件发生变化,更新失败，打印失败日志
                    //if(!(*ite->second)(ite->first)){
                    if(!(*ite->second)(ite->first)){
                        cout<<"更新失败"<<endl;
                        continue;
                    }

                    //更新成功， 纪录版本号
                    g_file_version[ite->first] = st.st_mtime;
                    cout <<"更新成功"<<endl;
                }

finally:
                    pthread_mutex_unlock(&g_mutex);
                sleep(1);
                }
                return NULL;
            }


            static bool isNewVersion(string fileName,  struct stat& st)
            {
                int32_t ret = stat(fileName.c_str(), &st); 
                if(ret < 0){
                    return false;
                }

                cout << "读取的文件的时间"<< st.st_mtime << endl;

                map<string, time_t>::iterator ite = g_file_version.find(fileName);
                //未纳入版本控制的新文件
                if(ite == g_file_version.end()){
                    return true;
                }
                cout <<"存入的时间戳" << ite->second<< endl; 
                // != 说明是版本发生变化
                return st.st_mtime != ite->second;
            }

            private:
            static map<string, time_t> g_file_version;
            //static map<string, update_file_fun_type> g_update_file_fun;
            static vector<pair<string, update_file_fun_type> > g_update_file_fun;
            static pthread_mutex_t g_mutex;
            volatile static bool g_start_flag ;

        };

        map<string, time_t>update_file_util::g_file_version = map<string, time_t>();
        vector<pair<string, update_file_fun_type> >  update_file_util::g_update_file_fun = vector<pair<string, update_file_fun_type> >();
        pthread_mutex_t update_file_util::g_mutex =  PTHREAD_MUTEX_INITIALIZER;
        volatile bool update_file_util::g_start_flag = false;


        class test{

            public:

                test(string file_name){
                    m_file_name = file_name;
                    cout << update_file_util::addUpdateFile("data1.txt", g_update_file_fun) << endl;
                    cout << update_file_util::addUpdateFile("data2.txt", g_update_file_fun2) << endl;
                }
            public:
                static  bool g_update_file_fun(string file_name){
                    cout<<"run g_update_file_fun"<<endl;
                    return true;
                }
                static bool g_update_file_fun2(string file_name){
                    cout << "run g_update_file_fun2" << endl;
                    return true;
                }
            private:
                string m_file_name;

        };


        int main(){


            //    test  client_test("data.txt");
            vector<pair<string, update_file_fun_type> > vec_fun_test;
            vec_fun_test.push_back(make_pair<string, update_file_fun_type>("data1.txt", test::g_update_file_fun));
            vec_fun_test.push_back(make_pair<string, update_file_fun_type>("data2.txt", test::g_update_file_fun2));

            while(1){

                for(vector<pair<string, update_file_fun_type> >::iterator ite = vec_fun_test.begin(); ite != vec_fun_test.end(); ++ite){
                    cout<<ite->first<< endl;
                    cout<<ite->second<< endl;
                    (*ite->second)(ite->first);
                }

                sleep(1);
                break;
            }

            cout << update_file_util::addUpdateFile("data1.txt", test::g_update_file_fun) << endl;
            cout << update_file_util::addUpdateFile("data2.txt", test::g_update_file_fun2) << endl;
            //update_file_util::addUpdateFile("data.txt", g_update_file_fun_1);
            while(1){
                cout << "main thread sleep 100"<< endl;
                sleep(100);
            }
            return 0;
        }
