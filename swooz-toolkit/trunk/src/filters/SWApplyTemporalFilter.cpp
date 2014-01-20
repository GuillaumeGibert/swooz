
/**
 * \file SWApplyTemporalFilter.cpp
 * \brief Defines SWApplyTemporalFilter
 * \author Guillaume Gibert
 * \date 01/06/13
 */

#include "filters\SWApplyTemporalFilter.h"

using namespace itpp;

void SWApplyTemporalFilter::ComputeFilterInitialCondition(const vec & b, const vec & a, vec &zi)
{
	int na, nb,  j, i;
	na = length(a);
	nb = length(b);

	//--------------------------------------
	// use sparse matrix to solve system of linear equations for initial conditions
	// zi are the steady-state states of the filter b(z)/a(z) in the state-space 
	//implementation of the 'filter' command.

	mat eye_1;
	eye_1 = eye(na-1);

	mat eye_2;
	eye_2 = eye(na-2);

	vec a1(na-1);
	vec zeros_1(na-2);
	zeros_1 = zeros(na-2);
	vec b1(na-1);
	vec a2(na-1);

	for(j=1 ; j<na; j++)
	{
		a1[j-1] = - a[j];
		b1[j-1] =   b[j];
		a2[j-1] =   a[j];
	}

	mat mat_conc_01;
	mat mat_zeros_1;

	mat_zeros_1 = zeros(na-2,1);	
	mat_conc_01 = concat_vertical(eye_2,transpose(mat_zeros_1));
	
	mat mat_a1;
	mat_a1 =zeros(na-1,1);
	mat_a1.set_col (0, a1);

	mat mat_conc_02;
	mat_conc_02 = concat_horizontal(mat_a1,mat_conc_01);
	
	mat mat_Num;
	mat_Num = eye_1 - mat_conc_02;


	vec vec_Denom(na-1);
	for(i=0 ; i<na-1 ; i++)
	{
		vec_Denom[i] = b1[i] - (a2[i] * b[0]);
	}

	zi = inv(mat_Num) * vec_Denom;
}

void SWApplyTemporalFilter::IIR_Filter (const vec & b, const vec & a, const vec & data, const vec & V0, vec &dataFiltered, vec &Vf)
{
	int i,j,i_V0;
	double Sum_a_tmp,Sum_b_tmp;
	double Sum_tmp_Vf;
	double V0_tmp;
	int na, nb, dataSize;
	na = length(a);
	nb = length(b);
	dataSize = length(data);
		
	if(dataSize<nb)
	{
		for (i=0;i<dataSize;i++)
		{
			Sum_b_tmp = 0.0;
			for (j=0;j<=i;j++)
			{
				Sum_b_tmp = Sum_b_tmp + (b[j] * data[i-j]);
			}	


			Sum_a_tmp = 0.0;
	
			for (j=0;j<=i;j++)
			{
				Sum_a_tmp = Sum_a_tmp + (a[j] * dataFiltered[i-j]);
			}
			dataFiltered[i] = Sum_b_tmp - Sum_a_tmp + V0[i];
		}

		for (i=0;i<(nb-1);i++)
		{
			Sum_tmp_Vf=0.0;
			V0_tmp = 0.0;
			for (j=0;j<(nb-1);j++)
			{
				if ((i+j)<(nb-1))
				{
					if ((dataSize-1-j) >= 0)
					{
						Sum_tmp_Vf = Sum_tmp_Vf + (b[i+j+1] * data[dataSize-1-j]) - (a[i+j+1]*dataFiltered[dataSize-1-j]);
						i_V0 = i+j+1;
					}
					if ((dataSize-1-j) < 0)
					{
						V0_tmp = V0[i_V0];
					}
				}
			}
			Vf[i] = Sum_tmp_Vf +  V0_tmp;
		}
	}
	else
	{
		for (i=0;i<nb-1;i++)
		{
			Sum_b_tmp = 0.0;
			for (j=0;j<=i;j++)
			{
				Sum_b_tmp = Sum_b_tmp + (b[j] * data[i-j]);
			}	
			Sum_a_tmp = 0.0;
	
			for (j=0;j<=i;j++)
			{
				Sum_a_tmp = Sum_a_tmp + (a[j] * dataFiltered[i-j]);
			}
			dataFiltered[i] = Sum_b_tmp - Sum_a_tmp + V0[i];	
		}
	
	
		for (i=nb-1;i<dataSize;i++)
		{
			Sum_b_tmp = 0.0;
			for (j=0;j<nb;j++)
			{
				Sum_b_tmp = Sum_b_tmp + (b[j] * data[i-j]);
			}
			Sum_a_tmp = 0.0;
			for (j=0;j<nb;j++)
			{
				Sum_a_tmp = Sum_a_tmp + (a[j] * dataFiltered[i-j]);
			}	
			dataFiltered[i] = Sum_b_tmp - Sum_a_tmp;
		}
	
		for (i=0;i<nb-1;i++)
		{	
			Sum_tmp_Vf=0.0;
			for (j=i;j<nb-1;j++)
			{
				Sum_tmp_Vf = Sum_tmp_Vf + (b[j+1] * data[dataSize-1-j+i]) - (a[j+1] * dataFiltered[dataSize-1-j+i]);
			}
			Vf[i] = Sum_tmp_Vf;
		}
	}
}


