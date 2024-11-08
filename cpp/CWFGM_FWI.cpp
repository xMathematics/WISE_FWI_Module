/**
 * WISE_FWI_Module: CWFGM_FWI.h
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
#include "CWFGM_FWI.h"
#include "fwi.h"
#include "types.h"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/////////////////////////////////////////////////////////////////////////////
// CCWFGM_FWI

HRESULT CCWFGM_FWI::HourlyFFMC_VanWagner(double in_ffmc, double rain, double temperature, double rh,
	double ws, std::uint32_t seconds_since_ffmc, double *ffmc) {
	if (!ffmc)
		return E_POINTER;
	try {
#ifdef _DEBUG
		weak_assert(seconds_since_ffmc <= (60 * 60));
		weak_assert(seconds_since_ffmc > 0);
#endif
		if (seconds_since_ffmc > (2 * 60 * 60))
			return E_INVALIDARG;

		WTimeSpan duration(seconds_since_ffmc);
		if ((*ffmc = calc_subdaily_ffmc_vanwagner(duration, in_ffmc, rain, temperature, rh, ws)) < 0.0) {
			weak_assert(false);
			return E_INVALIDARG;
		}
	}
	catch (...) {
		weak_assert(false);
		*ffmc = -97.0;
		return E_INVALIDARG;
	}
	return S_OK;
}


HRESULT CCWFGM_FWI::HourlyFFMC_Lawson(double in_prev_std_ffmc, double in_curr_std_ffmc, double /*rain*/, double /*temperature*/, double rh,
	double /*ws*/, /*in*/ unsigned long seconds_into_day, double *ffmc) {
	if (!ffmc)
		return E_POINTER;
	try {
		WTimeSpan ts((std::int64_t)(std::int32_t)seconds_into_day);
		if ((*ffmc = calc_hourly_ffmc_lawson_contiguous(in_prev_std_ffmc, in_curr_std_ffmc, ts, rh * 100.0, rh * 100.0, rh * 100.0, false)) < 0.0)
		{
			weak_assert(false);
			return E_INVALIDARG;
		}
	}
	catch (...) {
		weak_assert(false);
		*ffmc = -97.0;
		return E_INVALIDARG;
	}
	return S_OK;
}


HRESULT CCWFGM_FWI::HourlyFFMC_VanWagner_Previous(double in_ffmc, double rain, double temperature, double rh,
	double ws, double *ffmc) {
	if (!ffmc)
		return E_POINTER;
	try {
		if ((*ffmc = calc_previous_hourly_ffmc_vanwagner(in_ffmc, rain, temperature, rh, ws)) < 0.0) {
			weak_assert(false);
			return E_INVALIDARG;
		}
	}
	catch (...) {
		weak_assert(false);
		*ffmc = -97.0;
		return E_INVALIDARG;
	}
	return S_OK;
}


HRESULT CCWFGM_FWI::HourlyFFMC_Lawson_Contiguous(double in_ffmc_prevday, double in_ffmc_currday, double /*rain*/, double /*temperature*/,
	double rh_0, double rh_t, double rh_1, double /*ws*/, unsigned long seconds_into_day, double *ffmc) {
	if (!ffmc)
		return E_POINTER;
	try {
		WTimeSpan ts((std::int64_t)(std::int32_t)seconds_into_day);
		if ((*ffmc = calc_hourly_ffmc_lawson_contiguous(in_ffmc_prevday, in_ffmc_currday, ts, rh_0 * 100.0, rh_t * 100.0, rh_1 * 100.0, true)) < 0.0) {
			weak_assert(false);
			return E_INVALIDARG;
		}
	}
	catch (...) {
		weak_assert(false);
		*ffmc = -97.0;
		return E_INVALIDARG;
	}
	return S_OK;
}


HRESULT CCWFGM_FWI::DailyFFMC_VanWagner(double in_ffmc, double rain, double temperature, double rh,
	double ws, double *ffmc) {
	if (!ffmc)
		return E_POINTER;
	try {
		if ((*ffmc = calc_daily_ffmc_vanwagner(in_ffmc, rain, temperature, rh, ws)) < 0.0) {
			weak_assert(false);
			return E_INVALIDARG;
		}
	}
	catch (...) {
		weak_assert(false);
		*ffmc = -97.0;
		return E_INVALIDARG;
	}
	return S_OK;
}


