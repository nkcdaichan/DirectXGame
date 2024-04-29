#pragma once
#include "Vector3D.h"
#include "Mesh.h"

class Projectile {
public:
	Projectile(const Vector3D& position, const Vector3D& direction, float speed);
	~Projectile();

	void update(float deltaTime);
	void render();

private:
	Vector3D m_position;
	Vector3D m_direction;
	float m_speed;
	MeshPtr m_mesh;
};
