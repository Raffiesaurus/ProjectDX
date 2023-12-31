#include "pch.h"
#include "Camera.h"

//camera for our app simple directX application. While it performs some basic functionality its incomplete. 
//

Camera::Camera() {
	//initalise values. 
	//Orientation and Position are how we control the camera. 
	m_orientation.x = 0.0f;		//rotation around x - pitch
	m_orientation.y = 0.0f;		//rotation around y - yaw
	m_orientation.z = 0.0f;		//rotation around z - roll	//we tend to not use roll a lot in first person

	m_position.x = 0.0f;		//camera position in space. 
	m_position.y = 0.0f;
	m_position.z = 0.0f;

	//These variables are used for internal calculations and not set.  but we may want to queary what they 
	//externally at points
	m_lookat.x = 0.0f;		//Look target point
	m_lookat.y = 0.0f;
	m_lookat.z = 0.0f;

	m_forward.x = 90.0f;		//forward/look direction
	m_forward.y = 0.0f;
	m_forward.z = 0.0f;

	m_right.x = 0.0f;
	m_right.y = 0.0f;
	m_right.z = 0.0f;

	//
	m_movespeed = 1.50;
	m_camRotRate = 15.0;

	//force update with initial values to generate other camera data correctly for first update. 
	Update();
}


Camera::~Camera() {}

void Camera::Update() {
	
	if (m_position.y != 0.75) {
		m_position.y = 0.75;
	}
	float r = 1.0f;
	float radianY = DirectX::XMConvertToRadians(m_orientation.y);
	float radianZ = DirectX::XMConvertToRadians(m_orientation.z);
	m_forward.x = r * sin(radianY) * cos(radianZ);
	m_forward.y = r * sin(radianZ);
	m_forward.z = r * cos(radianY) * cos(radianZ);
	m_forward.Normalize();
	m_forward.Cross(DirectX::SimpleMath::Vector3::UnitY, m_right);
	m_lookat = m_position + m_forward;
	m_cameraMatrix = (DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_lookat, DirectX::SimpleMath::Vector3::UnitY));
}

DirectX::SimpleMath::Matrix Camera::getCameraMatrix() {
	return m_cameraMatrix;
}

void Camera::setPosition(DirectX::SimpleMath::Vector3 newPosition) {
	if (m_position.y <= 0.5) {
		m_position.y = 0.5;
	}
	m_position = newPosition;
}

DirectX::SimpleMath::Vector3 Camera::getPosition() {
	return m_position;
}

DirectX::SimpleMath::Vector3 Camera::getForward() {
	return m_forward;
}

DirectX::SimpleMath::Vector3 Camera::getRight() {
	return m_right;
}

void Camera::setRotation(DirectX::SimpleMath::Vector3 newRotation) {
	m_orientation = newRotation;
}

DirectX::SimpleMath::Vector3 Camera::getRotation() {
	return m_orientation;
}

float Camera::getMoveSpeed() {
	return m_movespeed;
}

float Camera::getRotationSpeed() {
	return m_camRotRate;
}
