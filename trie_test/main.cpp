#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<fstream>
#include<string>
#include<string.h>
#include<vector>
#include<algorithm>
using namespace std;

typedef int CONDITION;
typedef struct _path{
    CONDITION m_condition;
    int m_target;
}Path;

//通过condition去检索Path的比较函数
bool path_cmp_condition(const Path& lhs, CONDITION c){
    return lhs.m_condition < c;
}


class State{
    private:
        State(int index, string info = ""):m_info(info){
            m_index = index;
        }

        //为一个状态添加一条路径
        bool add_one_path(CONDITION c, int target);
        //给定一个条件返回下一个跳转状态， －1 代表 没有这条路径
        int getNextState(CONDITION condition)const;

    private:
        //有序的纪录所有的状态跳转路径
        vector<Path>         m_child;
        //不为空串说明该状态是一个完整的词
        string               m_info; 
        //便于调试
        int                  m_index;
        friend class ATire;
};


int State::getNextState(CONDITION condition)const{
    vector<Path>::const_iterator ite = lower_bound(m_child.begin(), m_child.end(), condition, path_cmp_condition);
    int next_state = ite - m_child.begin();

    //防止［7］[15]这种会数组越界的情况 
    if(next_state >= m_child.size()){
        return -1;
    }

    return m_child[next_state].m_condition == condition ? m_child[next_state].m_target : -1; 
}

bool State::add_one_path(CONDITION c, int target){

    //首先判定该跳转路径是否已经存在
    vector<Path>::iterator ite = lower_bound(m_child.begin(), m_child.end(), c, path_cmp_condition);
    int insert_pos = ite - m_child.begin();
    bool is_should_add = false;
    //如果返回最后一个位置，单独判断
    if(ite == m_child.end() || ite == m_child.begin()){
        is_should_add = true;
        goto finally;
    }

    is_should_add = m_child[insert_pos].m_condition != c;

finally:
    if(!is_should_add){
        cout <<"没有插入路径:" << "c:" << c << "\tt:"<<target<<endl;
        return false;
    }

    Path new_path;
    new_path.m_condition = c;
    new_path.m_target = target;
    cout <<"插入路径:" << "c:" << c << "\tt:"<<target<<endl;  
    cout<<"插入前:{";
    for(vector<Path>::iterator ite = m_child.begin(); ite != m_child.end(); ++ite){
        cout << ite->m_condition<< "->" << ite->m_target<< ",";
    }
    cout <<"}"<<endl;
    m_child.insert(m_child.begin() + insert_pos, new_path);
    
    cout<<"插入后:{";
    for(vector<Path>::iterator ite = m_child.begin(); ite != m_child.end(); ++ite){
        cout << ite->m_condition<< "->" << ite->m_target<< ",";
    }
    cout <<"}"<<endl;
    return true;
}

class ATire{
    public:
        ATire(){
            m_min_word_len = 1000000;
            m_max_word_len = -1;
            State root_state(0);
            m_states.push_back(root_state);
        }

        //将一个word加入字典, explain代表word对应的意义
        bool add_word(const string& word, const string& explain);

        //字典中的词完全匹配query词
        //返回结果：该query词在字典中的意义，空串代表字典中没有改词
        string equal_search(const string& query)const;

        //字典中的词完全匹配query的前缀，比如字典中大饭店对应北京， 大饭店.*也能返回北京
        //黑龙潭对应北京， 黑龙潭公园对应大理， 这种情况需要找到最大的前缀匹配
        //返回结果：该query词在字典中的意义，空串代表字典中没有改词
        string prefix_search(const string& query)const;

        //query词中办包含字典中词
        //例如字典中含有北京对应city1， 那么query 为.*北京.*能返回city1
        //query 词为.*云南大理.*能在匹配字典中一条数据，.*云南.*也能匹配到一条数据， 这种情况需要找到最大的匹配
        string contain_search(const string& query)const;

