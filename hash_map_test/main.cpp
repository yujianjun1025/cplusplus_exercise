#include <iostream>
#include <string>
#include <ext/hash_map>
using namespace std;
using namespace __gnu_cxx;
namespace __gnu_cxx
{
    template<> struct hash<string>
    {
        size_t operator()(const string& s) const
        {
            return __stl_hash_string(s.c_str());
        }
    };
}

int main(int argc, char** argv)
{

   hash_map<string, string> strHash;
   string key, value;
   cout << "please input key and value" << endl;
   while(cin >> key >> value){
       if(key=="kill"){
           cout<<"key == kill"<< endl;
            break;
       }
       cout<< "key == " << key << endl;
       cout<< "value == " << value << endl;
        strHash.insert(pair<string, string>(key, value));
   }

  hash_map<string, string>::iterator it;
  while(cin>>key){
       if(key=="kill"){
          cout<<"key == kill"<< endl;
          break;
       }
        cout << "key == " << key <<endl;
        it = strHash.find(key);
        if(it != strHash.end()){
            cout << "the first == " << it->first<< endl;
            cout << "the second == " << it->second<< endl;
        }else{
            cout << "didn't hava the key" << endl;
        }
  }

  return 0;
}

