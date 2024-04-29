#pragma once
#include "Rect.h"

class Asteroid {
public:
	Asteroid(); // コンストラクタ
	~Asteroid(); // デストラクタ

	// その他のメンバ関数やメンバ変数を追加する
	void move(); // 移動関数
	Rect getRect() const { return m_rect; } // Asteroid の Rect を取得する関数

	// Spaceship との衝突判定を行う関数
	bool checkCollision(const Rect& spaceshipRect) const;

	// Asteroid が Spaceship に当たった際にダメージを与える関数
	void hitSpaceship();

	// Asteroid が破壊されたかどうかを返す関数
	bool isDestroyed() const { return m_isDestroyed; }
private:
	// Asteroid クラスのメンバ変数
	Rect m_rect; // Asteroid の Rect
	bool m_isDestroyed; // Asteroid が破壊されたかどうかを示すフラグ
};