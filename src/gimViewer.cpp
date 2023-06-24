#include "gimViewer.h"

//#include "ccViewerApplication.h"

//Qt
#include <QMessageBox>

//qCC_glWindow
#include "gimGLWindow.h"

//common dialogs
//#include <ccCameraParamEditDlg.h>
//#include <ccDisplayOptionsDlg.h>
//#include <ccStereoModeDlg.h>
//
////qCC_db
//#include <ccGenericMesh.h>
//#include <ccHObjectCaster.h>
//#include <ccPointCloud.h>
//
////plugins
//#include "ccGLPluginInterface.h"
//#include "ccIOPluginInterface.h"
//#include "ccPluginManager.h"


//Camera parameters dialog
//static ccCameraParamEditDlg* s_cpeDlg = nullptr;

GimViewer::GimViewer(QWidget* parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
	, m_glWindow(nullptr)
	//, m_selectedObject(nullptr)
	//, m_3dMouseInput(nullptr)
{
	ui.setupUi(this);

#ifdef Q_OS_LINUX
	//we reset the whole stylesheet but we keep the StatusBar style
	setStyleSheet(QString());
	setStyleSheet("QStatusBar{background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 rgb(200,200,200), stop:1 rgb(255,255,255));}");
#endif

	//setWindowTitle(QString("GimViewer v%1").arg(ccApp->versionLongStr(false)));

	//insert GL window in a vertical layout
	{
		QVBoxLayout* verticalLayout = new QVBoxLayout(ui.GLframe);
		verticalLayout->setSpacing(0);
		const int margin = 10;
		verticalLayout->setContentsMargins(margin, margin, margin, margin);

		bool stereoMode = QSurfaceFormat::defaultFormat().stereo();

		QWidget* glWidget = nullptr;
		CreateGLWindow(m_glWindow, glWidget, stereoMode);
		assert(m_glWindow && glWidget);

		verticalLayout->addWidget(glWidget);
	}

	updateGLFrameGradient();

	//m_glWindow->setRectangularPickingAllowed(false); //multiple entities picking not supported

	//UI/display synchronization
	ui.actionFullScreen->setChecked(false);
	ui.menuSelected->setEnabled(false);
	reflectLightsState();
	reflectPerspectiveState();
	reflectPivotVisibilityState();

#ifdef CC_3DXWARE_SUPPORT
	enable3DMouse(true);
#else
	ui.actionEnable3DMouse->setEnabled(false);
#endif

	//Signals & slots connection
	connect(m_glWindow, &GimGLWindow::filesDropped, this, qOverload<QStringList>(&GimViewer::addToDB), Qt::QueuedConnection);
	//connect(m_glWindow, &GimGLWindow::entitySelectionChanged, this, &GimViewer::selectEntity);
	connect(m_glWindow, &GimGLWindow::exclusiveFullScreenToggled, this, &GimViewer::onExclusiveFullScreenToggled);

	//"Options" menu
	connect(ui.actionDisplayParameters, &QAction::triggered, this, &GimViewer::showDisplayParameters);
	connect(ui.actionEditCamera, &QAction::triggered, this, &GimViewer::doActionEditCamera);
	//"Display > Standard views" menu
	connect(ui.actionSetViewTop, &QAction::triggered, this, &GimViewer::setTopView);
	connect(ui.actionSetViewBottom, &QAction::triggered, this, &GimViewer::setBottomView);
	connect(ui.actionSetViewFront, &QAction::triggered, this, &GimViewer::setFrontView);
	connect(ui.actionSetViewBack, &QAction::triggered, this, &GimViewer::setBackView);
	connect(ui.actionSetViewLeft, &QAction::triggered, this, &GimViewer::setLeftView);
	connect(ui.actionSetViewRight, &QAction::triggered, this, &GimViewer::setRightView);
	connect(ui.actionSetViewIso1, &QAction::triggered, this, &GimViewer::setIsoView1);
	connect(ui.actionSetViewIso2, &QAction::triggered, this, &GimViewer::setIsoView2);

	//"Options > Perspective" menu
	connect(ui.actionSetOrthoView, &QAction::triggered, this, &GimViewer::setOrthoView);
	connect(ui.actionSetCenteredPerspectiveView, &QAction::triggered, this, &GimViewer::setCenteredPerspectiveView);
	connect(ui.actionSetViewerPerspectiveView, &QAction::triggered, this, &GimViewer::setViewerPerspectiveView);
	//"Options > Rotation symbol" menu
	connect(ui.actionSetPivotAlwaysOn, &QAction::triggered, this, &GimViewer::setPivotAlwaysOn);
	connect(ui.actionSetPivotRotationOnly, &QAction::triggered, this, &GimViewer::setPivotRotationOnly);
	connect(ui.actionSetPivotOff, &QAction::triggered, this, &GimViewer::setPivotOff);
	//"Options > 3D mouse" menu
	connect(ui.actionEnable3DMouse, &QAction::toggled, this, &GimViewer::enable3DMouse);
	//"Display > Lights & Materials" menu
	connect(ui.actionToggleSunLight, &QAction::toggled, this, &GimViewer::toggleSunLight);
	connect(ui.actionToggleCustomLight, &QAction::toggled, this, &GimViewer::toggleCustomLight);
	//"Options" menu
	connect(ui.actionGlobalZoom, &QAction::triggered, this, &GimViewer::setGlobalZoom);
	connect(ui.actionEnableStereo, &QAction::toggled, this, &GimViewer::toggleStereoMode);
	connect(ui.actionFullScreen, &QAction::toggled, this, &GimViewer::toggleFullScreen);
	connect(ui.actionLockRotationVertAxis, &QAction::triggered, this, &GimViewer::toggleRotationAboutVertAxis);

	//"Options > Selected" menu
	connect(ui.actionShowColors, &QAction::toggled, this, &GimViewer::toggleColorsShown);
	connect(ui.actionShowNormals, &QAction::toggled, this, &GimViewer::toggleNormalsShown);
	connect(ui.actionShowMaterials, &QAction::toggled, this, &GimViewer::toggleMaterialsShown);
	connect(ui.actionShowScalarField, &QAction::toggled, this, &GimViewer::toggleScalarShown);
	connect(ui.actionShowColorRamp, &QAction::toggled, this, &GimViewer::toggleColorbarShown);
	connect(ui.actionZoomOnSelectedEntity, &QAction::triggered, this, &GimViewer::zoomOnSelectedEntity);
	connect(ui.actionDelete, &QAction::triggered, this, &GimViewer::doActionDeleteSelectedEntity);


	//"Shaders" menu
	connect(ui.actionNoFilter, &QAction::triggered, this, &GimViewer::doDisableGLFilter);

	//"Help" menu
	connect(ui.actionAbout, &QAction::triggered, this, &GimViewer::doActionAbout);
	connect(ui.actionHelpShortcuts, &QAction::triggered, this, &GimViewer::doActionDisplayShortcuts);

	loadPlugins();
}

GimViewer::~GimViewer()
{
	release3DMouse();

	//if (s_cpeDlg)
	//{
	//	delete s_cpeDlg;
	//	s_cpeDlg = nullptr;
	//}

	//ccHObject* currentRoot = m_glWindow->getSceneDB();
	//if (currentRoot)
	//{
	//	m_glWindow->setSceneDB(nullptr);
	//	//m_glWindow->redraw();
	//	delete currentRoot;
	//}
#ifdef CC_GL_WINDOW_USE_QWINDOW
	m_glWindow->setParent(0);
#endif
}

void GimViewer::loadPlugins()
{
}

void GimViewer::doDisableGLFilter()
{
}

void GimViewer::doEnableGLFilter()
{
}

void GimViewer::doActionDeleteSelectedEntity()
{
}

bool GimViewer::checkForLoadedEntities()
{
	return true;
}

void GimViewer::updateDisplay()
{
	updateGLFrameGradient();

	//m_glWindow->redraw();
}

void GimViewer::updateGLFrameGradient()
{
}

void GimViewer::addToDB(QStringList filenames)
{
}


void GimViewer::showDisplayParameters()
{
}

void GimViewer::doActionEditCamera()
{
}

void GimViewer::reflectPerspectiveState()
{
}

bool GimViewer::checkStereoMode()
{
	return true;
}

void GimViewer::setOrthoView()
{
}

void GimViewer::setCenteredPerspectiveView()
{
}

void GimViewer::setViewerPerspectiveView()
{
}

void GimViewer::reflectPivotVisibilityState()
{
}

void GimViewer::setPivotAlwaysOn()
{
}

void GimViewer::setPivotRotationOnly()
{
}

void GimViewer::setPivotOff()
{
}

void GimViewer::reflectLightsState()
{
}

void GimViewer::toggleSunLight(bool state)
{
}

void GimViewer::toggleCustomLight(bool state)
{
}

void GimViewer::toggleStereoMode(bool state)
{
}

void GimViewer::toggleFullScreen(bool state)
{
}

void GimViewer::onExclusiveFullScreenToggled(bool state)
{
}

void GimViewer::toggleRotationAboutVertAxis()
{
}

void GimViewer::doActionDisplayShortcuts()
{
	QMessageBox msgBox;
	QString text;
	text += "Shortcuts:\n\n";
	text += "F2 : Set orthographic view\n";
	text += "F3 : Set object-centered perspective\n";
	text += "F4 : Set viewer-based perspective\n";
	text += "F6 : Toggle sun light\n";
	text += "F7 : Toggle custom light\n";
	text += "F8 : Toggle Console display\n";
	text += "F9 : Toggle full screen\n";
	text += "F11: Toggle exclusive full screen\n";
	text += "Z  : Zoom on selected entity\n";
	text += "L  : Lock rotation around Z\n";
	text += "B  : Enter/leave bubble view mode\n";
	text += "DEL: Delete selected entity\n";
	text += "+  : Zoom in\n";
	text += "-  : Zoom out\n";
	text += "\n";
	text += "Shift + C: Toggle color ramp visibility\n";
	text += "Shift + up arrow: activate previous SF\n";
	text += "Shift + down arrow: activate next SF\n";
	text += "\n";
	text += "Ctrl + D: Display parameters\n";
	text += "Ctrl + C: Camera parameters\n";
	text += "\n";
	text += "Left click: Select entity\n";
	//text += "Ctrl + left click: Select multiple entities (toggle)\n";
	//text += "Alt + left button hold: Select multiple entities (rectangular area)\n";
	text += "Shift + left click (on a point/triangle): spawn a label\n";
	text += "Right click (on a label): expand/collapse\n";
	msgBox.setText(text);
	msgBox.exec();
}

void GimViewer::setTopView()
{
	m_glWindow->setView(CC_TOP_VIEW);
}

void GimViewer::setBottomView()
{
	m_glWindow->setView(CC_BOTTOM_VIEW);
}

void GimViewer::setFrontView()
{
	m_glWindow->setView(CC_FRONT_VIEW);
}

void GimViewer::setBackView()
{
	m_glWindow->setView(CC_BACK_VIEW);
}

void GimViewer::setLeftView()
{
	m_glWindow->setView(CC_LEFT_VIEW);
}

void GimViewer::setRightView()
{
	m_glWindow->setView(CC_RIGHT_VIEW);
}

void GimViewer::setIsoView1()
{
	m_glWindow->setView(CC_ISO_VIEW_1);
}

void GimViewer::setIsoView2()
{
	m_glWindow->setView(CC_ISO_VIEW_2);
}

void GimViewer::toggleColorsShown(bool state)
{
}

void GimViewer::toggleNormalsShown(bool state)
{
}

void GimViewer::toggleMaterialsShown(bool state)
{
}

void GimViewer::toggleScalarShown(bool state)
{
}

void GimViewer::toggleColorbarShown(bool state)
{
}

void GimViewer::changeCurrentScalarField(bool state)
{
}

void GimViewer::setGlobalZoom()
{
}

void GimViewer::zoomOnSelectedEntity()
{
}

void GimViewer::doActionAbout()
{
}

/*** 3D MOUSE SUPPORT ***/

void GimViewer::release3DMouse()
{
#ifdef CC_3DXWARE_SUPPORT
	if (m_3dMouseInput)
	{
		m_3dMouseInput->disconnect(); //disconnect from the driver
		disconnect(m_3dMouseInput); //disconnect from Qt ;)

		delete m_3dMouseInput;
		m_3dMouseInput = 0;
	}
#endif
}

void GimViewer::enable3DMouse(bool state)
{
#ifdef CC_3DXWARE_SUPPORT
	if (m_3dMouseInput)
		release3DMouse();

	if (state)
	{
		m_3dMouseInput = new Mouse3DInput(this);
		if (m_3dMouseInput->connect(this, "GimViewer"))
		{
			QObject::connect(m_3dMouseInput, &Mouse3DInput::sigMove3d, this, &GimViewer::on3DMouseMove);
			QObject::connect(m_3dMouseInput, &Mouse3DInput::sigReleased, this, &GimViewer::on3DMouseReleased);
			QObject::connect(m_3dMouseInput, &Mouse3DInput::sigOn3dmouseKeyDown, this, &GimViewer::on3DMouseKeyDown);
			QObject::connect(m_3dMouseInput, &Mouse3DInput::sigOn3dmouseKeyUp, this, &GimViewer::on3DMouseKeyUp);
			QObject::connect(m_3dMouseInput, &Mouse3DInput::sigOn3dmouseCMDKeyDown, this, &GimViewer::on3DMouseCMDKeyDown);
			QObject::connect(m_3dMouseInput, &Mouse3DInput::sigOn3dmouseCMDKeyUp, this, &GimViewer::on3DMouseCMDKeyUp);
		}
		else
		{
			delete m_3dMouseInput;
			m_3dMouseInput = 0;

			ccLog::Warning("[3D Mouse] No device found");
			state = false;
		}
	}
	else
	{
		ccLog::Warning("[3D Mouse] Device has been disabled");
	}
#else
	state = false;
#endif

	ui.actionEnable3DMouse->blockSignals(true);
	ui.actionEnable3DMouse->setChecked(state);
	ui.actionEnable3DMouse->blockSignals(false);
}

void GimViewer::on3DMouseKeyUp(int)
{
	//nothing right now
}

// ANY CHANGE/BUG FIX SHOULD BE REFLECTED TO THE EQUIVALENT METHODS IN QCC "MainWindow.cpp" FILE!
void GimViewer::on3DMouseKeyDown(int key)
{
#ifdef CC_3DXWARE_SUPPORT

	switch (key)
	{
	case Mouse3DInput::V3DK_MENU:
		//should be handled by the driver now!
		break;
	case Mouse3DInput::V3DK_FIT:
	{
		if (m_selectedObject)
			zoomOnSelectedEntity();
		else
			setGlobalZoom();
	}
	break;
	case Mouse3DInput::V3DK_TOP:
		setTopView();
		break;
	case Mouse3DInput::V3DK_LEFT:
		setLeftView();
		break;
	case Mouse3DInput::V3DK_RIGHT:
		setRightView();
		break;
	case Mouse3DInput::V3DK_FRONT:
		setFrontView();
		break;
	case Mouse3DInput::V3DK_BOTTOM:
		setBottomView();
		break;
	case Mouse3DInput::V3DK_BACK:
		setBackView();
		break;
	case Mouse3DInput::V3DK_ROTATE:
		//should be handled by the driver now!
		break;
	case Mouse3DInput::V3DK_PANZOOM:
		//should be handled by the driver now!
		break;
	case Mouse3DInput::V3DK_ISO1:
		setIsoView1();
		break;
	case Mouse3DInput::V3DK_ISO2:
		setIsoView2();
		break;
	case Mouse3DInput::V3DK_PLUS:
		//should be handled by the driver now!
		break;
	case Mouse3DInput::V3DK_MINUS:
		//should be handled by the driver now!
		break;
	case Mouse3DInput::V3DK_DOMINANT:
		//should be handled by the driver now!
		break;
	case Mouse3DInput::V3DK_CW:
	case Mouse3DInput::V3DK_CCW:
	{
		if (m_glWindow)
		{
			CCVector3d axis(0, 0, -1);
			CCVector3d trans(0, 0, 0);
			ccGLMatrixd mat;
			double angle = M_PI / 2;
			if (key == Mouse3DInput::V3DK_CCW)
				angle = -angle;
			mat.initFromParameters(angle, axis, trans);
			m_glWindow->rotateBaseViewMat(mat);
			m_glWindow->redraw();
		}
	}
	break;
	case Mouse3DInput::V3DK_ESC:
	case Mouse3DInput::V3DK_ALT:
	case Mouse3DInput::V3DK_SHIFT:
	case Mouse3DInput::V3DK_CTRL:
	default:
		ccLog::Warning("[3D mouse] This button is not handled (yet)");
		//TODO
		break;
	}

#endif
}
void GimViewer::on3DMouseCMDKeyUp(int cmd)
{
	//nothing right now
}

void GimViewer::on3DMouseCMDKeyDown(int cmd)
{
#ifdef CC_3DXWARE_SUPPORT
	switch (cmd)
	{
		//ccLog::Print(QString("on3DMouseCMDKeyDown Cmd = %1").arg(cmd));
	case Mouse3DInput::V3DCMD_VIEW_FIT:
	{
		if (m_selectedObject)
			zoomOnSelectedEntity();
		else
			setGlobalZoom();
	}
	break;
	case Mouse3DInput::V3DCMD_VIEW_TOP:
		setTopView();
		break;
	case Mouse3DInput::V3DCMD_VIEW_LEFT:
		setLeftView();
		break;
	case Mouse3DInput::V3DCMD_VIEW_RIGHT:
		setRightView();
		break;
	case Mouse3DInput::V3DCMD_VIEW_FRONT:
		setFrontView();
		break;
	case Mouse3DInput::V3DCMD_VIEW_BOTTOM:
		setBottomView();
		break;
	case Mouse3DInput::V3DCMD_VIEW_BACK:
		setBackView();
		break;
	case Mouse3DInput::V3DCMD_VIEW_ISO1:
		setIsoView1();
		break;
	case Mouse3DInput::V3DCMD_VIEW_ISO2:
		setIsoView2();
		break;
	case Mouse3DInput::V3DCMD_VIEW_ROLLCW:
	case Mouse3DInput::V3DCMD_VIEW_ROLLCCW:
	{
		if (m_glWindow)
		{
			CCVector3d axis(0, 0, -1);
			CCVector3d trans(0, 0, 0);
			ccGLMatrixd mat;
			double angle = M_PI / 2;
			if (cmd == Mouse3DInput::V3DCMD_VIEW_ROLLCCW)
				angle = -angle;
			mat.initFromParameters(angle, axis, trans);
			m_glWindow->rotateBaseViewMat(mat);
			m_glWindow->redraw();
		}
	}
	break;
	case Mouse3DInput::V3DCMD_VIEW_SPINCW:
	case Mouse3DInput::V3DCMD_VIEW_SPINCCW:
	{
		if (m_glWindow)
		{
			CCVector3d axis(0, 1, 0);
			CCVector3d trans(0, 0, 0);
			ccGLMatrixd mat;
			double angle = M_PI / 2;
			if (cmd == Mouse3DInput::V3DCMD_VIEW_SPINCCW)
				angle = -angle;
			mat.initFromParameters(angle, axis, trans);
			m_glWindow->rotateBaseViewMat(mat);
			m_glWindow->redraw();
		}
	}
	case Mouse3DInput::V3DCMD_VIEW_TILTCW:
	case Mouse3DInput::V3DCMD_VIEW_TILTCCW:
	{
		if (m_glWindow)
		{
			CCVector3d axis(1, 0, 0);
			CCVector3d trans(0, 0, 0);
			ccGLMatrixd mat;
			double angle = M_PI / 2;
			if (cmd == Mouse3DInput::V3DCMD_VIEW_TILTCCW)
				angle = -angle;
			mat.initFromParameters(angle, axis, trans);
			m_glWindow->rotateBaseViewMat(mat);
			m_glWindow->redraw();
		}
	}
	break;
	default:
		ccLog::Warning("[3D mouse] This button is not handled (yet)");
		//TODO
		break;
	}
#endif
}

void GimViewer::on3DMouseMove(std::vector<float>& vec)
{
#ifdef CC_3DXWARE_SUPPORT
	if (m_glWindow)
		Mouse3DInput::Apply(vec, m_glWindow);
#endif
}

void GimViewer::on3DMouseReleased()
{
	////active window?
	//if (m_glWindow && m_glWindow->getPivotVisibility() == GimGLWindow::PIVOT_SHOW_ON_MOVE)
	//{
	//	//we have to hide the pivot symbol!
	//	m_glWindow->showPivotSymbol(false);
	//	m_glWindow->redraw();
	//}
}
