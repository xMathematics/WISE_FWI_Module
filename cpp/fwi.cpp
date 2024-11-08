/**
 * WISE_FWI_Module: fwi.h
 * Copyright (C) 2023  WISE
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "intel_check.h"
#if __has_include(<mathimf.h>)
#include <mathimf.h>
#else
#include <cmath>
#endif

#include "fwi.h"

#include <cassert>
#include "angles.h"


// tolerance for convergance of previous ffmc calculations
#define TOLERANCE 0.0000001

double calc_subdaily_ffmc_vanwagner(const WTimeSpan &ts, const double in_ffmc, const double rain, double temperature, double rh, double ws) {

	/* this is the hourly ffmc routine given wx and previous ffmc */
	if ((in_ffmc < 0.0) || (in_ffmc > 101.0) ||
	    (rain < 0.0) || (rain > 300.0))
		return -98;

	if (temperature < -50.0)
		temperature = -50.0;
	else if (temperature > 60.0)
		temperature = 60.0;

	if (rh < 0.0)
		rh = 0.0;
	else if (rh > 1.0)
		rh = 1.0;

	if (ws > 200.0)
		ws = 200.0;
	else if (ws < 0.0)
		ws = 0.0;

	double factor, hour_frac = (double)ts.GetTotalSeconds() / 60.0 / 60.0;
	double hour_frac2 = hour_frac - floor(hour_frac);
	if (hour_frac2 > 1e-4)
		factor = 147.27723;
	else
		factor = 147.2;

	double mo, ed, ew, moew, moed, xm, a1, e, moe, xkd;
	const double rhp = rh * 100.0;				// input is 0..1, to match old equations we'll go to 0..100

	mo = factor * (101.0 - in_ffmc) / (59.5 + in_ffmc);	// equation 2a.  this calculation is the same as in calc_ffmc, but is called wmo

	// this calculation is also found below - but the if statement is
	// slightly different so it may be worth reviewing
	if (rain != 0) {					
		mo += rain * 42.5 * exp(-100.0 / (251.0 - mo)) * (1.0 - exp(-6.93 / rain));		// equation 12
	}
	if (mo > 250.0)
		mo = 250.0;

	// ed is also calculated the same way
	ed = 0.942 * pow(rhp, 0.679)
			+ (11.0 * exp((rhp - 100.0) / 10.0))
			+ 0.18 * (21.1 - temperature) * (1.0 - exp(-0.115 * rhp));
								// equation 8a
	moed = mo - ed;

	// ew is also calculated the same way
	ew = 0.618 * pow(rhp, 0.753)
			+ (10.0 * exp((rhp - 100.0) / 10.0))
			+ 0.18 * (21.1 - temperature)
			* (1.0 - exp(-0.115 * rhp));            // equation 8b

	moew = mo - ew;

	if (moed == 0.0 || (moew >= 0.0 && moed < 0.0)) {
		xm = mo;
		if (moed == 0.0)	e = ed;
		if (moew >= 0.0)	e = ew;
	}
	else {
		if (moed > 0.0) {
			a1 = rh;
			e = ed;
			moe = moed;
		}
		else {
			a1 = 1.0 - rh;
			e = ew;
			moe = moew;
		}
		xkd = (0.424 * (1.0 - pow(a1, 1.7)) + (0.0694 * sqrt(ws) * (1.0 - pow(a1, 8.0))));	// equation 4
		// xkd is calculated the same as the calc's for k1 below
		xkd = xkd * 0.0579 * exp(0.0365 * temperature);	// equation 6, similar to below: 'cept of using 0.581, we use 0.0579
		xm = e + moe * pow(10.0, -xkd * hour_frac);	// also similar to below for calc's for wm
	}

	double c_f = 59.5 * (250.0 - xm) / (factor + xm);	// was similar to below, but the below code had these
	if (c_f > 101.0)																	// range checking if statements, so I've duplicated them
		c_f = 101.0;																		// here too.
	else if (c_f < 0.0)
		c_f = 0.0;
	return c_f;
}

/////////////////////////////////////////////////////////////////////
//                                                                 //
//  This routine calculates ffmc backwards through time based on   //
//  a one hour step.  It expects the current ffmc and the weather  //
//  from the previous hour.                                        //
//                                                                 //
/////////////////////////////////////////////////////////////////////  
double calc_previous_hourly_ffmc_vanwagner(const double current_ffmc, 
					   const double rain, 
					   double temperature, 
					   double rh, 
					   double ws) {
	/* this is the hourly ffmc routine given wx and previous ffmc */
	if ((current_ffmc < 0.0) || (current_ffmc > 101.0) ||
	    (rain < 0.0) || (rain > 300.0))
		return -98;

	if (temperature < -50.0)
		temperature = -50.0;
	else if (temperature > 60.0)
		temperature = 60.0;

	if (rh < 0.0)
		rh = 0.0;
	else if (rh > 1.0)
		rh = 1.0;

	if (ws > 200.0)
		ws = 200.0;
	else if (ws < 0.0)
		ws = 0.0;

	double out_ffmc, out_ffmc_prior, in_ffmc, diff_mc;

	in_ffmc = current_ffmc;

	out_ffmc = calc_subdaily_ffmc_vanwagner(WTimeSpan(0, 1, 0, 0), in_ffmc, rain, temperature, rh, ws);

	diff_mc = fabs(out_ffmc - current_ffmc);
	while (diff_mc > TOLERANCE ) {
		if (out_ffmc > current_ffmc) {
			in_ffmc -= diff_mc/2;
		}
		else {
			in_ffmc += diff_mc/2;
		}

		out_ffmc_prior = out_ffmc;	
		out_ffmc = calc_subdaily_ffmc_vanwagner(WTimeSpan(0, 1, 0, 0), in_ffmc, rain, temperature, rh, ws);
		
		diff_mc = fabs(out_ffmc - current_ffmc);
		// check for error conditions
		if (out_ffmc < 0.0 || out_ffmc > 101.0) {
			diff_mc = 0.0;
			in_ffmc = current_ffmc;
			break;
		}

		// if the output using the previous weather is insensitive to
		// changes in the input ffmc, then the routine has found the
		// correct answer
		if (fabs(out_ffmc-out_ffmc_prior) < TOLERANCE) {
			diff_mc = 0.0;
			break;
		}
	}
	return in_ffmc;
}


