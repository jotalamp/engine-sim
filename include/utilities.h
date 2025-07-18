#ifndef ATG_ENGINE_SIM_UTILITIES_H
#define ATG_ENGINE_SIM_UTILITIES_H

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define DBG std::cout << "[" << __FILENAME__ << ":" << __LINE__ << "]\n";
#define D(s) std::cout << "[" << __FILENAME__ << ":" << __LINE__ << "]\t" << s << std::endl;

double modularDistance(double a, double b, double mod = 1.0);
double positiveMod(double x, double mod);
double erfApproximation(double x);

template <typename t>
inline t clamp(t x, t x0 = static_cast<t>(0.0), t x1 = static_cast<t>(1.0))
{
    if (x <= x0)
        return x0;
    else if (x >= x1)
        return x1;
    else
        return x;
}

#endif /* ATG_ENGINE_SIM_UTILITIES_H */
