cmake_minimum_required( VERSION 3.21 )

# set CMAKE_BUILD_TYPE to "Release" if not defined (like call from command line)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Set build type to Release if not specified" FORCE)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/CMakeFunctions.cmake)

project(Qt-DAB VERSION 6)
set (objectName qt-dab-6.9.2)
set (objectName "Qt-DAB")
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTORCC ON)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "/usr/local/bin" CACHE PATH "default install path" FORCE )
endif()

add_definitions(-DAPP_NAME="${objectName}")
add_definitions(-DPRJ_NAME="${PROJECT_NAME}")
add_definitions(-DPRJ_VERS="${PROJECT_VERSION}")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # regarding using QWT < 6.2.0 (6.1.4) to avoid many deprecated massages (only suppressed on release versions)
    add_definitions(-DQT_NO_DEPRECATED_WARNINGS)
endif()

if(MINGW)
	add_definitions ( -municode)
endif()

add_definitions (-D__HAS_CHANNEL__)
add_definitions (-D__FFTW3__)

# handle GIT hash
get_git_commit_hash() # returns GIT_COMMIT_HASH
print(GIT_COMMIT_HASH)
if (GIT_COMMIT_HASH)
    add_definitions("-DGITHASH=\"${GIT_COMMIT_HASH}\"")
else ()
    add_definitions("-DGITHASH=\"(unknown)\"")
endif ()


########################################################################
# select the release build type by default to get optimization flags
########################################################################
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

### make sure our local CMake Modules path comes first
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/Modules)

######################################################################
#
#	if you want the audio output sent over the internet:
set (TCP_STREAMER OFF)

#####################################################################
#
# select one or more devices (set to ON)
# Note that since there are problems with the rtlsdr interface under Windows we split the rtlsdr interface
# into two versions one for Linux and one for Window
option(RTLSDR_LINUX OFF)	# use for Linux type systems
option(RTLSDR_WIN OFF) 		# only for windows
option(AIRSPY OFF)  
option(SDRPLAY_V2 ON) 
option(SDRPLAY_V3 OFF)
option(HACKRF OFF)
option(LIMESDR OFF) 
option(PLUTO OFF) 
option(SOAPY OFF)
option(SPY_SERVER_8 OFF)
option(SPY_SERVER_16 OFF)
option(UHD OFF) 		# untested
option(RTL_TCP OFF)      

# additional options
option(USE_LTO "Set -flto flag" ON)  
# special release version flags
if (USE_LTO)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
endif ()

option(FDK_AAC ON)
option(DATA_STREAMER OFF)
option(VITERBI_SSE ON)
option(VITERBI_NEON OFF)


####################################################################
#	Use first option on "slow" cpus, i.e. RPI 3 or so
#	Most of the FFT processing will be done in functions eunning
#	in its own thread
#add_definitions (-D__MSC_THREAD__)	# do not uncomment this
#
#	Since more than one backend may be active, activating this
#	option makes that each backend runs in its own thread
add_definitions (-D__THREADED_BACKEND__)	# uncomment for use for an RPI
#
########################################################################
########################################################################

	find_package (PkgConfig)
	find_package (LSBId)

	find_package (Qt6Core REQUIRED)
	find_package (Qt6Widgets REQUIRED)
	find_package (Qt6Network REQUIRED)
	find_package (Qt6Xml REQUIRED)
	include_directories (
	  ${Qt6Network_INCLUDE_DIRS}
	  ${Qt6Xml_INCLUDE_DIRS}
	)

	search_for_library (QWT Qwt)
	search_for_library (ZLIB zlib)

	find_package(FFTW3f)
        if (NOT FFTW3F_FOUND)
            message(FATAL_ERROR "please install FFTW3")
        endif ()
	include_directories ( ${FFTW_INCLUDE_DIRS})
        list(APPEND extraLibs ${FFTW3F_LIBRARIES})

	if (NOT APPLE)
	   find_library (PTHREADS pthread)
	   if (NOT(PTHREADS))
	      message (FATAL_ERROR "please install libpthread")
	   else (NOT(PTHREADS))
	      set (extraLibs ${extraLibs} ${PTHREADS})
	   endif (NOT(PTHREADS))
	endif (NOT APPLE)