double calc_daily_ffmc_vanwagner(const double in_ffmc, const double rain, double temperature, double rh, double ws) {
	if ((in_ffmc < 0.0) || (in_ffmc > 101.0) ||
	    (rain < 0.0) || (rain > 600.0))
		return -98;

	if (temperature < -50.0)
		temperature = -50.0;
	else if (temperature > 60.0)
		temperature = 60.0;

	if (rh < 0.0)
		rh = 0.0;
	else if (rh > 1.0)
		rh = 1.0;

	if (ws > 200.0)
		ws = 200.0;
	else if (ws < 0.0)
		ws = 0.0;

	double wmo,fo,wm,ed,ew,rf;
	double ko, kd, k1, kw;
	double c_f;

	const double rhp = rh * 100.0;				// input is 0..1, to match old equations we'll go to 0..100
	fo = in_ffmc;
	wmo = (147.2 * (101.0 - fo)) / (59.5 + fo);
	if (rain > 0.5)	{
		rf = rain - 0.5;
		if (wmo > 150.0) {
			double tmp = (wmo - 150.0);
			tmp = tmp * tmp;
			wmo = wmo + 42.5 * rf * (exp(-100.0 / (251.0 - wmo))) * (1.0 - exp(-6.93 / rf))
			    + 0.0015 * tmp * sqrt(rf);
		} else	wmo = wmo + 42.5 * rf * (exp(-100.0 / (251.0 - wmo))) * (1.0 - exp(-6.93 / rf));
	}
	if (wmo > 250.0)					// this 'if' statement moved outside of the nexted 'if rain' statement to
		wmo = 250.0;						// match Mike's code


	ed = 0.942 * pow(rhp, 0.679)
			+ (11.0 * exp((rhp - 100.0) / 10.0))
			+ 0.18 * (21.1 - temperature) * (1.0 - exp(-0.115 * rhp));

	ew = 0.618 * pow(rhp, 0.753)
			+ (10.0 * exp((rhp - 100.0) / 10.0))
			+ 0.18 * (21.1 - temperature)
			* (1.0 - exp(-0.115 * rhp));              						// eqn 5
	if ((wmo < ed) && (wmo < ew)) {
		k1 = 0.424 * (1.0 - pow((100.0 - rhp) / 100.0, 1.7))    // eqn 7a
				+ 0.0694 * sqrt(ws) * (1.0 - pow(1.0 - rh, 8.0));
		kw = k1 * 0.581 * exp(0.0365 * temperature);        		// eqn 7b
		wm = ew - (ew - wmo) / pow(10.0, kw);                		// eqn 9
	}
	else if (wmo > ed) {
		ko =  0.424 * (1.0 - pow(rh, 1.7))
				+ 0.0694 * sqrt(ws) * (1.0 - pow(rh, 8.0));   			// eqn 6a
		kd = ko * 0.581 * exp(0.0365 * temperature);          	// eqn 6b
		wm = ed + (wmo - ed) / pow(10.0, kd);                 	// eqn 8
	}
	else
		wm = wmo;

	c_f = 59.5 * (250.0 - wm) / (147.2 + wm);
	if (c_f > 101.0)
		c_f = 101.0;
	else if (c_f < 0.0)
		c_f = 0.0;
	return c_f;
}

/*  Lawson's Interpolation method for FFMC */

/* the Lawson stuff is the code which takes the Equilibrium (from Kerry)
 * calculations calculated from noon and determines an FFMC .  This code
 * originated from Mike Wotton's source file called diurffmc.c
 */

/*
 * Supporting tables for various routines  
 * moved out of the routines by Mac on 2001/12/13
 * to support backwards hourly calculations
 */


/* Morning Hours Tables
 * One for each of three RH levels
 */

// Low rh
static const double L[9][39] = {
		{9999, 17.5, 30.0, 40.0, 50.0, 55.0, 60.0, 65.0, 70.0, 72.0, 74.0, 75.0, 76.0, 77.0, 78.0, 79.0, 80.0, 81.0, 82.0, 83.0, 84.0, 85.0, 86.0, 87.0, 88.0, 89.0, 90.0, 91.0, 92.0, 93.0, 94.0, 95.0, 96.0, 97.0, 98.0, 99.0,100.0,100.9,101.0 },
		{600 , 48.3, 49.4, 51.1, 53.5, 55.1, 56.9, 59.1, 61.7, 62.9, 64.1, 64.8, 65.5, 66.2, 66.9, 67.7, 68.5, 69.4, 70.2, 71.1, 72.1, 73.1, 74.1, 75.2, 76.3, 77.5, 78.7, 80.0, 81.3, 82.7, 84.1, 85.7, 87.2, 88.8, 90.4, 91.9, 93.2, 93.8, 93.8 },
		{700 , 50.7, 52.1, 53.9, 56.3, 57.9, 59.7, 61.8, 64.3, 65.4, 66.6, 67.2, 67.9, 68.6, 69.3, 70.0, 70.7, 71.5, 72.3, 73.2, 74.0, 75.0, 75.9, 76.9, 77.9, 79.0, 80.2, 81.4, 82.6, 83.9, 85.2, 86.6, 88.1, 89.6, 91.1, 92.6, 93.9, 94.5, 94.5 },
		{800 , 53.3, 54.9, 56.8, 59.3, 60.9, 62.6, 64.7, 67.0, 68.1, 69.2, 69.8, 70.4, 71.0, 71.6, 72.3, 73.0, 73.7, 74.5, 75.3, 76.1, 76.9, 77.8, 78.7, 79.7, 80.6, 81.7, 82.8, 83.9, 85.1, 86.3, 87.7, 89.0, 90.4, 91.9, 93.3, 94.6, 95.3, 95.3 },
		{900 , 59.6, 60.7, 62.2, 64.4, 65.7, 67.3, 69.1, 71.2, 72.1, 73.2, 73.7, 74.2, 74.8, 75.4, 76.0, 76.7, 77.3, 78.0, 78.7, 79.5, 80.3, 81.1, 81.9, 82.8, 83.7, 84.7, 85.7, 86.7, 87.8, 89.0, 90.1, 91.4, 92.6, 93.9, 95.2, 96.3, 96.8, 96.8 },
		{1000, 66.8, 67.2, 68.2, 69.9, 70.9, 72.2, 73.8, 75.6, 76.5, 77.4, 77.9, 78.4, 78.9, 79.4, 80.0, 80.5, 81.1, 81.8, 82.4, 83.1, 83.8, 84.5, 85.3, 86.1, 86.9, 87.8, 88.7, 89.7, 90.6, 91.7, 92.7, 93.8, 94.9, 96.0, 97.1, 97.9, 98.4, 98.4 },
		{1100, 74.5, 74.5, 74.9, 75.9, 76.6, 77.6, 78.8, 80.3, 81.0, 81.9, 82.4, 83.0, 83.6, 84.1, 84.7, 85.2, 85.8, 86.3, 86.9, 87.4, 88.0, 88.5, 89.0, 89.6, 90.1, 90.6, 91.1, 91.6, 92.1, 92.6, 93.1, 93.8, 94.9, 96.0, 97.1, 97.9, 98.4, 98.4 },
		{1159, 83.0, 82.5, 82.3, 82.4, 82.7, 83.2, 84.1, 85.2, 85.8, 86.5, 86.8, 87.2, 87.6, 87.9, 88.2, 88.6, 88.9, 89.2, 89.6, 89.9, 90.2, 90.5, 90.9, 91.2, 91.5, 91.8, 92.1, 92.4, 92.7, 93.0, 93.3, 93.8, 94.9, 96.0, 97.1, 97.9, 98.4, 98.4 },
		{1200, 83.0, 82.5, 82.3, 82.4, 82.7, 83.2, 84.1, 85.2, 85.8, 86.5, 86.8, 87.2, 87.6, 87.9, 88.2, 88.6, 88.9, 89.2, 89.6, 89.9, 90.2, 90.5, 90.9, 91.2, 91.5, 91.8, 92.1, 92.4, 92.7, 93.0, 93.3, 93.8, 94.9, 96.0, 97.1, 97.9, 98.4, 98.4 }
	};

