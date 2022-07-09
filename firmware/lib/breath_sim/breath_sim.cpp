/*
Public Invention's Ox Project is an open source hardware design for an oxygen
concentrator for use by field hospitals around the world. This team aims to
design an oxygen concentrator that can be manufactured locally while overcoming
challenges posed by human resources, hospital location (geographically),
infrastructure and logistics; in addition, this project attempts the minimum
documentation expected of their design for international approval whilst
tackling regulatory requirements for medical devices. Copyright (C) 2021
Robert Read, Ben Coombs, and Darío Hereñú.

This program includes free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

// Real-time breathing simulator

#include <iostream>
#include <chrono>
#include <cmath>

// sudo apt install libboost-all-dev
#include <boost/numeric/odeint.hpp>

using namespace std::chrono;
using namespace boost::numeric::odeint;
typedef runge_kutta_dopri5<double> stepper_type;

////////// HELPER FUNCTIONS //////////

// Time since Linux epoch in milliseconds.
uint64_t TimeSinceEpochMs()
{
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

////////// LUNG MODEL //////////

class LungModel
{
    private:
        float C_C;      // Collapsible airway compliance (l/cmH20)
        float C_cw;     // Chest wall compliance (l/cmH20)
        float C_l;      // Lung compliance (l/cmH20)
        float C_syst;   // Compliance of the lung-chest wall system (l/cmH20)
        float F;        // Airflow at the mouth (cmH20)
        float P_A;      // Alveolar pressure (cmH20)
        float P_C;      // Collapsible airway pressure (cmH20)
        float P_cw;     // Chest wall pressure (cmH20)
        float P_el;     // Dynamic lung elastic recoil (cmH20)
        float P_ext;    // Pressure at mouth (cmH20)
        float P_l;      // Static lung elastic recoil (cmH20)
        float P_mus;    // Muscular pressure (cmH20)
        float P_pl;     // Plural pressure (cmH20)
        float P_ref;    // Environmental pressure (cmH20)
        float P_syst;   // Elastic recoil of the lung-chest wall system (cmH20)
        float P_tm;     // Transmural pressure (cmH20)
        float R_C;      // Collapsible airway resistance (cmH2*l-1*s)
        // float R_LT;     // Lung tissue resistance (cmH2*l-1*s)
        float R_S;      // Small airway resistance (cmH2*l-1*s)
        float R_U;      // Upper airway resistance (cmH2*l-1*s)
        // float TLC;      // Total lung capacity (l)
        float V_A;      // Aveolar space volume (l)
        float V_C;      // Collapsible airway volume (l)
        float V_cw;     // Chest wall volume (l)
        // float V_D;      // Dead space volume (l)
        float V_L;      // Total lung volume (l)
        // float V_R;      // Residual volume (l)

        // Model parameters
        float A_c = 7.09;
        float A_c_dash = 5.6;
        float A_l = 0.2;
        float A_s = 2.2;
        float A_u = 0.34;
        float B_c = 37.3;
        float B_c_dash = 3.73;
        float B_cw = 3.5;
        float B_l = 0.5;
        float B_s = 0.02;
        float D_c = 0.7;
        float D_c_dash = 0.999;
        float D_cw = 0.999;
        float F_min = -6.2;
        float F_max = 8.3;
        float K_l = 1.0;
        float K_s = 10.9;
        float K_u = 0.46;
        float K_c = 0.21;
        float R_LT = 0.2;
        float TLC = 5.19;
        float V_star = 5.3;
        float V0_A = 2.2;
        float V0_C = 0.05;
        float V_Cmin = 0.015;
        float V_Cmax = 0.185;
        float V_D = 0.05;
        float V_R = 1.42;

    public:
        // solve wrt V_A and V_C over [0, T]
        // initial conditions: V_A(0) = V0_A and V_C(0) = V0_C
        // P_pl is the input of the model
        // With a ventilator P_ext is non-zero
        void system_of_equations() {
            //(R_U + R_C) * V_C + (R_U + R_C) * V_A + P_tm + P_pl = 0;
            //(R_S + R_LT) * V_A + P_l - P_tm = 0;
            R_U = A_u + K_u * abs(F);
            R_C = K_c * pow((V_Cmax / V_C), 2);
            R_S = A_s * exp(-K_s * ((V_A - V_R)/(V_star - V_R))) + B_s;
            P_tm = A_c - B_c * pow(((V_C/V_Cmax) - D_c), 2); // when V_Cmin <= V_C <= (V_Cmax/2)
            P_tm = A_c_dash - B_c_dash * log((V_Cmax/V_C) - D_c_dash); // when (V_Cmax/2) < V_C <= V_Cmax
            P_l = A_l * exp(K_l * V_A) - B_l;
        }
};

class PleuralPressureWaveformGenerator {
    private:
        float RR = 12; // respiratory rate, typically 12 - 18 breaths per minute
    public:
        void set_respiratory_rate(float rr) {
            if (rr > 0) {
                RR = rr;
            } else {
                // rr must be positive
                std::cout << "Respiratory rate must be positive!" << std::endl;
            }
        }
};

class RegularBreathingWaveform : PleuralPressureWaveformGenerator {
    private:
        // regular breathing - model 1
        float P_r_pl;
        float a_r = -4.0785;
        float b_r = -2.719;
        float c_r = 0.40785;
        float T_r_r = 5.0;
        float t_l_r = 4.25; // 0.85*T_r
        float v_off_r = 2.474;
    public:
        // model 1 - equation 26
        void model(float t) {
            if (t >=0 && t < t_l_r) {
                P_r_pl = a_r + b_r * sin((M_PI * t) / t_l_r); // + v_off?
            } else if (t >= t_l_r && t <= T_r_r) {
                P_r_pl = a_r + c_r * sin ((M_PI * (t - t_l_r)) / (T_r_r - t_l_r));  // + v_off?
            } else {
                // t is out of range
                std::cout << "t is out of range" << std::endl;
            }
        }
};

class QuietBreathingWaveform : PleuralPressureWaveformGenerator {
    private:
        // quiet breathing - model 2
        float P_q_pl;
        float C_q = 1.0;
        float R_q = 0.3;
        float Amp_q = 1.0;
        float t_p_q = 2.0;
        float T_r_q = 5.0;
        float phi_q = 0;
        float g_q = -2.4471;
        float v_off_q = -1.7075;
    public:
        // model 2 - equation 27
        // in quiet breathing, pressure is less than atmosphere
        void model(float t) {
            //float q = R_q * C_q * P_dot + P_q
            // TODO: get equation from reference
        }
};

class TidalBreathingWaveform : PleuralPressureWaveformGenerator {
    private:
        // tidal breathing - model 3
        float P_t_pl;
        float C_t = 2.0;
        float R_t = 3;
        float Amp_t = -4.0;
        float t_p_t = 1.5;
        float T_r_t = 5.0;
        float phi_t = 0;
        float g_t = 1.0;
        float v_off_t = -2.635;
    public:
        void model(float t) {
            // TODO: get equation from reference
        }
};

////////// LUNG STATE MACHINE //////////

enum class LungCondition
{
    Good,
    Asthmatic,
    Covid,
    Pneumonia,
    Collapsed
};

class Lungs
{
private:
    uint64_t _volume;
    uint64_t _compliance;
    uint64_t _last_ms;
    uint64_t _delta_ms;
    LungCondition _condition;

public:
    Lungs()
    {
        _volume = 700;
        _compliance = 50;
        _last_ms = 0;
        _delta_ms = 0;
        _condition = LungCondition::Good;
    }
    Lungs(uint64_t volume, uint64_t compliance, LungCondition condition)
    {
        _volume = volume;
        _compliance = compliance;
        _last_ms = 0;
        _delta_ms = 0;
        _condition = condition;
    }
    bool Step(uint64_t ms)
    {
        if (ms < _last_ms)
        {
            std::cout << "Time must go forward!" << std::endl;
            return false;
        }
        _delta_ms = ms - _last_ms;
        _last_ms = ms;

        // Calculate next step
        // TODO
        std::cout << "Step..." << std::endl;


        return true;
    }
    void SetState(LungCondition condition)
    {
        _condition = condition;
    }
    LungCondition GetState()
    {
        return _condition;
    }
};

////////// PATIENT STATE MACHINE //////////

class Patient
{
private:
    int _mass;
    int _age;
    Lungs *_lungs;

public:
    Patient()
    {
        _mass = 80;
        _age = 40;
        Lungs _lungs(700, 50, LungCondition::Good);
    };
    bool Step(int ms) {
        _lungs->Step(ms);
        return true;
    }
};

////////// MAIN //////////

int main(int argc, char **argv)
{
    std::cout << "Lung sim" << std::endl;
    //Patient patient;
    Lungs lungs;

    uint64_t last_ms = TimeSinceEpochMs();
    for (int i = 0; i < 10; i++)
    {
        uint64_t ms = last_ms + 1;
        //uint64_t ms = TimeSinceEpochMs();
        if (ms > last_ms) {
            std::cout << "ms: " << ms << std::endl;
            lungs.Step(ms);
            last_ms = ms;
        }
        // bool result = patient.Step(ms);
        // if (result == false) {
        //     return 1; // Program failure
        // }
    }

    return 0;
}