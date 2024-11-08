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

#pragma once


#if defined(_MSC_VER) || defined(__CYGWIN__)
#  ifdef FWI_EXPORTS
#    ifdef __GNUC__
#      define FWICOM_API __attribute__((dllexport))
#      define NO_THROW __attribute__((nothrow))
#    else
#      define FWI_API __declspec(dllexport)
#      define NO_THROW __declspec(nothrow)
#    endif
#  else
#    ifdef __GNUC__
#      define FWI_API __attribute__((dllimport))
#      define NO_THROW __attribute__((nothrow))
#    else
#      define FWI_API __declspec(dllimport)
#      define NO_THROW __declspec(nothrow)
#    endif
#  endif
#else
#  define NO_THROW __attribute__((nothrow))
#  if __GNUC__ >= 4
#    define FWI_API __attribute__((visibility("default")))
#  else
#    define FWI_API
#  endif
#endif


// main symbols
#include <cstdint>
#include <array>

#include "hresult.h"


/**	CFFDRS FWI Implementation
 * 
 * The FWI standard is the first major subsystem of the CFFDRS to be completed.  It provides relative measures of fuel moisture and fire behavior potential.  It is encapsulated in its own COM object.  A COM interface was chosen over a regular DLL interface only so that applications programmed in other languages could use this functionality.  This object does not support the standard COM IPersistStream, IPersistStreamInit, and IPersistStorage interfaces, since this object does not maintain any state information, it is only a collection of methods.
 */
