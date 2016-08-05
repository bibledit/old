#include <iostream>
#include <limits>
#include <random>

using namespace std;

int main()
{
    default_random_engine eng ((random_device())());

    uniform_int_distribution <int> idis (0, numeric_limits <int>::max());

    for (int i = 0; i < 10; ++i) {
        int r = idis (eng);
        cout << r << endl;
    }

}
