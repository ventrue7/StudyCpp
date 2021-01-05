#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/test/framework.hpp>

#define BOOST_TEST_DYN_LINK

namespace datelib {

    enum Period {
        Month = 1,
        Year = 12
    };

    enum Month {
        January = 0,
        February = 1,
        March = 2,
        April = 3,
        May = 4,
        June = 5,
        July = 6,
        August = 7,
        September = 8,
        October = 9,
        November = 10,
        December = 11,
        Jan = 0,
        Feb = 1,
        Mar = 2,
        Apr = 3,
        Jun = 5,
        Jul = 6,
        Aug = 7,
        Sep = 8,
        Oct = 9,
        Nov = 10,
        Dec = 11
    };

    class Date {
    public:
        Date(int m, int y) {
            if (y < 1900 || y > 9999) {
                throw std::runtime_error("year " + std::to_string(y) + " is out of bound [1900,9999]");
            }
            if (m > 11 || m < 0) {
                throw std::runtime_error("month " + std::to_string(m) + " is out of bound Jan to Dec");
            }
            m_date = (y - 1900) * 12 + m;
        }

        Date(int date) : m_date(date) {
            checkDate(date);
        }

        Date() {};

        int year() const {
            return m_date / 12+1900;
        }

        int month() const {
            return m_date % 12;
        }

        static int getMinDate() {
            return 0;       // Jan 1900
        }

        static int getMaxDate() {
            return 97199;    // Dec 9999
        }

        static Date minDate() {
            static const Date minimum_date(getMinDate());
            return minimum_date;
        }

        static Date maxDate() {
            static const Date maximum_date(getMaxDate());
            return maximum_date;
        }

        void static checkDate(int &date) {
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


        Date &operator+=(int months) {
            int temp_date = m_date + months;
            checkDate(temp_date);
            m_date = temp_date;
            return *this;
        }

        Date &operator+=(const Period &p) {
            int temp_date = m_date + p;
            checkDate(temp_date);
            m_date = temp_date;
            return *this;
        }

        Date &operator-=(const Period &p) {
            int temp_date = m_date - p;
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
            Date temp(*this);
            temp += p;
            return temp;
        }

        Date operator-(const Period &p) const {
            Date temp(*this);
            temp -= p;
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
}
class DateTest {
public:
    static void testConsistency() {
        int minDate = datelib::Date::minDate().getDate()+1,
                maxDate = datelib::Date::maxDate().getDate();

        if(datelib::Date::maxDate().month()!=11 || datelib::Date::maxDate().year()!=9999
        ||datelib::Date::minDate().month()!=0 || datelib::Date::minDate().year()!=1900)
            std::cout << "max or min date is not correct\n";

//        //unit test for checkDate function
//        datelib::Date test1(minDate-2);
//        datelib::Date test2(maxDate+1);

//      unit test for checker in constructor
//        datelib::Date test3(12,1999);
//        datelib::Date test4(1,1888);


        int mold = datelib::Date(minDate - 1).month(),
                yold = datelib::Date(minDate - 1).year();

        for (int i = minDate; i <= maxDate; i++) {

            datelib::Date t(i);
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
            if (!((m == mold + 1) || (m == 0 && mold == 11 && y == yold + 1)))
                std::cout << "wrong month or year of date increment: \n"
                          << "date: " << i << "\n"
                          << "month,year: " << m << "," << y << "\n";


            mold = m;
            yold = y;

            // check month definition
            if (m < 0 || m > 11)
                std::cout << "invalid month: \n"
                          << "date:  " << i << "\n"
                          << "month: " << m << "\n";

            // create the same date with a different constructor
            datelib::Date s(m, y);
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
        datelib::Date d(0, 2006);
        if (d.month() != datelib::January ||
            d.year() != 2006 ||
            d.getDate() != (2006-1900)*12+0) {
            std::cout << "Iso date failed\n"
                      << " month:         " << d.month() << "\n"
                      << " year:          " << d.year();
        }
        datelib::Date d0 = d;
        datelib::Date d1 = ++d0;
        datelib::Date d2 = d0++;
        datelib::Date d3 = d0 + datelib::Month;
        datelib::Date d4 = d0 + datelib::Year;

        if (d1.month() != d2.month() || d1.year() != d2.year() || d1 != d2)
            std::cout << "Pre-increment or Post-increment is not correct\n";
        if (d1 > d0 || d1 >= d0 || d1 == d0)
            std::cout << "operator larger than, larger than or equal to, equal to is not correct\n";
        if (d3.month() != d0.month() + 1 || d3.year() != d0.year())
            std::cout << "Add period month is not correct\n";
        if (d4.month() != d0.month() || d4.year() != d0.year() + 1)
            std::cout << "Add period year is not correct\n";
        if (d3 != d0+1 || d4!= d0 +12)
            std::cout << "Add month or year is not correct\n";

        datelib::Date d5 = --d0;
        datelib::Date d6 = d0--;
        datelib::Date d7 = d0 - datelib::Month;
        datelib::Date d8 = d0 - datelib::Year;

        if (d5.month() != d6.month() || d5.year() != d6.year() || d5 != d6)
            std::cout << "Pre-decrement or Post-decrement is not correct\n";
        if (d5 < d0 || d5 <= d0 || d5 == d0)
            std::cout << "Operator smaller than, smaller than or euqal to, equal to is not correct\n";

        if (!((d7.month() == d0.month() - 1 && d7.year() == d0.year())||
        (d0.month()==0 && d7.month()==11 && d7.year() == d0.year()-1)))
            std::cout << "Subtract period month is not correct\n";
        if (d8.month()!=d0.month() || d8.year()!=d0.year()-1)
            std::cout << "Subtract period year is not correct\n";
        if (d8 != d0 -12 || d7 != d0-1)
            std::cout << "Subtract month or year is not correct\n";

    }

};


boost::unit_test::test_suite *init_unit_test_suite(int /*argc*/, char * /*argv*/[]) {
    std::cout << "start test\n";
    boost::unit_test_framework::test_suite *suite = BOOST_TEST_SUITE("Date tests");
    suite->add(BOOST_TEST_CASE(&DateTest::testConsistency));
    suite->add(BOOST_TEST_CASE(&DateTest::isoDates));
    boost::unit_test_framework::framework::master_test_suite().add(suite);
    return 0;
}



