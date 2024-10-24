#
/*
 *    Copyright (C) 2013 .. 2023
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

#include	"dab-constants.h"
#include	<QMainWindow>
#include	<QStringList>
#include	<QScrollArea>
#include	<QStandardItemModel>
#ifdef	_SEND_DATAGRAM_
#include	<QUdpSocket>
#endif
#include	<QVector>
#include	<QComboBox>
#include	<QByteArray>
#include	<QLabel>
#include	<QTimer>
#include	"ui_dabradio-6.h"
#include	"ringbuffer.h"
#include	"process-params.h"
#include	"converter_48000.h"
#include	"scanlist-handler.h"
#include	"dl-cache.h"
#include	"content-table.h"
#include	"dxDisplay.h"
#include	<memory>
#include	<mutex>
#ifdef	DATA_STREAMER
#include	"tcp-server.h"
#endif
#include	"scan-handler.h"
#include	"epgdec.h"
#include	"epg-decoder.h"

#include	"device-chooser.h"
#include	"display-widget.h"
#include	"snr-viewer.h"

#include	"findfilenames.h"

#include	"scheduler.h"

#include	"http-handler.h"
#include	"tii-mapper.h"

#include	"ensemble-handler.h"
#include	"logger.h"

class	QSettings;
class	ofdmHandler;
class	deviceHandler;
class	audioPlayer;
class	common_fft;
class	timeTableHandler;
class	audioDisplay;
#ifdef	HAVE_PLUTO_RXTX
class	dabStreamer;
#endif

class techData;
//#include	"ui_config-helper.h"
class	configHandler;

#define FAST_DECODER    0100
#define ALT1_DECODER    0200
#define ALT2_DECODER    0300


#define SCAN_BUTTON		QString ("scanButton")
#define	SPECTRUM_BUTTON		QString ("spectrumButton")
#define	SCANLIST_BUTTON		QString ("scanListButton")
#define PRESET_BUTTON           QString ("presetButton")
#define PREVSERVICE_BUTTON	QString ("prevServiceButton")
#define NEXTSERVICE_BUTTON	QString ("nextServiceButton")
#define	CONFIG_BUTTON		QString ("configButton")
#define	HTTP_BUTTON		QString ("httpButton")
/*
 *	The main gui object. It inherits from
 *	QWidget and the generated form
 */

class dabService {
public:
	QString		channel;
	QString		serviceName;
	uint32_t	SId;
	int		SCIds;
	int		subChId;
	bool		valid;
	bool		is_audio;
	bool		announcement_going;
	FILE		*fd;
	FILE		*frameDumper;
	dabService () {
	   fd		= nullptr;
	   frameDumper	= nullptr;
	   valid	= false;
	   is_audio	= false;
	   announcement_going	= false;
	}
	~dabService	() {}
};

struct	theTime {
	int	year;
	int	month;
	int	day;
	int	hour;
	int	minute;
	int	second;
};

class	channelDescriptor {
public:
	channelDescriptor () {
	cleanChannel ();
}
	~channelDescriptor () {
}

	QString		channelName;
	int		tunedFrequency;
	bool		realChannel;
	bool		etiActive;
	int		serviceCount;	// from FIC or nothing
	int		nrServices;	// measured
	QString		ensembleName;
	std::vector<dabService> backgroundServices;
	dabService	currentService;
	uint32_t	Eid;
	bool		has_ecc;
	uint8_t		ecc_byte;
	QString		countryName;
	int		nrTransmitters;
	int		snr;
	std::vector<transmitterDesc>	transmitters;
//	QByteArray	transmitters;
	int8_t		mainId;
	int8_t		subId;
	position	targetPos;
	QString		transmitterName;
	float		height;
	float		distance;
	float		corner;
	bool		audioActive;

