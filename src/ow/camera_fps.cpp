#include <cmath>
#include <vector>
#include <algorithm>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ow/camera_fps.hpp>
#include <ow/utils.hpp>

ow::camera_fps::camera_fps(glm::vec3 position, glm::vec3 up, float yaw, float pitch) noexcept
	: m_pos(position)
	, m_front(glm::vec3(0.0f, 0.0f, -1.0f))
	, m_up{}
	, m_right{}
	, m_world_up(up)
	, m_yaw(yaw)
	, m_pitch(pitch)
	, m_mov_speed(DEFAULT_SPEED)
	, m_mouse_sensitivity(DEFAULT_SENSITIVITY)
	, m_fov(DEFAULT_FOV) {
	_update_camera_vectors();
}

void ow::camera_fps::process_movement(camera_movement direction, float dt) {
	float velocity = m_mov_speed * dt;
	switch (direction) {
	case FORWARD:
		m_pos += m_front * velocity;
		break;
	case BACKWARD:
		m_pos -= m_front * velocity;
		break;
	case LEFT:
		m_pos -= m_right * velocity;
		break;
	case RIGHT:
		m_pos += m_right * velocity;
		break;
	case UP:
		m_pos += m_up * velocity;
		break;
	case DOWN:
		m_pos -= m_up * velocity;
		break;
	default:
		logger << "Warning: unknown direction catched.\n";
		break;
	}
}

void ow::camera_fps::process_mouse_movement(float xoffset, float yoffset, bool constrain_pitch) {
	xoffset *= m_mouse_sensitivity;
	yoffset *= m_mouse_sensitivity;

	m_yaw   += xoffset;
	m_pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrain_pitch) {
		m_pitch = std::clamp(m_pitch, glm::radians(-89.f), glm::radians(89.f));
	}

	// Update front, right and up Vectors using the updated Euler angles
	_update_camera_vectors();
}

void ow::camera_fps::process_mouse_scroll(float yoffset) {
	m_fov -= yoffset;
	if (m_fov <= 1.0f) {
		m_fov = 1.0f;
	} else if (m_fov >= static_cast<float>(M_PI) / 4.f) {
		m_fov = static_cast<float>(M_PI) / 4.f;
	}
}

void ow::camera_fps::_update_camera_vectors() {
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = std::cos(m_yaw) * std::cos(m_pitch);
	front.y = std::sin(m_pitch);
	front.z = std::sin(m_yaw) * std::cos(m_pitch);

	// Normalize the vectors, because their length gets closer to 0
	// the more you look up or down which results in slower movement.
	m_front = glm::normalize(front);
	m_right = glm::normalize(glm::cross(m_front, m_world_up));
	m_up	= glm::normalize(glm::cross(m_right, m_front));
}