void SWApplyTemporalFilter::Filtfilt (const vec & b, const vec & a, const vec & data, vec &dataFiltered)
{
	int na, nb, dataSize, j,  LengthEdgeTransients;
	na = length(a);
	nb = length(b);
	dataSize = length(data);
	LengthEdgeTransients = 3*(nb - 1 );

	vec x_b(dataSize + (2*LengthEdgeTransients));
	vec y_b(dataSize + (2*LengthEdgeTransients));
	vec y_b_2(dataSize + (2*LengthEdgeTransients));
	vec y_c(dataSize + (2*LengthEdgeTransients));
	vec y_c_2(dataSize + (2*LengthEdgeTransients));

	x_b = zeros(dataSize+ (2*LengthEdgeTransients));
	y_b = zeros(dataSize+ (2*LengthEdgeTransients));
	y_b_2 = zeros(dataSize+ (2*LengthEdgeTransients));
	y_c = zeros(dataSize+ (2*LengthEdgeTransients));
	y_c_2 = zeros(dataSize+ (2*LengthEdgeTransients));
	
	
	vec zi(na-1);	
	ComputeFilterInitialCondition(b,a, zi);

	for(j=0 ; j<LengthEdgeTransients; j++)
	{
		x_b [j]	 = (2*data[0]) - data[LengthEdgeTransients-j];	
	}
	for(j=0 ; j<dataSize; j++)
	{
		x_b [j+LengthEdgeTransients] = data[j];
	}

	for(j=0 ; j<LengthEdgeTransients ; j++)
	{
		x_b [j+LengthEdgeTransients+dataSize]	 = (2*data[dataSize-1]) - data[dataSize - j - 2];		
	}

	vec zi_Chan(na-1);
	for(j=0 ; j<na-1; j++)
	{
			zi_Chan[j] = zi[j]*x_b[0];
	}

	vec vec_FinalStates(na-1);

	IIR_Filter (b, a, x_b, zi_Chan,y_b,vec_FinalStates);

	for(j=0 ; j<dataSize + (2*LengthEdgeTransients); j++)
	{
		y_c[j] = y_b [(dataSize + (2*LengthEdgeTransients)) -1 -j];
	}

	vec zi_Chan_2(na-1);

	for(j=0 ; j< na-1; j++)
	{
		zi_Chan_2[j] = zi[j]*y_c[0];				
	}
	
	IIR_Filter (b, a, y_c, zi_Chan_2,y_b_2,vec_FinalStates);

		
	for(j=0 ; j<dataSize + (2*LengthEdgeTransients); j++)
	{
		y_c_2[j] = y_b_2 [(dataSize + (2*LengthEdgeTransients)) -1 -j];
	}

	
	for(j=0 ; j<dataSize; j++)
	{
		dataFiltered[j]	 = y_c_2[j+LengthEdgeTransients];	
	}
}




void SWApplyTemporalFilter::itppFilter(const vec & denomCoefFilter, const vec & numCoefFilter, const vec & input, vec & output)
{ 
	output = filter( denomCoefFilter, numCoefFilter, input);
}


void SWApplyTemporalFilter::SWFilter(const vec & denomCoefFilter, const vec & numCoefFilter, const vec & input, vec & output)
{ 
	Filtfilt (denomCoefFilter, numCoefFilter, input, output);
}
