#include <iostream>
#include <string>
#include <vector>
#include <locale>
using namespace std;

vector<wstring> spliteQuery(const wstring& query){

    vector<wstring> res;
    int minSpliteLen = 2; //wstring("北京").length();
    int maxSpliteLen = 4; //wstring("上海黄浦").length();
    int strLen = query.length();
    cout << " minSpliteLen == " << minSpliteLen <<endl;
    cout << " maxSpliteLen == " << maxSpliteLen <<endl;
    cout << "strLen == " << strLen <<endl;

    for( int i = minSpliteLen; i <= maxSpliteLen; i+=2){
        for(int j = 0; j <= strLen - i; j++){
            res.push_back( wstring(query.substr(j,i).begin(), query.substr(j,i).end()));
        }
        
    }

    return res;
}
int main(int argc, char** argv){

     wcout.imbue(std::locale("chs"));
    string key;    
    while(cin >> key){
        if(key == "K"){
            break;
        }

        wstring wkey(key.begin(), key.end());
        wcout << L"输入的key为"<< wkey << endl;
    //    vector<wstring> vec = spliteQuery(key);
    //    wcout << "key == " << key << "splite result" << endl;
    //    for(vector<wstring>::iterator it = vec.begin(); it != vec.end(); ++it){
    //        wcout<<*it<<endl;
    //    }
        
    }

    return 0;
}
