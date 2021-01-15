#include <iostream>
#include <random>
#include <vector>
#include <math.h>
#include <boost/test/unit_test.hpp>
#include <boost/test/framework.hpp>
#include <boost/assert.hpp>
#include <ql/time/date.hpp>
#include <ql/time/period.hpp>
#define BOOST_TEST_DYN_LINK

namespace simulationlib{

    template <class T>
    class RandomNumber{
    public:
        RandomNumber(size_t engine_num, double mean, double stddev):m_norm_dist(mean,stddev){
            m_engine.resize(engine_num);
            static std::random_device rd;
            for(size_t i = 0; i < engine_num; ++i){
                m_engine[i] = std::mt19937(rd());
            }
        };
        RandomNumber(size_t engine_num, const std::vector<unsigned int> seed, double mean, double stddev)
        :m_norm_dist(mean,stddev){
            m_engine.resize(engine_num);
            static std::random_device rd;
            for(size_t i = 0; i < engine_num; ++i){
                m_engine[i] = std::mt19937(seed[i]);
            }
        }

        T getNormalRand(size_t i){
            return m_norm_dist(m_engine[i]);
        }

    private:
        std::vector<std::mt19937> m_engine;
        std::normal_distribution<T> m_norm_dist;
    };


    template <class T>
    class RandomMatrix{
    public:
        RandomMatrix(size_t num, unsigned long steps,double mean, double stddev)
        :m_num(num),m_steps(steps),m_rand_num {num, mean, stddev},m_rand_matrix(num, std::vector<T> (steps, 0)){
        }
        void createRandMatrix(){
            for(size_t i = 0; i < m_num;++i){
                for (unsigned long j = 0; j < m_steps; ++j){
                    m_rand_matrix[i][j] = m_rand_num.getNormalRand(i);
                }
            }
        }
        T getRandNum(size_t i, unsigned long j){
            return m_rand_matrix[i][j];
        }
    private:
        size_t m_num;
        unsigned long m_steps;
        RandomNumber<T> m_rand_num;
        std::vector<std::vector<T>> m_rand_matrix;
    };

    class RateInterpolation{
    public:
        RateInterpolation(std::vector<double> tenors, std::vector<double> rates):m_rates(rates),m_tenors(tenors){};
        double getRate(double tenor){
            int index = std::lower_bound (m_tenors.begin(), m_tenors.end(), tenor) - m_tenors.begin();
            if(index == 0) return m_rates[0];
            else if(index >= m_tenors.size()) return m_rates.back();
            else{
                double slope = (m_rates[index] - m_rates[index-1])/(m_tenors[index] - m_tenors[index-1]);
                return m_rates[index-1] + slope*(tenor - m_tenors[index-1]);
            }
        }
    private:
        std::vector<double> m_tenors;
        std::vector<double> m_rates;
    };


    template <class T>
    class LiborRateSimulation{
    public:
        LiborRateSimulation(size_t paths,unsigned long steps, double maturity, double tau, int numer,
                            std::vector<T> init_rates, std::vector<T> init_tenors,
                            std::vector<T> sigma, std::vector<std::vector<T>> corr)
        :m_paths(paths),m_steps(steps),m_maturity(maturity),m_tau(tau),m_numer(numer),
        m_ri(init_tenors, init_rates),m_sigma(sigma),m_corr(corr),m_rand_matrix{paths, steps,0,1}{
            m_dt = maturity/steps;
        };

        std::vector<std::vector<T>> LiborSimulation(){
            double mu = 0;
            std::vector<T> F(m_paths+1, 0);
            for(int m = 0; m < F.size();++m)
                F[m] = m_ri.getRate(m);

            std::vector<std::vector<T>> rates(m_paths, std::vector<T> (m_steps, 0));
            for(unsigned long i = 0; i < m_steps;++i){//time step
                for(int j = 1; j <= m_paths; ++j){//multiple Libor paths
                    if(j < m_numer) {
                        mu = 0;
                        for (int k = j + 1; k < m_numer; ++k) {
                            mu -= (m_corr[j-1][k-1]* m_sigma[k-1]*m_sigma[j-1] * m_tau * F[k - 1])
                                                    / (1 + m_tau * F[k - 1]) *m_dt;
                        }
                    }
                    else{
                        mu = 0;
                        for (int k = m_numer; k <= j; ++k) {
                            mu += (m_corr[j-1][k-1] * m_sigma[k-1]*m_sigma[j-1] * m_tau * F[k - 1])
                                                        / (1 + m_tau * F[k - 1]) *m_dt;
                        }
                    }
                    F[j]=F[j]*exp(mu*m_dt - 0.5*m_sigma[j]*m_sigma[j]*m_dt+
                            m_sigma[j-1]*m_rand_matrix.getRandNum(j-1,i)*sqrt(m_dt));
                    rates[j-1][i]=F[j];
                }
            }
            return rates;
        }

    private:
        RandomMatrix<T> m_rand_matrix;
        RateInterpolation m_ri;
        size_t m_paths;//number of swap rates
        unsigned long m_steps;//number of steps
        std::vector<std::vector<T>> m_corr;//correlation matrix
        std::vector<T> m_sigma;//rate vol
        double m_maturity;//time to maturity of swaption
        double m_dt;//length of simulation step
        double m_tau;//fraction of forward rates
        int m_numer; //numeraire of underlying rate
    };
}

int main(){

    std::vector<double> tenors {1.0/12.0, 1.0/6.0, 0.25, 0.5, 1,2,3,5,7,10,20,30};
    std::vector<double> rates {0.09, 0.09, 0.09, 0.09, 0.1,0.16, 0.23,0.49,0.82,1.15,1.69, 1.88};
    simulationlib::RateInterpolation ri (tenors, rates);
    double rr = ri.getRate(3.1);


    size_t paths = 5;
    unsigned long steps = 100;
    double maturity = 5;
    double tau = 0.25;
    int numer = 2;
    std::vector<double> init_rates {0.05, 0.06, 0.07, 0.09, 0.1,0.16, 0.23,0.49,0.82,1.15,1.69, 1.88};
    std::vector<double> init_tenors {1.0/12.0, 1.0/6.0, 0.25, 0.5, 1,2,3,5,7,10,20,30};
    std::vector<double> sigma {0.3,0.4,0.5, 0.6, 0.7};
    std::vector<std::vector<double>> corr {{1,0.9, 0.8, 0.5, 0.85},{0.9,1,0.87, 0.95, 0.75},{0.8,0.87,1,0.65,0.4},
            {0.5,0.95,0.65,1,0.3},{0.85,0.75,0.4,0.3,1}};
    simulationlib::LiborRateSimulation<double> lmm_test{paths, steps,maturity, tau, numer, init_rates, init_tenors, sigma, corr};
    std::vector<std::vector<double>> LiborRates = lmm_test.LiborSimulation();

    return 1;
}