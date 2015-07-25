#include<iostream>
#include <vector>
#include <string>
#include <string.h>
using namespace std;

vector<string> split(string& str, const char* c)
{
  std::string::size_type pos1, pos2;
  pos1 = 0;
  pos2 = str.find(c);
  vector<string> res;
  string::size_type split_length = strlen(c);
   cout << "split_length == " << split_length << endl;
  while(std::string::npos != pos2)
  {
    res.push_back(str.substr(pos1, pos2-pos1));
    pos1 = pos2 + split_length;
    pos2 = str.find(c, pos1);
  }
  if(pos1 != str.length())
    res.push_back(str.substr(pos1));
  return res;

} 

int main(){

    string s;
    while(cin >> s){

        vector<string> vec = split(s, "A");
        cout << "输入str:" << s<<endl;
        cout << "分割结果:" << endl;
        for(vector<string>::iterator it =vec.begin(); it != vec.end(); ++it ){
            cout << *it <<"@";

        }    
        cout << endl;
        
    }

    return 0;

}
