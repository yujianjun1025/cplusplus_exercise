
#include<fstream>
#include <iostream>

using namespace std;


void  read(string file_name){
    ifstream f_slogan(file_name.c_str());
    if(!f_slogan.is_open()){
        cout <<"ERROR Open file failed"<< endl;
        return ;
    }   

    cout << "成功打开文件" << endl;


    string s, t;
    while(f_slogan >> s >> t){ 
        cout << "读入数据内容s == "<< s << "t == " << t << endl;
        //  (*tmp)[s] = t;
        //      }   
   }
}


int main(){
        
    read("./data/slogan.txt");

    return 0;
}


