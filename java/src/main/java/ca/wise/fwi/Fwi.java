/***********************************************************************
 * REDapp - Fwi.java
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

import ca.hss.annotations.Source;

import static ca.hss.math.General.DEGREE_TO_RADIAN;

/**
 * A class for computing FFMC values.
 *
 * @author Travis Redpath
 * @version 1.0 10/12/2011
 */
@Source(sourceFile="CWFGM_FWI.cpp", project="FWICOM")
public class Fwi {
	private static final double TOLERANCE = 0.0000001;

	protected Fwi() { }

	/**
	 * A table of values for low relative humidity.
	 */
	private static final double L[][] = {
			{ 9999, 17.5, 30.0, 40.0, 50.0, 55.0, 60.0, 65.0, 70.0, 72.0, 74.0,
					75.0, 76.0, 77.0, 78.0, 79.0, 80.0, 81.0, 82.0, 83.0, 84.0,
					85.0, 86.0, 87.0, 88.0, 89.0, 90.0, 91.0, 92.0, 93.0, 94.0,
					95.0, 96.0, 97.0, 98.0, 99.0, 100.0, 100.9, 101.0 },
			{ 600, 48.3, 49.4, 51.1, 53.5, 55.1, 56.9, 59.1, 61.7, 62.9, 64.1,
					64.8, 65.5, 66.2, 66.9, 67.7, 68.5, 69.4, 70.2, 71.1, 72.1,
					73.1, 74.1, 75.2, 76.3, 77.5, 78.7, 80.0, 81.3, 82.7, 84.1,
					85.7, 87.2, 88.8, 90.4, 91.9, 93.2, 93.8, 93.8 },
			{ 700, 50.7, 52.1, 53.9, 56.3, 57.9, 59.7, 61.8, 64.3, 65.4, 66.6,
					67.2, 67.9, 68.6, 69.3, 70.0, 70.7, 71.5, 72.3, 73.2, 74.0,
					75.0, 75.9, 76.9, 77.9, 79.0, 80.2, 81.4, 82.6, 83.9, 85.2,
					86.6, 88.1, 89.6, 91.1, 92.6, 93.9, 94.5, 94.5 },
			{ 800, 53.3, 54.9, 56.8, 59.3, 60.9, 62.6, 64.7, 67.0, 68.1, 69.2,
					69.8, 70.4, 71.0, 71.6, 72.3, 73.0, 73.7, 74.5, 75.3, 76.1,
					76.9, 77.8, 78.7, 79.7, 80.6, 81.7, 82.8, 83.9, 85.1, 86.3,
					87.7, 89.0, 90.4, 91.9, 93.3, 94.6, 95.3, 95.3 },
			{ 900, 59.6, 60.7, 62.2, 64.4, 65.7, 67.3, 69.1, 71.2, 72.1, 73.2,
					73.7, 74.2, 74.8, 75.4, 76.0, 76.7, 77.3, 78.0, 78.7, 79.5,
					80.3, 81.1, 81.9, 82.8, 83.7, 84.7, 85.7, 86.7, 87.8, 89.0,
					90.1, 91.4, 92.6, 93.9, 95.2, 96.3, 96.8, 96.8 },
			{ 1000, 66.8, 67.2, 68.2, 69.9, 70.9, 72.2, 73.8, 75.6, 76.5, 77.4,
					77.9, 78.4, 78.9, 79.4, 80.0, 80.5, 81.1, 81.8, 82.4, 83.1,
					83.8, 84.5, 85.3, 86.1, 86.9, 87.8, 88.7, 89.7, 90.6, 91.7,
					92.7, 93.8, 94.9, 96.0, 97.1, 97.9, 98.4, 98.4 },
			{ 1100, 74.5, 74.5, 74.9, 75.9, 76.6, 77.6, 78.8, 80.3, 81.0, 81.9,
					82.4, 83.0, 83.6, 84.1, 84.7, 85.2, 85.8, 86.3, 86.9, 87.4,
					88.0, 88.5, 89.0, 89.6, 90.1, 90.6, 91.1, 91.6, 92.1, 92.6,
					93.1, 93.8, 94.9, 96.0, 97.1, 97.9, 98.4, 98.4 },
			{ 1159, 83.0, 82.5, 82.3, 82.4, 82.7, 83.2, 84.1, 85.2, 85.8, 86.5,
					86.8, 87.2, 87.6, 87.9, 88.2, 88.6, 88.9, 89.2, 89.6, 89.9,
					90.2, 90.5, 90.9, 91.2, 91.5, 91.8, 92.1, 92.4, 92.7, 93.0,
					93.3, 93.8, 94.9, 96.0, 97.1, 97.9, 98.4, 98.4 },
			{ 1200, 83.0, 82.5, 82.3, 82.4, 82.7, 83.2, 84.1, 85.2, 85.8, 86.5,
					86.8, 87.2, 87.6, 87.9, 88.2, 88.6, 88.9, 89.2, 89.6, 89.9,
					90.2, 90.5, 90.9, 91.2, 91.5, 91.8, 92.1, 92.4, 92.7, 93.0,
					93.3, 93.8, 94.9, 96.0, 97.1, 97.9, 98.4, 98.4 } };