// Medium rh
static const double M[9][39] = {
		{9999, 17.5, 30.0, 40.0, 50.0, 55.0, 60.0, 65.0, 70.0, 72.0, 74.0, 75.0, 76.0, 77.0, 78.0, 79.0, 80.0, 81.0, 82.0, 83.0, 84.0, 85.0, 86.0, 87.0, 88.0, 89.0, 90.0, 91.0, 92.0, 93.0, 94.0, 95.0, 96.0, 97.0, 98.0, 99.0,100.0,100.9,101.0 },
		{600 , 34.8, 39.2, 43.2, 47.6, 50.0, 52.6, 55.4, 58.4, 59.7, 61.1, 61.8, 62.5, 63.3, 64.0, 64.8, 65.6, 66.4, 67.2, 68.1, 68.9, 69.8, 70.8, 71.7, 72.7, 73.8, 74.8, 75.9, 77.1, 78.3, 79.5, 80.8, 82.2, 83.6, 85.0, 86.5, 88.0, 89.1, 89.1 },
		{700 , 36.3, 40.5, 44.3, 48.7, 51.2, 53.8, 56.7, 59.9, 61.3, 62.7, 63.4, 64.2, 64.9, 65.7, 66.5, 67.4, 68.2, 69.1, 70.0, 70.9, 71.9, 72.8, 73.9, 74.9, 75.9, 77.0, 78.2, 79.3, 80.5, 81.8, 83.1, 84.4, 85.7, 87.0, 88.3, 89.5, 90.2, 90.2 },
		{800 , 37.8, 41.7, 45.5, 49.8, 52.3, 55.1, 58.1, 61.4, 62.8, 64.3, 65.1, 65.9, 66.7, 67.5, 68.4, 69.3, 70.1, 71.1, 72.0, 73.0, 74.0, 75.0, 76.0, 77.1, 78.2, 79.3, 80.5, 81.7, 82.9, 84.1, 85.4, 86.6, 87.9, 89.1, 90.2, 91.2, 91.6, 91.6 },
		{900 , 44.6, 48.2, 51.6, 55.6, 57.8, 60.3, 63.0, 66.0, 67.3, 68.6, 69.3, 70.1, 70.8, 71.6, 72.3, 73.1, 73.9, 74.8, 75.6, 76.5, 77.4, 78.3, 79.3, 80.3, 81.3, 82.3, 83.4, 84.5, 85.7, 86.8, 88.0, 89.2, 90.5, 91.7, 92.8, 93.8, 94.4, 94.4 },
		{1000, 52.5, 55.5, 58.5, 61.9, 63.9, 66.0, 68.4, 71.0, 72.1, 73.3, 73.9, 74.5, 75.2, 75.9, 76.5, 77.2, 77.9, 78.7, 79.4, 80.2, 81.0, 81.9, 82.7, 83.6, 84.5, 85.5, 86.5, 87.5, 88.5, 89.6, 90.8, 91.9, 93.1, 94.3, 95.5, 96.7, 97.3, 97.3 },
		{1100, 61.6, 64.0, 66.3, 69.0, 70.6, 72.3, 74.2, 76.4, 77.3, 78.3, 79.0, 79.6, 80.3, 80.9, 81.5, 82.2, 82.8, 83.4, 84.0, 84.6, 85.3, 85.9, 86.5, 87.1, 87.7, 88.3, 88.9, 89.4, 90.0, 90.6, 91.2, 91.9, 93.1, 94.3, 95.5, 96.7, 97.3, 97.3 },
		{1159, 72.1, 73.5, 75.0, 76.9, 77.9, 79.2, 80.6, 82.2, 82.9, 83.6, 84.0, 84.4, 84.8, 85.2, 85.6, 86.0, 86.4, 86.7, 87.1, 87.5, 87.9, 88.2, 88.6, 88.9, 89.3, 89.7, 90.0, 90.3, 90.7, 91.0, 91.4, 91.9, 93.1, 94.3, 95.5, 96.7, 97.3, 97.3 },
		{1200, 72.1, 73.5, 75.0, 76.9, 77.9, 79.2, 80.6, 82.2, 82.9, 83.6, 84.0, 84.4, 84.8, 85.2, 85.6, 86.0, 86.4, 86.7, 87.1, 87.5, 87.9, 88.2, 88.6, 88.9, 89.3, 89.7, 90.0, 90.3, 90.7, 91.0, 91.4, 91.9, 93.1, 94.3, 95.5, 96.7, 97.3, 97.3 }
	};

