#include "Projectile.h"
#include "MeshManager.h"
#include "RenderSystem.h"

Projectile::Projectile(const Vector3D& position, const Vector3D& direction, float speed)
	: m_position(position), m_direction(direction), m_speed(speed) {
	// メッシュのロード（例：sphere.obj）
	m_mesh = MeshManager::getInstance()->createMeshFromFile(L"Assets//Meshes//sphere.obj");
	if (!m_mesh) {
		throw std::exception("Failed to load mesh for projectile.");
	}
}

Projectile::~Projectile() {}

void Projectile::update(float deltaTime) {
	// 発射物の移動処理
	m_position = m_position + (m_direction * m_speed * deltaTime);
}

void Projectile::render() {
	// 発射物の描画
	if (m_mesh) {
		// メッシュの描画
		RenderSystem::get()->getImmediateDeviceContext()->drawIndexed(m_mesh->getIndexCount(), 0, 0);
	}
}