#######################################################################
#
#	Here we really start

	include_directories (
	           ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	           ${QT6Widgets_INCLUDES}
	           ${QT_QTCORE_INCLUDE_DIR}
	           ${QT_QTGUI_INCLUDE_DIR}
	           ./support
	           ./forms-v7
	           ./forms-v7/new-display
	           ./forms-v7/new-display/scopes
	           ./forms-v7/snr-viewer
	           ..
	           ../includes
	           ../includes/ofdm
	           ../includes/protection
	           ../includes/backend
	           ../includes/backend/audio
	           ../includes/backend/data
	           ../includes/backend/data/journaline
	           ../includes/backend/data/mot
	           ../includes/backend/data/epg
	           ../includes/support
	           ../src/support/tii-library
	           ../includes/support/buttons
	           ../src/support/viterbi-spiral
	           ../includes/output
	           ../includes/output/Qt6
	           ../qt-devices
	           ../qt-devices/filereaders/
	           ../qt-devices/filereaders/new-reader
	           ../qt-devices/filereaders/rawfiles-new
	           ../qt-devices/filereaders/xml-filereader
	           ../eti-handler
	           /usr/include/
	)

	set (${objectName}_HDRS
	     ./radio.h
	     ./support/super-frame.h
	     ./support/techdata.h 
             ./support/clickable-label.h 
             ./support/icon-label.h 
             ./support/scan-handler.h 
             ./support/scantable-handler.h 
             ./support/aboutdialog.h 
	     ./support/ensemble-handler.h
	     ./support/config-handler.h
	     ./support/audiosystem-selector.h
	     ./support/dxDisplay.h
	     ./forms-v7/new-display/display-widget.h 
             ./forms-v7/new-display/scopes/correlation-scope.h 
             ./forms-67/new-display/scopes/spectrum-scope.h 
             ./forms-v7/new-display/scopes/null-scope.h 
             ./forms-v7/new-display/scopes/channel-scope.h 
	     ./forms-v7/new-display/dev-scope.h
             ./forms-v7/new-display/scopes/waterfall-scope.h 
             ./forms-v7/new-display/scopes/iqdisplay.h 
	     ./forms-v7/new-display/scopes/audio-display.h
	     ./forms-v7/new-displat/scopes/spectrogram-data.h
	     ./forms-v7/snr-viewer/snr-viewer.h
	     ../ofdm-handler.h
	     ../eti-handler/eti-generator.h
	     ../includes/dab-constants.h
	     ../includes/crc-handlers.h
	     ../includes/bit-extractors.h
	     ../includes/mot-content-types.h 
             ../includes/ofdm/sample-reader.h
	     ../includes/ofdm/correlator.h
	     ../includes/ofdm/freqsyncer.h
	     ../includes/ofdm/ofdm-decoder.h
	     ../includes/ofdm/phasetable.h
	     ../includes/ofdm/freq-interleaver.h
	     ../includes/ofdm/fic-handler.h
	     ../includes/ofdm/ensemble.h
	     ../includes/ofdm/fib-config.h
	     ../includes/ofdm/fib-printer.h
	     ../includes/ofdm/fib-decoder.h
	     ../includes/ofdm/fib-table.h
	     ../includes/ofdm/tii-detector.h
	     ../includes/ofdm/tii-detector-1.h
	     ../includes/ofdm/tii-detector-2.h
	     ../includes/ofdm/timesyncer.h
	     ../includes/ofdm/estimator.h
	     ../includes/protection/protTables.h
	     ../includes/protection/protection.h
	     ../includes/protection/uep-protection.h
	     ../includes/protection/eep-protection.h
	     ../includes/backend/firecode-checker.h
	     ../includes/backend/frame-processor.h
	     ../includes/backend/charsets.h
	     ../includes/backend/galois.h
	     ../incluces/backend/reed-solomon.h
	     ../includes/backend/msc-handler.h
	     ../includes/backend/backend.h
	     ../includes/backend/backend-deconvolver.h
	     ../includes/backend/backend-driver.h
	     ../includes/backend/audio/mp4processor.h
	     ../includes/backend/audio/bitWriter.h
	     ../includes/backend/audio/mp2processor.h
	     ../includes/backend/data/ip-datahandler.h
	     ../includes/backend/data/adv-datahandler.h
	     ../includes/backend/data/tdc-datahandler.h
	     ../includes/backend/data/journaline-datahandler.h
	     ../includes/backend/data/journaline-screen.h
	     ../includes/backend/data/journaline/dabdatagroupdecoder.h
	     ../includes/backend/data/journaline/crc_8_16.h
	     ../includes/backend/data/journaline/log.h
	     ../includes/backend/data/journaline/newssvcdec_impl.h
	     ../includes/backend/data/journaline/Splitter.h
	     ../includes/backend/data/journaline/dabdgdec_impl.h
	     ../includes/backend/data/journaline/newsobject.h
	     ../includes/backend/data/journaline/NML.h
	     ../includes/backend/data/epg/epgdec.h
	     ../includes/backend/data/epg/epg-compiler.h
	     ../includes/backend/data/epg/xml-extractor.h
	     ../includes/backend/data/virtual-datahandler.h
	     ../includes/backend/data/pad-handler.h
	     ../includes/backend/data/mot/mot-handler.h
	     ../includes/backend/data/mot/mot-object.h
	     ../includes/backend/data/mot/mot-dir.h
	     ../includes/backend/data/data-processor.h
	     ../includes/output/audio-player.h
	     ../includes/support/distances.h 
	     ../includes/support/time-converter.h 
	     ../includes/support/logger.h 
	     ../includes/support/position-handler.h 
	     ../includes/support/cacheElement.h 
	     ../includes/support/settingNames.h 
	     ../includes/support/settings-handler.h 
	     ../includes/support/fft-handler.h 
	     ../includes/support/wavWriter.h 
	     ../includes/support/converter_48000.h 
	     ../includes/support/ringbuffer.h
	     ../includes/support/Xtan2.h
	     ../includes/support/dab-params.h
	     ../includes/support/dab-tables.h
	     ../includes/support/viterbi-spiral/viterbi.h
	     ../includes/support/preset-handler.h
	     ../includes/support/presetcombobox.h
	     ../includes/support/scanlist-handler.h
	     ../includes/support/scheduler.h
	     ../includes/support/schedule-selector.h
	     ../includes/support/element-selector.h
	     ../includes/support/time-table.h
	     ../includes/support/findfilenames.h
	     ../includes/support/content-table.h
	     ../includes/support/dl-cache.h
	     ../includes/support/ITU_Region_1.h
	     ../includes/support/coordinates.h
	     ../includes/support/skin-handler.h
	     ../includes/support/mapport.h
	     ,,.includes/support/http-handler.h
	     ,,.includes/support/converted_map.h
	     ../includes/support/fir-filters.h
             ../src/support/tii-library/tii-reader.h
	     ../src/support/tii-library/tii-mapper.h
             ../src/support/tii-library/uploader.h
	     ../includes/support/font-chooser.h 
	     ../includes/support/buttons/smallcombobox.h 
             ../includes/support/buttons/newpushbutton.h 
             ../includes/support/buttons/normalpushbutton.h 
             ../includes/support/buttons/smallpushbutton.h 
	     ../includes/support/buttons/verysmallpushbutton.h 
             ../includes/support/buttons/smallqlistview.h 
             ../includes/support/buttons/smallspinbox.h 
#	     ../includes/scopes-qwt6/spectrogramdata.h
	     ../qt-devices/device-handler.h
	     ../qt-devices/device-exceptions.h
	     ../qt-devicces/xml-filewriter.h
	)

	set (${objectName}_SRCS
	     ${${objectName}_SRCS}
	     ./main.cpp
	     ./radio.cpp
	     ./support/super-frame.cpp
	     ./support/techdata.cpp
	     ./support/clickable-label.cpp 
             ./support/icon-label.cpp 
             ./support/scan-handler.cpp 
             ./support/scantable-handler.cpp 
             ./support/aboutdialog.cpp 
             ./support/ensemble-handler.cpp 
             ./support/config-handler.cpp 
             ./support/audiosystem-selector.cpp 
	     ./support/dxDisplay.cpp
	     ./forms-v7/new-display/display-widget.cpp 
             ./forms-v7/new-display/scopes/spectrum-scope.cpp 
             ./forms-v7/new-display/scopes/correlation-scope.cpp 
             ./forms-v7/new-display/scopes/null-scope.cpp 
             ./forms-v7/new-display/scopes/channel-scope.cpp 
             ./forms-v7/new-display/scopes/dev-scope.cpp 
             ./forms-v7/new-display/scopes/waterfall-scope.cpp 
             ./forms-v7/new-display/scopes/iqdisplay.cpp 
	     ./forms-v7/new-display/scopes/audio-display.cpp
	     ./forms-v7/new-display/scopes/spectrogramdata.cpp
	     ./forms-v7/snr-viewer/snr-viewer.cpp
	     ../ofdm-handler.cpp
	     ../eti-handler/eti-generator.cpp
             ../src/ofdm/sample-reader.cpp
	     ../src/ofdm/ofdm-decoder.cpp
	     ../src/ofdm/correlator.cpp
	     ../src/ofdm/freqsyncer.cpp
	     ../src/ofdm/phasetable.cpp
	     ../src/ofdm/freq-interleaver.cpp
	     ../src/ofdm/fic-handler.cpp
	     ../src/ofdm/ensemble.cpp
	     ../src/ofdm/fib-config.cpp
	     ../src/ofdm/fib-printer.cpp
	     ../src/ofdm/fib-decoder.cpp
	     ../src/ofdm/tii-detector.cpp
	     ../src/ofdm/tii-detector-1.cpp
	     ../src/ofdm/tii-detector-2.cpp
	     ../src/ofdm/timesyncer.cpp
	     ../src/ofdm/estimator.cpp
	     ../src/protection/protTables.cpp
	     ../src/protection/protection.cpp
	     ../src/protection/eep-protection.cpp
	     ../src/protection/uep-protection.cpp
	     ../src/backend/firecode-checker.cpp
	     ../src/backend/charsets.cpp
	     ../src/backend/galois.cpp
	     ../src/backend/reed-solomon.cpp
	     ../src/backend/msc-handler.cpp
	     ../src/backend/backend.cpp
	     ../src/backend/backend-deconvolver.cpp
	     ../src/backend/backend-driver.cpp
	     ../src/backend/audio/mp4processor.cpp
	     ../src/backend/audio/bitWriter.cpp
	     ../src/backend/audio/mp2processor.cpp
	     ../src/backend/data/ip-datahandler.cpp
	     ../src/backend/data/adv-datahandler.cpp
	     ../src/backend/data/journaline-datahandler.cpp
	     ../src/backend/data/journaline-screen.cpp
	     ../src/backend/data/journaline/crc_8_16.c
	     ../src/backend/data/journaline/log.c
	     ../src/backend/data/journaline/newssvcdec_impl.cpp
	     ../src/backend/data/journaline/Splitter.cpp
	     ../src/backend/data/journaline/dabdgdec_impl.c
	     ../src/backend/data/journaline/newsobject.cpp
	     ../src/backend/data/journaline/NML.cpp
	     ../src/backend/data/epg/epg-compiler.cpp
	     ../src/backend/data/epg/xml-extractor.cpp
	     ../src/backend/data/tdc-datahandler.cpp
	     ../src/backend/data/pad-handler.cpp
	     ../src/backend/data/mot/mot-handler.cpp
	     ../src/backend/data/mot/mot-object.cpp
	     ../src/backend/data/mot/mot-dir.cpp
	     ../src/backend/data/data-processor.cpp
	     ../src/output/audio-player.cpp
	     ../src/support/distances.cpp
	     ../src/support/time-converter.cpp
	     ../src/support/logger.cpp
	     ../src/support/position-handler.cpp
	     ../src/support/settings-handler.cpp
	     ../src/support/fft-handler.cpp
	     ../src/support/wavWriter.cpp
	     ../src/support/converter_48000.cpp
	     ../src/support/viterbi-spiral/viterbi.cpp
	     ../src/support/dab-params.cpp
	     ../src/support/dab-tables.cpp
	     ../src/support/preset-handler.cpp
	     ../src/support/presetcombobox.cpp
	     ../src/support/scanlist-handler.cpp
	     ../src/support/scheduler.cpp
	     ../src/support/schedule-selector.cpp
	     ../src/support/element-selector.cpp
	     ../src/support/time-table.cpp
	     ../src/support/findfilenames.cpp
	     ../src/support/content-table.cpp
	     ../src/support/ITU_Region_1.cpp
	     ../src/support/coordinates.cpp
	     ../src/support/skin-handler.cpp
	     ../src/support/mapport.cpp
	     ../src/support/tii-library/tii-reader.cpp
             ../src/support/tii-library/uploader.cpp
	     ../src/support/tii-library/tii-mapper.cpp
	     ../src/support/http-handler.cpp
	     ../src/support/bandpass-filter.cpp
	     ../src/support/fir-filters.cpp
	     ../src/support/font-chooser.cpp
	     ../src/support/buttons/smallcombobox.cpp
	     ../src/support/buttons/newpushbutton.cpp
	     ../src/support/buttons/normalpushbutton.cpp
	     ../src/support/buttons/smallpushbutton.cpp
	     ../src/support/buttons/verysmallpushbutton.cpp
	     ../src/support/buttons/smallqlistview.cpp
	     ../src/support/buttons/smallspinbox.cpp
#	     ../src/scopes-qwt6/spectrogramdata.cpp
#	     ../src/scopes-qwt6/spectrogramdata.cpp 
	     ../qt-devices/xml-filewriter.cpp
	)

	set (${objectName}_MOCS
	     ./radio.h
	     ./support/super-frame.h
	     ./support/techdata.h
	     ./support/clickable-label.h 
	     ./support/icon-label.h 
	     ./support/scan-handler.h 
	     ./support/scantable-handler.h 
	     ./support/aboutdialog.h 
	     ./support/ensemble-handler.h 
	     ./support/config-handler.h 
	     ./support/audiosystem-selector.h
	     ./support/dxDisplay.h
	     ./forms-v7/new-display/display-widget.h 
	     ./forms-v7/new-display/scopes/correlation-scope.h 
	     ./forms-v7/new-display/scopes/spectrum-scope.h 
	     ./forms-v7/new-display/scopes/null-scope.h 
	     ./forms-v7/new-display/scopes/channel-scope.h 
	     ./forms-v7/new-display/scopes/dev-scope.h 
	     ./forms-v7/new-display/scopes/waterfall-scope.h 
	     ./forms-v7/new-display/scopes/iqdisplay.h 
	     ./forms-v7/new-display/scopes/audio-display.h
 	     ./forms-v7/snr-viewer/snr-viewer.h
	     ../ofdm-handler.h
	     ../includes/ofdm/sample-reader.h
	     ../includes/ofdm/ofdm-decoder.h
	     ../includes/ofdm/correlator.h
	     ../includes/ofdm/freqsyncer.h
	     ../includes/ofdm/fic-handler.h
	     ../includes/ofdm/fib-decoder.h
	     ../includes/ofdm/tii-detector.h
	     ../includes/ofdm/estimator.h
	     ../includes/backend/msc-handler.h
	     ../includes/backend/backend.h
	     ../includes/backend/audio/mp2processor.h
	     ../includes/backend/audio/mp4processor.h
	     ../includes/backend/data/virtual-datahandler.h
	     ../includes/backend/data/pad-handler.h
	     ../includes/backend/data/mot/mot-handler.h
	     ../includes/backend/data/mot/mot-object.h
	     ../includes/backend/data/mot/mot-dir.h
	     ../includes/backend/data/ip-datahandler.h
	     ../includes/backend/data/adv-datahandler.h
	     ../includes/backend/data/tdc-datahandler.h
	     ../includes/backend/data/journaline-datahandler.h
	     ../includes/backend/data/journaline-screen.h
	     ../includes/backend/data/epg/epg-compiler.h
	     ../includes/backend/data/epg/xml-extractor.h
	     ../includes/backend/data/data-processor.h
	     ../includes/output/audio-player.h
	     ../includes/support/converter_48000.h
	     ../includes/support/preset-handler.h
	     ../includes/support/presetcombobox.h
	     ../includes/support/font-chooser.h
	     ../includes/support/buttons/smallcombobox.h 
	     ../includes/support/buttons/newpushbutton.h 
	     ../includes/support/buttons/normalpushbutton.h 
	     ../includes/support/buttons/smallpushbutton.h 
	     ../includes/support/buttons/verysmallpushbutton.h 
	     ../includes/support/buttons/smallqlistview.h 
	     ../includes/support/buttons/smallspinbox.h 
	     ../includes/support/scanlist-handler.h
	     ../includes/support/scheduler.h
	     ../includes/support/schedule-selector.h
	     ../includes/support/element-selector.h
	     ../includes/support/time-table.h
	     ../includes/support/content-table.h
	     ../includes/support/coordinates.h
	     ../includes/support/skin-handler.h
	     ../includes/support/mapport.h
	     ../includes/support/http-handler.h
	)

	set (${objectName}_UIS
	        ${${objectName}_UIS}
	     ./support/aboutdialog.ui
	     ./forms-v7/dabradio-6.ui
	     ./forms-v7/technical_data.ui
	     ./forms-v7/config-helper.ui
	     ./forms-v7/new-display/scopewidget.ui
	     ./forms-v7/snr-viewer/snr-widget.ui
	     ../qt-devices/filereaders/xml-filereader/xmlfiles.ui
	   )


