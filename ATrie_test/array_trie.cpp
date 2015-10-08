#include "array_trie.h"
#include<string>
#include<string.h>
#include<algorithm>

//通过condition去检索Path的比较函数
bool path_cmp_condition(const Path& lhs, CONDITION c){
    return lhs.m_condition < c;
}

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
    if(ite == m_child.end()){
        is_should_add = true;
        goto finally;
    }

    is_should_add = m_child[insert_pos].m_condition != c;

finally:
    if(!is_should_add){
        return false;
    }

    Path new_path;
    new_path.m_condition = c;
    new_path.m_target = target;
    m_child.insert(m_child.begin() + insert_pos, new_path);
    return true;
}

bool ATrie::add_word(const string& word, const string& explain, DType dict_type, const string& origin){

    const char* p = word.c_str();

    int str_len = strlen(p);
    if(str_len == 0){
        return false;
    }

    m_min_word_len = str_len < m_min_word_len ? str_len : m_min_word_len;
    m_max_word_len = str_len > m_max_word_len ? str_len : m_max_word_len;

    int next_state = -1;
    State cur_state = m_states[0];
    for(int i = 0; i < str_len - 1; i++){
        CONDITION c = *p++;
        next_state = cur_state.getNextState(c);
        if(next_state == -1){


            AItem aitem;
            aitem.m_index.dict_flag = 0;
            aitem.m_index.dict_type = 0;
            aitem.m_index.dict_index = m_states.size();
            State new_state(aitem);
            m_states.push_back(new_state);
            m_states[cur_state.m_aitem.m_index.dict_index].add_one_path(c, aitem.m_index.dict_index);
            next_state = aitem.m_index.dict_index; 
        }
        cur_state = m_states[next_state];
    }

    CONDITION c = *p++;

    //最后一个字符特殊处理, 因为要加入对应的字典说明信息
    next_state = cur_state.getNextState(c);
    if(next_state == -1){

        AItem aitem;
        aitem.m_index.dict_flag = 1;
        aitem.m_index.dict_type = DTypeValue[dict_type];
        aitem.m_index.dict_index = m_states.size();
        aitem.m_origin = origin.empty() ? word : origin ;
        aitem.m_explain = DTypeName[dict_type] +"="+ explain;
        State new_state(aitem);
        m_states.push_back(new_state);
        m_states[cur_state.m_aitem.m_index.dict_index].add_one_path(c, aitem.m_index.dict_index);
    }
    //考虑一个词有不同的词语解释的情况
    else if(next_state != -1 && m_states[next_state].m_aitem.m_index.dict_type != DTypeValue[dict_type]){

    //    m_states[next_state].m_aitem.m_origin = origin.empty() ? word : origin ;
        m_states[next_state].m_aitem.m_index.dict_type |= DTypeValue[dict_type];
        if(m_states[next_state].m_aitem.m_index.dict_flag){
            m_states[next_state].m_aitem.m_explain += "\t" + DTypeName[dict_type] + "=" + explain;
        }
        else{
            m_states[next_state].m_aitem.m_index.dict_flag = 1;
            m_states[next_state].m_aitem.m_explain =  DTypeName[dict_type] + "=" + explain;
        }
    }

    return true;
}

AItem ATrie::equal_search(const string& query) const{

    AItem ret;
    ret.m_index.dict_flag = 0;
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

    if(*p == '\0' && cur_state.m_aitem.m_index.dict_flag){
        return cur_state.m_aitem;
    }

    return ret;
}

AItem ATrie::prefix_search(const string& query) const{

    AItem ret;
    ret.m_index.dict_flag = 0;
    int next_state = -1;
    State cur_state = m_states[0];
    string max_match_info = "";
    const char* p = query.c_str();
    while(*p != '\0'){

        //说明能够前缀匹配
        if(cur_state.m_aitem.m_index.dict_flag){
            ret = cur_state.m_aitem;
        }
        CONDITION c = *p++;
        next_state = cur_state.getNextState(c);
        // 不能继续前缀匹配
        if(next_state == -1) {
            break; 
        }

        cur_state = m_states[next_state];
    }

    if(cur_state.m_aitem.m_index.dict_flag){
        return cur_state.m_aitem;
    }
    return ret;
}

//该方法不支持去杂词， query能完全被分割成字典中的词语才会返回值才不会为empty
vector<AItem> ATrie::split_item(const string& query) const{

    vector<AItem> ret;

    int last_pos = 0;
    int query_len = query.size();
    //不断的最大前缀匹配
    while(last_pos < query_len){

        AItem max_item;
        max_item.m_index.dict_flag = 0;
        string remain_query = query.substr(last_pos);
        int remain_query_len = remain_query.size();
        const char* p = remain_query.c_str();

        int next_state = -1;
        State cur_state = m_states[0];
        int max_prefix_match_len = 0;
        int i =0;
        for(; i < remain_query_len; i++){
            //说明能够前缀匹配
            if(cur_state.m_aitem.m_index.dict_flag){
                max_item = cur_state.m_aitem;
                max_prefix_match_len = i;
            }
            CONDITION c = *p++;
            next_state = cur_state.getNextState(c);

            //不能继续进行前缀匹配
            if(next_state == -1) {
                break; 
            }
            cur_state = m_states[next_state];
        }

        if(cur_state.m_aitem.m_index.dict_flag){
            max_item = cur_state.m_aitem;
            max_prefix_match_len = i;
        }

        if(!max_item.m_index.dict_flag){
            ret.clear();
            break;
        }

        last_pos += max_prefix_match_len;
        ret.push_back(max_item);
    }

    return ret;
}

AItem ATrie::contain_search(const string& query)const{

    AItem ret;
    ret.m_index.dict_flag = 0;
    const char* p = query.c_str();
    int query_len = query.size();
    for(int i = 0; i <= query_len - m_min_word_len; i++){
        ret = prefix_search(query.substr(i));
        if(ret.m_index.dict_flag){
            return ret;
        }
    }

    return ret;
}