// High rh
static const double H[9][39] = {
		{9999, 17.5, 30.0, 40.0, 50.0, 55.0, 60.0, 65.0, 70.0, 72.0, 74.0, 75.0, 76.0, 77.0, 78.0, 79.0, 80.0, 81.0, 82.0, 83.0, 84.0, 85.0, 86.0, 87.0, 88.0, 89.0, 90.0, 91.0, 92.0, 93.0, 94.0, 95.0, 96.0, 97.0, 98.0, 99.0,100.0,100.9,101.0 },
		{600,  28.2, 33.4, 37.9, 42.9, 45.6, 48.5, 51.7, 55.1, 56.5, 58.0, 58.8, 59.5, 60.3, 61.2, 62.0, 62.9, 63.7, 64.6, 65.5, 66.5, 67.4, 68.4, 69.4, 70.5, 71.6, 72.7, 73.8, 75.0, 76.2, 77.4, 78.7, 80.0, 81.4, 82.7, 84.1, 85.4, 86.3, 86.3 },
		{700,  30.0, 34.8, 39.0, 43.8, 46.5, 49.4, 52.5, 55.9, 57.3, 58.8, 59.6, 60.4, 61.2, 62.1, 62.9, 63.8, 64.7, 65.7, 66.6, 67.6, 68.6, 69.6, 70.7, 71.8, 72.9, 74.1, 75.3, 76.5, 77.8, 79.1, 80.5, 81.9, 83.3, 84.8, 86.2, 87.6, 88.4, 88.4 },
		{800,  31.9, 36.2, 40.2, 44.8, 47.4, 50.2, 53.3, 56.7, 58.2, 59.7, 60.5, 61.3, 62.2, 63.0, 63.9, 64.8, 65.7, 66.7, 67.7, 68.7, 69.8, 70.8, 71.9, 73.1, 74.3, 75.5, 76.8, 78.1, 79.4, 80.8, 82.3, 83.8, 85.3, 86.9, 88.4, 89.8, 90.6, 90.6 },
		{900,  37.7, 42.1, 46.1, 50.5, 52.9, 55.5, 58.4, 61.5, 62.8, 64.2, 64.9, 65.6, 66.4, 67.1, 67.9, 68.7, 69.5, 70.4, 71.3, 72.1, 73.1, 74.0, 75.0, 76.0, 77.0, 78.1, 79.2, 80.3, 81.5, 82.7, 84.0, 85.3, 86.7, 88.1, 89.5, 90.8, 91.7, 91.7 },
		{1000, 44.4, 48.9, 52.7, 56.8, 59.1, 61.4, 63.9, 66.7, 67.8, 69.0, 69.6, 70.2, 70.9, 71.5, 72.2, 72.9, 73.6, 74.3, 75.0, 75.8, 76.6, 77.3, 78.2, 79.0, 79.9, 80.8, 81.7, 82.6, 83.6, 84.7, 85.8, 86.9, 88.0, 89.3, 90.5, 91.8, 92.8, 92.8 },
		{1100, 52.1, 56.5, 60.2, 63.9, 65.9, 67.9, 70.1, 72.3, 73.3, 74.3, 74.9, 75.5, 76.1, 76.6, 77.2, 77.8, 78.4, 79.0, 79.5, 80.1, 80.7, 81.2, 81.8, 82.4, 82.9, 83.5, 84.0, 84.6, 85.1, 85.6, 86.2, 86.9, 88.0, 89.3, 90.5, 91.8, 92.8, 92.8 },
		{1159, 60.9, 65.2, 68.6, 71.8, 73.5, 75.1, 76.7, 78.4, 79.1, 79.8, 80.2, 80.5, 80.8, 81.2, 81.5, 81.8, 82.1, 82.5, 82.8, 83.1, 83.4, 83.7, 84.0, 84.3, 84.6, 84.9, 85.2, 85.5, 85.8, 86.1, 86.4, 86.9, 88.0, 89.3, 90.5, 91.8, 92.8, 92.8 },
		{1200, 60.9, 65.2, 68.6, 71.8, 73.5, 75.1, 76.7, 78.4, 79.1, 79.8, 80.2, 80.5, 80.8, 81.2, 81.5, 81.8, 82.1, 82.5, 82.8, 83.1, 83.4, 83.7, 84.0, 84.3, 84.6, 84.9, 85.2, 85.5, 85.8, 86.1, 86.4, 86.9, 88.0, 89.3, 90.5, 91.8, 92.8, 92.8 }
	};


/*
 * main table for the remainder of hours for all of the rh ranges
 */
