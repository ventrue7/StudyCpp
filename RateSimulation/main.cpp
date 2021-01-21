#include <iostream>
#include <random>
#include <vector>
#include <math.h>
#include <algorithm>
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
            createRandMatrix();
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
        double getRate(double t1, double t2){
            return (getRate(t2)*t2-getRate(t1)*t1)/(t2-t1);
        }
    private:
        std::vector<double> m_tenors;
        std::vector<double> m_rates;
    };


    template <class T>
    class LiborRateSimulation{
    public:
        LiborRateSimulation(int simulation_nums, double projection_years,unsigned long num_time_steps, double maturity,
                            double rate_freq,std::vector<T> rates, std::vector<T> tenors)
        :m_simulation_nums(simulation_nums),m_projection_years(projection_years),m_num_time_steps(num_time_steps),m_maturity(maturity),
         m_rate_freq(rate_freq),m_ri(tenors, rates),m_init_rates(maturity/rate_freq-1, 0),
        m_sigma(maturity/rate_freq-1, 0), m_corr(maturity/rate_freq-1, std::vector<T> (maturity/rate_freq-1,0)),
        m_output(m_simulation_nums){
            m_dt = projection_years/num_time_steps;
            m_num_rates = maturity/rate_freq-1;
        };

        void setInitRate(){//get initial rates from time 0 rate curve
            for(int i = 1; i <= m_num_rates;++i)
                m_init_rates[i-1] = m_ri.getRate(m_rate_freq*i,m_rate_freq*i+m_rate_freq);
        }

        void setCorr(double rho_inf, double lambda, double kai){
            for(int i = 1; i <= m_num_rates;++i){
                for(int j = 1; j <= m_num_rates;++j){
                    m_corr[i-1][j-1] = rho_inf+(1-rho_inf)*
                            exp(-lambda*abs(i*m_rate_freq-j*m_rate_freq)/(1+kai*std::min(i*m_rate_freq,j*m_rate_freq)));
                }
            }
        }
        T getCorr(int i, int j){
            return m_corr[i-1][j-1];
        }


        void setVol(double a, double b, double c, double d){
            for(int i = 1; i <= m_num_rates;++i){//this vol form allows a humped shape of instantaneous volatility
                m_sigma[i-1] = (a*(i*m_rate_freq)+d)*exp(-b*(i*m_rate_freq))+c;
            }
        }

        T getVol(double t){
            return m_sigma[t/m_rate_freq-1];
        }

        T validateVol(int rate_index, std::vector<int> vol_tenors){
            //Target Variance
            T target_var = 0.0;
            for(int i = 0; i < vol_tenors.size(); ++i){//loop all the vol_tenors
                target_var+=getVol(vol_tenors[i]*m_rate_freq)*getVol(vol_tenors[i]*m_rate_freq)*m_dt;
            }

            //Calculated Variance
            std::vector<T> selected_rates(m_simulation_nums, 0.0);
            std::vector<T> selected_norm_rates(m_simulation_nums, 0.0);
            T square_sum = 0.0, sum = 0.0,calc_var = 0.0;
            for(int i = 0; i < m_simulation_nums; ++i){//number of simulation
                selected_rates[i]= m_output[i][rate_index-1];
                selected_norm_rates[i] = log(selected_rates[i]);
                square_sum += selected_norm_rates[i]*selected_norm_rates[i];
                sum += selected_norm_rates[i];
            }
            calc_var = square_sum/(m_simulation_nums-1) - sum*sum/(m_simulation_nums-1)/m_simulation_nums;

            T test_stat = (m_simulation_nums-1)*sqrt(calc_var/target_var);//Test Statistic
            
            std::cout<<"Test Statistic: "<<test_stat<<", Degree of Freedom: "<<m_simulation_nums-1<<std::endl;
            std::cout<<"Calculated Variance: "<<calc_var<<", Target Variance: "<<target_var
            <<", ABS Error: "<<abs(calc_var-target_var)<<", Percentage Error: "<<calc_var/target_var - 1.0<<std::endl;

            return calc_var/target_var - 1.0;
        }

        std::vector<std::vector<T>> LiborSimulation(){
            for(int i = 0; i < m_simulation_nums; ++i){//number of simulation
                m_output[i]= LiborSimulationOnePath();
            }
            return m_output;
        }

        std::vector<T> LiborSimulationOnePath(){
            RandomMatrix<T> rand_matrix(m_num_rates, m_num_time_steps,0,1);//generate random number matrix
            double mu = 0;//drift term
            std::vector<T> F = m_init_rates;

            for(unsigned long i = 0; i < m_num_time_steps;++i){//time step
                for(int j = 1; j <= m_num_rates; ++j){//Libor rates
                    mu = 0;
                    for (int k = 1; k <= j; ++k) {//calculate drift term
                        mu += (getCorr(j,k) * getVol(k*m_rate_freq) * getVol(j*m_rate_freq) * m_rate_freq * F[k - 1])
                              / (1 + m_rate_freq * F[k - 1]) * m_dt;
                    }
                    F[j-1]=F[j-1]*exp(mu*m_dt - 0.5*getVol(j*m_rate_freq)*getVol(j*m_rate_freq)*m_dt+
                                              getVol(j*m_rate_freq)*rand_matrix.getRandNum(j-1,i)*sqrt(m_dt));
                }
            }
            return F;
        }

    private:
        RateInterpolation m_ri;

        int m_simulation_nums;

        //simulation steps
        double m_projection_years;//simulation years
        unsigned long m_num_time_steps;//number of steps
        double m_dt;//length of simulation step

        //Libor Curves
        double m_maturity;//time to maturity of Libor Curve
        double m_rate_freq;//libor frequency
        int m_num_rates;//number of Libor rates

        std::vector<T> m_init_rates;
        std::vector<std::vector<T>> m_corr;//correlation matrix
        std::vector<T> m_sigma;//rate vol
        std::vector<std::vector<T>> m_output;
    };
}

int main(){

    int simulation_nums = 1000000;//number of simulations

    double projection_years = 0.75;//years of projection
    unsigned long num_time_steps = 3;//number of time steps
    //projection_years/num_time_steps = length of each time step

    double maturity = 1;//term structure maturity
    double rate_freq = 0.25;//libor length
    //maturity/rate_freq = number of rates in term structure

    //time 0 rates
    std::vector<double> init_rates {0.0005, 0.0006, 0.0007, 0.0009, 0.001,0.0016, 0.0023,0.0049,0.0082,0.0115,0.0169, 0.0188};
    std::vector<double> init_tenors {1.0/12.0, 1.0/6.0, 0.25, 0.5, 1,2,3,5,7,10,20,30};
    simulationlib::LiborRateSimulation<double> lmm_test{simulation_nums,projection_years, num_time_steps,maturity,
                                                        rate_freq, init_rates, init_tenors};

    //set vol assumption
    double a = 0.19, b = 0.97, c = 0.08, d = 0.01;
    lmm_test.setVol(a, b, c, d);

    //set correlation assumption
    double rho_inf = 0.99, lambda = 0.5, kai = 0.5;
    lmm_test.setCorr(rho_inf, lambda, kai);

    //set time 0 rates
    lmm_test.setInitRate();

    //simulation
    std::vector<std::vector<double>> libor_rates = lmm_test.LiborSimulation();


    int rate_index = 3;
    std::vector<int> vol_tenors{3,3,3};
    double error = lmm_test.validateVol(rate_index, vol_tenors);
    return 1;
}