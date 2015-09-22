#include "array_trie.h"
#include <iostream>
#include <fstream>
using namespace std;

int main(){

    ifstream f_trip_poicity("./trip_poicity.txt"); 
    string s;
    ATrie* tmp_trip_poicity = new ATrie();

    while (getline(f_trip_poicity,s) ) { 
        vector<string> poi_city = split(s, "\t");
        if (poi_city.size() != 2) {
            cout << "数据格式错误" << endl;
            continue;
        }   
        tmp_trip_poicity->add_word(poi_city[0], poi_city[1], BIT_1, "");
    }   
    f_trip_poicity.close();

    cout << "hello world"<<endl;
    return 0;
}