	/**
	 * A table of values for medium relative humidity.
	 */
	private static final double M[][] = {
			{ 9999, 17.5, 30.0, 40.0, 50.0, 55.0, 60.0, 65.0, 70.0, 72.0, 74.0,
					75.0, 76.0, 77.0, 78.0, 79.0, 80.0, 81.0, 82.0, 83.0, 84.0,
					85.0, 86.0, 87.0, 88.0, 89.0, 90.0, 91.0, 92.0, 93.0, 94.0,
					95.0, 96.0, 97.0, 98.0, 99.0, 100.0, 100.9, 101.0 },
			{ 600, 34.8, 39.2, 43.2, 47.6, 50.0, 52.6, 55.4, 58.4, 59.7, 61.1,
					61.8, 62.5, 63.3, 64.0, 64.8, 65.6, 66.4, 67.2, 68.1, 68.9,
					69.8, 70.8, 71.7, 72.7, 73.8, 74.8, 75.9, 77.1, 78.3, 79.5,
					80.8, 82.2, 83.6, 85.0, 86.5, 88.0, 89.1, 89.1 },
			{ 700, 36.3, 40.5, 44.3, 48.7, 51.2, 53.8, 56.7, 59.9, 61.3, 62.7,
					63.4, 64.2, 64.9, 65.7, 66.5, 67.4, 68.2, 69.1, 70.0, 70.9,
					71.9, 72.8, 73.9, 74.9, 75.9, 77.0, 78.2, 79.3, 80.5, 81.8,
					83.1, 84.4, 85.7, 87.0, 88.3, 89.5, 90.2, 90.2 },
			{ 800, 37.8, 41.7, 45.5, 49.8, 52.3, 55.1, 58.1, 61.4, 62.8, 64.3,
					65.1, 65.9, 66.7, 67.5, 68.4, 69.3, 70.1, 71.1, 72.0, 73.0,
					74.0, 75.0, 76.0, 77.1, 78.2, 79.3, 80.5, 81.7, 82.9, 84.1,
					85.4, 86.6, 87.9, 89.1, 90.2, 91.2, 91.6, 91.6 },
			{ 900, 44.6, 48.2, 51.6, 55.6, 57.8, 60.3, 63.0, 66.0, 67.3, 68.6,
					69.3, 70.1, 70.8, 71.6, 72.3, 73.1, 73.9, 74.8, 75.6, 76.5,
					77.4, 78.3, 79.3, 80.3, 81.3, 82.3, 83.4, 84.5, 85.7, 86.8,
					88.0, 89.2, 90.5, 91.7, 92.8, 93.8, 94.4, 94.4 },
			{ 1000, 52.5, 55.5, 58.5, 61.9, 63.9, 66.0, 68.4, 71.0, 72.1, 73.3,
					73.9, 74.5, 75.2, 75.9, 76.5, 77.2, 77.9, 78.7, 79.4, 80.2,
					81.0, 81.9, 82.7, 83.6, 84.5, 85.5, 86.5, 87.5, 88.5, 89.6,
					90.8, 91.9, 93.1, 94.3, 95.5, 96.7, 97.3, 97.3 },
			{ 1100, 61.6, 64.0, 66.3, 69.0, 70.6, 72.3, 74.2, 76.4, 77.3, 78.3,
					79.0, 79.6, 80.3, 80.9, 81.5, 82.2, 82.8, 83.4, 84.0, 84.6,
					85.3, 85.9, 86.5, 87.1, 87.7, 88.3, 88.9, 89.4, 90.0, 90.6,
					91.2, 91.9, 93.1, 94.3, 95.5, 96.7, 97.3, 97.3 },
			{ 1159, 72.1, 73.5, 75.0, 76.9, 77.9, 79.2, 80.6, 82.2, 82.9, 83.6,
					84.0, 84.4, 84.8, 85.2, 85.6, 86.0, 86.4, 86.7, 87.1, 87.5,
					87.9, 88.2, 88.6, 88.9, 89.3, 89.7, 90.0, 90.3, 90.7, 91.0,
					91.4, 91.9, 93.1, 94.3, 95.5, 96.7, 97.3, 97.3 },
			{ 1200, 72.1, 73.5, 75.0, 76.9, 77.9, 79.2, 80.6, 82.2, 82.9, 83.6,
					84.0, 84.4, 84.8, 85.2, 85.6, 86.0, 86.4, 86.7, 87.1, 87.5,
					87.9, 88.2, 88.6, 88.9, 89.3, 89.7, 90.0, 90.3, 90.7, 91.0,
					91.4, 91.9, 93.1, 94.3, 95.5, 96.7, 97.3, 97.3 } };

	/**
	 * A table of values for high relative humidity.
	 */
	private static final double H[][] = {
			{ 9999, 17.5, 30.0, 40.0, 50.0, 55.0, 60.0, 65.0, 70.0, 72.0, 74.0,
					75.0, 76.0, 77.0, 78.0, 79.0, 80.0, 81.0, 82.0, 83.0, 84.0,
					85.0, 86.0, 87.0, 88.0, 89.0, 90.0, 91.0, 92.0, 93.0, 94.0,
					95.0, 96.0, 97.0, 98.0, 99.0, 100.0, 100.9, 101.0 },
			{ 600, 28.2, 33.4, 37.9, 42.9, 45.6, 48.5, 51.7, 55.1, 56.5, 58.0,
					58.8, 59.5, 60.3, 61.2, 62.0, 62.9, 63.7, 64.6, 65.5, 66.5,
					67.4, 68.4, 69.4, 70.5, 71.6, 72.7, 73.8, 75.0, 76.2, 77.4,
					78.7, 80.0, 81.4, 82.7, 84.1, 85.4, 86.3, 86.3 },
			{ 700, 30.0, 34.8, 39.0, 43.8, 46.5, 49.4, 52.5, 55.9, 57.3, 58.8,
					59.6, 60.4, 61.2, 62.1, 62.9, 63.8, 64.7, 65.7, 66.6, 67.6,
					68.6, 69.6, 70.7, 71.8, 72.9, 74.1, 75.3, 76.5, 77.8, 79.1,
					80.5, 81.9, 83.3, 84.8, 86.2, 87.6, 88.4, 88.4 },
			{ 800, 31.9, 36.2, 40.2, 44.8, 47.4, 50.2, 53.3, 56.7, 58.2, 59.7,
					60.5, 61.3, 62.2, 63.0, 63.9, 64.8, 65.7, 66.7, 67.7, 68.7,
					69.8, 70.8, 71.9, 73.1, 74.3, 75.5, 76.8, 78.1, 79.4, 80.8,
					82.3, 83.8, 85.3, 86.9, 88.4, 89.8, 90.6, 90.6 },
			{ 900, 37.7, 42.1, 46.1, 50.5, 52.9, 55.5, 58.4, 61.5, 62.8, 64.2,
					64.9, 65.6, 66.4, 67.1, 67.9, 68.7, 69.5, 70.4, 71.3, 72.1,
					73.1, 74.0, 75.0, 76.0, 77.0, 78.1, 79.2, 80.3, 81.5, 82.7,
					84.0, 85.3, 86.7, 88.1, 89.5, 90.8, 91.7, 91.7 },
			{ 1000, 44.4, 48.9, 52.7, 56.8, 59.1, 61.4, 63.9, 66.7, 67.8, 69.0,
					69.6, 70.2, 70.9, 71.5, 72.2, 72.9, 73.6, 74.3, 75.0, 75.8,
					76.6, 77.3, 78.2, 79.0, 79.9, 80.8, 81.7, 82.6, 83.6, 84.7,
					85.8, 86.9, 88.0, 89.3, 90.5, 91.8, 92.8, 92.8 },
			{ 1100, 52.1, 56.5, 60.2, 63.9, 65.9, 67.9, 70.1, 72.3, 73.3, 74.3,
					74.9, 75.5, 76.1, 76.6, 77.2, 77.8, 78.4, 79.0, 79.5, 80.1,
					80.7, 81.2, 81.8, 82.4, 82.9, 83.5, 84.0, 84.6, 85.1, 85.6,
					86.2, 86.9, 88.0, 89.3, 90.5, 91.8, 92.8, 92.8 },
			{ 1159, 60.9, 65.2, 68.6, 71.8, 73.5, 75.1, 76.7, 78.4, 79.1, 79.8,
					80.2, 80.5, 80.8, 81.2, 81.5, 81.8, 82.1, 82.5, 82.8, 83.1,
					83.4, 83.7, 84.0, 84.3, 84.6, 84.9, 85.2, 85.5, 85.8, 86.1,
					86.4, 86.9, 88.0, 89.3, 90.5, 91.8, 92.8, 92.8 },
			{ 1200, 60.9, 65.2, 68.6, 71.8, 73.5, 75.1, 76.7, 78.4, 79.1, 79.8,
					80.2, 80.5, 80.8, 81.2, 81.5, 81.8, 82.1, 82.5, 82.8, 83.1,
					83.4, 83.7, 84.0, 84.3, 84.6, 84.9, 85.2, 85.5, 85.8, 86.1,
					86.4, 86.9, 88.0, 89.3, 90.5, 91.8, 92.8, 92.8 } };