HRESULT CCWFGM_FWI::DMC(double in_dmc, double rain, double temperature,  double latitude,  double longitude,  unsigned short month,
	double rh, double *dmc) {
	if (!dmc)
		return E_POINTER;
	weak_assert(latitude);
	weak_assert(longitude);
	if (month > 11)
		return E_INVALIDARG;
	try {
		if ((*dmc = calc_dmc(in_dmc, rain, temperature, latitude, longitude, month, rh)) < 0.0) {
			weak_assert(false);
			return E_INVALIDARG;
		}
	}
	catch (...) {
		weak_assert(false);
		*dmc = -97.0;
		return E_INVALIDARG;
	}
	return S_OK;
}


HRESULT CCWFGM_FWI::DC(double in_dc, double rain, double temperature,  double latitude,  double longitude,  unsigned short month, double *dc) {
	if (!dc)
		return E_POINTER;
	weak_assert(latitude);
	weak_assert(longitude);
	if (month > 11)
		return E_INVALIDARG;
	try {
		if ((*dc = calc_dc(in_dc, rain, temperature, latitude, longitude, month)) < 0.0) {
			weak_assert(false);
			return E_INVALIDARG;
		}
	}
	catch (...) {
		weak_assert(false);
		*dc = -97.0;
		return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT CCWFGM_FWI::FF(double ffmc, std::uint32_t seconds_since_ffmc, double *ff) {
	if (!ff)
		return E_POINTER;
	try {
		WTimeSpan duration(seconds_since_ffmc);
		*ff = calc_ff(duration, ffmc);
	}
	catch (...) {
#ifdef _DEBUG
		weak_assert(false);
#endif
		*ff = -97.0;
		return E_INVALIDARG;
	}
	return S_OK;
}


HRESULT CCWFGM_FWI::ISI_FWI(double ffmc, double ws, std::uint32_t seconds_since_ffmc, double *isi) {
	if (!isi)
		return E_POINTER;
	try {
		double m_ff;
		WTimeSpan duration(seconds_since_ffmc);
		*isi = calc_isi(duration, ffmc, ws, &m_ff);
	}
	catch (...) {
		weak_assert(false);
		*isi = -97.0;
		return E_INVALIDARG;
	}
	return S_OK;
}


HRESULT CCWFGM_FWI::ISI_FBP(double ffmc, double ws, std::uint32_t seconds_since_ffmc, double *isi) const {
	if (!isi)
		return E_POINTER;
	try {
		double m_ff_fbp;
		WTimeSpan duration(seconds_since_ffmc);
		*isi = calc_isi_fbp(duration, ffmc, ws, &m_ff_fbp);
	}
	catch (...) {
		weak_assert(false);
		*isi = -97.0;
		return E_INVALIDARG;
	}
	return S_OK;
}


HRESULT CCWFGM_FWI::BUI(double dc, double dmc, double *bui) const {
	if (!bui)
		return E_POINTER;
	try {
		*bui = calc_bui(dc, dmc);
	}
	catch (...) {
		weak_assert(false);
		*bui = -97.0;
		return E_INVALIDARG;
	}
	return S_OK;
}


HRESULT CCWFGM_FWI::FWI(double isi, double bui, double *fwi) const {
	if (!fwi)
		return E_POINTER;
	try {
		*fwi = calc_fwi(isi, bui);
	}
	catch (...) {
		weak_assert(false);
		*fwi = -97.0;
		return E_INVALIDARG;
	}
	return S_OK;
}


HRESULT CCWFGM_FWI::DSR(double fwi, double *dsr) {
	if (!dsr)
		return E_POINTER;
	try {
		*dsr = calc_dsr(fwi);
	}
	catch (...) {
		weak_assert(false);
		*dsr = -97.0;
		return E_INVALIDARG;
	}
	return S_OK;
}