	void	cleanChannel () {
	realChannel	= true;
	serviceCount	= -1;
	nrServices	= -1;
	tunedFrequency	= -1;
	ensembleName	=  "";
	nrTransmitters	= 0;
	transmitters. resize (0);
	countryName	= "";
	targetPos	= position {0, 0};
	mainId		= -1;
	subId		= -1;
	transmitterName	= "";
	Eid		= 0;
	has_ecc		= false;
	snr		= 0;
	height		= -1;
	distance	= -1;
	audioActive	= false;
	currentService. valid		= false;
	currentService. frameDumper	= nullptr;
	}
};

class RadioInterface: public QWidget, private Ui_dabradio {
Q_OBJECT
public:
		RadioInterface		(QSettings	*,
	                                 const QString	&,	//scanlist
	                                 const QString	&,	//presets
	                                 const QString	&,	//freqExt
	                                 const QString	&,	//schedule
	                                 bool,			// errorreport
	                                 int32_t	dataPort,
	                                 int32_t	clockPort,
	                                 int,
	                                 QWidget	*parent = nullptr);
		~RadioInterface		();

	bool	channelOn		();
	bool	devScopeOn		();
protected:
	bool			eventFilter (QObject *obj, QEvent *event);
private:
	FILE			*dlTextFile;
	RingBuffer<Complex>	theSpectrumBuffer;
	RingBuffer<Complex>	theIQBuffer;
	RingBuffer<Complex>	theTIIBuffer;
	RingBuffer<Complex>	theNULLBuffer;
	RingBuffer<Complex>	theChannelBuffer;
	RingBuffer<float>	theSNRBuffer;
	RingBuffer<float>	theResponseBuffer;
	RingBuffer<uint8_t>	theFrameBuffer;
	RingBuffer<uint8_t>	theDataBuffer;
	RingBuffer<std::complex<int16_t>>	theAudioBuffer;
	RingBuffer<float>	stdDevBuffer;

	displayWidget		theNewDisplay;
	snrViewer		theSNRViewer;
//	QFrame			theDataDisplay;
	dlCache			theDLCache;
	tiiMapper		theTIIProcessor;
	findfileNames		theFilenameFinder;
	Scheduler		theScheduler;
	RingBuffer<std::complex<int16_t>>	theTechData;
	converter_48000		theAudioConverter;

	scanListHandler		theScanlistHandler;
	deviceChooser		theDeviceChoser;
	dxDisplay		theDXDisplay;
	logger			theLogger;
	scanHandler		theSCANHandler;
	configHandler		*configHandler_p;
	ensembleHandler		*the_ensembleHandler;
	QLabel			*dxDisplayText;

//	end of variables that are initalized

	bool			autoStart_http		();
	dabService		checkPresets		();
	int			int_configValue		(const QString &, int);

	position		localPos;
	dabService		nextService;
	std::vector<QPixmap>	strengthLabels;
	httpHandler		*mapHandler;
	processParams		globals;
	QString			SystemVersion;
	QString			version;
	int			fmFrequency;
	contentTable		*contentTable_p;
	contentTable		*scanTable_p;
	channelDescriptor	channel;
	QDialog			*the_aboutLabel;
	int			maxDistance;
	bool			error_report;
	techData		*techWindow_p;
//	Ui_configWidget		configWidget;
	QSettings		*dabSettings_p;
	int16_t			tii_delay;
	int32_t			dataPort;
	bool			stereoSetting;
	std::atomic<bool>	running;
	deviceHandler		*inputDevice_p;
//
//	for the peaklevel indicator
//	float			peakLeftDamped;
//	float			peakRightDamped;

	QString			labelStyle;
#ifdef	HAVE_PLUTO_RXTX
	dabStreamer		*streamerOut_p;
#endif
	ofdmHandler		*theOFDMHandler;
	audioPlayer		*soundOut_p;
#ifdef	DATA_STREAMER
	tcpServer		*dataStreamer_p;
#endif
#ifdef	CLOCK_STREAMER
	tcpServer		*clockStreamer_p;
#endif
	CEPGDecoder		epgHandler;
	epgDecoder		epgProcessor;
	QString			epgPath;
	QTimer			epgTimer;
	QTimer			pauzeTimer;
	QString			path_for_tiiFile;
	QString			path_for_pictures;
	QString			path_for_files;
#ifdef	_SEND_DATAGRAM_
	QUdpSocket		dataOut_socket;
	QString			ipAddress;
	int32_t			port;
#endif
	bool			sourceDumping;
	bool			audioDumping;
	void			set_Colors		();
	void			set_channelButton	(int);