static const double MAIN[22][39] = {
		{9999, 17.5, 30.0, 40.0, 50.0, 55.0, 60.0, 65.0, 70.0, 72.0, 74.0, 75.0, 76.0, 77.0, 78.0, 79.0, 80.0, 81.0, 82.0, 83.0, 84.0, 85.0, 86.0, 87.0, 88.0, 89.0, 90.0, 91.0, 92.0, 93.0, 94.0, 95.0, 96.0, 97.0, 98.0, 99.0,100.0,100.9,101.0 },
		{100 , 23.4, 32.9, 40.5, 47.8, 51.4, 54.9, 58.3, 61.8, 63.3, 64.8, 65.5, 66.3, 67.1, 67.9, 68.8, 69.6, 70.5, 71.4, 72.3, 73.2, 74.1, 75.1, 76.1, 77.1, 78.1, 79.1, 80.2, 81.3, 82.4, 83.5, 84.7, 85.9, 87.1, 88.3, 89.5, 90.7, 91.6, 91.6 },
		{200 , 24.3, 33.0, 39.9, 46.8, 50.2, 53.6, 56.9, 60.4, 61.8, 63.4, 64.1, 64.9, 65.7, 66.5, 67.4, 68.2, 69.1, 70.0, 70.9, 71.8, 72.7, 73.7, 74.7, 75.7, 76.7, 77.8, 78.9, 80.0, 81.1, 82.3, 83.4, 84.7, 85.9, 87.2, 88.4, 89.6, 90.5, 90.5 },
		{300 , 25.2, 33.1, 39.4, 45.8, 49.0, 52.3, 55.6, 59.0, 60.5, 62.0, 62.7, 63.5, 64.3, 65.1, 66.0, 66.8, 67.7, 68.6, 69.5, 70.4, 71.4, 72.3, 73.3, 74.4, 75.4, 76.5, 77.6, 78.7, 79.8, 81.0, 82.2, 83.5, 84.7, 86.0, 87.3, 88.5, 89.4, 89.4 },
		{400 , 26.2, 33.2, 38.9, 44.8, 47.9, 51.0, 54.3, 57.7, 59.1, 60.6, 61.4, 62.2, 63.0, 63.8, 64.6, 65.5, 66.3, 67.2, 68.1, 69.1, 70.0, 71.0, 72.0, 73.0, 74.1, 75.2, 76.3, 77.4, 78.6, 79.8, 81.0, 82.3, 83.6, 84.9, 86.2, 87.5, 88.4, 88.4 },
		{500 , 27.2, 33.3, 38.4, 43.9, 46.7, 49.8, 52.9, 56.4, 57.8, 59.3, 60.1, 60.8, 61.6, 62.5, 63.3, 64.2, 65.0, 65.9, 66.8, 67.8, 68.7, 69.7, 70.7, 71.7, 72.8, 73.9, 75.0, 76.2, 77.4, 78.6, 79.8, 81.1, 82.5, 83.8, 85.2, 86.4, 87.3, 87.3 },
		{559 , 28.2, 33.4, 37.9, 42.9, 45.7, 48.6, 51.7, 55.1, 56.5, 58.0, 58.8, 59.6, 60.4, 61.2, 62.0, 62.9, 63.8, 64.6, 65.6, 66.5, 67.5, 68.4, 69.5, 70.5, 71.6, 72.7, 73.8, 75.0, 76.2, 77.4, 78.7, 80.0, 81.4, 82.7, 84.1, 85.4, 86.3, 86.3 },
		{600 , 28.2, 33.4, 37.9, 42.9, 45.7, 48.6, 51.7, 55.1, 56.5, 58.0, 58.8, 59.6, 60.4, 61.2, 62.0, 62.9, 63.8, 64.6, 65.6, 66.5, 67.5, 68.4, 69.5, 70.5, 71.6, 72.7, 73.8, 75.0, 76.2, 77.4, 78.7, 80.0, 81.4, 82.7, 84.1, 85.4, 86.3, 86.3 },
		{1200, 17.5, 27.7, 34.4, 40.9, 44.5, 48.2, 52.5, 57.3, 59.4, 61.7, 62.9, 64.2, 65.5, 66.9, 68.5, 70.5, 73.8, 76.4, 78.4, 80.0, 81.5, 82.8, 84.0, 85.2, 86.3, 87.5, 88.6, 89.7, 90.8, 91.9, 92.9, 94.0, 95.0, 96.0, 97.0, 97.9, 98.7, 98.7 },
		{1300, 17.5, 28.3, 35.8, 43.2, 47.2, 51.5, 56.0, 61.0, 63.2, 65.5, 66.7, 67.9, 69.3, 70.7, 72.2, 73.9, 76.3, 78.2, 79.8, 81.1, 82.4, 83.7, 84.8, 86.0, 87.1, 88.2, 89.3, 90.4, 91.4, 92.5, 93.5, 94.6, 95.6, 96.6, 97.6, 98.5, 99.3, 99.3 },
		{1400, 17.5, 29.0, 37.2, 45.6, 50.1, 54.8, 59.8, 65.1, 67.3, 69.6, 70.8, 72.0, 73.3, 74.6, 76.1, 77.4, 78.7, 79.9, 81.1, 82.3, 83.4, 84.6, 85.7, 86.8, 87.9, 88.9, 90.0, 91.0, 92.1, 93.1, 94.1, 95.1, 96.1, 97.1, 98.1, 99.1,100.0,100.0 },
		{1500, 17.5, 29.5, 38.6, 47.8, 52.5, 57.4, 62.4, 67.5, 69.6, 71.8, 72.9, 74.0, 75.1, 76.3, 77.5, 78.7, 79.9, 81.0, 82.1, 83.2, 84.2, 85.3, 86.4, 87.4, 88.5, 89.5, 90.5, 91.5, 92.6, 93.6, 94.6, 95.6, 96.6, 97.6, 98.6, 99.6,100.4,100.4 },
		{1600, 17.5, 30.0, 40.0, 50.0, 55.0, 60.0, 65.0, 70.0, 72.0, 74.0, 75.0, 76.0, 77.0, 78.0, 79.0, 80.0, 81.0, 82.0, 83.0, 84.0, 85.0, 86.0, 87.0, 88.0, 89.0, 90.0, 91.0, 92.1, 93.1, 94.1, 95.1, 96.1, 97.1, 98.1, 99.1,100.1,101.0,101.0 },
		{1700, 17.8, 30.6, 40.8, 51.0, 56.1, 61.0, 65.8, 70.4, 72.2, 74.0, 75.0, 75.9, 76.8, 77.8, 78.7, 79.7, 80.6, 81.6, 82.6, 83.5, 84.5, 85.5, 86.5, 87.5, 88.5, 89.5, 90.5, 91.5, 92.5, 93.5, 94.5, 95.5, 96.5, 97.6, 98.6, 99.6,100.4,100.4 },
		{1800, 18.0, 31.1, 41.6, 52.0, 57.1, 62.0, 66.6, 70.7, 72.3, 74.0, 74.9, 75.7, 76.6, 77.5, 78.4, 79.3, 80.2, 81.2, 82.1, 83.0, 84.0, 84.9, 85.9, 86.9, 87.9, 88.9, 89.9, 90.9, 91.9, 92.9, 93.9, 95.0, 96.0, 97.1, 98.1, 99.1, 99.9, 99.9 },
		{1900, 18.5, 31.8, 42.4, 52.6, 57.5, 62.0, 66.2, 70.0, 71.6, 73.2, 74.0, 74.8, 75.7, 76.5, 77.4, 78.2, 79.1, 80.0, 80.9, 81.8, 82.8, 83.7, 84.6, 85.6, 86.6, 87.5, 88.5, 89.5, 90.5, 91.5, 92.6, 93.6, 94.6, 95.7, 96.7, 97.8, 98.6, 98.6 },
		{2000, 19.1, 32.5, 43.2, 53.3, 57.9, 62.0, 65.9, 69.4, 70.9, 72.4, 73.1, 73.9, 74.7, 75.5, 76.3, 77.2, 78.0, 78.9, 79.8, 80.6, 81.5, 82.5, 83.4, 84.3, 85.3, 86.2, 87.2, 88.2, 89.2, 90.2, 91.2, 92.3, 93.3, 94.3, 95.4, 96.4, 97.4, 97.4 },
		{2100, 19.9, 32.5, 42.6, 52.1, 56.5, 60.5, 64.3, 67.8, 69.3, 70.8, 71.5, 72.3, 73.1, 73.9, 74.8, 75.6, 76.5, 77.3, 78.2, 79.1, 80.0, 80.9, 81.9, 82.8, 83.8, 84.8, 85.8, 86.8, 87.8, 88.8, 89.9, 90.9, 92.0, 93.1, 94.2, 95.2, 96.2, 96.2 },
		{2200, 20.7, 32.6, 42.1, 51.0, 55.2, 59.1, 62.7, 66.2, 67.7, 69.2, 70.0, 70.8, 71.6, 72.4, 73.2, 74.1, 74.9, 75.8, 76.7, 77.6, 78.5, 79.4, 80.4, 81.3, 82.3, 83.3, 84.3, 85.4, 86.4, 87.5, 88.6, 89.6, 90.8, 91.9, 93.0, 94.1, 95.0, 95.0 },
		{2300, 21.6, 32.7, 41.5, 50.0, 53.9, 57.6, 61.2, 64.7, 66.2, 67.7, 68.5, 69.3, 70.1, 70.9, 71.7, 72.5, 73.4, 74.3, 75.2, 76.1, 77.0, 77.9, 78.9, 79.9, 80.9, 81.9, 82.9, 84.0, 85.0, 86.1, 87.2, 88.4, 89.5, 90.7, 91.8, 92.9, 93.9, 93.9 },
		{2400, 22.5, 32.8, 41.0, 48.9, 52.7, 56.3, 59.8, 63.3, 64.7, 66.2, 67.0, 67.8, 68.6, 69.4, 70.2, 71.1, 71.9, 72.8, 73.7, 74.6, 75.5, 76.5, 77.5, 78.5, 79.5, 80.5, 81.5, 82.6, 83.7, 84.8, 86.0, 87.1, 88.3, 89.5, 90.7, 91.8, 92.7, 92.7 },
		{2500, 23.4, 32.9, 40.5, 47.8, 51.4, 54.9, 58.3, 61.8, 63.3, 64.8, 65.5, 66.3, 67.1, 67.9, 68.8, 69.6, 70.5, 71.4, 72.3, 73.2, 74.1, 75.1, 76.1, 77.1, 78.1, 79.1, 80.2, 81.3, 82.4, 83.5, 84.7, 85.9, 87.1, 88.3, 89.5, 90.7, 91.6, 91.6 }
	};

