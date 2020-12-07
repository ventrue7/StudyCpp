#include <iostream>


class Date {
public:
    Date(int m, int y) {
        try {
            if (y < 1900 || y > 2200) {
                throw std::runtime_error("year " + std::to_string(y) + " is out of bound [1900,2199]");
            }
        }
        catch (const std::exception &e) {
            std::cout << e.what() << "\n";
        }

        try {
            if (m > 12 || m < 1) {
                throw std::runtime_error("month " + std::to_string(m) + " is out of bound Jan to Dec");
            }
        }
        catch (const std::exception &e) {
            std::cout << e.what() << "\n";
        }

        m_date = (y - 1900) * 12 + m;
    }

    Date(int date) : m_date(date) {
        checkDate(date);
    }

    Date() {};

    int year() const{
        return (m_date%12!=0)?(m_date / 12 + 1900):(m_date / 12 + 1900-1);
    }

    int month() const{
        return (m_date%12!=0)?m_date%12:12;
    }

    static int getMinDate() {
        return 1;       // Jan 1901
    }

    static int getMaxDate() {
        return 3600;    // Dec 2199
    }

    static Date minDate() {
        static const Date minimum_date(getMinDate());
        return minimum_date;
    }

    static Date maxDate() {
        static const Date maximum_date(getMaxDate());
        return maximum_date;
    }

    void checkDate(int &date) {
        try {
            if (date < getMinDate()) {
                throw std::runtime_error("date is smaller than min date");
            }
        }
        catch (const std::exception &e) {
            std::cout << e.what() << "\n";
        }
        try {
            if (date > getMaxDate()) {
                throw std::runtime_error("date is larger than max date");
            }
        }
        catch (const std::exception &e) {
            std::cout << e.what() << "\n";
        }

    }

    int getDate() const {
        return m_date;
    }

    Date &operator+=(int months) {
        int temp_date = m_date + months;
        checkDate(temp_date);
        m_date = temp_date;
        return *this;
    }

    Date &operator-=(int months) {
        int temp_date = m_date - months;
        checkDate(temp_date);
        m_date = temp_date;
        return *this;
    }

    Date &operator++() {
        int temp_date = m_date + 1;
        checkDate(temp_date);
        m_date = temp_date;
        return *this;
    }

    Date operator++(int) {
        Date temp(*this);
        ++*this;
        return temp;
    }

    Date &operator--() {
        int temp_date = m_date - 1;
        checkDate(temp_date);
        m_date = temp_date;
        return *this;
    }

    Date operator--(int) {
        Date temp(*this);
        --*this;
        return temp;
    }

    Date operator+(int months) const {
        Date temp(*this);
        temp += months;
        return temp;
    }

    Date operator-(int months) const {
        Date temp(*this);
        temp -= months;
        return temp;
    }


    friend bool operator<(const Date &d1, const Date &d2) {
        return (d1.getDate() < d2.getDate());
    }

    friend bool operator<=(const Date &d1, const Date &d2) {
        return (d1.getDate() <= d2.getDate());
    }

    friend bool operator>(const Date &d1, const Date &d2) {
        return (d1.getDate() > d2.getDate());
    }

    friend bool operator>=(const Date &d1, const Date &d2) {
        return (d1.getDate() >= d2.getDate());
    }

    friend bool operator==(const Date &d1, const Date &d2) {
        return (d1.getDate() == d2.getDate());
    }

    friend bool operator!=(const Date &d1, const Date &d2) {
        return (d1.getDate() != d2.getDate());
    }


private:
    int m_date;
};


int main() {


    int minDate = Date::minDate().getDate()+1,
        maxDate = Date::maxDate().getDate();

    int mold  = Date(minDate-1).month(),
        yold  = Date(minDate-1).year();

    for (int i=minDate; i<=maxDate; i++) {

        Date t(i);
        int serial = t.getDate();

        // check serial number consistency
        if (serial != i)
            std::cout<<"inconsistent date\n"
                     <<"original: "<<i<<"\n"
                     <<"original: "<<serial<<"\n";

        int     m  = t.month(),
                y  = t.year();

        //std::cout<<"current date "<<i<<","<<m<<","<<y<<"\n";
        //check if skipping any month
        if(!( (m == mold+1) ||(m == 1 && mold == 12)))
            std::cout<<"wrong month of date increment: \n"
                     << "date: " << i << "\n"
                     << "month,year: "<< m << "," << y<< "\n";

        mold = m; yold = y;

        // check month definition
        if (m < 1 || m > 12)
            std::cout<<"invalid month: \n"
                     << "date:  " << i << "\n"
                     << "month: " << m<< "\n";

        // create the same date with a different constructor
        Date s(m,y);
        // check number consistency
        serial = s.getDate();
        if (serial != i)
            std::cout<<"inconsistent date:\n"
                     << "date: " << i << "\n"
                     << "cloned date: " <<  serial << "\n"
                     << "month,year: "<< m << "," << y<< "\n";
    }

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
