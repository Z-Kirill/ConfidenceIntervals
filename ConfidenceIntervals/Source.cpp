#include <algorithm>
#include <iostream>
#include <fstream>
#include "owens.h"

using namespace std;

struct Limits
{
	double estimate;
	double UpL;
	double DownL;
};

double Gaussian(double x, double a) //using cumulative distribution
{
	return 0.5 * (1 + erf((x - a) / sqrt(2)));
}
double DownLimit(double estimate, double CL)
{
	double p1 = estimate - 20;
	double p2 = estimate;
	double p3;
	double alpha = (1.0 - CL) / 2;
	while (abs(p1 - p2) > 0.001)
	{
		p3 = (p1 + p2) / 2;
		if ((Gaussian(p3, estimate) - alpha) > 0) p2 = p3;
		else if ((Gaussian(p3, estimate) - alpha) < 0) p1 = p3;
		else return p3;
	}
	return p1;
}
double UpLimit(double estimate, double CL)
{
	double p1 = estimate;
	double p2 = estimate + 20;
	double p3;
	double alpha = 1.0 - (1.0 - CL) / 2;
	while (abs(p1 - p2) > 0.001)
	{
		p3 = (p1 + p2) / 2;
		if ((Gaussian(p3, estimate) - alpha) > 0) p2 = p3;
		else if ((Gaussian(p3, estimate) - alpha) < 0) p1 = p3;
		else return p3;
	}
	return p1;
}

double Normal(double x, double a, double a0=0) //using cumulative distribution
{
	return 0.5 * (1 + erf((x - a) / sqrt(2)));
}

double SkewNormal(double x, double a, double a0) //using cumulative distribution
{
	return Normal(x, a, a0) - 2 * t(x - a, 0);
}

double UpLimit(double Distribution(double, double, double), double estimate, double CL, double a0)
{
	estimate = max(estimate, a0);

	double p1 = estimate;
	double p2 = estimate + 20;
	double p3;

	double alpha = 1.0 - (1.0 - CL) / 2;

	while (abs(p1 - p2) > 0.001)
	{
		p3 = (p1 + p2) / 2;
		if ((Distribution(p3, estimate, a0) - alpha) > 0) p2 = p3;
		else if ((Distribution(p3, estimate, a0) - alpha) < 0) p1 = p3;
		else break;
	}

	return p3;
}

double DownLimit(double Distribution(double, double, double), double estimate, double CL, double a0)
{
	double p1 = estimate - 20;
	double p2 = estimate;
	double p;

	double q1 = estimate - 20;
	double q2 = estimate;
	double q;

	double alpha = (1.0 - CL) / 2;
	double beta = 2 * alpha;

	while (abs(p1 - p2) > 0.001)
	{
		p = (p1 + p2) / 2;
		if ((Distribution(p, estimate, a0) - alpha) > 0) p2 = p;
		else if ((Distribution(p, estimate, a0) - alpha) < 0) p1 = p;
		else break;
	}

	while (abs(q1 - q2) > 0.001)
	{
		q = (q1 + q2) / 2;
		if ((Distribution(q, estimate, a0) - beta) > 0) q2 = q;
		else if ((Distribution(q, estimate, a0) - beta) < 0) q1 = q;
		else break;
	}

	double r = UpLimit(Distribution, a0, CL, a0);

	if (a0 > q) return a0;
	if (q > r) return max(p, r);
	return q;
}
 
int main()
{
	ofstream fout("fout.txt");
	double CL; //confidence level
	CL = 0.9;
	double a0; //apriori parameter
	a0 = 0;
	Limits L[601];
	double a = -1.0;
	for (int i = 0; i < 601; ++i, a += 0.01)
	{
		L[i].estimate = a;
		L[i].DownL = DownLimit(SkewNormal, a, CL, 0);
		L[i].UpL = UpLimit(SkewNormal, a, CL, 0);
		fout << L[i].estimate << ' ' << L[i].DownL << ' ' << L[i].UpL << endl;
	}
	cout << CL << endl;
} 