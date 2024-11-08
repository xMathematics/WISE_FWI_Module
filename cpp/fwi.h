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

#pragma once

#include "WTime.h"

using namespace HSS_Time;

double calc_subdaily_ffmc_vanwagner(const WTimeSpan &ts, const double in_ffmc, const double rain, double temperature, const double rh, double ws);
double calc_previous_hourly_ffmc_vanwagner(const double current_ffmc, 
					   const double rain, 
					   double temperature, 
					   const double rh, 
					   double ws);
double calc_daily_ffmc_vanwagner(const double in_ffmc, const double rain, double temperature, const double rh, double ws);

double calc_hourly_ffmc_lawson(double ff_ffmc, WTimeSpan ts, double rh);
double calc_hourly_ffmc_lawson_contiguous(double ff_ffmc_prev, double ff_ffmc_curr, const WTimeSpan &ts, double rh_0, double rh_t, double rh_1, bool contiguous);

double calc_dmc (const double in_dmc, const double rain, double temperature, const double latitude, const double longitude, const std::uint16_t mm, const double rh);// duff moisture code
double calc_dc  (const double in_dc, double rain, double temperature, const double latitude, const double longitude, const std::uint16_t mm);		// drought code

double calc_ff  (const WTimeSpan &ts, const double ffmc);							// the ffmc func. from the ISI eq.
double calc_isi (const WTimeSpan &ts, const double ffmc, const double ws, double *sf);			// initial spread index
double calc_isi1(/*double ffmc,*/ const double ws, const double sf);
double calc_isi_fbp (const WTimeSpan &ts, const double ffmc, const double ws, double *sf);			// initial spread index
double calc_isi_fbp1(/*double ffmc,*/ const double ws, const double sf);
double calc_bui (const double dc, const double dmc);						// build-up index

double calc_fwi	(const double isi, const double bui);
double calc_dsr	(const double fwi ) ;
