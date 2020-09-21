#include "data.h"

#include <random>

using namespace shogun;

SGMatrix<float64_t> LinSpace(double s, double e, size_t n)
{
    SGMatrix<float64_t> x_values(1, n);
    double step = (e - s) / n;
    double v = s;
    for (size_t i = 0; i < n ;i++)
    {
        x_values.set_element(v, 0, i);
        v += step;
    }
    x_values.set_element(e, 0, n - 1);

    return x_values;
}

std::pair<shogun::SGMatrix<float64_t>, shogun::SGVector<float64_t> > GenarateData(size_t num_sample, size_t seed, bool no_noise)
{
    SGMatrix<float64_t> x_values(1, num_sample);
    SGVector<float64_t> y_values(num_sample);

    std::mt19937 re(seed);
    std::normal_distribution<float64_t> dist;

    for (size_t i = 0; i < num_sample; i++)
    {
        auto x_val = dist(re);
        x_values.set_element(x_val, 0, i);

        auto y_val = std::cos(M_PI * x_val) + (no_noise ? 0 : (dist(re) * 0.3));
        y_values.set_element(y_val, i);
    }
    return {x_values, y_values};
}
