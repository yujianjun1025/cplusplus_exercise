#include <iostream>
#include <string>
#include <vector>
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


vector<string> spliteQuery(const string& query){

    vector<string> res;
    int minSpliteLen = 4; //string("北京").length();
    int maxSpliteLen = 8; //string("上海黄浦").length();
    int strLen = query.length();
    cout << " minSpliteLen == " << minSpliteLen <<endl;
    cout << " maxSpliteLen == " << maxSpliteLen <<endl;
    cout << "strLen == " << strLen <<endl;
    for( int i = minSpliteLen; i <= maxSpliteLen; i+=2){
        for(int j = 0; j <= strLen - i; j++){
            res.push_back(query.substr(j,i));
        }
    }
    return res;
}

hash_map<string, string> init_hash_data(){

   hash_map<string, string> strHash;
   strHash.insert(pair<string, string>("北京", "beijing_"));
   strHash.insert(pair<string, string>("123",  "123"));
   strHash.insert(pair<string, string>("12北京", "123北京"));
   strHash.insert(pair<string, string>("abc北京", "abc北京"));

   return strHash;
}
int main(int argc, char** argv){
    string key;   
    
    hash_map<string, string> strHash = init_hash_data(); 
    while(cin >> key){
        if(key == "K"){
            break;
        }

        vector<string> vec = spliteQuery(key);
        cout << "key == " << key << "splite result" << endl;
        for(vector<string>::iterator it = vec.begin(); it != vec.end(); ++it){
            cout<<*it<<endl;
        }
        
        cout<<"尝试检索结果"<<endl;
        for(vector<string>::iterator it = vec.begin(); it != vec.end(); ++it){
            
            hash_map<string, string>::iterator ite = strHash.find(*it);

            if(ite != strHash.end()){
                      cout<<"find the key; key == "<< ite->first << "\tvalue ==" <<ite->second<<endl;
                }
        }
        
    }

    return 0;
}
