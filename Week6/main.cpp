#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/test/framework.hpp>
#include <boost/assert.hpp>
#include <ql/time/date.hpp>
#include <ql/time/period.hpp>
#define BOOST_TEST_DYN_LINK

namespace datelib {


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

        Date(const QuantLib::Date &ql_date){
            m_date = convertDate(ql_date);
        }

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

        int static convertPeriod(const QuantLib::Period &p){
            switch(p.units()) {
                case QuantLib::Months:
                    return p.length();
                case QuantLib::Years:
                     return p.length()*12;
                default:
                    throw std::runtime_error("Only Month or Year is supported");

            }
        }


        int static convertDate(const QuantLib::Date &ql_date){
            int temp_date = (ql_date.month()-1)+(ql_date.year()-1900)*12;
            checkDate(temp_date);
            return temp_date;
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

        Date &operator+=(const QuantLib::Period &p) {
            int temp_date = m_date + convertPeriod(p);
            checkDate(temp_date);
            m_date = temp_date;
            return *this;
        }

        Date &operator-=(const QuantLib::Period &p) {
            int temp_date = m_date - convertPeriod(p);
            checkDate(temp_date);
            m_date = temp_date;
            return *this;
        }

        Date operator+(const QuantLib::Period &p) const {
            Date temp(*this);
            temp += convertPeriod(p);
            return temp;
        }

        Date operator-(const QuantLib::Period &p) const {
            Date temp(*this);
            temp -= convertPeriod(p);
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


        friend bool operator<(const QuantLib::Date &d1, const Date &d2) {
            return (convertDate(d1) < d2.getDate());
        }

        friend bool operator<(const Date &d1, const QuantLib::Date &d2) {
            return (d1.getDate() < convertDate(d2));
        }

        friend bool operator<=(const QuantLib::Date &d1, const Date &d2) {
            return (convertDate(d1) <= d2.getDate());
        }

        friend bool operator<=(const Date &d1, const QuantLib::Date &d2) {
            return (d1.getDate() <= convertDate(d2));
        }

        friend bool operator>(const QuantLib::Date &d1, const Date &d2) {
            return (convertDate(d1) > d2.getDate());
        }

        friend bool operator>(const Date &d1, const QuantLib::Date &d2) {
            return (d1.getDate() > convertDate(d2));
        }

        friend bool operator>=(const QuantLib::Date &d1, const Date &d2) {
            return (convertDate(d1) >= d2.getDate());
        }

        friend bool operator>=(const Date &d1, const QuantLib::Date &d2) {
            return (d1.getDate() >= convertDate(d2));
        }

        friend bool operator==(const QuantLib::Date &d1, const Date &d2) {
            return (convertDate(d1) == d2.getDate());
        }

        friend bool operator==(const Date &d1, const QuantLib::Date &d2) {
            return (d1.getDate() == convertDate(d2));
        }

        friend bool operator!=(const QuantLib::Date &d1, const Date &d2) {
            return (convertDate(d1) != d2.getDate());
        }

        friend bool operator!=(const Date &d1, const QuantLib::Date &d2) {
            return (d1.getDate() != convertDate(d2));
        }


    private:
        int m_date;
    };
}
class DateTest {
public:

    static void testConstructor(){
        datelib::Date d(0, 2006);
        datelib::Date d1 ((2006-1900)*12+0);

        //test constructor
        BOOST_ASSERT_MSG(d1==d,"constructor is not correct");

        //test getDate()
        BOOST_ASSERT_MSG(d.getDate()==(2006-1900)*12+0,"getDate function is not correct");


}