	/**
	 * Main table for the remainder of hours for all of the relative humidity
	 * ranges.
	 */
	private static final double MAIN[][] = {
			{ 9999, 17.5, 30.0, 40.0, 50.0, 55.0, 60.0, 65.0, 70.0, 72.0, 74.0,
					75.0, 76.0, 77.0, 78.0, 79.0, 80.0, 81.0, 82.0, 83.0, 84.0,
					85.0, 86.0, 87.0, 88.0, 89.0, 90.0, 91.0, 92.0, 93.0, 94.0,
					95.0, 96.0, 97.0, 98.0, 99.0, 100.0, 100.9, 101.0 },
			{ 100, 23.4, 32.9, 40.5, 47.8, 51.4, 54.9, 58.3, 61.8, 63.3, 64.8,
					65.5, 66.3, 67.1, 67.9, 68.8, 69.6, 70.5, 71.4, 72.3, 73.2,
					74.1, 75.1, 76.1, 77.1, 78.1, 79.1, 80.2, 81.3, 82.4, 83.5,
					84.7, 85.9, 87.1, 88.3, 89.5, 90.7, 91.6, 91.6 },
			{ 200, 24.3, 33.0, 39.9, 46.8, 50.2, 53.6, 56.9, 60.4, 61.8, 63.4,
					64.1, 64.9, 65.7, 66.5, 67.4, 68.2, 69.1, 70.0, 70.9, 71.8,
					72.7, 73.7, 74.7, 75.7, 76.7, 77.8, 78.9, 80.0, 81.1, 82.3,
					83.4, 84.7, 85.9, 87.2, 88.4, 89.6, 90.5, 90.5 },
			{ 300, 25.2, 33.1, 39.4, 45.8, 49.0, 52.3, 55.6, 59.0, 60.5, 62.0,
					62.7, 63.5, 64.3, 65.1, 66.0, 66.8, 67.7, 68.6, 69.5, 70.4,
					71.4, 72.3, 73.3, 74.4, 75.4, 76.5, 77.6, 78.7, 79.8, 81.0,
					82.2, 83.5, 84.7, 86.0, 87.3, 88.5, 89.4, 89.4 },
			{ 400, 26.2, 33.2, 38.9, 44.8, 47.9, 51.0, 54.3, 57.7, 59.1, 60.6,
					61.4, 62.2, 63.0, 63.8, 64.6, 65.5, 66.3, 67.2, 68.1, 69.1,
					70.0, 71.0, 72.0, 73.0, 74.1, 75.2, 76.3, 77.4, 78.6, 79.8,
					81.0, 82.3, 83.6, 84.9, 86.2, 87.5, 88.4, 88.4 },
			{ 500, 27.2, 33.3, 38.4, 43.9, 46.7, 49.8, 52.9, 56.4, 57.8, 59.3,
					60.1, 60.8, 61.6, 62.5, 63.3, 64.2, 65.0, 65.9, 66.8, 67.8,
					68.7, 69.7, 70.7, 71.7, 72.8, 73.9, 75.0, 76.2, 77.4, 78.6,
					79.8, 81.1, 82.5, 83.8, 85.2, 86.4, 87.3, 87.3 },
			{ 559, 28.2, 33.4, 37.9, 42.9, 45.7, 48.6, 51.7, 55.1, 56.5, 58.0,
					58.8, 59.6, 60.4, 61.2, 62.0, 62.9, 63.8, 64.6, 65.6, 66.5,
					67.5, 68.4, 69.5, 70.5, 71.6, 72.7, 73.8, 75.0, 76.2, 77.4,
					78.7, 80.0, 81.4, 82.7, 84.1, 85.4, 86.3, 86.3 },
			{ 600, 28.2, 33.4, 37.9, 42.9, 45.7, 48.6, 51.7, 55.1, 56.5, 58.0,
					58.8, 59.6, 60.4, 61.2, 62.0, 62.9, 63.8, 64.6, 65.6, 66.5,
					67.5, 68.4, 69.5, 70.5, 71.6, 72.7, 73.8, 75.0, 76.2, 77.4,
					78.7, 80.0, 81.4, 82.7, 84.1, 85.4, 86.3, 86.3 },
			{ 1200, 17.5, 27.7, 34.4, 40.9, 44.5, 48.2, 52.5, 57.3, 59.4, 61.7,
					62.9, 64.2, 65.5, 66.9, 68.5, 70.5, 73.8, 76.4, 78.4, 80.0,
					81.5, 82.8, 84.0, 85.2, 86.3, 87.5, 88.6, 89.7, 90.8, 91.9,
					92.9, 94.0, 95.0, 96.0, 97.0, 97.9, 98.7, 98.7 },
			{ 1300, 17.5, 28.3, 35.8, 43.2, 47.2, 51.5, 56.0, 61.0, 63.2, 65.5,
					66.7, 67.9, 69.3, 70.7, 72.2, 73.9, 76.3, 78.2, 79.8, 81.1,
					82.4, 83.7, 84.8, 86.0, 87.1, 88.2, 89.3, 90.4, 91.4, 92.5,
					93.5, 94.6, 95.6, 96.6, 97.6, 98.5, 99.3, 99.3 },
			{ 1400, 17.5, 29.0, 37.2, 45.6, 50.1, 54.8, 59.8, 65.1, 67.3, 69.6,
					70.8, 72.0, 73.3, 74.6, 76.1, 77.4, 78.7, 79.9, 81.1, 82.3,
					83.4, 84.6, 85.7, 86.8, 87.9, 88.9, 90.0, 91.0, 92.1, 93.1,
					94.1, 95.1, 96.1, 97.1, 98.1, 99.1, 100.0, 100.0 },
			{ 1500, 17.5, 29.5, 38.6, 47.8, 52.5, 57.4, 62.4, 67.5, 69.6, 71.8,
					72.9, 74.0, 75.1, 76.3, 77.5, 78.7, 79.9, 81.0, 82.1, 83.2,
					84.2, 85.3, 86.4, 87.4, 88.5, 89.5, 90.5, 91.5, 92.6, 93.6,
					94.6, 95.6, 96.6, 97.6, 98.6, 99.6, 100.4, 100.4 },
			{ 1600, 17.5, 30.0, 40.0, 50.0, 55.0, 60.0, 65.0, 70.0, 72.0, 74.0,
					75.0, 76.0, 77.0, 78.0, 79.0, 80.0, 81.0, 82.0, 83.0, 84.0,
					85.0, 86.0, 87.0, 88.0, 89.0, 90.0, 91.0, 92.1, 93.1, 94.1,
					95.1, 96.1, 97.1, 98.1, 99.1, 100.1, 101.0, 101.0 },
			{ 1700, 17.8, 30.6, 40.8, 51.0, 56.1, 61.0, 65.8, 70.4, 72.2, 74.0,
					75.0, 75.9, 76.8, 77.8, 78.7, 79.7, 80.6, 81.6, 82.6, 83.5,
					84.5, 85.5, 86.5, 87.5, 88.5, 89.5, 90.5, 91.5, 92.5, 93.5,
					94.5, 95.5, 96.5, 97.6, 98.6, 99.6, 100.4, 100.4 },
			{ 1800, 18.0, 31.1, 41.6, 52.0, 57.1, 62.0, 66.6, 70.7, 72.3, 74.0,
					74.9, 75.7, 76.6, 77.5, 78.4, 79.3, 80.2, 81.2, 82.1, 83.0,
					84.0, 84.9, 85.9, 86.9, 87.9, 88.9, 89.9, 90.9, 91.9, 92.9,
					93.9, 95.0, 96.0, 97.1, 98.1, 99.1, 99.9, 99.9 },
			{ 1900, 18.5, 31.8, 42.4, 52.6, 57.5, 62.0, 66.2, 70.0, 71.6, 73.2,
					74.0, 74.8, 75.7, 76.5, 77.4, 78.2, 79.1, 80.0, 80.9, 81.8,
					82.8, 83.7, 84.6, 85.6, 86.6, 87.5, 88.5, 89.5, 90.5, 91.5,
					92.6, 93.6, 94.6, 95.7, 96.7, 97.8, 98.6, 98.6 },
			{ 2000, 19.1, 32.5, 43.2, 53.3, 57.9, 62.0, 65.9, 69.4, 70.9, 72.4,
					73.1, 73.9, 74.7, 75.5, 76.3, 77.2, 78.0, 78.9, 79.8, 80.6,
					81.5, 82.5, 83.4, 84.3, 85.3, 86.2, 87.2, 88.2, 89.2, 90.2,
					91.2, 92.3, 93.3, 94.3, 95.4, 96.4, 97.4, 97.4 },
			{ 2100, 19.9, 32.5, 42.6, 52.1, 56.5, 60.5, 64.3, 67.8, 69.3, 70.8,
					71.5, 72.3, 73.1, 73.9, 74.8, 75.6, 76.5, 77.3, 78.2, 79.1,
					80.0, 80.9, 81.9, 82.8, 83.8, 84.8, 85.8, 86.8, 87.8, 88.8,
					89.9, 90.9, 92.0, 93.1, 94.2, 95.2, 96.2, 96.2 },
			{ 2200, 20.7, 32.6, 42.1, 51.0, 55.2, 59.1, 62.7, 66.2, 67.7, 69.2,
					70.0, 70.8, 71.6, 72.4, 73.2, 74.1, 74.9, 75.8, 76.7, 77.6,
					78.5, 79.4, 80.4, 81.3, 82.3, 83.3, 84.3, 85.4, 86.4, 87.5,
					88.6, 89.6, 90.8, 91.9, 93.0, 94.1, 95.0, 95.0 },
			{ 2300, 21.6, 32.7, 41.5, 50.0, 53.9, 57.6, 61.2, 64.7, 66.2, 67.7,
					68.5, 69.3, 70.1, 70.9, 71.7, 72.5, 73.4, 74.3, 75.2, 76.1,
					77.0, 77.9, 78.9, 79.9, 80.9, 81.9, 82.9, 84.0, 85.0, 86.1,
					87.2, 88.4, 89.5, 90.7, 91.8, 92.9, 93.9, 93.9 },
			{ 2400, 22.5, 32.8, 41.0, 48.9, 52.7, 56.3, 59.8, 63.3, 64.7, 66.2,
					67.0, 67.8, 68.6, 69.4, 70.2, 71.1, 71.9, 72.8, 73.7, 74.6,
					75.5, 76.5, 77.5, 78.5, 79.5, 80.5, 81.5, 82.6, 83.7, 84.8,
					86.0, 87.1, 88.3, 89.5, 90.7, 91.8, 92.7, 92.7 },
			{ 2500, 23.4, 32.9, 40.5, 47.8, 51.4, 54.9, 58.3, 61.8, 63.3, 64.8,
					65.5, 66.3, 67.1, 67.9, 68.8, 69.6, 70.5, 71.4, 72.3, 73.2,
					74.1, 75.1, 76.1, 77.1, 78.1, 79.1, 80.2, 81.3, 82.4, 83.5,
					84.7, 85.9, 87.1, 88.3, 89.5, 90.7, 91.6, 91.6 } };

