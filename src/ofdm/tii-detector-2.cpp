#
/*
 *    Copyright (C) 2014 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
 *
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	"tii-detector-2.h"
#include	<cstdio>
#include	<cinttypes>
#include	<cstring>
#include	<QSettings>
#include	"settingNames.h"
#include	"settings-handler.h"

#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif

static
uint8_t table [] = {
	0017,		// 0 0 0 0 1 1 1 1		0
	0027,		// 0 0 0 1 0 1 1 1		1
	0033,		// 0 0 0 1 1 0 1 1		2
	0035,		// 0 0 0 1 1 1 0 1		3
	0036,		// 0 0 0 1 1 1 1 0		4
	0047,		// 0 0 1 0 0 1 1 1		5
	0053,		// 0 0 1 0 1 0 1 1		6
	0055,		// 0 0 1 0 1 1 0 1		7
	0056,		// 0 0 1 0 1 1 1 0		8
	0063,		// 0 0 1 1 0 0 1 1		9

	0065,		// 0 0 1 1 0 1 0 1		10
	0066,		// 0 0 1 1 0 1 1 0		11
	0071,		// 0 0 1 1 1 0 0 1		12
	0072,		// 0 0 1 1 1 0 1 0		13
	0074,		// 0 0 1 1 1 1 0 0		14
	0107,		// 0 1 0 0 0 1 1 1		15
	0113,		// 0 1 0 0 1 0 1 1		16
	0115,		// 0 1 0 0 1 1 0 1		17
	0116,		// 0 1 0 0 1 1 1 0		18
	0123,		// 0 1 0 1 0 0 1 1		19

	0125,		// 0 1 0 1 0 1 0 1		20
	0126,		// 0 1 0 1 0 1 1 0		21
	0131,		// 0 1 0 1 1 0 0 1		22
	0132,		// 0 1 0 1 1 0 1 0		23
	0134,		// 0 1 0 1 1 1 0 0		24
	0143,		// 0 1 1 0 0 0 1 1		25
	0145,		// 0 1 1 0 0 1 0 1		26
	0146,		// 0 1 1 0 0 1 1 0		27
	0151,		// 0 1 1 0 1 0 0 1		28	
	0152,		// 0 1 1 0 1 0 1 0		29

	0154,		// 0 1 1 0 1 1 0 0		30
	0161,		// 0 1 1 1 0 0 0 1		31
	0162,		// 0 1 1 1 0 0 1 0		32
	0164,		// 0 1 1 1 0 1 0 0		33
	0170,		// 0 1 1 1 1 0 0 0		34
	0207,		// 1 0 0 0 0 1 1 1		35
	0213,		// 1 0 0 0 1 0 1 1		36
	0215,		// 1 0 0 0 1 1 0 1		37
	0216,		// 1 0 0 0 1 1 1 0		38
	0223,		// 1 0 0 1 0 0 1 1		39

	0225,		// 1 0 0 1 0 1 0 1		40
	0226,		// 1 0 0 1 0 1 1 0		41
	0231,		// 1 0 0 1 1 0 0 1		42
	0232,		// 1 0 0 1 1 0 1 0		43
	0234,		// 1 0 0 1 1 1 0 0		44
	0243,		// 1 0 1 0 0 0 1 1		45
	0245,		// 1 0 1 0 0 1 0 1		46
	0246,		// 1 0 1 0 0 1 1 0		47
	0251,		// 1 0 1 0 1 0 0 1		48
	0252,		// 1 0 1 0 1 0 1 0		49

	0254,		// 1 0 1 0 1 1 0 0		50
	0261,		// 1 0 1 1 0 0 0 1		51
	0262,		// 1 0 1 1 0 0 1 0		52
	0264,		// 1 0 1 1 0 1 0 0		53
	0270,		// 1 0 1 1 1 0 0 0		54
	0303,		// 1 1 0 0 0 0 1 1		55
	0305,		// 1 1 0 0 0 1 0 1		56
	0306,		// 1 1 0 0 0 1 1 0		57
	0311,		// 1 1 0 0 1 0 0 1		58
	0312,		// 1 1 0 0 1 0 1 0		59

	0314,		// 1 1 0 0 1 1 0 0		60
	0321,		// 1 1 0 1 0 0 0 1		61
	0322,		// 1 1 0 1 0 0 1 0		62
	0324,		// 1 1 0 1 0 1 0 0		63
	0330,		// 1 1 0 1 1 0 0 0		64
	0341,		// 1 1 1 0 0 0 0 1		65
	0342,		// 1 1 1 0 0 0 1 0		66
	0344,		// 1 1 1 0 0 1 0 0		67
	0350,		// 1 1 1 0 1 0 0 0		68
	0360		// 1 1 1 1 0 0 0 0		69
};

		TII_Detector_B::TII_Detector_B (uint8_t dabMode,
	                                       QSettings	*dabSettings):
	                                      theTable (dabMode),
	                                      params (dabMode),
	                                      T_u (params. get_T_u ()),
	                                      carriers (params. get_carriers ()),
	                                      my_fftHandler (params. get_T_u (),
	                                                    false) {
	this	-> dabSettings	= dabSettings;
	theBuffer. resize	(T_u);
	window. resize 		(T_u);
	for (int i = 0; i < T_u; i ++)
	   window [i] = 0.54 - 0.46 * cos (2 * M_PI * (DABFLOAT)i / T_u);

	refTable.               resize (T_u);
        for (int i = 0; i < T_u; i ++)
           refTable [i] = Complex (0, 0);
//
//      generate the refence values using the format we have after
//      doing an FFT
        for (int i = 1; i <= carriers / 2; i ++) {
           float Phi_k = theTable. get_Phi (i);
           refTable [i] = Complex (cos (Phi_k), sin (Phi_k));
           Phi_k = theTable. get_Phi (-i);
           refTable [T_u - i] = Complex (cos (Phi_k), sin (Phi_k));
        }

	memset (invTable, 0x377, 256);
	for (int i = 0; i < 70; ++i) 
	    invTable [table [i]] = i;
	detectMode_new	= false;
//	detectMode_new	= true;
	tiiThreshold	= value_i (dabSettings, CONFIG_HANDLER, 
	                                     "tiiThreshold", 4);
}

		TII_Detector_B::~TII_Detector_B () {
}

void	TII_Detector_B::setMode	(bool b) {
	detectMode_new = b;
}

void	TII_Detector_B::reset	() {
	for (int i = 0; i < T_u; i ++)
	   theBuffer [i] = Complex (0, 0);
}

void	TII_Detector_B::set_tiiThreshold	(int t) {
	tiiThreshold = t;
}


//	To eliminate (reduce?) noise in the input signal, we might
//	add a few spectra before computing (up to the user)
void	TII_Detector_B::addBuffer (std::vector<Complex> v) {

	for (int i = 0; i < T_u; i ++)
	   v [i] = v [i] *  window [i];
	my_fftHandler. fft (v);

	for (int i = 0; i < T_u; i ++)
	   theBuffer [i] += v [i];
}
//
//	Note that the input is fft output, not yet reordered
void	TII_Detector_B::collapse (std::vector<Complex> &inVec, float *outVec) {

	for (int i = 0; i < carriers / 8; i ++) {	
	   int carr = - carriers / 2 + 2 * i;

	   outVec [i] = abs (real (inVec [(T_u + carr) % T_u] *
	                            conj (inVec [(T_u + carr + 1) % T_u])));
	   carr	= - carriers / 2 + 1 * carriers / 4 + 2 * i;
	   outVec [i] += abs (real (inVec [(T_u + carr) % T_u] *
	                            conj (inVec [(T_u + carr + 1) % T_u])));
	   carr	= - carriers / 2 + 2 * carriers / 4 + 2 * i + 1;
	   outVec [i] += abs (real (inVec [(T_u + carr) % T_u] *
	                            conj (inVec [(T_u + carr + 1) % T_u])));

	   carr	= - carriers / 2 + 3 * carriers / 4 + 2 * i + 1;
	   outVec [i] += abs (real (inVec [(T_u + carr) % T_u] *
	                            conj (inVec [(T_u + carr + 1) % T_u])));
	}
}

bool	TII_Detector_B::isPeak (const Complex *v, int index, float avg) {
	if (abs (v [index]) < 2 * avg)
	   return false;
	if (abs(v [index + 1]) < 2 * avg)
	   return false;
	return true;
	if (index > 2)
	   if (abs (v [index - 2]) > 0.75 * abs (v [index]))
	      return false;
	if (abs (v [index + 1 + 2]) > 0.75 * abs (v [index + 1]))
	   return false;
	return true;
}
	
static
uint8_t bits [] = {0x80, 0x40, 0x20, 0x10 , 0x08, 0x04, 0x02, 0x01};

//
//	We determine first the offset of the "best fit", the offset
//	indicates the subId
#define	NUM_GROUPS	8
#define	GROUPSIZE	24

QVector<tiiData>	TII_Detector_B::processNULL () {
float	hulpTable	[NUM_GROUPS * GROUPSIZE]; // collapses values
float	C_table		[GROUPSIZE];		  // contains the values
int	D_table		[GROUPSIZE];	// count of indices in C_table with data
float	avgTable	[NUM_GROUPS];

QVector<tiiData> theResult;
	bool dxMode	= true;
//	we map the "carriers" carriers (complex values) onto
//	a collapsed vector of "carriers / 8" length, 
//	considered to consist of 8 segments of 24 values
//	Each "value" is the sum of 4 pairs of subsequent carriers,
//	taken from the 4 quadrants -768 .. 385, 384 .. -1, 1 .. 384, 385 .. 768
	collapse (theBuffer, hulpTable);

//	since the "energy levels" in the different GROUPSIZE'd values
//	may differ, we compute an average for each of the
//	NUM_GROUPS GROUPSIZE - value groups. 

	int tiiThreshold	=
	                   value_i (dabSettings, DAB_GENERAL,
	                                      "tiiThreshold", 4);
	while (true) {
	   memset (avgTable, 0, NUM_GROUPS * sizeof (float));

	   for (int i = 0; i < NUM_GROUPS; i ++) {
	      avgTable [i] = 0;
	      for (int j = 0; j < GROUPSIZE; j ++) 
	         avgTable [i] += hulpTable [i * GROUPSIZE + j];

	      avgTable [i] /= GROUPSIZE;
	   }
//
//	Determining the offset is then easy, look at the corresponding
//	elements in the NUM_GROUPS sections and mark the highest ones.
//	The summation of the high values are stored in the C_table,
//	the number of times the limit is reached in the group
//	is recorded in the D_table
//
//	So, basically we look into GROUPSIZE colums of NUMGROUPS
//	values and look for the maximum
//	Threshold 4 * avgTable is 6 dB, we consider that a minimum
//	measurement shows that that is a reasonable value,
//	alternatively, we could take the "minValue" as reference
//	and "raise" the threshold. However, that might be
//	too  much for 8-bit incoming values
	   memset (D_table, 0, GROUPSIZE * sizeof (int));
	   memset (C_table, 0, GROUPSIZE * sizeof (float));
//
//	We only use the C and D table to locate the start offset
	   for (int i = 0; i < GROUPSIZE; i ++) {
	      for (int j = 0; j < NUM_GROUPS; j ++) {
	         if (hulpTable [j * GROUPSIZE + i] > tiiThreshold * avgTable [j]) {
	            C_table [i] += hulpTable [j * GROUPSIZE + i];
	            D_table [i] ++;
	         }
	      }
	   }
	   float newAvg	= 0;
	   for (int i = 0; i < NUM_GROUPS; i ++)
	      newAvg += avgTable [i];
	   newAvg /= NUM_GROUPS;

//	we extract from this result the highest values that
//	meet the constraint of 4 values being sufficiently high
	   float	maxTable	= 0;
	   int		maxIndex	= -1;
	
	   for (int j = 0; j < GROUPSIZE; j ++) {
	      if ((D_table [j] >= 4) && (C_table [j] > maxTable)) {
	         maxTable = C_table [j];
	         maxIndex = j;
	         break;
	      }
	   }

	   if (maxIndex < 0)
	      return theResult;

	float strength = 10 * log10 (maxTable / (4 * newAvg));
//	The - almost - final step is then to figure out which
//	group contributed most, obviously only where maxIndex  > 0
//	we start with collecting the values of the correct
//	elements of the NUM_GROUPS groups

	   float x [NUM_GROUPS];
	   for (int i = 0; i < NUM_GROUPS; i ++) {
	      x [i] = hulpTable [maxIndex + GROUPSIZE * i];
	   }

//	find the best match
	   int finInd = -1;
	   if (detectMode_new) {
	      float mm = 0;
	      for (int k = 0; k < (int)(sizeof (table)); k ++) {
	         float val = 0;
	         for (int l = 0; l < NUM_GROUPS; l ++)
	            if (table [k] & (bits [l] != 0))
	               val += x [l];
	         if  (val > mm) {
	            mm = val;
	            finInd = k;
	         }
	      }
	      if (finInd != -1) {
	         tiiData v;
	         v. subId	= maxIndex;
	         v. mainId	= finInd;
	         v. strength	= strength;
	         theResult. push_back (v);
	         for (int i = 0; i < 8; i ++) {
	            if (table [finInd] & bits [i]) {
	               int index = maxIndex + i * 24;
	               hulpTable [index] = 0;
	            }
	         }
	      }
	   }
	   else {		// detectMode_new is false
////	we extract the four max values as bits
	      float theStrength = 0;
	      uint16_t pattern	= 0;
	      for (int i = 0; i < 4; i ++) {
	         float mmax	= 0;
	         int ind	= -1;
	         for (int k = 0; k < NUM_GROUPS; k ++) {
	            if (x [k] > mmax) {
	               mmax = x [k];
	               ind  = k;
	            }
	         }
	         if (ind != -1) {
	            x [ind] = 0;
	            pattern |= bits [ind];
	         }
	      }
	      finInd = invTable [pattern];
	      for (int i = 0; i < 8; i ++) {
	         if (pattern & bits [i]) {
	            int index = maxIndex + i * 24;
	             hulpTable [index] = 0;
	         }
	      }
	      tiiData v;
	      v. subId		= maxIndex;
	      v. mainId		= finInd;	
	      v. strength	= strength;
	      theResult. push_back (v);
	   }
	   if (!dxMode)
	      break;
	}
	return theResult;
}

