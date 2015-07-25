#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<fstream>
#include<string>
#include<vector>
#include<algorithm>

using namespace std;

void display(const vector<int>& vec, const string& explain = "explain"){

    int length = vec.size();
    cout << explain <<" length: " << length<< endl;
    for(int i = 0; i < length; i++){
        cout << vec[i] << "  ";
    }
    cout << endl;

}

int getRand(int max=100){

    return rand()%max;

}

int cmp(int left, int right){

    return left >  right;
}

vector<int> get_sorted_vec(bool asc=true, int length = 10){
    vector<int> vec;
    for(int i = 0; i < length ; i++){
        vec.push_back(getRand(20));
    }

    display(vec, "初始顺序");

    if(asc){
        sort(vec.begin(), vec.end());
        display(vec, "升序排序结果");
    }
    else{
        sort(vec.begin(), vec.end(), cmp);
        display(vec, "降序排序结果");
    }

    return vec;
}

void get_insert_poi_test(){

    vector<int> vec = get_sorted_vec(false);
    for(int i = 0 ; i < 20; i++){
        vector<int>::iterator location = lower_bound(vec.begin(), vec.end(), i, cmp) ;
        cout << i << " shoule be insert: " << location - vec.begin() << endl ;
    }
}


vector<int> str2vec(const string& str)
{
    const char* p = str.c_str();
    vector<int> vec;

    while( *p != '\0'){
        unsigned char byte = *p;
        vec.push_back((int)byte);
        ++p;
    }

    return vec;
}


class KV{
    public:
        KV(int k, int v):m_k(k), m_v(v){}
    public:
        int m_k;
        int m_v;
};


vector<KV> get_kv_vec(){
    vector<KV> vec;
    for(int i = 0; i < 10; i ++){
            KV kv(getRand(), getRand());
            vec.push_back(kv);
    }
    return vec;
}
bool cmp_kv(KV kv, int k){
    return kv.m_k < k;
}

bool cmp_kv_self(KV lhs, KV rhs){
    return lhs.m_k < rhs.m_k;
}
void kv_test(){
   

    vector<KV> kv = get_kv_vec();
    sort(kv.begin(), kv.end(), cmp_kv_self);
    vector<KV>::iterator ite = kv.begin();
    for(; ite != kv.end(); ++ite){
        cout<< (*ite).m_k << ":" << (*ite).m_v << "\t";
    }
    cout << endl;

    for(int i = 1; i < 20; i++){
        vector<KV>::iterator ite = lower_bound(kv.begin(), kv.end(), i, cmp_kv);
        cout << i << " shoule be insert: " << ite - kv.begin() << endl ;
    }

}

int main(){


    srand((unsigned)time(0));
    //get_sorted_vec();
//    get_insert_poi_test();
    kv_test();
    return 0;
}
