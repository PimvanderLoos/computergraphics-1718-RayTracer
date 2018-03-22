#ifndef SOLVERS_H_
#define SOLVERS_H_

class Solvers
{
    public:
        // Solve quadratic function
        // return false if no solution
        // x0 <= x1
        // uses pass by reference (hence the &)
    	static bool quadratic(double a, double b, double c, double &x0, double &x1);

		static bool quartic(double a, double b, double c, double d, double e, double *roots);

		static void cubic(double c3, double c2, double c1, double c0, double &x1, double &x2, double &x3, int &nr);
};

#endif