	private static final double RHCLASS[][][] = {
			{ { 600, 630 }, { 700, 730 }, { 800, 830 }, { 900, 930 },
					{ 1000, 1030 }, { 1100, 1130 }, { 1159, 1200 },
					{ 1200, 1200 } },
			{ { 87, 3 }, { 77, 3 }, { 67, 3 }, { 62, 3 }, { 57, 3 },
					{ 54.5, 3 }, { 52, 3 }, { 52, 3 } },
			{ { 87, 2 }, { 77, 2 }, { 67, 2 }, { 62, 2 }, { 57, 2 },
					{ 54.5, 2 }, { 52, 2 }, { 52, 2 } },
			{ { 68, 1 }, { 58, 1 }, { 48, 1 }, { 43, 1 }, { 38, 1 },
					{ 35.5, 1 }, { 33, 1 }, { 33, 1 } } };

	/**
	 * Calculates hourly FFMC (fine fuel moisture code), using Van Wagner's
	 * mathematical model, based on the previous hour's FFMC and provided
	 * conditions. FFMC is a numerical rating of the moisture content of litter
	 * and other cured fine fuels.
	 *
	 * @param inFFMC
	 *            The previous time's Van Wagner FFMC value.
	 * @param rain
	 *            Precipitation since observed FFMC (mm).
	 * @param temperature
	 *            The temperature (Celsius).
	 * @param rh
	 *            Relative humidity expressed as a fraction ([0..1]).
	 * @param ws
	 *            Wind speed (kph).
	 * @param span
	 *            Seconds since observed ffmc.
	 * @return Calculated FFMC value. (-98.0 on failure)
	 */
	public static double hourlyFFMCVanWagner(double inFFMC, double rain,
			double temperature, double rh, double ws, long span) {
		if ((inFFMC < 0.0) || (inFFMC > 101.0) || (temperature > 60.0)
				|| (rain < 0.0) || (rain > 300.0) || (span > 7200)
				|| (span <= 0))
			return -98;

		if (temperature < -50.0)
			temperature = -50.0;
		else if (temperature > 45.0)
			temperature = 45.0;

		if (rh < 0.0)
			rh = 0.0;
		else if (rh > 1.0)
			rh = 1.0;

		if (ws > 200.0)
			ws = 200.0;
		else if (ws < 0.0)
			ws = 0.0;

		double hourFrac = (double) span / 3600.0;
		double hourFrac2 = hourFrac - Math.floor(hourFrac);
		double factor;
		if (hourFrac2 > 1e-4)
			factor = 147.27723;
		else
			factor = 147.2;

		double rhp = rh * 100.0;

		double mo = factor * (101.0 - inFFMC) / (59.5 + inFFMC); // equation 2a
		if (rain != 0) {
			mo += rain * 42.5 * (Math.expm1(-100.0 / (251.0 - mo)) + 1)
					* (1.0 - (Math.expm1(-6.93 / rain) + 1)); // equation 12
//			if (mo > 150.0) {
//				double tmp = mo - 150.0;
//				mo += 0.0015 * tmp * tmp * Math.sqrt(rain); // equation 13
//			}
		}
		if (mo > 250.0)
			mo = 250.0;

		double convenience = (0.18 * (21.1 - temperature) * (-Math.expm1(-0.115
				* rhp)));
		double ed = 0.942 * Math.pow(rhp, 0.679)
				+ (11.0 * (Math.expm1((rhp - 100.0) / 10.0) + 1.0))
				+ convenience; // equation 8a
		double moed = mo - ed;
		double ew = 0.618 * Math.pow(rhp, 0.753)
				+ (10.0 * (Math.expm1((rhp - 100.0) / 10.0) + 1.0))
				+ convenience; // equation 8b
		double moew = mo - ew;

		double xm, e, a1, moe, xkd;
		if (moed == 0.0 || (moew >= 0.0 && moed < 0.0)) {
			xm = mo;
		} else {
			if (moed > 0.0) {
				a1 = rh;
				e = ed;
				moe = moed;
			} else {
				a1 = 1.0 - rh;
				e = ew;
				moe = moew;
			}
			xkd = (0.424 * (1.0 - Math.pow(a1, 1.7)) + (0.0694 * Math.sqrt(ws) * (1.0 - Math
					.pow(a1, 8.0)))); // equation 4
			xkd = xkd * 0.0579 * (Math.expm1(0.0365 * temperature) + 1.0); // equation
																			// 6
			xm = e + (moe * Math.pow(10.0, -xkd * hourFrac));
		}

		double cf = 59.5 * (250.0 - xm) / (factor + xm);
		if (cf > 101.0)
			cf = 101.0;
		else if (cf < 0.0)
			cf = 0.0;

		return cf;
	}

