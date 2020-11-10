#include <iostream>
#include <vector>
#include <regex>
#include <type_traits>
using namespace std;


//print function of iterator
template<class It>
void print(It start, It end) {
    for (; start != end; ++start) {
        cout << *start << endl;
    }
}

//an function that skips every other element in a vector
template<typename T>
vector<T> skip_element(vector<T> input){
    vector<T> output;
    for(typename vector<T>::iterator it = input.begin(); it != input.end();++it){
        if((it-input.begin()+1)%2) output.push_back(*it);
    }
    return output;
}

//an iterator that skips every other element in a vector
template <typename T>
class vector_data{
public:
    vector_data(const vector<T> data){
        m_size = data.size();
        m_data  = new T[m_size];
        for(int i = 0; i <m_size;++i){
            m_data[i] = data[i];
        }
    }
    ~vector_data(){
        delete [] m_data;
    }

    class iterator{
    public:
        using self_type = iterator;
        using value_type = T;
        using reference = T&;
        using pointer = T*;
        iterator(pointer ptr, int idx, int length):m_ptr(ptr),m_idx(idx),m_length(length){}

        reference operator*() {return *m_ptr;}
        pointer operator->() {return m_ptr; }

        self_type& operator++(){
            self_type temp = *this;
            m_ptr++;
            m_idx++;
            if(m_idx<m_length){
                m_ptr++;
                m_idx++;
            }
            return temp;
        }
        self_type operator++(int){
            m_ptr++;
            m_idx++;
            if(m_idx<m_length){
                m_ptr++;
                m_idx++;
            }
            return *this;
        }
        self_type& operator--(){self_type temp = *this;m_ptr-=2;return temp;}
        self_type operator--(int){m_ptr-=2;return *this;}

        bool operator== ( const self_type& other ) const { return m_ptr == other.m_ptr ; }
        bool operator!= ( const self_type& other ) const {return m_ptr != other.m_ptr;}

    private:
        pointer m_ptr;
        int m_length;
        int m_idx;
    };

    iterator begin() { return iterator(m_data,0, m_size); }
    iterator end() { return iterator(m_data+m_size,m_size, m_size);}

    
private:
    T* m_data;
    int m_size;
};


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
    vector<int> input = {2,4,6,8,10,12,14,16,18,20,22};
    vector<int> output = skip_element(input);
    vector_data<int> input2 (input);
    vector<int> output2, output3;

    cout<<"STL iterator"<<endl;
    print( input.begin(), input.end() );
    cout<<"custom iterator"<<endl;
    print( input2.begin(), input2.end() );


    for(vector_data<int>::iterator i = input2.begin(); i != input2.end(); i++)
    {
        output2.push_back(*i);
        cout<<"skip element output using iterator 1: "<<*i<<endl;
    }

    //skip element using lambda expression
    vector<int> output4(output.size());
    bool condition = true;
    copy_if(input.begin(), input.end(), output4.begin(),
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
