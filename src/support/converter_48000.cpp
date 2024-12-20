#
/*
 *    Copyright (C) 2011 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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
#include	"converter_48000.h"
#include	"radio.h"
#include	<cstdio>
/*
 */
	converter_48000::converter_48000 (RadioInterface *mr):
	                                mapper_16 (16000, 48000, 2 * 1600),
	                                mapper_24 (24000, 48000, 2 * 2400),
	                                mapper_32 (32000, 48000, 2 * 3200) {
	                              
}

	converter_48000::~converter_48000 () {
}

int	converter_48000::convert (complex16 *V,
	                         int32_t amount, int32_t rate,
	                         std::vector<float> 	&outB) {
	switch (rate) {
	   case 16000:	
	      return convert_16000 (V, amount, outB);
	   case 24000:
	      return convert_24000 (V, amount, outB);
	   case 32000:
	      return convert_32000 (V, amount, outB);
	   default:
	   case 48000:
	      return convert_48000 (V, amount, outB);
	}
}

void	converter_48000::start_audioDump         (const QString &fileName) {
	theWriter. init (fileName);
}

void	converter_48000::stop_audioDump          () {
	locker. lock ();
	theWriter. close ();
	locker. unlock ();
}

//	scale up from 16 -> 48
//	amount gives number of pairs
int	converter_48000::convert_16000	(complex16 *V,
	                                 int amount,
	                                 std::vector<float> &out) {
Complex buffer [mapper_16. getOutputsize ()];
int	teller = 0;
	out. resize (0);
	for (int i = 0; i < amount; i ++) {
	   int	result;
	   if (mapper_16.
	            convert (Complex (real (V [i]) / 32767.0,
	                              imag (V [i]) / 32767.0),
	                                           buffer, &result)) {
	      
	      dump (buffer, result);
	      out. resize (out. size () + 2 * result);
	      for (int j = 0; j < result; j ++) {
	         out [teller ++] = real (buffer [j]);
	         out [teller ++] = imag (buffer [j]);
	      }
	   }
	}
	return teller;
}

//	scale up from 24000 -> 48000
//	amount gives number of pairs
int	converter_48000::convert_24000	(complex16 *V,
	                                 int amount,
	                                 std::vector<float> &out) {
Complex buffer	[mapper_24. getOutputsize ()];
int	teller	= 0;

	out. resize (2 * mapper_24. getOutputsize ());
	for (int i = 0; i < amount; i ++) {
	   int	result;
	   if (mapper_24.
	            convert (Complex (real (V [i]) / 32767.0,
	                              imag (V [i]) / 32767.0),
	                                           buffer, &result)) {
	      dump (buffer, result);
	      for (int j = 0; j < result; j ++) {
	         out [teller ++] = real (buffer [j]);
	         out [teller ++] = imag (buffer [j]);
	      }
	   }
	}
	return teller;
}

//	scale up from 32000 -> 48000
//	amount is number of pairs
int	converter_48000::convert_32000	(complex16 *V,
	                                 int amount,
	                                 std::vector<float> &out) {
Complex      buffer	[mapper_32. getOutputsize()];
int	teller	= 0;

	out. resize (2 * mapper_32. getOutputsize ());
	for (int i = 0; i < amount; i ++) {
	   int	result;
	   if (mapper_32.
	            convert (Complex (real (V [i]) / 32767.0,
	                              imag (V [i]) / 32767.0),
	                                           buffer, &result)) {
	      dump (buffer, result);
	      for (int j = 0; j < result; j ++) {
	         out [teller ++] = real (buffer [j]);
	         out [teller ++] = imag (buffer [j]);
	      }
	   }
	}
	return teller;
}

int	converter_48000::convert_48000	(complex16 *V,
	                                 int amount,
	                                 std::vector<float> & out) {
Complex buffer [amount];
	out. resize (2 * amount);
	for (int i = 0; i < amount; i ++) {
	   buffer [i]	= Complex (real (V [i]) / 32768.0, 
	                           imag (V [i]) / 32768.0);
	   out [2 * i] = real (buffer [i]);
	   out [2 * i + 1] = imag (buffer [i]);
	}
	dump (V, amount);
	return 2 * amount;
}

void	converter_48000::dump (const Complex *buffer, int nrSamples) {
	if (!theWriter. isActive ())
           return;
        int16_t lBuf [2 * nrSamples];
        for (int i = 0; i < nrSamples; i ++) {
           lBuf [2 * i]         = real (buffer [i]) * 32768;
           lBuf [2 * i + 1]     = imag (buffer [i]) * 32768;
        }
        locker. lock();
        theWriter. write (lBuf, nrSamples);
        locker. unlock ();
}


void	converter_48000::dump (const complex16 *buffer,
	                                          int nrSamples) {
	if (!theWriter. isActive ())
	   return;
	int16_t lBuf [2 * nrSamples];
	for (int i = 0; i < nrSamples; i ++) {
	   lBuf [2 * i] 	= real (buffer [i]);
	   lBuf [2 * i + 1]	= imag (buffer [i]);
	}
	locker. lock();
	theWriter. write (lBuf, nrSamples);
        locker. unlock ();
}

