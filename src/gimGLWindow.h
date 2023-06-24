#pragma once

// Qt
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLExtensions>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>

#include <QSurfaceFormat>
#include <QPoint>
#include <QKeyEvent>

// Other
#include "ccLog.h"
#include <memory>

//#include <set>
//#include <unordered_set>
//! View orientation
enum CC_VIEW_ORIENTATION {
	CC_TOP_VIEW,	/**< Top view (eye: +Z) **/
	CC_BOTTOM_VIEW,	/**< Bottom view **/
	CC_FRONT_VIEW,	/**< Front view **/
	CC_BACK_VIEW,	/**< Back view **/
	CC_LEFT_VIEW,	/**< Left view **/
	CC_RIGHT_VIEW,	/**< Right view **/
	CC_ISO_VIEW_1,	/**< Isometric view 1: front, right and top **/
	CC_ISO_VIEW_2,	/**< Isometric view 2: back, left and top **/
};

class Camera;

class GimGLWindow : public QOpenGLWidget ,protected QOpenGLFunctions_4_5_Core
{
	Q_OBJECT
public:
	GimGLWindow(QSurfaceFormat* format = nullptr, QOpenGLWidget* parent = nullptr, bool silentInitialization = false);
	~GimGLWindow();
	virtual void setView(CC_VIEW_ORIENTATION orientation, bool redraw = true);


Q_SIGNALS:
	//! Signal emitted when files are dropped on the window
	void filesDropped(const QStringList& filenames);
	//! Signal emitted when an entity is selected in the 3D view
	//void entitySelectionChanged(ccHObject* entity);
	//! Signal emitted when the exclusive fullscreen is toggled
	void exclusiveFullScreenToggled(bool exclusive);

protected: //other methods

	////these methods are now protected to prevent issues with Retina or other high DPI displays
	////(see glWidth(), glHeight(), qtWidth(), qtHeight(), qtSize(), glSize()
	//int width() const { return ccGLWindowParent::width(); }
	//int height() const { return ccGLWindowParent::height(); }
	//QSize size() const { return ccGLWindowParent::size(); }

	////! Returns the current (OpenGL) view matrix
	///** Warning: may be different from the 'view' matrix returned by getBaseViewMat.
	//	Will call automatically updateModelViewMatrix if necessary.
	//**/
	//virtual const ccGLMatrixd& getModelViewMatrix();
	////! Returns the current (OpenGL) projection matrix
	///** Will call automatically updateProjectionMatrix if necessary.
	//**/
	//virtual const ccGLMatrixd& getProjectionMatrix();

	////! Processes the clickable items
	///** \return true if an item has been clicked
	//**/
	//bool processClickableItems(int x, int y);

	////! Sets current font size
	///** Warning: only used internally.
	//	Change 'defaultFontSize' with setDisplayParameters instead!
	//**/
	//void setFontPointSize(int pixelSize);

	////events handling
	//void mousePressEvent(QMouseEvent* event) override;
	//void mouseMoveEvent(QMouseEvent* event) override;
	//void mouseDoubleClickEvent(QMouseEvent* event) override;
	//void mouseReleaseEvent(QMouseEvent* event) override;
	//void wheelEvent(QWheelEvent* event) override;
	//bool event(QEvent* evt) override;

	bool initialize();
	//GLuint defaultQtFBO() const;

protected:
	virtual void initializeGL() override;
	virtual void resizeGL(int w, int h) override;
	virtual void paintGL() override;

	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

private:
	bool createShader();
	uint loadTexture(const QString& path);


protected: //members

	//! Unique ID
	int m_uniqueID;

	//! Initialization state
	bool m_initialized;

public:
	bool m_silentInitialization;

	//Default OpenGL functions set
	using gimQOpenGLFunctions = QOpenGLFunctions_4_5_Core;

	//! Returns the set of OpenGL functions
	inline gimQOpenGLFunctions* functions() const { return context() ? context()->versionFunctions<gimQOpenGLFunctions>() : nullptr; }

	//gimQOpenGLFunctions* m_glFunctions;

	QOpenGLExtension_ARB_framebuffer_object	m_glExtFunc;

	bool m_glExtFuncSupported;

private:
	QOpenGLShaderProgram lightingShader, lampShader;

	QTimer* m_pTimer = nullptr;
	int     m_nTimeValue = 0;

	uint VBO, cubeVAO, lightVAO;
	uint diffuseMap, specularMap;

	// camera
	std::unique_ptr<Camera> m_camera;
	bool m_bLeftPressed;
	QPoint m_lastPos;

};

inline void CreateGLWindow(GimGLWindow*& window, QWidget*& widget, bool stereoMode = false, bool silentInitialization = false)
{
	QSurfaceFormat format = QSurfaceFormat::defaultFormat();
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	format.setStereo(stereoMode);

	window = new GimGLWindow(&format, nullptr, silentInitialization);

#ifdef CC_GL_WINDOW_USE_QWINDOW
	widget = new GimGLWindow(window);
#else
	widget = window;
#endif
}
