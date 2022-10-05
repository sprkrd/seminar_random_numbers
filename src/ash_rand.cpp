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

#include <ash_rand/rng.hpp>

#include <iostream>
using namespace std;

int main() {
    auto seed = ash_rand::produce_random_seed();
    ash_rand::randu rng(seed);
    cout << ash_rand::randu::max() << endl;
    cout << rng() << endl;
    cout << rng() << endl;
    cout << rng() << endl;
    cout << rng() << endl;
    cout << rng() << endl;
}