static const double RHCLASS[4][8][2] = {
		{{600, 630}, {700, 730}, {800, 830}, {900, 930}, {1000, 1030}, {1100, 1130}, {1159, 1200}, {1200, 1200}},
		{{87, 3}   , {77, 3}   , {67, 3}   , {62, 3}   , {57, 3}     , {54.5, 3}   , {52, 3}     , {52, 3}     },
		{{87, 2}   , {77, 2}   , {67, 2}   , {62, 2}   , {57, 2}     , {54.5, 2}   , {52, 2}     , {52, 2}     },
		{{68, 1}   , {58, 1}   , {48, 1}   , {43, 1}   , {38, 1}     , {35.5, 1}   , {33, 1}     , {33, 1}     }
	};

/*--------------------------------------------------------------------------*/
static double intrp(const double I1, const double I2, const double I3, const double I4, const double fraction, const WTimeSpan &ts) {
					/*--------------------------------------------------------------------------*
					 This function performs linear interpolation in two directions to interpolate
					 between FFMC values and Time values.
					 *--------------------------------------------------------------------------*/
	double I12;
	double I34;
	double I14;
	double minutes = ts.GetMinutes();

	I12 = I1 + ((I2 - I1) * fraction);

	I34 = I3 + ((I4 - I3) * fraction);

	if (ts.GetHours() == 11)
		I14 = I12 + ((I34 - I12) / 59.0) * minutes;
	else
		I14 = I12 + ((I34 - I12) / 60.0) * minutes;

	return I14;
}


/*--------------------------------------------------------------------------*
	This function returns adjffmc for low rh conditions during morning hours
	*--------------------------------------------------------------------------*/
static double LOWRH(const WTimeSpan &ts, const double ff_ffmc, const int tindex) {
	int i = 1;
	double adjffmc, fraction;

	while (ff_ffmc >= L[0][i])
		i++;
	i--;
  
	fraction = (ff_ffmc - L[0][i]) / (L[0][i + 1] - L[0][i]);
	adjffmc = intrp(L[tindex][i], L[tindex][i + 1], L[tindex + 1][i], L[tindex + 1][i + 1], fraction, ts);
	return adjffmc;
}

/*--------------------------------------------------------------------------*
	This function returns adjffmc for medium rh conditions during morning hours
	*--------------------------------------------------------------------------*/
static double MEDRH(const WTimeSpan &ts, const double ff_ffmc, const int tindex) {
	int i = 1;
	double adjffmc, fraction;

	while (ff_ffmc >= M[0][i])
		i++;
	i--;

	fraction = (ff_ffmc - M[0][i]) / (M[0][i + 1] - M[0][i]);
	adjffmc = intrp(M[tindex][i], M[tindex][i + 1], M[tindex + 1][i], M[tindex + 1][i + 1], fraction, ts);
	return adjffmc;
}

/*--------------------------------------------------------------------------*
	This function returns adjffmc for high rh conditions during morning hours
	This is the default routine if no rh is specified as an input (ie. rh=0)
	*--------------------------------------------------------------------------*/
static double HIGHRH(const WTimeSpan &ts, const double ff_ffmc, const int tindex) {
	int i = 1;
	double adjffmc, fraction;

	while (ff_ffmc >= H[0][i])
		i++;
	i--;

	fraction = (ff_ffmc - H[0][i]) / (H[0][i + 1] - H[0][i]);
	adjffmc = intrp(H[tindex][i], H[tindex][i + 1], H[tindex + 1][i], H[tindex + 1][i + 1], fraction, ts);
	return adjffmc;
}

/*--------------------------------------------------------------------------*
	This function returns adjffmc all hours except morning hours (0600 - 1159)
	*--------------------------------------------------------------------------*/ 
static double MAINTBL(const WTimeSpan &ts, const double ff_ffmc) {
	int i = 1;
	int tindex;
	double adjffmc, fraction;


	int hour = ts.GetHours() * 100 + ts.GetMinutes();

	  if (hour < 100)				// This statement was actually not here before, I added 
		hour = hour + 2400;		// according to Judi's codes. If this is not here, there is a 
													// a jog here at about 9:00 am.

	while (hour >= MAIN[i][0])
		i++;
	tindex = --i;

	i = 1;
	while (ff_ffmc >= MAIN[0][i])
		i++;
	i--;

	fraction = (ff_ffmc - MAIN[0][i]) / (MAIN[0][i+1] - MAIN[0][i]);
	adjffmc = intrp(MAIN[tindex][i], MAIN[tindex][i + 1], MAIN[tindex + 1][i], MAIN[tindex + 1][i + 1], fraction, ts);
	return adjffmc;
}



