#include "fl.h"
void 	get_set_domain(t_fuzzvariable_set *s)
{
	double min = 100000;
	double max = -min;

	double v;
	double n;
	double w; 
	double x;
	if (!(strcmp(s->wave, TOKEN_W_TSK)))
	{
		s->min = -1; s->max = 1;
		return ;
	}
	else if (!strcmp(s->wave,TOKEN_W_ENDSIGMOIDE) || !strcmp(s->wave , TOKEN_W_BEGINSIGMOIDE))
	{
		s->min = s->range[0] ;
		s->max = s->range[1] ;
	}
	else if (!strcmp(s->wave ,TOKEN_W_PSHAPE))
	{
		s->min = s->range[0] ;
		s->max = s->range[3] ;
	}
	else if (!strcmp(s->wave,TOKEN_W_TRIANGLE)
			|| !strcmp(s->wave, TOKEN_W_TRAPEZE))
	{
		x = (double)!strcmp(s->wave, TOKEN_W_TRIANGLE);

		//printf("X=%f\n",x);
		if (s->range[0] < min)
			min = s->range[0];
		if (x == 0)
		{
			if (s->range[3] > max)
				max = s->range[3];
		}
		else
		{
			if (s->range[2] > max)
				max = s->range[2];

		}
		if (s->range[0] == s->range[1])
		{
			v = s->range[0];
			if (v < min)
				min = v;
		}
		if (x == 0)
		{
			if (s->range[3] == s->range[2])
			{
				v = s->range[3];
				if (v > max)
					max = v;
			}
		}
		else
		{
			if (s->range[2] == s->range[1])
			{
				v = s->range[2];
				if (v > max)
					max = v;
			}

		}
		s->min = min;
		s->max = max;
	}
	else if (!strcmp(s->wave ,TOKEN_W_GAUSSIAN))
	{
		n = s->range[1];
		while (gaussian(n, s->range[0], s->range[1]) > 0.01)
			n--;
		if (n < min)
			min = n;
		n = s->range[1] ;
		while (gaussian(n, s->range[0], s->range[1]) > 0.01)
			n++;
		if (n > max)
			max = n;
		s->min = min;
		s->max = max;

	}
	else if (!strcmp(s->wave ,TOKEN_W_SIGMOIDE))
	{
		w = s->range[0] >= 0 ? 1 : -1;
		x = 0;
		n = s->range[1] ;
		if (w >=0)
		{
			while ( 1)
			{
				x = sigmoide(n, s->range[0], s->range[1]);
				if (x <= 0.1)
					break;
				n -= 0.1;
			}
			if (n < min)
				min = n;
			n = s->range[1] ;
			while ( 1)
			{
				x = sigmoide(n, s->range[0], s->range[1]);
				if (x >= 0.9)
					break;
				n += 0.1;
			}
			if (n > max)
				max = n;
		}
		else
		{
			while ( 1)
			{
				x = sigmoide(n, -s->range[0], s->range[1]);
				if (x <= 0.1)
					break;
				n -= 0.1;
			}
			if (n < min)
				min = n;
			n = s->range[1] ;
			while ( 1)
			{
				x = sigmoide(n, -s->range[0], s->range[1]);
				if (x >= 0.9)
					break;
				n += 0.1;
			}
			if (n > max)
				max = n;

		}
		s->min = min;
		s->max = max;

	}
	else{printf("should be error on subset_get_domain\n");}
	s->center = centroideMethod(s);
}
double centroideMethod(t_fuzzvariable_set *mf)
{
	double k=50;                        // The function is divided into "k" steps
	double min = mf->min;
	double max = mf->max;
	double step=(max - min)/k;         // Calculate the step function
	//+------------------------------------------------------------------+
	//| Calculate a center of gravity as integral                        |
	//+------------------------------------------------------------------+
	double ptLeft=0.0;
	double ptCenter= 0.0;
	double ptRight = 0.0;
	double valLeft=0.0;
	double valCenter= 0.0;
	double valRight = 0.0;
	double val2Left=0.0;
	double val2Center= 0.0;
	double val2Right = 0.0;
	double numerator=0.0;
	double denominator=0.0;
	for(double i=0; i<k; i++)
	{
		if(i==0)
		{
			ptRight=min;
			valRight=mf->fuzzyfy(ptRight, mf);
			val2Right=ptRight*valRight;
		}
		ptLeft=ptRight;
		ptCenter= min+step *(i+0.5);
		ptRight = min+step *(i+1);
		valLeft=valRight;
		valCenter= mf->fuzzyfy(ptCenter, mf);
		valRight = mf->fuzzyfy(ptRight, mf);
		val2Left=val2Right;
		val2Center= ptCenter * valCenter;
		val2Right = ptRight * valRight;
		numerator+=step *(val2Left+4*val2Center+val2Right)/3.0;
		denominator+=step *(valLeft+4*valCenter+valRight)/3.0;
	}
	if(denominator!=0)
	{
		//--- return result
		return (numerator / denominator);
	}
	//--- return NAN
	return (log(-1));
}



