#
/*
 *    Copyright (C) 2015 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB
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
 *
 *	MOT handling is a crime, here we have a single class responsible
 *	for handling a MOT directory
 */
#include	"mot-dir.h"
#include	"radio.h"

	motDirectory::motDirectory (RadioInterface *mr,
	                            uint16_t	transportId,
	                            int16_t	segmentSize,
	                            int32_t	dirSize,
	                            int16_t	objects,
	                            uint8_t	*segment,
	                            bool	backgroundFlag) {

	this	-> myRadioInterface	= mr;
	this	-> backgroundFlag	= backgroundFlag;
	for (int i = 0; i < 512; i ++)
	   marked [i] = false;
	num_dirSegments	= -1;
	this	-> transportId	= transportId;
	this	-> dirSize	= dirSize;
	this	-> numObjects	= objects;
	this	-> dir_segmentSize	= segmentSize;
//	fprintf (stderr, "transportId %d, dirSize %d, numObjects %d, segmentSize %d\n",
//	                             transportId, dirSize, objects, segmentSize);
	connect (this, &motDirectory::report_startDir,
	         mr, &RadioInterface::report_startDir); 
	connect (this, &motDirectory::report_completeDir,
	         mr, &RadioInterface::report_completeDir);
	dir_segments. resize (dirSize);
	motComponents. resize (objects);
	for (int i = 0; i < objects; i ++) {
	   motComponents [i]. inUse = false;
	   motComponents [i]. motSlide	= nullptr;
	}
	report_startDir (numObjects);
	dir_segments. resize (segmentSize);
	memcpy (&dir_segments [0], segment, segmentSize);
	marked [0] = true;
	if (segmentSize >= dirSize)
	   analyse_theDirectory();
}

	motDirectory::~motDirectory () {
	for (int i = 0; i < numObjects; i ++) 
	   if (motComponents [i]. inUse &&
	                   (motComponents [i]. motSlide != nullptr)) 
	      delete motComponents [i]. motSlide;
}

motObject	*motDirectory::getHandle (uint16_t transportId) {
int i;
	for (i = 0; i < numObjects; i ++)
	   if ((motComponents [i]. inUse) &&
	             (motComponents [i]. transportId == transportId))
	      return motComponents [i]. motSlide;

	return nullptr;
}

void	motDirectory::setHandle (motObject *h, uint16_t transportId) {
int	i;

	for (i = 0; i < numObjects; i ++)
	   if (!motComponents [i]. inUse) {
	      motComponents [i]. inUse		= true;
	      motComponents [i]. transportId	= transportId;
	      motComponents [i]. motSlide	= h;
	      return;
	   }
}
//
//	unfortunately, directory segments do not need to come in
//	in order
void	motDirectory::directorySegment (uint16_t transportId,
	                                uint8_t	*segment,
	                                int16_t	segmentNumber,
	                                int32_t	segmentSize,
	                                bool	lastSegment) {
int16_t	i;

//	fprintf (stderr, "adding dir segment %d\n", segmentNumber);
	if (this -> transportId != transportId)
	   return;
	if (this -> marked [segmentNumber])
	   return;
	if (lastSegment)
	   this -> num_dirSegments = segmentNumber + 1;
	this	-> marked [segmentNumber] = true;
	uint8_t	*address = &dir_segments [segmentNumber * dir_segmentSize];
	memcpy (address, segment, segmentSize);
//
//	we are "complete" if we know the number of segments and
//	all segments are "in"
	if (this -> num_dirSegments != -1) {
	   for (i = 0; i < this -> num_dirSegments; i ++)
	      if (!this -> marked [i])
	         return;
//	   yes we have all data to build up the directory
	   analyse_theDirectory();
	   
//	   fprintf (stderr, "Going to inspect the directory\n");
	}
}
//
//	This is the tough one, we collected the bits, and now
//	we need to extract the "motObject"s from it

void	motDirectory::analyse_theDirectory () {
uint32_t	currentBase	= 11;	// in bytes
//uint8_t	*data			= dir_segments;
uint16_t extensionLength	= (dir_segments [currentBase] << 8) |
	                                         dir_segments [currentBase + 1];
	report_completeDir ();
	currentBase += 2;
//	int PLI = dir_segments [currentBase] >> 6;
//	int paramId	= dir_segments [currentBase] & 0x3F;
//	fprintf (stderr, "PLI = %d, paramId = %d\n", PLI, paramId);
	currentBase +=  extensionLength;
	
	for (int i = 0; i < numObjects; i ++) {
	   uint16_t transportId	= (dir_segments [currentBase] << 8) |
	                                    dir_segments [currentBase + 1];
	   if (transportId == 0)	// just a dummy
	      break;

//	   fprintf (stderr, "motObject with transportId %d and \n", transportId);
	   uint8_t *segment	= &dir_segments [currentBase + 2];
	   motObject *handle	= new motObject (myRadioInterface,
	                                         true,
	                                         transportId,
	                                         segment,
	                                         -1,
	                                         false,
	                                         backgroundFlag);
	   
	   currentBase		+= 2 + handle -> get_headerSize();
	   setHandle (handle, transportId);
	}
}

uint16_t	motDirectory::get_transportId() {
	return transportId;
}

