#include "./array_trie.h"
#include <iostream>
#include <fstream>
using namespace std;


void display_trie(ATrie* atrie){

    for(vector<State>::iterator ite = atrie->m_states.begin(); ite != atrie->m_states.end(); ++ite){
        cout << ite->m_aitem.m_index.dict_index <<"\t"<< ite->m_aitem.m_index.dict_flag  << endl; 
        if(ite->m_aitem.m_index.dict_flag){
            cout << ite->m_aitem.m_origin<< "@"<< ite->m_aitem.m_explain << endl;
        }
        for(vector<Path>::iterator it_path = ite->m_child.begin(); it_path  != ite->m_child.end(); ++it_path){
            cout <<"\t"<< (int)it_path->m_condition << " --> "<< it_path->m_target << endl;
        }
    }
    cout << "" << endl;

}
int main(int argc, char** args){


    string file_name = argc <= 1 ? "./trip_poicity.txt" : args[1];
    ifstream f_trip_poicity(file_name.c_str()); 
    string s;
    ATrie* tmp_trip_poicity = new ATrie();

    while (getline(f_trip_poicity,s) ) { 
        vector<string> poi_city = split(s, "\t");
        if (poi_city.size() != 2) {
            cout << "数据格式错误" << endl;
            continue;
        }   
        tmp_trip_poicity->add_word(poi_city[0], poi_city[1]); 
    }   
    f_trip_poicity.close();

    display_trie(tmp_trip_poicity);
    cout << "hello world"<<endl;
    return 0;
}
