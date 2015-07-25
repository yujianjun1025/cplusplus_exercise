#include <iostream>  
#include <string>  
#include <locale>  
using namespace std;  
  
int main()  
{  
    string s1="第一";  
    wstring s2=L"第二";  
    cout<<s1<<endl;  
    wcout.imbue(locale("chs"));  
    wcout<<s2<<endl;  
    return 0;
}  