double triangle_fuzzyfy(double x, t_fuzzvariable_set *this)
{
	double r = 0;

	// case R
	if (this->range[0] == this->range[1])
	{
		if (x > this->range[2])
			r= 0;
		else if (this->range[1] <= x && x <= this->range[2])
			r = (this->range[2] - x) / (this->range[2] - this->range[1]);
		else if (x < this->range[1])
			r = 1;
		return r;
	}
	// case L
	else if (this->range[2] == this->range[1])
	{
		if (x < this->range[0])
			r= 0;
		else if (this->range[0] <= x && x <= this->range[1])
			r = (x - this->range[0] ) / (this->range[1] - this->range[0]);
		else if (x > this->range[1])
			r = 1;
		return r;
	}


	if (x > this->range[0] && x <= this->range[1])
	{
		r = (x - this->range[0]) / (this->range[1] - this->range[0]);
	}
	else if (x > this->range[1] && x <= this->range[2])
	{
		r =  ( (this->range[2] - x) / (this->range[2] - this->range[1]));
	}
	return r;
}

double trapez_fuzzyfy(double x, t_fuzzvariable_set *this)
{
	double r = 0;
	// case R
	if (this->range[0] == this->range[1])
	{
		if (x > this->range[3])
			r= 0;
		else if (this->range[2] <= x && x <= this->range[3])
			r = (this->range[3] - x) / (this->range[3] - this->range[2]);
		else if (x < this->range[2])
			r = 1;
		return r;
	}
	// case L
	else if (this->range[2] == this->range[3])
	{
		if (x < this->range[0])
			r= 0;
		else if (this->range[0] <= x && x <= this->range[1])
			r = (x - this->range[0] ) / (this->range[1] - this->range[0]);
		else if (x > this->range[1])
			r = 1;
		return r;
	}
	//
	if ( x < this->range[0] || x > this->range[3])
		r = 0;
	else if ( this->range[0] <= x && x <= this->range[1])
		r = (x - this->range[0]) / (this->range[1] - this->range[0]);
	else if (this->range[1] <= x && x <= this->range[2])
		r = 1;
	else if (this->range[2] <= x && x <= this->range[3])
		r = (this->range[3] - x) / (this->range[3] - this->range[2]);
	return r;
}

double gaussian_fuzzyfy(double x, t_fuzzvariable_set *this)
{
	//  double div = Math.pow(d, 2) * 2;
	//        double num = -(Math.pow(x-c, 2));
	//        return Math.exp((num / div), 1);

	double div;
	double num;
	div = pow(this->range[0], 2) * 2;
	num = -(pow(x-this->range[1],2));
	return exp(num/div);
	//return gaussian(x, this->range[0], this->range[1])
}


double sigmoide_fuzzyfy(double x, t_fuzzvariable_set *this)
{
	return 1 / ( exp(-this->range[0] * (x-this->range[1])) + 1 );

	//	return sigmoide(x, this->range[0], this->range[1])
}
double endsigmoide_fuzzyfy(double x, t_fuzzvariable_set *this)
{
	double m_a = this->range[0];
	double m_b = this->range[1];
	if(x<=m_a)
	{
		return (0.0);
	}
	else if(m_a<x && x<=(m_a+m_b)/2.0)
	{
		return (2.0*((x-m_a)/(m_b-m_a))*((x-m_a)/(m_b-m_a)));
	}
	else if((m_a+m_b)/2.0<x && x<m_b)
	{
		return (1.0 - 2.0*((x - m_b)/(m_b - m_a))*((x - m_b)/(m_b - m_a)));
	}
	else
	{
		return (1.0);
	}
}
double beginsigmoide_fuzzyfy(double x, t_fuzzvariable_set *this)
{
	double m_a = this->range[0];
	double m_b = this->range[1];
	if(x<=m_a)
	{
		//--- return result
		return (1.0);
	}
	else if(m_a<x && x<=(m_a+m_b)/2.0)
	{
		//--- return result
		return (1.0 - 2.0*((x - m_a)/(m_b - m_a))*((x - m_a)/(m_b - m_a)));
	}
	else if((m_a+m_b)/2.0<=x && x<=m_b)
	{
		//--- return result
		return (2.0*((x-m_b)/(m_b-m_a))*((x-m_b)/(m_b-m_a)));
	}
	else
	{//--- x >= m_b
		//--- return result
		return (0.0);
	}

}
double pshape_fuzzyfy(double x, t_fuzzvariable_set *this)
{
	double m_a = this->range[0];
	double m_b = this->range[1];
	double m_c = this->range[2];
	double m_d = this->range[3];
	if(x<=m_a)
	{
		return(0.0);
	}
	else if(m_a<x && x<=(m_a+m_b)/2.0)
	{
		return(2.0 *((x-m_a)/(m_b-m_a))*((x-m_a)/(m_b-m_a)));
	}
	else if((m_a+m_b)/2.0<x && x<m_b)
	{
		return(1.0-2.0 *((x-m_b)/(m_b-m_a))*((x-m_b)/(m_b-m_a)));
	}
	else if(m_b<=x && x<=m_c)
	{
		return(1.0);
	}
	else if(m_c<x && x<=(m_c+m_d)/2.0)
	{
		return(1.0-2.0 *((x-m_c)/(m_d-m_c))*((x-m_c)/(m_d-m_c)));
	}
	else if((m_c+m_d)/2.0<x && x<m_d)
	{
		return(2.0 *((x-m_d)/(m_d-m_c))*((x-m_d)/(m_d-m_c)));
	}
	else
	{
		return(0.0);
	}
	return 0;
}

