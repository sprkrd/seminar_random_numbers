//Experiments with random number generation

//Copyright (C) 2022 Alejandro Suárez Hernández

//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "ash_rand/rng.hpp"

#include <cmath>
#include <iostream>
using namespace std;


constexpr int k_number_of_points = 1000000;

using Random = ash_rand::pcg32;

int main() {
    //uint64_t seed = ash_rand::produce_random_seed();
    uint64_t seed = 42;
    Random rng(seed);

    //int count_inside_circle = 0;
    //std::uniform_real_distribution unif_dist(-1.0, 1.0);
    //for (int i = 0; i < k_number_of_points; ++i) {
        //double x = unif_dist(rng);
        //double y = unif_dist(rng);

        //double dist_sq = x*x + y*y;
        //count_inside_circle += dist_sq < 1;
    //}
    
    
    int count_inside_sphere = 0;
    std::uniform_real_distribution unif_dist(-1.0, 1.0);
    for (int i = 0; i < k_number_of_points; ++i) {
        double x = unif_dist(rng);
        double y = unif_dist(rng);
        double z = unif_dist(rng);

        double dist_sq = x*x + y*y + z*z;
        count_inside_sphere += dist_sq < 1;
    }

    //double pi_hat = (4.0/k_number_of_points) * count_inside_circle;
    double pi_hat = (6.0/k_number_of_points) * count_inside_sphere;
    double error_abs = std::fabs(pi_hat - M_PI);
    double error_rel = 100 * error_abs / M_PI;
    cout << "Pi estimation: " << pi_hat << endl;
    cout << "Absolute error: " << error_abs << endl;
    cout << "Relative error: " << error_rel << "%" << endl;
}

