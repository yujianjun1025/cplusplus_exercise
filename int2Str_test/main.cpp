#include<stdio.h>
#include <iostream>
#include <map>
using namespace std; 
int main()
{
    int n = 65535;
    char t[256];
    std::string s;
 
    sprintf(t, "%d", n);
    s = t;
    cout <<"sprintf 转化结果:" << s << endl;

    map<string, string> map;
    map["a"] = n;

    cout<< "直接赋值转化结果:" << map["a"] << endl;

    return 0;
}
