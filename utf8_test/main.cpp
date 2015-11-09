#include<iostream>
#include<string>
using namespace std;

int main(){
    
    string line;
    while(cin >> line){
        
        int str_len = line.size();

        const char* p = line.c_str();
        for(int i =0; i < str_len; i++){
        
            unsigned char c = *p++ ;
            cout << (int)c << " ";
        }

        cout << endl;
    }
}