    static void testOperator(){
        datelib::Date d(0, 2006);


        //test operator++(int)
        datelib::Date d1 = d++;
        BOOST_ASSERT_MSG(d1+1==d,"Post-increment operator is not correct");

        //test operator++
        datelib::Date d2 = ++d;
        BOOST_ASSERT_MSG(d2==d,"Pre-increment operator is not correct");

        //test operator--(int)
        datelib::Date d3 = d--;
        BOOST_ASSERT_MSG(d3-1==d,"Post-decrement is not correct");

        //test operator--
        datelib::Date d4 = --d;
        BOOST_ASSERT_MSG(d4==d,"Pre-decrement operator is not correct");

        //test operator+=(int months)
        datelib::Date d5 = d+1;++d;
        BOOST_ASSERT_MSG(d5==d,"Operator+=(int months) is not correct");

        //test operator-=(int months)
        datelib::Date d6 = d-1;--d;
        BOOST_ASSERT_MSG(d6==d,"Operator-=(int months) is not correct");

        //test &operator+=(int months)
        datelib::Date d7 = d+1;d+=1;
        BOOST_ASSERT_MSG(d7==d,"Operator+=(int months) is not correct");

        //test &operator+=(int months)
        datelib::Date d8 = d-1;d-=1;
        BOOST_ASSERT_MSG(d8==d,"Operator-=(int months) is not correct");

        //test operator<
        datelib::Date d9 = d-1;
        BOOST_ASSERT_MSG(d9<d,"Operator< is not correct");

        //test operator<=
        datelib::Date d10 = d-1;
        datelib::Date d11 = d;
        BOOST_ASSERT_MSG(d10<=d,"Operator<= is not correct");
        BOOST_ASSERT_MSG(d11<=d,"Operator<= is not correct");

        //test operator>
        datelib::Date d12 = d+1;
        BOOST_ASSERT_MSG(d12>d,"Operator> is not correct");

        //test >=
        datelib::Date d13 = d+1;
        datelib::Date d14 = d;
        BOOST_ASSERT_MSG(d13>=d,"Operator>= is not correct");
        BOOST_ASSERT_MSG(d14>=d,"Operator>= is not correct");

        //test  operator==
        datelib::Date d15 = d;
        BOOST_ASSERT_MSG(d15==d,"Operator= is not correct");

        //test operator!=
        datelib::Date d16 = d+1;
        BOOST_ASSERT_MSG(d16!=d,"Operator!= is not correct");


    }

    static void testQuantPeriod(){
        //test QuantLib period
        datelib::Date d(0, 2006);

        //test operator+ period
        QuantLib::Period p_m(QuantLib::Frequency::Monthly);
        QuantLib::Period p_y(QuantLib::Frequency::Annual);
        datelib::Date d1 = d + p_m;
        datelib::Date d2 = d + p_y;
        BOOST_ASSERT_MSG(d+1==d1,"Add Period month is not correct");
        BOOST_ASSERT_MSG(d+12==d2,"Add Period year is not correct");

        //test operator- period
        datelib::Date d3 = d - p_m;
        datelib::Date d4 = d - p_y;
        BOOST_ASSERT_MSG(d-1==d3,"Subtract Period month is not correct");
        BOOST_ASSERT_MSG(d-12==d4,"Subtract Period year is not correct");

        //test operator+= period
        datelib::Date d5 = d; d5+= p_m;
        datelib::Date d6 = d; d6+= p_y;
        BOOST_ASSERT_MSG(d+1==d5,"Add Period month is not correct");
        BOOST_ASSERT_MSG(d+12==d6,"Add Period year is not correct");

        //test operator-= period
        datelib::Date d7 = d; d7-= p_m;
        datelib::Date d8 = d; d8-= p_y;
        BOOST_ASSERT_MSG(d-1==d7,"Subtract Period month is not correct");
        BOOST_ASSERT_MSG(d-12==d8,"Subtract Period year is not correct");

    }

