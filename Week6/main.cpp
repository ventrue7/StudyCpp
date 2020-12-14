#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/test/framework.hpp>
#define BOOST_TEST_DYN_LINK

using namespace boost::unit_test_framework;

enum Period {
    Month = 1,
    Year = 12
};

enum Month {
    January   = 1,
    February  = 2,
    March     = 3,
    April     = 4,
    May       = 5,
    June      = 6,
    July      = 7,
    August    = 8,
    September = 9,
    October   = 10,
    November  = 11,
    December  = 12,
    Jan = 1,
    Feb = 2,
    Mar = 3,
    Apr = 4,
    Jun = 6,
    Jul = 7,
    Aug = 8,
    Sep = 9,
    Oct = 10,
    Nov = 11,
    Dec = 12
};

class Date {
public:
    Date(int m, int y) {
        if (y < 1900 || y > 9999) {
            throw std::runtime_error("year " + std::to_string(y) + " is out of bound [1900,9999]");
        }
        if (m > 12 || m < 1) {
            throw std::runtime_error("month " + std::to_string(m) + " is out of bound Jan to Dec");
        }
        m_date = (y - 1900) * 12 + m;
        //std::cout<<"m_date: "<<m_date<<std::endl;
    }

    Date(int date) : m_date(date) {
        checkDate(date);
    }

    Date() {};

    int year() const {
        return (m_date % 12 != 0) ? (m_date / 12 + 1900) : (m_date / 12 + 1900 - 1);
    }

    int month() const {
        return (m_date % 12 != 0) ? m_date % 12 : 12;
    }

    static int getMinDate() {
        return 1;       // Jan 1900
    }

    static int getMaxDate() {
        return 97200;    // Dec 9999
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
        if (date < getMinDate()) {
            throw std::runtime_error("date is smaller than min date");
        }
        if (date > getMaxDate()) {
            throw std::runtime_error("date is larger than max date");
        }
    }

    int getDate() const {
        return m_date;
    }

    int getPeriod(const Period &p) const {
        int temp_month;
        switch (p) {
            case Month:
                temp_month = 1;
                break;
            case Year:
                temp_month = 12;
                break;
            default:
                throw std::runtime_error("unknown date unit");
        }
        return temp_month;
    }


    Date &operator+=(int months) {
        int temp_date = m_date + months;
        checkDate(temp_date);
        m_date = temp_date;
        return *this;
    }

    Date &operator+=(const Period &p) {
        int temp_month = getPeriod(p);
        int temp_date = m_date + temp_month;
        checkDate(temp_date);
        m_date = temp_date;
        return *this;
    }

    Date &operator-=(const Period &p) {
        int temp_month = getPeriod(p);
        int temp_date = m_date - temp_month;
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


    Date operator+(const Period &p) const {
        int temp_month = getPeriod(p);
        Date temp(*this);
        temp += temp_month;
        return temp;
    }

    Date operator-(const Period &p) const {
        int temp_month = getPeriod(p);
        Date temp(*this);
        temp -= temp_month;
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

class DateTest {
public:
    static void testConsistency() {
        int minDate = Date::minDate().getDate() + 1,
                maxDate = Date::maxDate().getDate();

        int mold = Date(minDate - 1).month(),
                yold = Date(minDate - 1).year();

        for (int i = minDate; i <= maxDate; i++) {

            Date t(i);
            int serial = t.getDate();

            // check serial number consistency
            if (serial != i)
                std::cout << "inconsistent date\n"
                          << "original: " << i << "\n"
                          << "original: " << serial << "\n";

            int m = t.month(),
                    y = t.year();

            //std::cout<<"current date "<<i<<","<<m<<","<<y<<"\n";
            //check if skipping any month
            if (!((m == mold + 1) || (m == 1 && mold == 12 && y == yold + 1)))
                std::cout << "wrong month or year of date increment: \n"
                          << "date: " << i << "\n"
                          << "month,year: " << m << "," << y << "\n";


            mold = m;
            yold = y;

            // check month definition
            if (m < 1 || m > 12)
                std::cout << "invalid month: \n"
                          << "date:  " << i << "\n"
                          << "month: " << m << "\n";

            // create the same date with a different constructor
            Date s(m, y);
            // check number consistency
            serial = s.getDate();
            if (serial != i)
                std::cout << "inconsistent date:\n"
                          << "date: " << i << "\n"
                          << "cloned date: " << serial << "\n"
                          << "month,year: " << m << "," << y << "\n";
        }

    }

    static void isoDates() {
        Date d(1, 2006);
        if (d.month() != January ||
            d.year() != 2006) {
            std::cout<<"Iso date failed\n"
                     << " month:         " << d.month() << "\n"
                     << " year:          " << d.year();
        }
        Date d0 = d;
        Date d1 = ++d0;
        Date d2 = d0++;
        if(d1.month() != d1.month() || d1.year() != d2.year()|| d1!= d2)
            std::cout<<"Pre-increment or Post-increment or is not correct\n";
        Date d3 = d0+Month;
        if(d3.month() != d0.month()+1 || d3.year() != d0.year())
            std::cout<<"Add period month is not correct\n";
        Date d4 = d0+Year;
        if(d4.month() != d0.month() || d4.year() != d0.year()+1)
            std::cout<<"Add period year is not correct\n";

    }
};

boost::unit_test::test_suite* init_unit_test_suite(int /*argc*/, char* /*argv*/[])
{
    std::cout << "start test\n";
    test_suite *suite = BOOST_TEST_SUITE("Date tests");
    suite->add(BOOST_TEST_CASE(&DateTest::testConsistency));
    suite->add(BOOST_TEST_CASE(&DateTest::isoDates));
    framework::master_test_suite().add(suite);
    return 0;
}