#########################################################################

	if (FDK_AAC)
	   find_package(Fdk-AAC)
	   if (NOT LIBFDK_AAC_FOUND)
	      message(FATAL_ERROR, "Please install libfdk-aac")
	   endif ()
	   include_directories (../helpers/specials/fdk-aac)
	   set($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ../includes/backend/audio/fdk-aac.h
	   )

	   set(${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../src/backend/audio/fdk-aac.cpp
	   )
	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ../includes/backend/audio/fdk-aac.h
	   )
	   list(APPEND extraLibs ${FDK_AAC_LIBRARIES})
	   add_definitions(-D__WITH_FDK_AAC__)
	elseif (NOT FDK_AAC)
	   find_package (Faad)
	   if (NOT FAAD_FOUND)
	      message(FATAL_ERROR "Please install libfaad")
	   endif ()
	   set(${objectName}_HDRS
	       ${${objectName}_HDRS}
	       ../includes/backend/audio/faad-decoder.h
	   )

	   set(${objectName}_SRCS
	       ${${objectName}_SRCS}
	       ../src/backend/audio/faad-decoder.cpp
	   )
	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ../includes/backend/audio/faad-decoder.h
	   )
	   add_definitions(-D__WITH_FAAD__)
	endif (FDK_AAC)

#	the viterbi decoder
#
	if (VITERBI_NEON)
	   set(VITERBI_SSE false)
	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ../src/support/viterbi-spiral/spiral-neon.h
	   )
	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../src/support/viterbi-spiral/spiral-neon.c
	   )
	   
	   add_definitions (-DNEON_AVAILABLE)
	elseif(VITERBI_SSE)
	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ../src/support/viterbi-spiral/spiral-sse.h
	   )
	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../src/support/viterbi-spiral/spiral-sse.c
	   )
	   add_definitions (-DSSE_AVAILABLE)
	else (VITERBI_SSE)
	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ../src/support/viterbi-spiral/spiral-no-sse.h
	   )
	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../src/support/viterbi-spiral/spiral-no-sse.c
	   )
	endif (VITERBI_NEON)

	if (TCP_STREAMER)
	   add_definitions (-DTCP_STREAMER)
	   find_package (Qt6Network REQUIRED)
	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ../includes/output/tcp-streamer.h
	   )
	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ../includes/output/tcp-streamer.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../src/output/tcp-streamer.cpp
	   )
	endif (TCP_STREAMER)

	if (DATA_STREAMER)
	   include_directories (
	      ../server-thread
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	             ../server-thread/tcp-server.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	             ../server-thread/tcp-server.cpp
	   )
	   add_definitions (-DDATA_STREAMER)
	endif (DATA_STREAMER)

	if (NOT TCP_STREAMER)
	   add_definitions (-DQT_AUDIO)
           find_package(Portaudio)
           if (NOT PORTAUDIO_FOUND)
               message(FATAL_ERROR "please install portaudio V19")
           endif ()
           list(APPEND extraLibs ${PORTAUDIO_LIBRARIES})
	   find_package (Qt6Multimedia REQUIRED)
	   include_directories (
	      ${Qt6Multimedia_INCLUDE_DIRS}
	   )
	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	       ../includes/output/audiosink.h
	       ../includes/output/Qt6/Qt-audio.h
	       ../includes/output/Qt6/Qt-audiodevice.h
	   )

	   set (${objectName}_MOCS
	       ${${objectName}_MOCS}
	       ../includes/output/audiosink.h
	       ../includes/output/Qt6/Qt-audio.h
	       ../includes/output/Qt6/Qt-audiodevice.h
	   )

	   set (${objectName}_SRCS
	      ${${objectName}_SRCS}
	      ../src/output/audiosink.cpp
	     ../src/output/Qt6/Qt-audio.cpp
	     ../src/output/Qt6/Qt-audiodevice.cpp
	   )
	endif (NOT TCP_STREAMER)

	QT6_WRAP_UI (UIS ${${objectName}_UIS}
	             ../qt-devices/filereaders/filereader-widget.ui)

	include_directories (
	          ${SDRPLAY_INCLUDES}
	          ${QT6Widgets_INCLUDES}
	          ${QT_QTCORE_INCLUDE_DIR}
	          ${QT_QTGUI_INCLUDE_DIR}
	          ${PORTAUDIO_INCLUDE_DIRS}
	          ${FAAD_INCLUDE_DIRS}
	)

	QT6_WRAP_CPP (MOCS ${${objectName}_MOCS})

	add_executable (${objectName}
	                ${${objectName}_SRCS}
	                ${UIS}
	                ${RSCS}
	                ${TRS}
	                ${MOCS}
	                resources.qrc
	)

	target_link_libraries (${objectName}
	                       PRIVATE
	                       Qt6::Widgets
	                       Qt6::Network
	                       Qt6::Xml
	                       Qt6::Multimedia
	                       ${RTLTCP_lib}
	                       ${extraLibs}
	                       ${FAAD_LIBRARIES}
	                       ${CMAKE_DL_LIBS}
	)

	add_subdirectory (../qt-devices /tmp)
	target_link_libraries (${objectName} PRIVATE ${objectName}_devices)

	INSTALL (TARGETS ${objectName} DESTINATION  ${CMAKE_INSTALL_PREFIX})
#	INSTALL (TARGETS ${objectName} DESTINATION ./linux-bin)

########################################################################
# Create uninstall target
########################################################################

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