    static void testMonthYear(){
        datelib::Date d(0, 2006);

        //test month()
        BOOST_ASSERT_MSG(d.month() == 0,"month function is not correct");

        //test year()
        BOOST_ASSERT_MSG(d.year() == 2006,"year function is not correct");

        //test month/year increment()
        datelib::Date d1 = d+1;
        datelib::Date d2 = d+12;
        BOOST_ASSERT_MSG(d1.month() == d.month()+1,"month increment is not correct");
        BOOST_ASSERT_MSG(d2.month() == d.month() && d2.year()==d.year()+1,"year increment is not correct");


    }

    static void testMinMax(){

        //test minDate()
        datelib::Date d_min = datelib::Date::minDate();
        BOOST_ASSERT_MSG(d_min.getDate() == 0,"minDate is not correct");
        BOOST_ASSERT_MSG(d_min.month() == 0 && d_min.year() == 1900,
                         "month or year of minDate is not correct");

        datelib::Date d_max = datelib::Date::maxDate();
        BOOST_ASSERT_MSG(d_max.getDate() == (9999-1900)*12+11,"minDate is not correct");
        BOOST_ASSERT_MSG(d_max.month() == 11 && d_max.year() == 9999,
                         "month or year of minDate is not correct");


}

    static void testQuantlibDate(){
        //test Quantlib Date
        QuantLib::Date d(1,QuantLib::Month::Jan,2006);
        datelib::Date d1(d);

        //test if QuantLib and datelib dates contain the same month and year
        BOOST_ASSERT_MSG(d.year()==d1.year(),"years of QuantLib and datelib dates are different");
        //datelib month starts at 0, QuantLib month starts at 1
        BOOST_ASSERT_MSG( d.month()==d1.month()+1,"months of QuantLib and datelib dates are different");

        //test operator==
        BOOST_ASSERT_MSG(d==d1,"operator == is not correct");
        BOOST_ASSERT_MSG(d1==d,"operator == is not correct");

        datelib::Date d2=d1+1;
        //test operator!=
        BOOST_ASSERT_MSG(d!=d2,"operator != is not correct");
        BOOST_ASSERT_MSG(d2!=d,"operator != is not correct");

        datelib::Date d3=d1-2;
        //test operator<
        BOOST_ASSERT_MSG(d<d2,"operator < is not correct");
        BOOST_ASSERT_MSG(d3<d,"operator < is not correct");

        //test operator<=
        BOOST_ASSERT_MSG(d<=d2,"operator <= is not correct");
        BOOST_ASSERT_MSG(d3<=d,"operator <= is not correct");
        BOOST_ASSERT_MSG(d<=d1,"operator <= is not correct");
        BOOST_ASSERT_MSG(d1<=d,"operator <= is not correct");


        //test operator>
        BOOST_ASSERT_MSG(d>d3,"operator > is not correct");
        BOOST_ASSERT_MSG(d2>d,"operator > is not correct");

        //test operator>=
        BOOST_ASSERT_MSG(d>=d3,"operator >= is not correct");
        BOOST_ASSERT_MSG(d2>=d,"operator >= is not correct");
        BOOST_ASSERT_MSG(d>=d1,"operator >= is not correct");
        BOOST_ASSERT_MSG(d1>=d,"operator >= is not correct");


    }
};


boost::unit_test::test_suite *init_unit_test_suite(int /*argc*/, char * /*argv*/[]) {
    std::cout << "start test\n";
    boost::unit_test_framework::test_suite *suite = BOOST_TEST_SUITE("Date tests");
    suite->add(BOOST_TEST_CASE(&DateTest::testConstructor));
    suite->add(BOOST_TEST_CASE(&DateTest::testOperator));
    suite->add(BOOST_TEST_CASE(&DateTest::testQuantPeriod));
    suite->add(BOOST_TEST_CASE(&DateTest::testMonthYear));
    suite->add(BOOST_TEST_CASE(&DateTest::testMinMax));
    suite->add(BOOST_TEST_CASE(&DateTest::testQuantlibDate));
    boost::unit_test_framework::framework::master_test_suite().add(suite);
    return 0;
}