	/**
	 * Calculates hourly FFMC (fine fuel moisture code), using Lawson's
	 * mathematical model, based on the previous and current daily FFMC values
	 * (using Van Wagner) and provided conditions. FFMC is a numerical rating of
	 * the moisture content of litter and other cured fine fuels.
	 *
	 * @param prevFFMC
	 *            The previous day's standard daily Van Wagner FFMC value.
	 * @param currFFMC
	 *            The current day's standard daily Van Wagner FFMC value.
	 * @param rh
	 *            Relative humidity expressed as a fraction ([0..1]).
	 * @param seconds_into_day
	 *            The time to calculate the FFMC value for (in milliseconds from
	 *            the epoch).
	 * @return Calculated FFMC value. (-98.0 on failure)
	 */
	public static double hourlyFFMCLawson(double prevFFMC, double currFFMC, double rh,
			long seconds_into_day) {
		if (seconds_into_day > 86400)
			return -98;
		return calcHourlyFFMCLawsonContiguous(prevFFMC, currFFMC,
				seconds_into_day, rh * 100.0, rh * 100.0, rh * 100.0, false);
	}

	/**
	 * Calculates the previous hourly FFMC (fine fuel moisture code), using Van
	 * Wagner's mathematical model, based on the current hour's FFMC and
	 * previous hour's provided conditions. FFMC is a numerical rating of the
	 * moisture content of litter and other cured fine fuels.
	 *
	 * @param currFFMC
	 *            The current time's Van Wagner FFMC value.
	 * @param rain
	 *            Precipitation in the prior hour (mm).
	 * @param temperature
	 *            The prior hours temperature (Celsius).
	 * @param rh
	 *            Relative humidity expressed as a fraction ([0..1]).
	 * @param ws
	 *            Wind speed (kph).
	 * @return Calculated previous FFMC value. (-98.0 on failure)
	 */
	public static double hourlyFFMCVanWagnerPrevious(double currFFMC, double rain, double temperature, double rh, double ws) {
		if ((currFFMC < 0.0) || (currFFMC > 101.0) ||  (temperature > 60.0) || (rain < 0.0) || (rain > 300.0))
			return -98;

		if (temperature < -50.0)
			temperature = -50.0;
		else if (temperature > 45.0)
			temperature = 45.0;

		if (rh < 0.0)
			rh = 0.0;
		else if (rh > 1.0)
			rh = 1.0;

		if (ws > 200.0)
			ws = 200.0;
		else if (ws < 0.0)
			ws = 0.0;

		double outFFMC, outFFMCPrior, inFFMC, diffMC;

		inFFMC = currFFMC;
		long hour = 3600;

		outFFMC = hourlyFFMCVanWagner(inFFMC, rain, temperature, rh, ws, hour);

		diffMC = Math.abs(outFFMC - currFFMC);
		while (diffMC > TOLERANCE) {
			if (outFFMC > currFFMC)
				inFFMC -= diffMC / 2.0;
			else
				inFFMC += diffMC / 2.0;

			outFFMCPrior = outFFMC;
			outFFMC = hourlyFFMCVanWagner(inFFMC, rain, temperature, rh, ws,
					hour);

			diffMC = Math.abs(outFFMC - currFFMC);
			// check for error conditions
			if (outFFMC < 0.0 || outFFMC > 101.0) {
				diffMC = 0.0;
				inFFMC = currFFMC;
				break;
			}

			// if the output using the previous weather is insensitive to
			// changes in the input FFMC, then the routine has found the
			// correct answer
			if (Math.abs(outFFMC - outFFMCPrior) < TOLERANCE) {
				diffMC = 0.0;
				break;
			}
		}

		return inFFMC;
	}

	/**
	 * Calculates a contiguous hourly FFMC (fine fuel moisture code), using
	 * Lawson's mathematical model, based on the daily Van Wagner FFMC values
	 * for the previous and current days, as well as current provided
	 * conditions. FFMC is a numerical rating of the moisture content of litter
	 * and other cured fine fuels. This technique uses linear interpolation
	 * between 11am and noon LST. It also applies similar smoothing in morning
	 * hours.
	 *
	 * @param prevFFMC
	 *            The previous day's standard daily Van Wagner FFMC value.
	 * @param currFFMC
	 *            The current day's standard daily Van Wagner FFMC value.
	 * @param rh0
	 *            Relative humidity at the start of the hour, expressed as a
	 *            fraction ([0..1]).
	 * @param rh
	 *            Instantaneous relative humidity expressed as a fraction
	 *            ([0..1]).
	 * @param rh1
	 *            Relative humidity at the end of the hour (start of next hour)
	 *            expressed as a fraction ([0..1]).
	 * @param seconds_into_day
	 *            The time to calculate the FFMC value for.
	 * @return Calculated FFMC value. (-98.0 on failure)
	 */
	public static double hourlyFFMCLawsonContiguous(double prevFFMC, double currFFMC,
			double rh0, double rh, double rh1, long seconds_into_day) {
		if (seconds_into_day > 86400)
			return -98;
		return calcHourlyFFMCLawsonContiguous(prevFFMC, currFFMC,
				seconds_into_day, rh0 * 100.0, rh * 100.0, rh1 * 100.0, true);
	}

