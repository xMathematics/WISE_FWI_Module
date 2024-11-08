/***********************************************************************
 * REDapp - FWICalculations.java
 * Copyright (C) 2015-2019 The REDapp Development Team
 * Homepage: http://redapp.org
 * 
 * REDapp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * REDapp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with REDapp. If not see <http://www.gnu.org/licenses/>. 
 **********************************************************************/

package ca.wise.fwi;

import static ca.hss.math.General.*;

import java.util.Calendar;

import ca.hss.times.WTimeSpan;
import ca.hss.times.TimeZoneInfo;
import ca.hss.times.WorldLocation;

public class FWICalculations {
	public double ystrdyFFMC;
	public double ystrdyDMC;
	public double ystrdyDC;
	public double noonTemp;
	public double noonRH;
	public double noonPrecip;
	public double noonWindSpeed;
	public double hrlyTemp;
	public double hrlyRH;
	public double hrlyPrecip;
	public double hrlyWindSpeed;
	public double dlyFFMC;
	public double dlyDMC;
	public double dlyDC;
	public double dlyISI;
	public double dlyBUI;
	public double dlyFWI;
	public double dlyDSR;
	public double hlyHFFMC;
	public double hlyHISI;
	public double hlyHFWI;
	public double prvhlyFFMC;
	public boolean calcHourly;
	public boolean useVanWagner;
	/**
	 * Calculate the previous hour's FFMC value using the Lawson hourly FFMC equation
	 * and use that value in the Van Wagner hourly FFMC method. Only has an effect if
	 * {@link FWICalculations#useVanWagner} is {@code true}.
	 */
	public boolean useLawsonPreviousHour = false;
	private WorldLocation wl;
	public Calendar m_date;
	
	/**
	 * Gets the currently set latitude.
	 * @return The latitude in degrees.
	 */
	public double getLatitude() {
		return RADIAN_TO_DEGREE(wl.getLatitude());
	}
	
	/**
	 * Set the latitude.
	 * @param value The latitude in degrees.
	 */
	public void setLatitude(double value) {
		wl.setLatitude(DEGREE_TO_RADIAN(value));
	}

	/**
	 * Gets the currently set longitude.
	 * @return The longitude in degrees.
	 */
	public double getLongitude() {
		return RADIAN_TO_DEGREE(wl.getLongitude());
	}
	
	/**
	 * Set the longitude.
	 * @param value The longitude in degrees.
	 */
	public void setLongitude(double value) {
		wl.setLongitude(DEGREE_TO_RADIAN(value));
	}
	
	/**
	 * Set the timezone.
	 * @param span
	 */
	public void setTimezone(WTimeSpan span) {
		wl.setTimezoneOffset(new WTimeSpan(span));
	}
	
	/**
	 * Set daylight savings time.
	 * @param span
	 */
	public void setDST(WTimeSpan span) {
		wl.setDSTAmount(new WTimeSpan(span));
	}

	public String m_init_timezone_code;

	public FWICalculations() {
		initializeInputValues();
		initializeOutputValues();
	}

	private void initializeOutputValues() {
		useVanWagner = false;
		hrlyTemp = 0.00;
		hrlyRH = 0.00;
		hrlyPrecip = 0.00;
		hrlyWindSpeed = 0.00;
		dlyFFMC = 0.00;
		dlyDMC = 0.00;
		dlyBUI = 0.00;
		dlyDC = 0.00;
		dlyISI = 0.00;
		dlyBUI = 0.00;
		dlyFWI = 0.00;
		dlyDSR = 0.00;
	}

	private void initializeInputValues() {
		wl = new WorldLocation();
		wl.setLatitude(DEGREE_TO_RADIAN(54.000012));
		wl.setLongitude(DEGREE_TO_RADIAN(-115.000021));
		TimeZoneInfo tmi = wl.guessTimeZone((short) 0);
		wl.setDSTAmount(tmi.getDSTAmount());
		m_init_timezone_code = tmi.getCode();
		wl.setStartDST(new WTimeSpan(0));
		wl.setEndDST(new WTimeSpan(0));
		wl.setTimezoneOffset(tmi.getTimezoneOffset());
		ystrdyFFMC = 85.00;
		ystrdyDMC = 25.00;
		ystrdyDC = 200.00;
		noonTemp = 0.00;
		noonRH = 0.00;
		noonPrecip = 0.00;
		noonWindSpeed = 0.00;
		calcHourly = false;
		hrlyTemp = 0.0;
		hrlyRH = 0.0;
		hrlyPrecip = 0.0;
		hrlyWindSpeed = 0.0;
		prvhlyFFMC = 0.0;
		m_date = Calendar.getInstance();
	}

	public void FWICalculateDailyStatisticsCOM() {
		int month = m_date.get(Calendar.MONTH) + 1;
		int hour = m_date.get(Calendar.HOUR_OF_DAY);
		int min = m_date.get(Calendar.MINUTE);
		int sec = m_date.get(Calendar.SECOND);
		double lat = wl.getLatitude();
		double lon = wl.getLongitude();

		dlyFFMC = Fwi.dailyFFMCVanWagner(ystrdyFFMC, noonPrecip, noonTemp,
				noonRH * 0.01, noonWindSpeed);
		dlyDC = Fwi.dC(ystrdyDC, noonPrecip, noonTemp, lat, lon, month - 1);
		dlyDMC = Fwi.dMC(ystrdyDMC, noonPrecip, noonTemp, lat, lon, month - 1,
				noonRH * 0.01);
		dlyBUI = Fwi.bui(dlyDC, dlyDMC);
		dlyISI = Fwi.isiFWI(dlyFFMC, noonWindSpeed, 24 * 60 * 60);
		dlyFWI = Fwi.fwi(dlyISI, dlyBUI);
		dlyDSR = Fwi.dsr(dlyFWI);
		if (calcHourly) {
			WTimeSpan ts = new WTimeSpan(0, hour, min, sec);
			ts.subtract(wl.getDSTAmount());
			
			if (useLawsonPreviousHour)
				prvhlyFFMC = Fwi.hourlyFFMCLawson(ystrdyFFMC, dlyFFMC,
						hrlyRH * 0.01, (ts.getHours() * 3600) - 3600);
			
			if (useVanWagner) {
				hlyHFFMC = Fwi.hourlyFFMCVanWagner(prvhlyFFMC, hrlyPrecip,
						hrlyTemp, hrlyRH * 0.01, hrlyWindSpeed, 60 * 60);
			} else {
				hlyHFFMC = Fwi.hourlyFFMCLawson(ystrdyFFMC, dlyFFMC,
						hrlyRH * 0.01, ts.getHours() * 3600);
			}
			
			if (!useLawsonPreviousHour)
				prvhlyFFMC = Fwi.hourlyFFMCLawson(ystrdyFFMC, dlyFFMC,
						hrlyRH * 0.01, (ts.getHours() * 3600) - 3600);
			
			hlyHISI = Fwi.isiFWI(hlyHFFMC, hrlyWindSpeed, sec + (min * 60));
			double bui;
			if (wl.getDSTAmount().getTotalMinutes() > 0) {
				if (hour < 13) {
					bui = Fwi.bui(ystrdyDC, ystrdyDMC);
				}
				else {
					bui = dlyBUI;
				}
			}
			else {
				if (hour < 12) {
					bui = Fwi.bui(ystrdyDC, ystrdyDMC);
				}
				else {
					bui = dlyBUI;
				}
			}
			hlyHFWI = Fwi.fwi(hlyHISI, bui);
		}
	}
}