class FWI_API CCWFGM_FWI
{
public:
	/**
	 * Default Constructor
	 */
	CCWFGM_FWI() = default;
	virtual ~CCWFGM_FWI() = default;

public:
	/**
	 * Calculates hourly FFMC (fine fuel moisture code), using Van Wagner's mathematical model, based on the previous hour's FFMC and provided conditions.
	 * FFMC is a numerical rating of the moisture content of litter and other cured fine fuels.
	 * \param in_ffmc The previous time's Van Wagner FFMC value
	 * \param rain Precipitation since observed FFMC, mm
	 * \param temperature Celsius
	 * \param rh Relative humidity expressed as a fraction ([0..1])
	 * \param ws Wind speed (kph)
	 * \param seconds_since_ffmc Seconds since observed FFMC
	 * \param ffmc Calculated FFMC value
   * 
	 * \retval E_POINTER The FFMC address provided is invalid
	 * \retval S_OK Successful
	 * \retval E_INVALIDARG Failure during calculation, or the parameter seconds_since_ffmc is greater than 7200 seconds
	 */
	virtual NO_THROW HRESULT HourlyFFMC_VanWagner(double in_ffmc, double rain, double temperature, double rh, double ws, std::uint32_t seconds_since_ffmc, double *ffmc);
	/**
	 * Calculates hourly FFMC (fine fuel moisture code), using Lawson's mathematical model, based on the previous and current daily FFMC values (using Van Wagner) and provided conditions.   FFMC is a numerical rating of the moisture content of litter and other cured fine fuels.
	 * \param prev_std_ffmc The previous day's standard daily Van Wagner FFMC value
	 * \param curr_std_ffmc The current day's standard daily Van Wagner FFMC value
	 * \param rain Precipitation, mm (ignored)
	 * \param temperature Celsius
	 * \param rh Relative humidity expressed as a fraction ([0..1])
	 * \param ws Wind speed (kph)
	 * \param seconds_into_day Local standard time
	 * \param ffmc Calculated FFMC value
   * 
	 * \retval E_POINTER The FFMC address provided is invalid
	 * \retval S_OK Successful
	 * \retval E_INVALIDARG Failure during calculation or if seconds_into_day is greater than 86400.
   */
	virtual NO_THROW HRESULT HourlyFFMC_Lawson(double in_ffmc_prevday, double in_ffmc_currday, double rain, double temperature, double rh, double ws, unsigned long seconds_into_day, double *ffmc);
	/**
	 * Calculates the previous hourly FFMC (fine fuel moisture code), using Van Wagner's mathematical model, based on the current hour's FFMC and previous hour's provided conditions.   FFMC is a numerical rating of the moisture content of litter and other cured fine fuels.
	 * \param current_ffmc The current time's Van Wagner FFMC value
	 * \param rain Precipitation in the prior hour, mm
	 * \param temperature Celsius
	 * \param rh Relative humidity expressed as a fraction ([0..1])
	 * \param ws Wind speed (kph)
	 * \param prev_ffmc Calculated previous FFMC value
   * 
	 * \retval E_POINTER The FFMC address provided is invalid
	 * \retval S_OK Successful
	 * \retval E_INVALIDARG Failure during calculation
   */
	virtual NO_THROW HRESULT HourlyFFMC_VanWagner_Previous(double current_ffmc, double rain, double temperature, double rh, double ws, double *prev_ffmc);
	/**
	 * Calculates a contiguous hourly FFMC (fine fuel moisture code), using Lawson's mathematical model, based on the daily Van Wagner FFMC values for the previous and current days, as well as  current provided conditions.   FFMC is a numerical rating of the moisture content of litter and other cured fine fuels.  This technique uses linear interpolation between 11am and noon LST.  It also applies similar smoothing in morning hours.
	 * \param in_ffmc_prevday The previous day's standard daily Van Wagner FFMC value
	 * \param in_ffmc_currday The current day's standard daily Van Wagner FFMC value
	 * \param rain Precipitation, mm (ignored)
	 * \param temperature Celsius
	 * \param rh_0 Relative humidity at the start of the hour, expressed as a fraction ([0..1])
	 * \param rh Instantaneous relative humidity expressed as a fraction ([0..1])
	 * \param rh_1 Relative humidity at the end of the hour (start of next hour) expressed as a fraction ([0..1])
	 * \param ws Wind speed (kph)
	 * \param seconds_into_day Local standard time
	 * \param ffmc Calculated FFMC value
   * 
	 * \retval E_POINTER The FFMC address provided is invalid
	 * \retval S_OK Successful
	 * \retval E_INVALIDARG Failure during calculation or if seconds_into_day is greater than 86400.
   */
	virtual NO_THROW HRESULT HourlyFFMC_Lawson_Contiguous(double in_ffmc_prevday, double in_ffmc_currday, double rain, double temperature, double rh_0, double rh, double rh_1, double ws, unsigned long seconds_into_day, double *ffmc);
	/**
	 * Calculates daily FFMC (fine fuel moisture code), based on the previous day's FFMC and provided conditions.   FFMC is a numerical rating of the moisture content of litter and other cured fine fuels.
	 * \param in_ffmc The previous day's Van Wagner FFMC value
	 * \param rain Precipitation in the prior 24 hours (noon to noon, LST), mm
	 * \param temperature Noon (LST) temperature, Celsius
	 * \param rh Relative humidity expressed as a fraction ([0..1])
	 * \param ws Wind speed (kph) at noon LST
	 * \param ffmc Calculated FFMC value
   *
	 * \retval E_POINTER The FFMC address provided is invalid
	 * \retval S_OK Successful
	 * \retval E_INVALIDARG Failure during calculation
   */
	virtual NO_THROW HRESULT DailyFFMC_VanWagner(double in_ffmc, double rain, double temperature, double rh, double ws, double *ffmc);
	/**
	 * Calculates daily DMC (duff moisture code) given the previous day's DMC and provided conditions.  DMC provides a numerical rating of the average moisture content of the loosely compact organic layers of a moderate depth (5-10cm).
	 * \param in_dmc The previous day's DMC value
	 * \param rain Precipitation in the prior 24 hours (noon to noon, LST), mm
	 * \param temperature Noon (LST) temperature, Celsius
	 * \param latitude Radians, used to determine the appropriate table as defined in "Latitude Considerations in Adapting the Canadian Forest Fire Weather Index System To Other Countries", M.E. Alexander, in prep as Index X in Weather Guide for the Canadian Forest Fire Danger Rating System by B.D. Lawson, O.B. Armitage.
	 * \param longitude Radians, reserved for future use, currently unused (may be if/as/when more regional constants are defined)
	 * \param month Origin 0 (January = 0, December = 11)
	 * \param rh Relative humidity expressed as a fraction ([0..1]) at noon LST
	 * \param dmc Calculated DMC value
   *
	 * \retval E_POINTER The DMC address provided is invalid
	 * \retval S_OK Successful
	 * \retval E_INVALIDARG Failure during calculation or month is greater than 11, assuming January is 0.
   */
	virtual NO_THROW HRESULT DMC(double in_dmc, double rain, double temperature, double latitude, double longitude, unsigned short month, double rh, double *dmc);
	/**
	 * Calculates DC (drought code) given the previous day's DC and provided conditions.  DC provides a numerical rating of the average moisture content of the deep (10-24 cm), compact organic layers.
	 * \param in_dc The previous day's DC value
	 * \param rain Precipitation in the prior 24 hours (noon to noon, LST), mm
	 * \param temperature Noon (LST) temperature, Celsius
	 * \param latitude Radians, used to determine the appropriate table as defined in "Latitude Considerations in Adapting the Canadian Forest Fire Weather Index System To Other Countries", M.E. Alexander, in prep as Index X in Weather Guide for the Canadian Forest Fire Danger Rating System by B.D. Lawson, O.B. Armitage.
	 * \param longitude Radians, reserved for future use, currently unused (may be if/as/when more regional constants are defined)
	 * \param month Origin 0 (January = 0, December = 11)
	 * \param dc Calculated DC value
   *
	 * \retval E_POINTER The DC address provided is invalid
	 * \retval S_OK Successful
	 * \retval E_INVALIDARG Failure during calculation or month is greater than 11, assuming January is 0.
   */
	virtual NO_THROW HRESULT DC(double in_dc, double rain, double temperature, double latitude, double longitude, unsigned short month, double *dc);
	/**
	 * Calculates f(F), which is used to calculate ISI from FFMC.
	 * \param ffmc FFMC value
	 * \param seconds_since_ffmc Seconds since observed ffmc
	 * \param ff Calculated f(F) value
   *
	 * \retval E_POINTER The FF address provided is invalid
	 * \retval S_OK Successful
	 * \retval E_INVALIDARG Failure during calculation
   */
	virtual NO_THROW HRESULT FF(double ffmc, std::uint32_t seconds_since_ffmc, double *ff);
	/**
	 * Calculates ISI from FFMC and wind speed.  ISI provides a numerical rating of the relative expected rate of fire spread.
	 * \param ffmc FFMC value
	 * \param ws Wind speed (kph)
	 * \param seconds_since_ffmc Seconds since observed ffmc
	 * \param isi Calculated ISI value
   *
	 * \retval E_POINTER The ISI address provided is invalid
	 * \retval S_OK Successful
	 * \retval E_INVALIDARG Failure during calculation
	 */
	virtual NO_THROW HRESULT ISI_FWI(double ffmc, double ws, std::uint32_t seconds_since_ffmc, double *isi);
	/**
	 * Calculates ISI from FFMC and wind speed.  ISI provides a numerical rating of the relative expected rate of fire spread.  The FBP system uses a local site-specific ISI influenced by topography.
	 * \param ffmc FFMC value
	 * \param ws Wind speed (kph)
	 * \param seconds_since_ffmc Seconds since observed ffmc
	 * \param isi Calculated ISI value
   *
	 * \retval E_POINTER The ISI address provided is invalid
	 * \retval S_OK Successful
	 * \retval E_INVALIDARG Failure during calculation
	 */
	virtual NO_THROW HRESULT ISI_FBP(double ffmc, double ws, std::uint32_t seconds_since_ffmc, double *isi) const;
	/**
	 * Calculate BUI (buildup index) from DC, DMC.  BUI provides a numerical, relative indication of the amount of fuel available for combustion.
	 * \param dc DC value
	 * \param dmc DMC value
	 * \param bui Calculated BUI value
   *
	 * \retval E_POINTER The BUI address provided is invalid
	 * \retval S_OK Successful
	 * \retval E_INVALIDARG Failure during calculation
	 */
	virtual NO_THROW HRESULT BUI(double dc, double dmc, double *bui) const;
	/**
	 * Calculates FWI (fire weather index) from ISI and BUI.  FWI provides a numerical, relative rating of fire intensity.
	 * \param isi ISI value
	 * \param bui BUI value
	 * \param fwi Calculated FWI value
   *
	 * \retval E_POINTER The FWI address provided is invalid
	 * \retval S_OK Successful
	 * \retval E_INVALIDARG Failure during calculation
	 */
	virtual NO_THROW HRESULT FWI(double isi, double bui, double *fwi) const;
	/**
	 * Calculates DSR from FWI.
	 * \param fwi FWI value
	 * \param dsr Calculated DSR value
   *
	 * \retval E_POINTER The DSR address provided is invalid
	 * \retval S_OK Successful
	 * \retval E_INVALIDARG Failure during calculation
	 */
	virtual NO_THROW HRESULT DSR(double fwi, double *dsr);
};
