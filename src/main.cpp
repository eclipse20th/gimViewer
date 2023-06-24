
//Qt
//#include <QDir>
//#include <QGLFormat>

//Local
#include "ccviewerlog.h"

//qCC_db
//#include <ccIncludeGL.h>
//#include <ccNormalVectors.h>
//#include <ccColorScalesManager.h>
//#include <ccMaterial.h>
//
////qCC_io
//#include <FileIOFilter.h>

//#include "ccViewerApplication.h"

#include <QApplication>
#include "gimViewer.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	GimViewer w;
	//a.setViewer(&w);

	//register minimal logger to display errors
	ccViewerLog logger(&w);
	ccLog::RegisterInstance(&logger);

	w.show();

	//w.checkForLoadedEntities();

	int result = a.exec();

	return result;
}