	/**
	 * Calculates daily FFMC (fine fuel moisture code), based on the previous
	 * day's FFMC and provided conditions. FFMC is a numerical rating of the
	 * moisture content of litter and other cured fine fuels.
	 *
	 * @param inFFMC
	 *            The previous day's Van Wagner FFMC value.
	 * @param rain
	 *            Precipitation in the prior 24 hours (noon to noon, LST) (mm).
	 * @param temperature
	 *            Noon (LST) temperature (Celsius).
	 * @param rh
	 *            Relative humidity expressed as a fraction ([0..1]).
	 * @param ws
	 *            Wind speed (kph) at noon LST.
	 * @return Calculated FFMC value. (-98.0 on failure)
	 */
	public static double dailyFFMCVanWagner(double inFFMC, double rain,
			double temperature, double rh, double ws) {
		if ((inFFMC < 0.0) || (inFFMC > 101.0) || (temperature > 60.0)
				|| (rain < 0.0) || (rain > 300.0))
			return -98;

		if (temperature < -50.0)
			temperature = -50.0;
		else if (temperature > 45.0)
			temperature = 45.0;

		if (rh < 0.0)
			rh = 0.0;
		else if (rh > 1.0)
			rh = 1.0;

		if (ws > 200.0)
			ws = 200.0;
		else if (ws < 0.0)
			ws = 0.0;

		double rhp = rh * 100.0; // input is 0..1, to match old equations we'll
									// go to 0..100

		double fo = inFFMC;
		double wmo = (147.2 * (101.0 - fo)) / (59.5 + fo);

		double rf;
		if (rain > 0.5) {
			rf = rain - 0.5;
			if (wmo > 150.0) {
				double tmp = (wmo - 150.0);
				tmp = tmp * tmp;
				wmo = wmo + 42.5 * rf
						* (Math.expm1(-100.0 / (251.0 - wmo)) + 1.0)
						* (-Math.expm1(-6.93 / rf)) + 0.0015 * tmp
						* Math.sqrt(rf);
			} else
				wmo = wmo + 42.5 * rf
						* (Math.expm1(-100.0 / (251.0 - wmo)) + 1.0)
						* (-Math.expm1(-6.93 / rf));
		}
		if (wmo > 250.0)
			wmo = 250.0;

		double ed = 0.942 * Math.pow(rhp, 0.679)
				+ (11.0 * (Math.expm1((rhp - 100.0) / 10.0) + 1.0)) + 0.18
				* (21.1 - temperature) * (-Math.expm1(-0.115 * rhp));

		double ew = 0.618 * Math.pow(rhp, 0.753)
				+ (10.0 * (Math.expm1((rhp - 100.0) / 10.0) + 1.0)) + 0.18
				* (21.1 - temperature) * (-Math.expm1(-0.115 * rhp)); // Equation
																		// 5

		double k1, kw, ko, kd, wm;
		if ((wmo < ed) && (wmo < ew)) {
			k1 = 0.424 * (1.0 - Math.pow((100.0 - rhp) / 100.0, 1.7)) + 0.0694
					* Math.sqrt(ws) * (1.0 - Math.pow(1.0 - rh, 8.0)); // Equation
																		// 7a
			kw = k1 * 0.581 * (Math.expm1(0.0365 * temperature) + 1.0); // Equation
																		// 7b
			wm = ew - (ew - wmo) / Math.pow(10.0, kw); // Equation 9
		} else if (wmo > ed) {
			ko = 0.424 * (1.0 - Math.pow(rh, 1.7)) + 0.0694 * Math.sqrt(ws)
					* (1.0 - Math.pow(rh, 8.0)); // Equation 6a
			kd = ko * 0.581 * (Math.expm1(0.0365 * temperature) + 1.0); // Equation
																		// 6b
			wm = ed + (wmo - ed) / Math.pow(10.0, kd); // Equation 8
		} else
			wm = wmo;

		double cf = 59.5 * (250.0 - wm) / (147.2 + wm);
		if (cf > 101.0)
			cf = 101.0;
		else if (cf < 0.0)
			cf = 0.0;

		return cf;
	}

	/**
	 * Calculates daily DMC (duff moisture code) given the previous day's DMC
	 * and provided conditions. DMC provides a numerical rating of the average
	 * moisture content of the loosely compact organic layers of a moderate
	 * depth (5-10cm).
	 *
	 * @param inDMC
	 *            The previous day's DMC value.
	 * @param rain
	 *            Precipitation in the prior 24 hours (noon to noon, LST) (mm).
	 * @param temperature
	 *            Noon (LST) temperature (Celsius).
	 * @param latitude
	 *            Radians, used to determine the appropriate table as defined in
	 *            "Latitude Considerations in Adapting the Canadian Forest Fire
	 *            Weather Index System To Other Countries", M.E. Alexander, in
	 *            prep as Index X in Weather Guide for the Canadian Forest Fire
	 *            Danger Rating System by B.D. Lawson, O.B. Armitage.
	 * @param longitude
	 *            Radians, reserved for future use, currently unused (may be
	 *            if/as/when more regional constants are defined).
	 * @param month
	 *            Origin 0 (January = 0, December = 11)
	 * @param rh
	 *            Relative humidity expressed as a fraction ([0..1]) at noon
	 *            LST.
	 * @return Calculated DMC value. (-98.0 on failure)
	 */
	public static double dMC(double inDMC, double rain, double temperature,
			double latitude, double longitude, int month, double rh) {
		if ((inDMC < 0.0) || (temperature > 60.0) || (rain < 0.0)
				|| (rain > 300.0) || (month > 11) || (month < 0))
			return -98;

		if (temperature < -50.0)
			temperature = -50.0;
		else if (temperature > 45.0)
			temperature = 45.0;

		if (rh < 0.0)
			rh = 0.0;
		else if (rh > 1.0)
			rh = 1.0;

		final double EL[] = { 6.5, 7.5, 9.0, 12.8, 13.9, 13.9, 12.4, 10.9, 9.4,
				8.0, 7.0, 6.0 };
		final double EL_N20[] = { 7.9, 8.4, 8.9, 9.5, 9.9, 10.2, 10.1, 9.7,
				9.1, 8.6, 8.1, 7.8 };
		final double EL_EQ[] = { 9.0, 9.0, 9.0, 9.0, 9.0, 9.0, 9.0, 9.0, 9.0,
				9.0, 9.0, 9.0 };
		final double EL_S20[] = { 10.1, 9.6, 9.1, 8.5, 8.1, 7.8, 7.9, 8.3, 8.9,
				9.4, 9.9, 10.2 };
		final double EL_NZ[] = { 11.5, 10.5, 9.2, 7.9, 6.8, 6.2, 6.5, 7.4, 8.7,
				10.0, 11.2, 11.8 };

		double el[];
		if (latitude >= DEGREE_TO_RADIAN(30.0))
			el = EL; // 081205: RWB: added the tables for EL_N20, EL_EQ, EL_S20
						// based on "Latitude Considerations in Adapting the
						// Canadian
		else if (latitude <= DEGREE_TO_RADIAN(-30.0))
			el = EL_NZ; // Forest Fire Weather Index System To Other Countries",
						// M.E. Alexander, in prep as Index X in Weather Guide
						// for the
		else if (latitude >= DEGREE_TO_RADIAN(10.0))
			el = EL_N20; // Canadian Forest Fire Danger Rating System by B.D.
							// Lawson, O.B. Armitage. With this, we also added
							// the FL_EQ table
		else if (latitude <= DEGREE_TO_RADIAN(-10.0))
			el = EL_S20; // below for DC.
		else
			el = EL_EQ;

		double c_d;

		double po = inDMC;

		double rk;
		if (temperature < -1.1)
			rk = 0.0;
		else
			rk = 1.894 * (temperature + 1.1) * (1.0 - rh) * el[month] * 0.01; // Equation
																				// 16,
																				// slightly
																				// modified
																				// to
																				// make
																				// RH
																				// 0..1
																				// instead
																				// of
																				// 0..100

		double rw, wmi, b, wmr, pr;
		if (rain > 1.5) {
			rw = 0.92 * rain - 1.27; // Equation 11
			wmi = 20.0 + (Math.exp(5.6348 - (po / 43.43)));	// Equation 12
			if (po <= 33.0)
				b = 100.0 / (0.5 + (0.3 * po)); // Equation 13a
			else if (po > 65.0)
				b = 6.2 * Math.log1p(po - 1.0) - 17.2; // Equation 13c
			else
				b = 14.0 - 1.3 * Math.log1p(po - 1.0); // Equation 13b
			wmr = wmi + (1000.0 * rw) / (48.77 + b * rw); // Equation 14
			pr = 43.43 * (5.6348 - Math.log1p(wmr - 21.0));

		} else
			pr = po;

		if (pr < 0.0)
			pr = 0.0;
		c_d = pr + rk;
		if (c_d < 0.0)
			c_d = 0.0;
		return c_d;
	}

