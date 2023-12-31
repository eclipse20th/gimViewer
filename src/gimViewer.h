﻿#ifndef CCVIEWER_H
#define CCVIEWER_H

//Qt
#include <QMainWindow>
#include <QStringList>

//GUIs
#include <ui_gimViewer.h>
//#include <ui_ccviewerAbout.h>

//System
#include <set>

class GimGLWindow;
//class ccHObject;
//class Mouse3DInput;

//! Application main window
class GimViewer : public QMainWindow
{
	Q_OBJECT

public:
	//! Default constructor
	GimViewer(QWidget* parent = 0, Qt::WindowFlags flags = 0);

	//! Default destructor
	~GimViewer();

	//! Adds entity to display db
	//void addToDB(ccHObject* entity);

	//! Checks for loaded entities
	/** If none, a message is displayed to invite the user
		to drag & drop files.
	**/
	bool checkForLoadedEntities();

public:

	//! Tries to load (and then adds to main db) a list of entity (files)
	/** \param filenames filenames to load
	**/
	void addToDB(QStringList filenames);

protected:

	//! Shows display parameters dialog
	void showDisplayParameters();

	//! Updates display to match display parameters
	void updateDisplay();

	//! Selects entity
	//void selectEntity(ccHObject* entity);

	//! Delete selected entity
	void doActionDeleteSelectedEntity();

	//! Slot called when the exclusive full screen mode is called
	void onExclusiveFullScreenToggled(bool);

	void doActionEditCamera();
	void toggleSunLight(bool);
	void toggleCustomLight(bool);
	void toggleStereoMode(bool);
	void toggleFullScreen(bool);
	void toggleRotationAboutVertAxis();
	void doActionAbout();
	void doActionDisplayShortcuts();
	void setPivotAlwaysOn();
	void setPivotRotationOnly();
	void setPivotOff();
	void setOrthoView();
	void setCenteredPerspectiveView();
	void setViewerPerspectiveView();
	void setGlobalZoom();
	void zoomOnSelectedEntity();

	//default views
	void setFrontView();
	void setBottomView();
	void setTopView();
	void setBackView();
	void setLeftView();
	void setRightView();
	void setIsoView1();
	void setIsoView2();

	//selected entity properties
	void toggleColorsShown(bool);
	void toggleNormalsShown(bool);
	void toggleMaterialsShown(bool);
	void toggleScalarShown(bool);
	void toggleColorbarShown(bool);
	void changeCurrentScalarField(bool);

	//3D mouse
	void on3DMouseMove(std::vector<float>&);
	void on3DMouseKeyUp(int);
	void on3DMouseKeyDown(int);
	void on3DMouseCMDKeyDown(int);
	void on3DMouseCMDKeyUp(int);
	void on3DMouseReleased();
	void enable3DMouse(bool state);

	//GL filters
	void doEnableGLFilter();
	void doDisableGLFilter();

protected: //methods

	//! Loads plugins (from files)
	void loadPlugins();

	//! Makes the GL frame background gradient match the OpenGL window one
	void updateGLFrameGradient();

	//! Updates perspective UI elements
	void reflectPerspectiveState();

	//! Updates pivot UI elements
	void reflectPivotVisibilityState();

	//! Updates lights UI elements
	void reflectLightsState();

	//! Checks whether stereo mode can be stopped (if necessary) or not
	bool checkStereoMode();

protected: //members

	//! Releases any connected 3D mouse (if any)
	void release3DMouse();

	//! Associated GL context
	GimGLWindow* m_glWindow;

	//! Currently selected object
	//ccHObject* m_selectedObject;

	//! 3D mouse handler
	//Mouse3DInput* m_3dMouseInput;

private:
	//! Associated GUI
	Ui::GimViewerClass ui;
};

#endif // CCVIEWER_H
