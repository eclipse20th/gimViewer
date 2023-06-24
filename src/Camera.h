#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>
#include <QMatrix4x4>
#include <QKeyEvent>

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 0.01f;
const float SENSITIVITY = 0.01f;
const float ZOOM = 45.0f;

class Camera
{
public:
	Camera(
		QVector3D position = QVector3D(0.0f, 0.0f, 0.0f), 
		QVector3D up = QVector3D(0.0f, 1.0f, 0.0f),
		float yaw = YAW, 
		float pitch = PITCH
	);
	~Camera();

	QMatrix4x4 getViewMatrix();
	void processMouseMovement(float xoffset, float yoffset, bool constraintPitch = true);
	void processMouseScroll(float yoffset);
	void processInput(float dt);

	QVector3D m_position;
	QVector3D m_worldUp;
	QVector3D m_front;

	QVector3D m_up;
	QVector3D m_right;

	//Eular Angles
	float m_picth;
	float m_yaw;

	//Camera options
	float m_movementSpeed;
	float m_mouseSensitivity;
	float m_zoom;

	//Keyboard multi-touch
	bool m_keys[1024];
private:
	void updateCameraVectors();
	void processKeyboard(Camera_Movement direction, float deltaTime);
};

#endif // CAMERA_H