	/**
	 * Calculates DC (drought code) given the previous day's DC and provided
	 * conditions. DC provides a numerical rating of the average moisture
	 * content of the deep (10-24 cm), compact organic layers.
	 *
	 * @param inDC
	 *            The previous day's DC value.
	 * @param rain
	 *            Precipitation in the prior 24 hours (noon to noon, LST) (mm).
	 * @param temperature
	 *            Noon (LST) temperature (Celsius).
	 * @param latitude
	 *            Radians, used to determine the appropriate table as defined in
	 *            "Latitude Considerations in Adapting the Canadian Forest Fire
	 *            Weather Index System To Other Countries", M.E. Alexander, in
	 *            prep as Index X in Weather Guide for the Canadian Forest Fire
	 *            Danger Rating System by B.D. Lawson, O.B. Armitage.
	 * @param longitude
	 *            Radians, reserved for future use, currently unused (may be
	 *            if/as/when more regional constants are defined).
	 * @param month
	 *            Origin 0 (January = 0, December = 11).
	 * @return Calculated DC value. (-98.0 on failure)
	 */
	public static double dC(double inDC, double rain, double temperature,
			double latitude, double longitude, int month) {
		if ((inDC < 0.0) || (temperature > 60.0) || (rain < 0.0)
				|| (rain > 300.0) || (month > 11) || (month < 0))
			return -98;

		if (temperature < -50.0)
			temperature = -50.0;
		else if (temperature > 45.0)
			temperature = 45.0;

		final double FL[] = { -1.6, -1.6, -1.6, 0.9, 3.8, 5.8, 6.4, 5.0, 2.4,
				0.4, -1.6, -1.6 };
		final double FL_EQ[] = { 1.4, 1.4, 1.4, 1.4, 1.4, 1.4, 1.4, 1.4, 1.4,
				1.4, 1.4, 1.4 };
		final double FL_NZ[] = { 6.4, 5.0, 2.4, 0.4, -1.6, -1.6, -1.6, -1.6,
				-1.6, 0.9, 3.8, 5.8 };
		double fl[];

		if (latitude >= DEGREE_TO_RADIAN(10.0))
			fl = FL;
		else if (latitude <= DEGREE_TO_RADIAN(-10.0))
			fl = FL_NZ;
		else
			fl = FL_EQ;

		if (temperature < -2.8)
			temperature = -2.8;

		double pe = (0.36 * (temperature + 2.8) + fl[month]) / 2.0;

		double dr, smi;
		if (rain <= 2.8)
			dr = inDC;
		else {
			rain = 0.83 * rain - 1.27;
			smi = 800.0 * (Math.expm1(-inDC / 400.0) + 1.0);
			dr = inDC - 400.0 * Math.log1p((3.937 * rain) / smi);
			if (dr < 0.0)
				dr = 0.0;
		}

		double cd = dr + pe;
		if (cd < 0.0)
			cd = 0.0;

		return cd;
	}

	/**
	 * Calculates f(F), which is used to calculate ISI from FFMC.
	 *
	 * @param ffmc
	 *            FFMC value.
	 * @param seconds
	 *            Time since observed FFMC.
	 * @return Calculated f(F) value.
	 */
	public static double ff(double ffmc, double seconds) {
		double hourFrac = seconds / 3600.0;
		double hourFrac2 = hourFrac - Math.floor(hourFrac);

		double factor;
		if (hourFrac2 > 1e-4)
			factor = 147.27723;
		else
			factor = 147.2;

		double fm = factor * (101.0 - ffmc) / (59.5 + ffmc);
		double sf = 91.9 * (Math.expm1(fm * (-0.1386)) + 1.0)
				* (1.0 + Math.pow(fm, 5.31) / 49300000.0);
		return sf;
	}

	/**
	 * Calculates ISI from FFMC and wind speed. ISI provides a numerical rating
	 * of the relative expected rate of fire spread.
	 *
	 * @param ffmc
	 *            FFMC value.
	 * @param ws
	 *            Wind speed (kph).
	 * @param seconds
	 *            Time since observed FFMC.
	 * @return Calculated ISI value.
	 */
	public static double isiFWI(double ffmc, double ws, long seconds) {
		double sf = ff(ffmc, seconds);
		return 0.208 * (sf) * (Math.expm1(0.05039 * ws) + 1.0);
	}

	/**
	 * Calculates ISI from FFMC and wind speed. ISI provides a numerical rating
	 * of the relative expected rate of fire spread. The FBP system uses a local
	 * site-specific ISI influenced by topography.
	 *
	 * @param ffmc
	 *            FFMC value.
	 * @param ws
	 *            Wind speed (kph).
	 * @param seconds
	 *            Time since observed FFMC.
	 * @return Calculated ISI value.
	 */
	public static double isiFBP(double ffmc, double ws, double seconds) {
		double sf = ff(ffmc, seconds);

		double fW;

		if (ws <= 40.0f) // used to use WS, try the m_wsv, Dennis, 12/05/2003
			fW = Math.expm1(0.05039 * ws) + 1.0; // Equation 53
		else
			fW = 12.0 * (-Math.expm1(-0.0818 * (ws - 28.0))); // Equation 53a

		return 0.208 * fW * (sf); // Equation 52
	}

	/**
	 * Calculate BUI (buildup index) from DC, DMC. BUI provides a numerical,
	 * relative indication of the amount of fuel available for combustion.
	 *
	 * @param dc
	 *            DC value.
	 * @param dmc
	 *            DMC value.
	 * @return Calculated BUI value.
	 */
	public static double bui(double dc, double dmc) {
		double bui;
		if ((dmc == 0.0) && (dc == 0.0))
			bui = 0.0;
		else
			bui = (0.8 * dc * dmc) / (dmc + 0.4 * dc);

		if (bui < dmc) {
			double p = (dmc - bui) / dmc;
			double cc = 0.92 + Math.pow(0.0114 * dmc, 1.7);
			bui = dmc - cc * p;
			if (bui < 0.0)
				bui = 0.0;
		}
		return bui;
	}

	/**
	 * Calculates FWI (fire weather index) from ISI and BUI. FWI provides a
	 * numerical, relative rating of fire intensity.
	 *
	 * @param isi
	 *            ISI value.
	 * @param bui
	 *            BUI value.
	 * @return Calculated FWI value.
	 */
	public static double fwi(double isi, double bui) {
		double bb;
		double fwi;
		if (bui > 80.0)
			bb = 0.1
					* isi
					* (1000.0 / (25.0 + 108.64 / (Math.expm1(0.023 * bui) + 1.0)));
		else
			bb = 0.1 * isi * (0.626 * Math.pow(bui, 0.809) + 2.0);

		if (bb <= 1.0)
			fwi = bb;
		else
			fwi = Math.expm1(2.72 * Math.pow(0.434 * Math.log1p(bb - 1.0),
					0.647)) + 1.0;
		return fwi;
	}