/*--------------------------------------------------------------------------*
	This function first checks that the program inputs are within the appropriate
	ranges. If any of the input values are out of range then a negative number is
	returned by the function. The following error codes may be returned by the
	function:
		-98.0 : One of the input values is out of range.
		-99.0 : Minutes is greater than 59 (out of range).

	NOTE: Appropriate ranges for inputs are defined as follows:
		hour    ( >= 1    and <= 2459  ) (integer)
		RH      ( >= 0    and <= 100   ) (integer)
		FF_FFMC ( >= 17.5 and <= 100.9 ) (one decimal place)

	Where: FF_FFMC = 17.5  corresponds to the lower limit of Van Wagner's
			original diurnal adjustment graphs;
	FF_FFMC = 100.9 corresponds to the theoretical upper limit of
			the current FF_FFMC scale.
*-----------------------------------------------------------------------------*/
double calc_hourly_ffmc_lawson(double ff_ffmc, WTimeSpan ts, double rh) {
	int minutes, tindex = 0, classidx = 0, i = 1;
	double adjffmc;

	/*------------------- Check validity of input data ---------------------------*/
	while (ts.GetTotalSeconds() < 0)
		ts += WTimeSpan(1, 0, 0, 0);
	int hour = ts.GetHours();
	if (ff_ffmc < 0.0 || ff_ffmc > 101.0) 
		return(-98.0);

	minutes = ts.GetMinutes();

	/*------ Check for low   -----*/
	if (ff_ffmc < 17.5)
		ff_ffmc = 17.5;
	/*----- end of FFMC scale ----*/
	
	if (rh < 0.0)
		rh = 0.0;
	else if (rh > 100.0)
		rh = 100.0;

	rh *= 100.0;
	rh = floor(rh + 0.5);
	rh *= 0.01;

	if (rh < 1.0)
		rh = 95; 

	/*------ Select the appropriate RH Class for table lookup  -------------------*/
	if ((hour >= 6)  && (hour <= 11)) {
		for (i = 0; i <= 7; i++) {
			if ((100.0 * hour) >= RHCLASS[0][i][0]) {
			}
			else {
				tindex = i;
				i = 10;
				if (minutes <= 30) {
					if (rh > RHCLASS[1][tindex - 1][0])
						classidx = 3;
					else if (rh < RHCLASS[3][tindex - 1][0])
						classidx = 1;
					else	classidx = 2;
				}
				else {
					if (rh > RHCLASS[1][tindex][0])
						classidx = 3;
					else if (rh < RHCLASS[3][tindex][0])
						classidx = 1;
					else	classidx = 2;
				}
			}
		}

		switch (classidx) {
			case 1:	adjffmc = LOWRH(ts, ff_ffmc, tindex);
				break;
			case 2:	adjffmc = MEDRH(ts, ff_ffmc, tindex);
				break;
			default:adjffmc = HIGHRH(ts, ff_ffmc, tindex);
				break;
		}
	}
	else {
		adjffmc = MAINTBL(ts, ff_ffmc);
		if (adjffmc < 0)
			adjffmc = 0;
		else if (adjffmc > 101.0)
			adjffmc = 101.0;
	}
	return adjffmc;
}


/* this is the hourly ffmc routine given wx and previous ffmc */
double calc_hourly_ffmc_lawson_contiguous(double ff_ffmc_prev, double ff_ffmc_curr, const WTimeSpan &ts, double rh_0, double rh_t, double rh_1, bool contiguous) {
	if ((ff_ffmc_prev < 0.0) || (ff_ffmc_prev > 101.0) || 
	    (ff_ffmc_curr < 0.0) || (ff_ffmc_curr > 101.0) ||
	    (ts < WTimeSpan(0, -12, 0, 0)) || (ts >= WTimeSpan(1, 11, 0, 0))) {
		weak_assert(false);
		return -98;
	}

	if (ts >= WTimeSpan(0, 12, 0, 0))
		return calc_hourly_ffmc_lawson(ff_ffmc_curr, ts, rh_t);

	if ((ts <= WTimeSpan(0, 5, 0, 0)) || (!contiguous))
		return calc_hourly_ffmc_lawson(ff_ffmc_prev, ts, rh_t);

	WTimeSpan h0(ts);
	h0.PurgeToHour();

	if (h0 == ts)
		return calc_hourly_ffmc_lawson(ff_ffmc_prev, h0, rh_0);

	WTimeSpan h1 = h0 + WTimeSpan(60 * 60);

	double ffmc1, ffmc2;
	ffmc1 = calc_hourly_ffmc_lawson(ff_ffmc_prev, h0, rh_0);
	if (h1 == WTimeSpan(12 * 60 * 60))
		ffmc2 = calc_hourly_ffmc_lawson(ff_ffmc_curr, h1, rh_1);
	else	ffmc2 = calc_hourly_ffmc_lawson(ff_ffmc_prev, h1, rh_1);
	std::int64_t sec = ts.GetTotalSeconds() % (60 * 60);
	return ((ffmc2 * (double)sec) + (ffmc1 * (60.0 * 60.0 - (double)sec))) / (60.0 * 60.0);
}


