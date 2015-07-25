#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<fstream>
#include<string>
#include<string.h>
#include<vector>
#include<algorithm>
using namespace std;

typedef char CONDITION;

class path{
    public:
        path(CONDITION condition, int target):m_condition(condition),m_target(target){
        }
        bool operator == (const path& other){
            return m_condition == other.m_condition;
        }

        friend ostream& operator << (ostream& out, const path& path){
            out << path.m_condition << " -> " << path.m_target;
            return out;
        }
    public:
        CONDITION m_condition;
        int m_target;
};

class State{
    public:
        State(int index, string info = ""):m_info(info){
            m_index = index;
        }

        //为一个状态添加一条路径
        bool add_one_path(const path& path, int insert_pos = -1);
        //给定一个条件返回下一个跳转状态， －1 代表 没有这条路径
        int getNextState(CONDITION condition);

        //便于调试
        friend ostream& operator << (ostream& out, const State& state){

            out <<"INDEX:" << state.m_index << " ";
            out << "path: { ";


            for(vector<path>::const_iterator ite = state.m_child.begin(); ite != state.m_child.end(); ++ite){
                out << *ite << ","; 
            }
            out << "}";
            out << "INFO: {" << state.m_info << "}" << endl;
            return out;

        }
    private:
        //将path排序的比较函数
        static bool path_cmp_path(const path& lhs, const path& rhs);
        //通过condition去检索path的比较函数
        static bool path_cmp_condition(const path& lhs, CONDITION c);
    public:
        //有序的纪录所有的状态跳转路径
        vector<path>         m_child;
        //不为空串说明该状态是一个完整的词
        string               m_info; 
        //便于调试
        int                  m_index;
};

bool State::path_cmp_path(const path& lhs, const path& rhs){
    return lhs.m_condition < rhs.m_condition;
}

bool State::path_cmp_condition(const path& lhs, CONDITION c){
    return lhs.m_condition < c;
}

int State::getNextState(CONDITION condition){
    vector<path>::iterator ite = lower_bound(m_child.begin(), m_child.end(), condition, path_cmp_condition);
    int next_state = ite - m_child.begin();

    //防止［7］[15]这种会数组越界的情况 
    if(next_state >= m_child.size()){
        return -1;
    }

    return m_child[next_state].m_condition == condition ? m_child[next_state].m_target : -1; 
}

bool State::add_one_path(const path& new_path, int insert_pos ){

    //插入之前没有指定应该插入的位置
    if(insert_pos == -1){
        vector<path>::iterator ite = lower_bound(m_child.begin(), m_child.end(), new_path.m_condition, path_cmp_condition);
        insert_pos = ite - m_child.begin();
    }

    //对于一个状态， 不能有condition相同的多条路径
    if(m_child.end() != find(m_child.begin(), m_child.end(), new_path)){
        return false;
    }

    m_child.insert(m_child.begin() + insert_pos, new_path);
    return true;
}

class ATire{
    public:
        ATire(){
            State root_state(0);
            m_states.push_back(root_state);
        }

        //将一个word加入字典, explain代表word对应的意义
        bool add_word(const string& word, const string& explain);


        //字典中的词完全匹配query的前缀，比如字典中大饭店对应北京， 大饭店X也能返回北京
        //返回结果：该query词在字典中的意义，空串代表字典中没有改词
        string dict_prefix_search(const string& query)const;

        //字典中的词完全匹配query词
        //返回结果：该query词在字典中的意义，空串代表字典中没有改词
        string search(const string& query)const;

        friend ostream& operator << (ostream& out, const ATire atrie){

            int index = 0;
            for(vector<State>::const_iterator ite = atrie.m_states.begin(); ite != atrie.m_states.end(); ++ite){
                out << index++ << ": " <<*ite<< endl;
            }

            return out;
        }
    private:
        //保存着所有的状态
        vector<State>        m_states;
};


bool ATire::add_word(const string& word, const string& explain){

    const char* p = word.c_str();

    int str_len = strlen(p);
    if(str_len == 0){
        return false;
    }

    int next_state = -1;
    State cur_state = m_states[0];
    for(int i = 0; i < str_len - 1; i++){

        CONDITION c = *p++;
        next_state = cur_state.getNextState(c);
        if(next_state == -1){
            State new_state(m_states.size());
            m_states.push_back(new_state);
            next_state = m_states.size() - 1;
            path path(c, next_state);
            m_states[cur_state.m_index].add_one_path(path);
        }
        cur_state = m_states[next_state];
    }

    CONDITION c = *p++;
    //最后一个字符特殊处理, 因为要加入对应的字典说明信息
    next_state = cur_state.getNextState(c);
    if(next_state == -1){
        State new_state(m_states.size(), explain);
        m_states.push_back(new_state);
        next_state = m_states.size() - 1;
        path path(c, next_state);
        m_states[cur_state.m_index].add_one_path(path);
    }

    return true;
}

//完全匹配
string ATire::search(const string& query) const{

    int next_state = -1;
    const char* p = query.c_str();
    State cur_state = m_states[0];
    vector<int> vec;
    vec.push_back(0);
    while(*p != '\0'){
        CONDITION c = *p++;
        next_state = cur_state.getNextState(c);
        vec.push_back(next_state);
        //说明前缀匹配失败
        if(next_state == -1) {
            break;
        }
        cur_state = m_states[next_state];
    }

    //    cout <<"search 路径{ ";
    //    for(vector<int>::iterator it = vec.begin(); it != vec.end(); ++it){
    //        cout << *it << " ";
    //    }
    //    cout<<"}" << endl;

    return *p == '\0' ? cur_state.m_info : "";

}

//字典中的词完全匹配query的前缀，比如字典中大饭店对应北京， 大饭店X也能返回北京
string ATire::dict_prefix_search(const string& query) const{

    int next_state = -1;
    State cur_state = m_states[0];
    const char* p = query.c_str();
    while(*p != '\0'){

        //说明能够前缀匹配
        if(cur_state.m_info != ""){
            return cur_state.m_info;

        }
        CONDITION c = *p++;
        next_state = cur_state.getNextState(c);
        //说明前缀匹配失败
        if(next_state == -1) {
            break;
        }

        cur_state = m_states[next_state];
    }

    return *p == '\0' ? cur_state.m_info : "";
}

vector<string> split(string& str, const char* c)                                                                                                                             
{
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

int main( int argc , char** argv){

    ATire tire;
    vector<string> keys;
    string key, value;

    cout << "argc:" << argc<<  endl; 
    ifstream f_input(argv[1]);
    string line;
    while(getline(f_input, line))
    {

        vector<string> vec = split(line, "\t");
        if(vec.size() != 2){
            continue;
        }
        key = vec[0];
        value = vec[1];
        tire.add_word(key, value);
        keys.push_back(key);
    }

    cout << tire << endl;
    vector<string>::iterator ite = keys.begin();
    for(;ite != keys.end(); ++ite){
        cout << "query:" << *ite << "\tvalue:" << tire.dict_prefix_search(*ite) << endl;
    }

    cout << "请输入query:";
    string query; 
    while(cin >> query){
        cout << "query:" << query << "\tvalue:" << tire.dict_prefix_search(query) << endl;
        cout << "请输入query:";
    }
    return 0;
}



































