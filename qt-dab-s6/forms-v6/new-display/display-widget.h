#
/*
 *    Copyright (C)  2016 .. 2024
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
 */
#pragma once

#include        "dab-constants.h"
#include	<QObject>
#include	<qwt.h>
#include	<qwt_plot.h>
#include	<QVector>
#include	<QByteArray>
#include	"fft-handler.h"

#include	"ui_scopewidget.h"

#define	SHOW_SPECTRUM		0
#define	SHOW_CORRELATION	1
#define	SHOW_NULL		2
#define	SHOW_TII		3
#define	SHOW_CHANNEL		4

class	RadioInterface;
class	QSettings;

class	spectrumScope;
class	waterfallScope;
class	nullScope;
class	correlationScope;
class	channelScope;
class	IQDisplay;

class	displayWidget: public QObject, public Ui_scopeWidget {
Q_OBJECT
public:
		displayWidget	(RadioInterface *,
	                         QSettings *);
		~displayWidget	();
	int	get_tab		();
	void	showSpectrum	(std::vector<Complex> &, int);
	void	show_null	(Complex *, int);
	void	showCorrelation	(std::vector<float> &,
	                         QVector<int> &);
	void	show_tii	(std::vector<Complex>, int);
	void	showIQ		(std::vector<Complex>);

	void	showChannel	(std::vector<Complex>);
	void	showQuality	(float, float, float);
	void	show_snr	(float);
	void	show_correction	(int);
	void	show_clockErr	(int);
	void	showFrequency	(float);

	void	show_cpuLoad	(float);
	void	showTransmitters	(QByteArray &);

	void	show		();
	void	hide		();
	bool	isHidden	();

private:
	QFrame			myFrame;
	fftHandler		theFFT;
	QSettings		*dabSettings;
	spectrumScope		*mySpectrumScope;
	waterfallScope		*myWaterfallScope;
	nullScope		*myNullScope;
	correlationScope	*myCorrelationScope;
	spectrumScope		*myTII_Scope;
	channelScope		*myChannelScope;
	IQDisplay		*myIQDisplay;

	int			currentTab;

private slots:
	void		switch_tab	(int);
};

