#include "BOSS.h"
#include "../Player/Player.h"



void BOSS::Initialize(Matrix4x4 viewMatrix) {

}

void BOSS::Update(Player* player, Matrix4x4 viewMatrix) {

}


void BOSS::OnCollision(const Bullet* bullet) {
	(bullet);
	HP_--;
}