	void			displaySlide	(const QPixmap &,	
	                                              const QString &t = "");
	QTimer			displayTimer;
	QTimer			channelTimer;
	QTimer			presetTimer;
	QTimer			muteTimer;
	int			muteDelay;
	int32_t			numberofSeconds;
	int16_t			ficBlocks;
	int16_t			ficSuccess;
	int			total_ficError;
	int			total_fics;
	void			connectGUI		();
	void			disconnectGUI		();

	struct theTime		localTime;
	struct theTime		UTC;
      
	size_t			previous_idle_time;
	size_t			previous_total_time;

	QPixmap			fetch_announcement (int id);

	QString			convertTime		(int, int, int, int, int);
	QString			convertTime		(struct theTime &);
	void			set_buttonColors	(QPushButton *,
	                                                 const QString &);
	QString			footText		();
	QString			presetText		();
	void			cleanScreen		();
	void			hideButtons		();
	void			showButtons		();
	deviceHandler		*create_device		(const QString &);
	timeTableHandler	*my_timeTable;

	void			start_etiHandler	();
	void			stop_etiHandler		();
	QString			checkDir		(const QString &);
//
	void			startAudioservice	(audiodata &);
	void			startPacketservice	(const QString &);
	void			startAudiodumping	();
	void			stopAudiodumping	();
	void			scheduled_audioDumping	();
	void			scheduled_dlTextDumping ();
	void			scheduled_ficDumping	();
	FILE			*ficDumpPointer;

	void			start_sourcedumping	();
	void			stop_sourcedumping	();
	void			startFramedumping	();
	void			stopFramedumping	();
	void			scheduled_frameDumping	(const QString &);
	void			startChannel		(const QString &);
	void			stopChannel		();
	void			stopService		(dabService &);
	void			startService		(dabService &);
	void			localSelect		(const QString &s);
	void			scheduleSelect		(const QString &s);

	QString			build_headLine		();
	QString			build_cont_addLine	(transmitterDesc &);
	void			show_for_single_scan	();
	void			show_for_continuous	();

	void			doStart_direct		();
	void			save_MOTObject		(QByteArray &,
	                                                 QString);

	void			save_MOTtext		(QByteArray &, int,
	                                                 const QString &);
	void			show_MOTlabel		(QByteArray &, int,
	                                                 const QString &,
	                                                 int, bool);
	void			stop_muting		();


//	short hands
	void                    new_channelIndex        (int);

	std::mutex		locker;
	void			set_soundLabel		(bool);

	void			start_scan_to_data	();
	void			start_scan_single	();
	void			start_scan_continuous	();
	void			next_for_scan_to_data	();
	void			next_for_scan_single	();
	void			next_for_scan_continuous	();
	void			stop_scan_to_data	();
	void			stop_scan_single	();
	void			stop_scan_continuous	();
signals:
	void			select_ensemble_font	();
	void			select_ensemble_fontSize	();
	void			select_ensemble_fontColor	();

public slots:
	void			channelSignal		(const QString &);
	void			handle_correlationSelector	(int);
	void			handle_dxSelector	(int);
	void			show_dcOffset		(float);
	void			startScanning		();
	void			stopScanning		();
	void			show_quality		(float, float, float);
	void			show_rsCorrections	(int, int);
	void			show_clock_error	(int);

	void			show_Corrector		(int, float);
	void			add_to_ensemble		(const QString &,
	                                                          int, int);
	void			name_of_ensemble	(int, const QString &);
	void			nrServices		(int);
	void			show_frameErrors	(int);
	void			show_rsErrors		(int);
	void			show_aacErrors		(int);
	void			show_ficSuccess		(bool);
	void			set_synced		(bool);
	void			show_label		(const QString &);
	void			handle_motObject	(QByteArray,
	                                                 QString,
	                                                 int, bool, bool);
	void			sendDatagram		(int);
	void			handle_tdcdata		(int, int);
	void			changeinConfiguration	(const QStringList &,
	                                                 const QStringList &);
	void			newAudio		(int, int, bool, bool);
//
	void			localSelect_SS		(const QString &,
	                                                 const QString &);
	