double calc_dmc(const double in_dmc, const double rain, double temperature, const double latitude, const double /*longitude*/, const std::uint16_t mm, double rh) {
	if ((in_dmc < 0.0) || (temperature > 60.0) ||
	    (rain < 0.0) || (rain > 600.0))
		return -98;

	if (temperature < -50.0)	temperature = -50.0;
	else if (temperature > 60.0)	temperature = 60.0;

	if (rh < 0.0)		rh = 0.0;
	else if (rh > 1.0)	rh = 1.0;

	static const double EL[12] =    { 6.5, 7.5, 9.0, 12.8, 13.9, 13.9, 12.4, 10.9, 9.4, 8.0, 7.0, 6.0 };
	static const double EL_N20[12]= { 7.9, 8.4, 8.9, 9.5, 9.9, 10.2, 10.1, 9.7, 9.1, 8.6, 8.1, 7.8 };
	static const double EL_EQ[12] = { 9.0, 9.0, 9.0, 9.0, 9.0, 9.0, 9.0, 9.0, 9.0, 9.0, 9.0, 9.0 };
	static const double EL_S20[12]= { 10.1, 9.6, 9.1, 8.5, 8.1, 7.8, 7.9, 8.3, 8.9, 9.4, 9.9, 10.2 };
	static const double EL_NZ[12] = { 11.5, 10.5, 9.2, 7.9, 6.8, 6.2, 6.5, 7.4, 8.7, 10.0, 11.2, 11.8 };

	const double *el;
									// from Cordy 060203 over the phone - slight change from the stuff in the paper from NZ but provided by Marty
									// Alexander

	if (latitude >= DEGREE_TO_RADIAN(30.0))
		el = EL;
	else if (latitude <= DEGREE_TO_RADIAN(-30.0))
		el = EL_NZ;	
	else if (latitude >= DEGREE_TO_RADIAN(10.0))
		el = EL_N20;
	else if (latitude <= DEGREE_TO_RADIAN(-10.0))
		el = EL_S20;
	else
		el = EL_EQ;
		
	double po, rk, wmi, rw, b, wmr, pr ;
	double c_d ;

	po = in_dmc;

	if (temperature < -1.1)
		rk = 0.0;
	else
		rk = 1.894 * (temperature + 1.1) * (1.0 - rh) * el[mm] * 0.01;
	if (rain > 1.5) {
		rw = 0.92 * rain - 1.27;				// eqn 11
		wmi = 20.0 + (exp(5.6348 - (po / 43.43)));		// eqn 12
		if (po <= 33.0)
			b = 100.0 / (0.5 + (0.3 * po));			// eqn 13a
		else if (po > 65.0)
			b = 6.2 * log(po) - 17.2;			// eqn 13c
		else
			b = 14.0 - 1.3 * log(po);			// eqn 13b
		wmr = wmi + (1000.0 * rw) / (48.77 + b * rw);		// eqn 14
		pr = 43.43 * (5.6348 - log(wmr - 20.0));

	}
	else
		pr = po;

	if (pr < 0.0)
		pr = 0.0;
	c_d = pr + rk;
	if (c_d < 0.0)
		c_d = 0.0;
	return c_d;
} 

//*********************** Drought Code *****************************************}
double calc_dc(const double in_dc, double rain, double temperature, const double latitude, const double /*longitude*/, const std::uint16_t mm/* 0..11 */){
	if ((in_dc < 0.0) ||
	    (rain < 0.0) || (rain > 600.0))
		return -98;

	if (temperature < -50.0)
		temperature = -50.0;
	else if (temperature > 60.0)
		temperature = 60.0;

	static const double FL[12]	= { -1.6, -1.6, -1.6, 0.9, 3.8, 5.8, 6.4, 5.0, 2.4, 0.4, -1.6, -1.6 };
	static const double FL_EQ[12]	= { 1.4, 1.4, 1.4, 1.4, 1.4, 1.4, 1.4, 1.4, 1.4, 1.4, 1.4, 1.4 };
	static const double FL_NZ[12]	= { 6.4, 5.0, 2.4, 0.4, -1.6, -1.6, -1.6, -1.6, -1.6, 0.9, 3.8, 5.8 };
	const double *fl;

	if (latitude >= DEGREE_TO_RADIAN(10.0))
		fl = FL;
	else if (latitude <= DEGREE_TO_RADIAN(-10.0))
		fl = FL_NZ;
	else
		fl = FL_EQ;

	double pe,dr,smi,c_d ;

	if (temperature < -2.8)
		temperature = -2.8;

	pe = (0.36 * (temperature + 2.8) + fl[mm]) / 2.0;
	if (rain <= 2.8) 
		dr = in_dc;
	else {
		// ********** rw wasn't defined so made it a real in this function's scope **********}
		rain = 0.83 * rain - 1.27;
		smi = 800.0 * exp(-in_dc / 400.0);
		dr = in_dc - 400.0 * log(1.0 + ((3.937 * rain) / smi));
		if (dr < 0.0)
			dr = 0.0;
	}
	c_d = dr + pe;
	if (c_d < 0.0)
		c_d = 0.0; 

	return c_d;
} 


double calc_ff(const WTimeSpan &ts, const double ffmc) {
	double factor, hour_frac = (double)ts.GetTotalSeconds() / 60.0 / 60.0;
	double hour_frac2 = hour_frac - floor(hour_frac);
	if (hour_frac2 > 1e-4)	factor = 147.27723;
	else			factor = 147.2;

	double fm = factor * (101.0 - ffmc) / (59.5 + ffmc);
	double sf = 91.9 * exp(fm * (-0.1386)) * (1.0 + pow(fm, 5.31) / 49300000.0);
	return sf;
}


double calc_isi(const WTimeSpan &ts, const double ffmc, const double ws, double *sf) {
	*sf = calc_ff(ts, ffmc);
	double isi = 0.208 * (*sf) * exp(0.05039 * ws);
	return isi;
}


double calc_isi1(const double ws, const double sf) {
	double isi = 0.208 * sf * exp(0.05039 * ws);
	return isi;
}


double calc_isi_fbp(const WTimeSpan &ts, const double ffmc, const double ws, double *sf) {
	*sf = calc_ff(ts, ffmc);

	double fW;

	if (ws <= 40.0)
		fW = exp(0.05039 * ws);				// equation 53
	else
		fW = 12.0 * (1 - exp(-0.0818 * (ws - 28.0)));	// equation 53a

	double isi = 0.208 * fW * (*sf);			// equation 52

	return isi;
}


double calc_isi_fbp1(/*double ffmc,*/ const double ws, const double sf) {
	double fW;
	if (ws <= 40.0)
		fW = exp(0.05039 * ws);				// equation 53
	else
		fW = 12.0 * (1 - exp(-0.0818 * (ws - 28.0)));	// equation 53a

	double isi = 0.208 * fW * sf;				// equation 52

	return isi;
}


double calc_bui(const double dc, const double dmc) {
	double bui;
	if ((dmc == 0.0) || (dc == 0.0))
		bui = 0.0;
	else	bui = (0.8 * dc * dmc) / (dmc + 0.4 * dc);

	if (bui < dmc) {
		double p = (dmc - bui) / dmc;
		double cc = 0.92 + pow(0.0114 * dmc, 1.7);
		bui = dmc - cc * p;
		if (bui < 0.0)
			bui = 0.0;
	}
	return bui;
}


double calc_fwi(const double isi, const double bui) {
	double bb;
	double fwi;
	if (bui > 80.0)
		bb = 0.1 * isi * (1000.0 / (25.0 + 108.64 / exp(0.023 * bui)));
	else	bb = 0.1 * isi * (0.626 * pow(bui, 0.809) + 2.0);

	if (bb <= 1.0)
		fwi = bb;
	else	fwi = exp(2.72 * pow(0.434 * log(bb), 0.647));
	return fwi;
}


double calc_dsr(const double fwi) {
	double dsr = 0.0272 * pow(fwi, 1.77);
	return dsr;
}
