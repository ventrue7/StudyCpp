#include <iostream>
#include <vector>
#include <list>
using namespace std;

//print function of iterator
template<class It>
void print(It start, It end) {
    for (; start != end; ++start) {
        cout << *start << endl;
    }
}

template<typename T, template <typename, typename> class Container>
class iterator_support{
public:
    iterator_support(){
    }
    ~iterator_support(){
    }

    class iterator{
    public:
        using self_type = iterator;
        using value_type = T;
        using reference = T&;
        using pointer = T*;
        iterator(typename Container<T, std::allocator<T>>::iterator it,
                 typename Container<T, std::allocator<T>>::iterator begin,
                 typename Container<T, std::allocator<T>>::iterator end,
                 int step = 2):m_it(it),m_begin(begin),m_end(end),m_step(step){}

        reference operator*() {return *m_it;}
        pointer operator->() {return m_it; }

        self_type &operator++() {
            int i = 0;
            while(i < m_step && m_it != m_end) {
                m_it++;
                i++;
            }
            return *this;
        }
        self_type operator++(int){
            self_type temp = *this;
            int i = 0;
            while(i < m_step && m_it != m_end) {
                m_it++;
                i++;
            }
            return temp;
        }
        self_type& operator--(){
            int i = 0;
            while(i < m_step && m_it != m_begin) {
                m_it--;
                i++;
            }
            return *this;
        }
        self_type operator--(int){
            self_type temp = *this;
            int i = 0;
            while(i < m_step && m_it != m_begin) {
                m_it--;
                i++;
            }
            return temp;
        }

        bool operator== ( const self_type& other ) const { return m_it == other.m_it ; }
        bool operator!= ( const self_type& other ) const { return m_it != other.m_it; }

    private:
        typename Container<T, std::allocator<T>>::iterator m_it,m_begin,m_end;
        int m_step;
    };

    iterator begin(Container<T, std::allocator<T>>& data ) { return iterator(data.begin(),data.begin(), data.end()); }
    iterator end(Container<T, std::allocator<T>>& data) { return iterator(data.end(),data.begin(), data.end());}
};



int main() {
//    skip element example
//    vector<int> input {1,2,3,4};
//    iterator_support<int, vector> input2{};
    list<int> input {1,2,3,4};
    iterator_support<int, list> input2{};

    cout<<"STL iterator"<<endl;
    print( input.begin(), input.end() );
    cout<<"custom iterator"<<endl;
    print( input2.begin(input), input2.end(input) );

    return 0;
}
