#
/*
 *    Copyright (C) 2013 .. 2017
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

#pragma once

#include	<QThread>
#include	"riff-reader.h"
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	<atomic>
class	newFiles;

class	newReader:public QThread {
Q_OBJECT
public:
			newReader	(newFiles *,
	                                 riffReader *,
	                                 RingBuffer<std::complex<float>> *); 
			~newReader	();
	void		startReader	();
	void		stopReader	();
private:
virtual void		run		();
	riffReader	*theReader;
	RingBuffer<std::complex<float> >	*theBuffer;
	uint64_t	period;
	std::atomic<bool>	running;
	newFiles	*parent;
	uint64_t	fileLength;
signals:
	void		setProgress	(int, float);
};