	void			setStereo		(bool);
	void			set_streamSelector	(int);
	void			no_signal_found		();
	void			show_mothandling	(bool);
	void			set_sync_lost		();
	void			closeEvent		(QCloseEvent *event);
	void			clockTime		(int, int, int,
	                                                 int, int,
	                                                 int, int, int, int);
	void			start_announcement	(const QString &,
	                                                      int, int);
	void			stop_announcement	(const QString &, int);
	void			newFrame		(int);

	void			set_epgData		(int, int,
	                                                 const QString &,
	                                                 const QString &);
	void			epgTimer_timeOut	();
	void			handle_presetSelect	(const QString &,
	                                                 const QString &);
	void			handle_contentSelector	(const QString &);
	
	void			http_terminate		();
	void			show_channel		(int);

	void			handle_iqSelector	();

	void			show_spectrum		(int);
	void			show_tii		(int, int);
	void			show_tii_spectrum       ();
	void			show_snr		(float);
	void			show_null		(int);
	void			showIQ			(int);
	void			show_correlation	(int, int,
	                                                 QVector<int> );
	void			show_stdDev		(int);
	void			showPeakLevel		(float, float);

	void			handle_techFrame_closed ();
	void			handle_configFrame_closed ();
	void			handle_deviceFrame_closed ();
	void			handle_newDisplayFrame_closed ();

	void			doStart			(const QString &);
	void			newDevice		(const QString &);
	void			handle_scheduleButton	();
	void			handle_devicewidgetButton	();
	void			handle_resetButton	();
	void			handle_dlTextButton		();
	void			handle_snrButton	();
	void			handle_sourcedumpButton	();
	void			scheduler_timeOut	(const QString &);
	void			show_changeLabel (const QStringList notInOld,
                                          	  const QStringList notInNew);
 
//	Somehow, these must be connected to the GUI
private slots:
	void			show_pauzeSlide		();
	void			handle_timeTable	();
	void			handle_contentButton	();
	void			handle_detailButton	();
	void			handle_scanButton	();
	void			handle_etiHandler	();

	void			handle_spectrumButton	();

	void			handle_scanListButton	();
	void			handle_presetButton	();
	void			handle_framedumpButton	();
	void			handle_audiodumpButton 	();
	void			handle_prevServiceButton        ();
	void			handle_nextServiceButton        ();

	void			handle_channelSelector		(const QString &);
	void			handle_nextChannelButton	();
	void			handle_prevChannelButton	();

	void			handle_muteButton		();
	void			handle_folderButton		();

	void			handle_scanListSelect	(const QString &);
	void			TerminateProcess	();
	void			updateTimeDisplay	();
	void			channel_timeOut		();

	void			start_background_task	(const QString &);

	void			setPresetService	();
	void			muteButton_timeOut	();

	void			handle_configButton	();
	void			handle_httpButton	();
	void			setVolume		(int);
	void			handle_snrLabel		();
//
//	color handlers
	void			handle_labelColor	();
	void			color_scanButton	();
	void			color_presetButton	();
	void			color_spectrumButton	();
	void			color_scanListButton	();
	void			color_prevServiceButton ();     
	void			color_nextServiceButton ();
	void			color_configButton	();
	void			color_httpButton	();

//
//	config handlers
public slots:
	void			set_transmitters_local		(bool);
	void			set_tii_detectorMode		(bool);
	void			handle_LoggerButton		(int);
	void			handle_set_coordinatesButton	();
//	void			handle_transmSelector		(int);
	void			handle_eti_activeSelector	(int);
	void			handle_loadTable		();
	void			handle_dcRemovalSelector	(bool);
	void			selectDecoder			(int);
	void			handle_aboutLabel		();
};
