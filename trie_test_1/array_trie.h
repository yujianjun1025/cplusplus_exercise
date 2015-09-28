#ifndef __ARRAY_TIRE
#define __ARRAY_TIRE
#include <vector>
#include <map>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
using namespace std;


typedef char CONDITION;
typedef struct _path{
    CONDITION m_condition;
    int m_target;
}Path;

typedef struct _index{
    uint32_t dict_flag:1; //为0 表示不是终止状态， 1代表是终止状态
    uint32_t dict_type:9; //标志字典类型
    uint32_t dict_index:22;//对应在存储所有状态的数组中的索引位置
}Index;


typedef struct _item{
    string m_origin;
    string m_explain;
    Index m_index;
}AItem;

//因为要考虑到今后一个词语有不同的字典类型， 比如凤凰岭既是景点又是商家
typedef enum _dict_type{
    BIT_1 = 1,
    BIT_2 = 2,
    BIT_3 = 3,
    BIT_4 = 4,
    BIT_5 = 5,
    BIT_6 = 6,
    BIT_7 = 7,
    BIT_8 = 8,
    BIT_9 = 9 
}DType;

const static unsigned int DTypeValue[] = {
    0x01 << 0,
    0x01 << 1,
    0x01 << 2,
    0x01 << 3,
    0x01 << 4,
    0x01 << 5,
    0x01 << 6,
    0x01 << 7,
    0x01 << 8
};

const static string DTypeName[] = { 
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9"
};

inline vector<string> split(string& str, const char* c)                                                                                                                              {
    vector<string> res;
    std::string::size_type pos1 = 0;
    std::string::size_type pos2 = str.find(c);
    string::size_type split_length = strlen(c);
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

class State{
    private:
        State(AItem item):m_aitem(item){
        } 

        //为一个状态添加一条路径
        bool add_one_path(CONDITION c, int target);
        //给定一个条件返回下一个跳转状态， －1 代表 没有这条路径
        int getNextState(CONDITION condition)const;

    public:
        //有序的纪录所有的状态跳转路径
        vector<Path> m_child;
        //保存终止状态对应的在词典中的意思, 以及在ATrie中对应的位置
        AItem m_aitem;
        friend class ATrie;
};

string dtypeValue2code(unsigned int dtype_value);

class ATrie{
    public:
        ATrie(){

            AItem item;
            item.m_index.dict_flag = 0;
            item.m_index.dict_type = BIT_1;
            item.m_index.dict_index = 0;
            State root_state(item);
            m_states.push_back(root_state);

            m_max_word_len = -1; 
            m_min_word_len = 1000000;
        }

        //将一个word加入字典, explain代表word对应的意义
        bool add_word(const string& word, const string& explain, DType dict_type = BIT_1,const string& origin = "");
        //字典中的词完全匹配query词
        //返回结果：该query词在字典中的意义，空串代表字典中没有改词
        AItem equal_search(const string& query)const;
        //字典中的词完全匹配query的前缀，比如字典中大饭店对应北京， 大饭店.*也能返回北京
        //黑龙潭对应北京， 黑龙潭公园对应大理， 这种情况需要找到最大的前缀匹配
        //返回结果：该query词在字典中的意义，空串代表字典中没有改词
        AItem prefix_search(const string& query)const;
        //query词中办包含字典中词
        //例如字典中含有北京对应city1， 那么query 为.*北京.*能返回city1
        //query 词为.*云南大理.*能在匹配字典中一条数据，.*云南.*也能匹配到一条数据， 这种情况需要找到最大的匹配
        AItem contain_search(const string& query)const;
        //将query词分成字典中的词语,不会去除杂词再匹配
        vector<AItem> split_item(const string& query) const;

    public:

        //保存最小和最大的词语长度
        int m_min_word_len;
        int m_max_word_len; 
        //保存着所有的状态
        vector<State> m_states;
};

//将一个item转化成多个词典的解释, 数组的下标即为字典的类型
inline vector<string> item2strings(const AItem& item){

    static int max_high_bit = sizeof(DTypeValue)/sizeof(DTypeValue[0]);
    vector<string> ret(max_high_bit+1);
    string str_explain = item.m_explain;
    vector<string> explains = split(str_explain, "\t");
    for(vector<string>::iterator ite = explains.begin(); ite != explains.end(); ++ite) {
        vector<string> key_value = split(*ite, "=");
        if(key_value.size() < 2){
            break;
        }
        ret[atoi(key_value[0].c_str())] = key_value[1];
    }

    return ret;
}

//将一个item转化成指定的字典类型解释
inline string item2string(const AItem& item, unsigned int dtype_index = 1){
    vector<string> ret = item2strings(item);
    return ret[dtype_index];
}

//返回格式[2]或者[2|3] 第二种格式 说明这个单词对应了 不同的字典类型,怎么解析都可以
inline string dtypeValue2code(unsigned int dtype_value){

    string ret = "[";
    static int max_high_bit = sizeof(DTypeValue)/sizeof(DTypeValue[0]);
    int init_value = 0x01;
    bool is_first = true;
    for(int i = 0; i < max_high_bit; i ++){

        if(dtype_value & (init_value << i) ){
            if(is_first){
                ret += DTypeName[i];
                is_first = false;
            }
            else{
                ret = ret + "|" + DTypeName[i];
            }
        }
    }

    ret += "]";
    return ret;
}

//根据分割的单词返回状态码
inline string get_status_code(const vector<AItem>& items ){

    string ret;
    for(vector<AItem>::const_iterator ite = items.begin(); ite != items.end(); ++ite){
        ret += dtypeValue2code(ite->m_index.dict_type);
    }
    return ret;

}
#endif