        //仅仅为了调试
        friend ostream& operator << (ostream& out, const ATire& atrie){

            int index = 0;
            for(vector<State>::const_iterator ite_state = atrie.m_states.begin(); ite_state != atrie.m_states.end(); ++ite_state){
                out <<"INDEX:" << index++<< " ";
                out << "childs:" << (*ite_state).m_child.end() - (*ite_state).m_child.begin()  << " "; 
                out << "Path: { ";
                for(vector<Path>::const_iterator ite_path = ite_state->m_child.begin(); ite_path != ite_state->m_child.end(); ++ite_path){
                    out << ite_path->m_condition << "->" << ite_path->m_target << ","; 
                }
                out << "} ";
                out << "INFO:" << (*ite_state).m_info<< endl;
            }
            out <<endl;
            out <<"min_word_len:"<<atrie.m_min_word_len << "\tmax_word_len:" << atrie.m_max_word_len; 
            out<<endl;
            return out;
        }
    private:
        //保存最小和最大的词语长度
        int                 m_min_word_len;
        int                 m_max_word_len; 
        //保存着所有的状态
        vector<State>        m_states;
};


bool ATire::add_word(const string& word, const string& explain){

    const char* p = word.c_str();

    int str_len = strlen(p);
    if(str_len == 0){
        return false;
    }

    m_min_word_len = str_len < m_min_word_len ? str_len : m_min_word_len;
    m_max_word_len = str_len > m_max_word_len ? str_len : m_max_word_len;

    int next_state = -1;
    State cur_state = m_states[0];
    cout<< "add_word path: {" << endl;
    for(int i = 0; i < str_len - 1; i++){

        CONDITION c = *p++;
        next_state = cur_state.getNextState(c);
        if(next_state == -1){
            int index = m_states.size();
            State new_state(index);
            m_states.push_back(new_state);
            m_states[cur_state.m_index].add_one_path(c, index);
            next_state = index;
        }
       cur_state = m_states[next_state];
    }

    CONDITION c = *p++;
    //最后一个字符特殊处理, 因为要加入对应的字典说明信息
    next_state = cur_state.getNextState(c);
    if(next_state == -1){
        int index = m_states.size();
        State new_state(index, explain);
        m_states.push_back(new_state);
        m_states[cur_state.m_index].add_one_path(c, index);
    }
    cout << "}" << endl;

    return true;
}

//完全匹配
string ATire::equal_search(const string& query) const{

    int next_state = -1;
    const char* p = query.c_str();
    State cur_state = m_states[0];
    while(*p != '\0'){
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

//字典中的词完全匹配query的前缀，比如字典中大饭店对应北京， 大饭店X也能返回北京
//黑龙潭对应北京， 黑龙潭公园对应大理， 这种情况需要找到最大的前缀匹配
string ATire::prefix_search(const string& query) const{

    int next_state = -1;
    State cur_state = m_states[0];
    string max_match_info = "";
    const char* p = query.c_str();
    vector<int> vec ;
    vec.push_back(0);
    cout << "search Path:{";
    while(*p != '\0'){

        //说明能够前缀匹配
        if(cur_state.m_info != ""){
            max_match_info = cur_state.m_info;
        }
        CONDITION c = *p++;
        cout << next_state <<" + " << c ;
        next_state = cur_state.getNextState(c);
        cout <<"->"<< next_state << ",";
        vec.push_back(next_state);
        // 不能继续前缀匹配
        if(next_state == -1) {
            break;
        }

        cur_state = m_states[next_state];
    }
    cout << "}" << endl;
    return cur_state.m_info != "" ? cur_state.m_info : max_match_info;
}

string ATire::contain_search(const string& query)const{

    const char* p = query.c_str();
    int query_len = strlen(p);
    for(int i = 0; i <= query_len - m_min_word_len; i++){
        string ret = prefix_search(query.substr(i));
        if(ret == ""){
            continue;
        }
        return ret;
    }

    return "";
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
        if(key == "" || value == ""){
            continue;
        }
        tire.add_word(key, value);
        keys.push_back(key);
    }

    cout << tire << endl;
    vector<string>::iterator ite = keys.begin();
    for(;ite != keys.end(); ++ite){
        string value = tire.prefix_search(*ite);
        cout << "query:" << *ite  << "\tlength:" << strlen((*ite).c_str())<< "\tvalue:" << value << endl;
    }

    //cout << tire << endl;

  //  cout << "请输入query:";
  //  string query; 
  //  while(cin >> query){
  //      cout << "query:" << query << "\tvalue:" << tire.prefix_search(query) << endl;
  //     // cout << "query:" << query << "\tvalue:" << tire.contain_search(query) << endl;
  //      cout << "请输入query:";
  //  }
    return 0;
}
