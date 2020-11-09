#include <iostream>
#include <vector>
#include <regex>
#include <type_traits>
using namespace std;

//an iterator that skips every other element in a vector
template<typename T>
vector<T> skip_element(vector<T> input){
    vector<T> output;
    for(typename vector<T>::iterator it = input.begin(); it != input.end();++it){
        if((it-input.begin()+1)%2) output.push_back(*it);
    }
    return output;
}
//SFINAE (substitution failure is not an error)
template<size_t N> void foo(const int (&arr)[N], const int *p){
    cout<< "Array of "<<N<<", distance "<<p-&arr[0]<<endl;
}
template <typename C> void foo(const C& c, typename C::const_iterator i){
    cout<<"Container of size "<<c.size()<<", distance ";
    cout<<i-c.begin()<<endl;
}



int main() {
    //skip element example
    vector<int> input = {2, 5, 6, 11, 13, 17, 19, 23, 26};
    vector<int> output = skip_element(input);
    //lambda expression
    vector<int> output2(output.size());
    bool condition = true;
    copy_if(input.begin(), input.end(), output2.begin(),
            [&condition](int i) {
            condition = !condition;
            return !condition;
            }
    );

    //lower bound; upper bound; equal range
    vector<int> bound_data = {4,5,5,5,6,7,8,9,10};
    int target_data = 5;
    vector<int>::iterator lower = lower_bound(bound_data.begin(),bound_data.end(),target_data);
    cout<<"the lower bound for value "<<target_data<<" is at position: "<<lower-bound_data.begin()<<endl;

    vector<int>::iterator upper = upper_bound(bound_data.begin(),bound_data.end(),target_data);
    cout<<"the upper bound for value "<<target_data<<" is at position: "<<upper-bound_data.begin()<<endl;

    pair<vector<int>::iterator,
            vector<int>::iterator> range = equal_range(bound_data.begin(),bound_data.end(),target_data);
    cout<<"the range of "<<target_data<<" is between: "<<range.first-bound_data.begin()
        <<" to "<<range.second-bound_data.begin()<<endl;

    //regex string
    const regex r1("a(b+)c"); //one or more "b”
    const regex r2("a(b*)c"); //0 or more "b”
    const regex r3("a(b?)c");// 0 or one "b”
    string word {"abbbc"};
    smatch m;
    cout<<"r1: "<<regex_match(word,m, r1)<<","<<m[1]<<endl;
    cout<<"r2: "<<regex_match(word,m, r2)<<","<<m[1]<<endl;
    cout<<"r2: "<<regex_match(word,m, r3)<<","<<m[1]<<endl;

    //regex replace
    const regex r("c+d+e+");
    const string fmt("MEOW");
    string s {"XXcdeYYYccdeeeZZceAA"};
    regex_replace(s, r, fmt);
    cout<<"after replacement:"<<regex_replace(s, r, fmt)<<endl;

    //SFINAE (substitution failure is not an error)
    int a[] = {11,22,33,44};
    int *p = &a[2];
    foo(a, p);//it will use the first foo function, because the c style array does not have a built in iterator
    vector<int> v(a, a+4);
    foo(v, v.begin()+3);
    return 0;
}
