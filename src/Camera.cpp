#include "Camera.h"
#include <QDebug>


Camera::Camera(QVector3D position, QVector3D up, float yaw, float pitch)
	: m_position(position)
	, m_worldUp(up)
	, m_front(-position)
	, m_picth(pitch)
	, m_yaw(yaw)
	, m_movementSpeed(SPEED)
	, m_mouseSensitivity(SENSITIVITY)
	, m_zoom(ZOOM)
{
	updateCameraVectors();

	for (uint i = 0; i != 1024; ++i)
		m_keys[i] = false;
}

Camera::~Camera()
{

}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
QMatrix4x4 Camera::getViewMatrix()
{
	QMatrix4x4 view;
	view.lookAt(m_position, m_position + m_front, m_up);
	return view;
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::processKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = m_movementSpeed * deltaTime;
	if (direction == FORWARD)
		m_position += m_front * velocity;
	if (direction == BACKWARD)
		m_position -= m_front * velocity;
	if (direction == LEFT)
		m_position -= m_right * velocity;
	if (direction == RIGHT)
		m_position += m_right * velocity;
	if (direction == UP)
		m_position += m_worldUp * velocity;
	if (direction == DOWN)
		m_position -= m_worldUp * velocity;
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::processMouseMovement(float xoffset, float yoffset, bool constraintPitch)
{
	xoffset *= m_mouseSensitivity;
	yoffset *= m_mouseSensitivity;

	m_yaw += xoffset;
	m_picth += yoffset;

	if (constraintPitch) {
		if (m_picth > 89.0f)
			m_picth = 89.0f;
		if (m_picth < -89.0f)
			m_picth = -89.0f;
	}

	updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::processMouseScroll(float yoffset)
{
	if (m_zoom >= 1.0f && m_zoom <= 45.0f)
		m_zoom -= yoffset;
	if (m_zoom > 45.0f)
		m_zoom = 45.0f;
	if (m_zoom < 1.0f)
		m_zoom = 1.0f;
}

void Camera::processInput(float dt)
{

	if (m_keys[Qt::Key_W]) processKeyboard(FORWARD, dt);
	if (m_keys[Qt::Key_S]) processKeyboard(BACKWARD, dt);
	if (m_keys[Qt::Key_A]) processKeyboard(LEFT, dt);
	if (m_keys[Qt::Key_D]) processKeyboard(RIGHT, dt);
	if (m_keys[Qt::Key_E]) processKeyboard(UP, dt);
	if (m_keys[Qt::Key_Q]) processKeyboard(DOWN, dt);
}

void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	QVector3D front;
	front.setX(cos(m_yaw) * cos(m_picth));
	front.setY(sin(m_picth));
	front.setZ(sin(m_yaw) * cos(m_picth));
	m_front = front.normalized();
	m_right = QVector3D::crossProduct(m_front, m_worldUp).normalized();
	m_up = QVector3D::crossProduct(m_right, m_front).normalized();
}
