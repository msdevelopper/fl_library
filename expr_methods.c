/** \file expr_methods.c */
#include <math.h>
#include "expr.h"
double sigmoide(double x, double a, double b)
{
	return 1 / ( exp(-a * (x-b)) + 1 );
}
double gaussian(double x, double a , double b)
{
	double div;
	double num;
	div = pow(a, 2) * 2;
	num = -(pow(x-b,2));
	return exp(num/div);
}
/*! \brief cosinus function
 *	@param[in] l array of arguments
 *	@return cos value of l[0]
 */
double d_cos(double l[5])
{
	return cos(l[0]);
}
/*! \brief acosinus function
 *	@param[in] l array of arguments
 *	@return acos value of l[0]
 */
double d_acos(double l[5])
{
	return acos(l[0]);
}
/*! \brief sinus function
 *	@param[in] l array of arguments
 *	@return sin value of l[0]
 */
double d_sin(double l[5])
{
	return sin(l[0]);
}
/*! \brief sigmoide function
 *	@param[in] l array of arguments
 *	@param[l[0]]  input value,
 *	@param[l[1]] inflexion of the curve
 *	@param[l[2]] is the point on curve where sigmoide(x)=0.5
 *	@return sigmoide result 
 */
double d_sigm(double l[5])
{
	return sigmoide(l[0], l[1], l[2]);
}
/*! \brief gaussian function
 *	@param[in] l array of arguments
 *	@param[l[1]] inflexion of the curve
 *	@param[l[2]] is the point on curve where gaussian(x)=1
 *	@return gaussian
 */
double d_gauss(double l[5])
{
	return gaussian(l[0], l[1], l[2]);
}
/*! \brief lt stand for 'less than'
 *	@param[in] l array of arguments
 *	@return  l[2] if l[0] less than l[1] otherwise l[0]
 */
double d_lt(double l[5])
{
	return l[0] < l[1] ? l[2] : l[0];
}
/*! \brief gt stand for 'greater than'
 *	@param[in] l array of arguments
 *	@return  l[2] if l[0] greater than l[1] otherwise l[0]
 */
double d_gt(double l[5])
{
	return l[0] > l[1] ? l[2] : l[0];
}
/*! \brief normalise function
 *	@param[in] l array of arguments
 *	@param[l[0]] input value 
 *	@param[l[1]] minimum value of the domain where input value stand  
 *	@param[l[2]] maximum value of the domain where input value stand  
 *	@param[l[3]] the output domain where output value stand between [0 , l[3]]
 *	@return normalised value
 */
double d_norm(double l[5])
{
	return ((l[0] - l[1]) / (l[2] - l[1]))*l[3];
}
/*! \brief normalise function
 *	@param[in] l array of arguments
 *	@param[l[0]] input value 
 *	@param[l[1]] minimum value of the domain where input value stand  
 *	@param[l[2]] maximum value of the domain where input value stand  
 *	@param[l[3]] the smallest value of the output domain 
 *	@param[l[4]] the greatest value of the output domain 
 *	@return normalised value in range [ l[3], l[4] ]
 */
double d_normdomain(double l[5])
{
	return l[3] + ((l[0] - l[1]) / (l[2] - l[1])) * (l[4] - l[3]);
}