	/**
	 * Calculates DSR from FWI.
	 *
	 * @param fwi
	 *            FWI value.
	 * @return Calculated DSR value.
	 */
	public static double dsr(double fwi) {
		double dsr = 0.0272 * Math.pow(fwi, 1.77);
		return dsr;
	}

	private static double calcHourlyFFMCLawsonContiguous(double prevFFMC,
			double currFFMC, long seconds_into_day, double rh0, double rht,
			double rh1, boolean contiguous) {
		if ((prevFFMC < 0.0) || (prevFFMC > 101.0) || (currFFMC < 0.0)
				|| (currFFMC > 101.0) || (seconds_into_day < -43200)
				|| (seconds_into_day >= 126000))
			return -98.0;

		if (seconds_into_day >= 43200)
			return calcHourlyFFMCLawson(currFFMC, seconds_into_day, rht);

		if ((seconds_into_day < 18000) || (!contiguous))
			return calcHourlyFFMCLawson(prevFFMC, seconds_into_day, rht);

		long h0 = seconds_into_day - (seconds_into_day % 3600);

		if (h0 == seconds_into_day)
			return calcHourlyFFMCLawson(prevFFMC, seconds_into_day, rh0);

		long h1 = h0 + 3600;

		double ffmc1, ffmc2;
		ffmc1 = calcHourlyFFMCLawson(prevFFMC, h0, rh0);

		if (h1 == (12 * 60 * 60))
			ffmc2 = calcHourlyFFMCLawson(currFFMC, h1, rh1);
		else
			ffmc2 = calcHourlyFFMCLawson(prevFFMC, h1, rh1);

		long sec = seconds_into_day % 3600;
		return ((ffmc2 * (double) sec) + (ffmc1 * (60.0 * 60.0 - (double) sec)))
				/ (60.0 * 60.0);
	}

	private static double calcHourlyFFMCLawson(double inFFMC, long seconds, double rh) {
		int tindex = 0, classidx = 0, i = 1;
		double adjffmc;

		while (seconds < 0)
			seconds = seconds + 86400;

		long hour = ((long) (((double) seconds) / 3600.0))
				- (((long) (((double) seconds) / 86400.0)) * 24);
		if (inFFMC < 0.0 || inFFMC > 101.0)
			return -98.0;

		long minutes = ((long) (((double) seconds) / 60.0))
				- (((long) (((double) seconds) / 3600.0)) * 60);

		if (inFFMC < 17.5)
			inFFMC = 17.5;

		if (rh < 0.0)
			rh = 0.0;
		else if (rh > 100.0)
			rh = 100.0;

		rh *= 100.0;
		rh = Math.floor(rh + 0.5);
		rh *= 0.01;

		if (rh < 1.0)
			rh = 95;

		if ((hour >= 6) && (hour <= 11)) {
			for (i = 0; i <= 7; i++) {
				if ((100.0 * hour) < RHCLASS[0][i][0]) {
					tindex = i;
					i = 10;
					if (minutes <= 30) {
						if (rh > RHCLASS[1][tindex - 1][0])
							classidx = 3;
						else if (rh < RHCLASS[3][tindex - 1][0])
							classidx = 1;
						else
							classidx = 2;
					} else {
						if (rh > RHCLASS[1][tindex][0])
							classidx = 3;
						else if (rh < RHCLASS[3][tindex][0])
							classidx = 1;
						else
							classidx = 2;
					}
				}
			}

			switch (classidx) {
			case 1:
				adjffmc = lowRH(seconds, inFFMC, tindex);
				break;
			case 2:
				adjffmc = medRH(seconds, inFFMC, tindex);
				break;
			default:
				adjffmc = highRH(seconds, inFFMC, tindex);
				break;
			}
		} else {
			adjffmc = mainTBL(seconds, inFFMC);
			if (adjffmc < 0)
				adjffmc = 0;
			else if (adjffmc > 101.0)
				adjffmc = 101.0;
		}
		return adjffmc;
	}

	private static double intrp(double i1, double i2, double i3, double i4,
			double fraction, long seconds) {
		double i12, i34, i14;
		long minutes = ((long) (((double) seconds) / 60.0))
				- (((long) (((double) seconds) / 3600.0)) * 60);

		i12 = i1 + ((i2 - i1) * fraction);
		i34 = i3 + ((i4 - i3) * fraction);

		long hour = ((long) (((double) seconds) / 3600.0))
				- (((long) (((double) seconds) / 86400.0)) * 24);
		if (hour == 11)
			i14 = i12 + ((i34 - i12) / 59.0) * minutes;
		else
			i14 = i12 + ((i34 - i12) / 60.0) * minutes;

		return i14;
	}

	private static double lowRH(long seconds, double ffFFMC, int tIndex) {
		int i = 1;
		double adjffmc, fraction;

		while (ffFFMC >= L[0][i])
			i++;
		i--;

		fraction = (ffFFMC - L[0][i]) / (L[0][i + 1] - L[0][i]);
		adjffmc = intrp(L[tIndex][i], L[tIndex][i + 1], L[tIndex + 1][i],
				L[tIndex + 1][i + 1], fraction, seconds);
		return adjffmc;
	}

	private static double medRH(long seconds, double ffFFMC, int tIndex) {
		int i = 1;
		double adjffmc, fraction;

		while (ffFFMC >= M[0][i])
			i++;
		i--;

		fraction = (ffFFMC - M[0][i]) / (M[0][i + 1] - M[0][i]);
		adjffmc = intrp(M[tIndex][i], M[tIndex][i + 1], M[tIndex + 1][i],
				M[tIndex][i + 1], fraction, seconds);
		return adjffmc;
	}

	private static double highRH(long seconds, double ffFFMC, int tindex) {
		int i = 1;
		double adjffmc, fraction;

		while (ffFFMC >= H[0][i])
			i++;
		i--;

		fraction = (ffFFMC - H[0][i]) / (H[0][i + 1] - H[0][i]);
		adjffmc = intrp(H[tindex][i], H[tindex][i + 1], H[tindex + 1][i],
				H[tindex + 1][i + 1], fraction, seconds);
		return adjffmc;
	}

	private static double mainTBL(long seconds, double ffFFMC) {
		int i = 1;
		int tindex;
		double adjffmc, fraction;

		long minutes = ((long) (((double) seconds) / 60.0))
				- (((long) (((double) seconds) / 3600.0)) * 60);
		long hours = ((long) (((double) seconds) / 3600.0))
				- (((long) (((double) seconds) / 86400.0)) * 24);
		long hour = hours * 100 + minutes;

		if (hour < 100)
			hour = hour + 2400;

		while (hour >= MAIN[i][0])
			i++;
		tindex = --i;

		i = 1;
		while (ffFFMC >= MAIN[0][i])
			i++;
		i--;

		fraction = (ffFFMC - MAIN[0][i]) / (MAIN[0][i + 1] - MAIN[0][i]);
		adjffmc = intrp(MAIN[tindex][i], MAIN[tindex][i + 1],
				MAIN[tindex + 1][i], MAIN[tindex + 1][i + 1], fraction, seconds);
		return adjffmc;
	}
}
