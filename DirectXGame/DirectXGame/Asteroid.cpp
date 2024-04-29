#include "Asteroid.h"

Asteroid::Asteroid() {
	// Asteroid の初期化処理
	// ここでは Rect の初期化などを行う
	m_isDestroyed = false;
}

Asteroid::~Asteroid() {
	// Asteroid の終了処理
	// ここでは必要ならばリソースの解放などを行う
}

void Asteroid::move() {
	// Asteroid の移動処理
	// ここでは Rect を移動させるなどの処理を行う
}

bool Asteroid::checkCollision(const Rect& spaceshipRect) const {
	// Spaceship との衝突判定を行う
	// ここでは Asteroid と Spaceship の Rect が重なっているかどうかを判定する
	// 重なっていれば true を返し、そうでなければ false を返す
	return m_rect.left < spaceshipRect.left + spaceshipRect.width &&
		m_rect.left + m_rect.width > spaceshipRect.left &&
		m_rect.top < spaceshipRect.top + spaceshipRect.height &&
		m_rect.top + m_rect.height > spaceshipRect.top;
}

void Asteroid::hitSpaceship() {
	// Asteroid が Spaceship に当たった際の処理
	// ここではダメージを与えるなどの処理を行う
	m_isDestroyed = true; // Asteroid を破壊する
}