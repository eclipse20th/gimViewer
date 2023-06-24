//##########################################################################
//#                                                                        #
//#                              CLOUDCOMPARE                              #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#          COPYRIGHT: CloudCompare project                               #
//#                                                                        #
//##########################################################################

#include <QtGlobal>
#include <QDate>

#ifdef Q_OS_MAC
#include <QFileOpenEvent>
#endif

#include "gimViewer.h"
#include "ccViewerApplication.h"


ccViewerApplication::ccViewerApplication( int &argc, char **argv, bool isCommandLine )
	: ccApplicationBase( argc, argv, isCommandLine, QString( "1.41.alpha (%1)" ).arg(QDate::currentDate().toString("MM-dd-yyyy")))
{
	setApplicationName( "CloudCompareViewer" );
}

void ccViewerApplication::setViewer(gimViewer*inViewer)
{
	mViewer = inViewer;
}

bool ccViewerApplication::event(QEvent *inEvent)
{
#ifdef Q_OS_MAC
	switch ( inEvent->type() )
	{
		case QEvent::FileOpen:
		{			
			if ( mViewer == nullptr )
			{
				return false;
			}
			
			mViewer->addToDB( { static_cast<QFileOpenEvent *>(inEvent)->file() } );
			return true;
		}
			
		default:
			break;
	}
#endif

	return ccApplicationBase::event( inEvent );
}