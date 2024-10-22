#include <Novice.h>
#include <Vector2.h>
#include "Easing.h"
#include <math.h>
#define _USE_MATH_DEFINES
#include <XInput.h>
#include <cmath>
#include "Blend.h"

const char kWindowTitle[] = "5103_このハナの一枝の内には尽きることのない言葉がこもっているから、生半可な気持ちと思わないで。";

struct Player {
	Vector2 pos;
	Vector2 targetPos;
	float radius;
	Vector2 checkPointPos;
	bool isAlive;
	int respawCoolTime;
	int transparency;
	bool isBlinking;
	int blinkingTime;
	int th;
	int screenPosX;
	bool isAnimation;
	int animeCount;
};

struct Particle {
	Vector2 pos;
	Vector2 velocity;
	float radius;
	int lifeTime;
	bool isActive;
	float baseAngle;
	float randomAngle;
	float speed;
};

// 敵の構造体
struct Enemy {
	Vector2 pos;
	int enemyType; // 敵の種類 (0: 円, 1: 縦方向, 2: 横方向)
	float theta;
	float amplitude;
	float radius;
};

struct Cursor {
	Vector2 pos;
};

struct Camera {
	Vector2 pos;
	Vector2 targetPos;
};

struct CheckPoint {
	Vector2 pos;
};

bool isCollision(Vector2 aPos, Vector2 bPos, float aRadius, float bRadius) {
	float dx = aPos.x - bPos.x;
	float dy = aPos.y - bPos.y;
	float distance = sqrtf(dx * dx + dy * dy);
	return distance < (aRadius + bRadius);
}

Vector2 circleEnemy(Vector2 pos, float amplitude, float &theta, float velocity) {
	Vector2 result;

	theta += float(M_PI) / (velocity * 100.0f);

	result.x = cosf(theta) * amplitude + pos.x;
	result.y = sinf(theta) * amplitude + pos.y;

	return result;
}


Vector2 verticalEnemy(Vector2 pos, float amplitude, float &theta, float velocity) {
	Vector2 result;

	theta += float(M_PI) / (velocity * 100.0f);

	result.x = pos.x;
	result.y = sinf(theta) * amplitude + pos.y;

	return result;
}

Vector2 horizonEnemy(Vector2 pos, float amplitude, float &theta, float velocity) {
	Vector2 result;

	theta += float(M_PI) / (velocity * 100.0f);

	result.x = sinf(theta) * amplitude + pos.x;
	result.y = pos.y;

	return result;
}

/// ///////////////
//
//
//Vector2 verticalEnemy(Vector2 pos, float amplitude, float &theta, float velocity) {
//	Vector2 result;
//
//	theta += float(M_PI) / velocity;
//
//	result.x = pos.x;
//	result.y = sinf(theta) * amplitude + pos.y;
//
//	return result;
//}
//
//Vector2 horizonEnemy(Vector2 pos, float amplitude, float velocity) {
//	Vector2 result;
//
//	float theta = 0.0f;
//
//	theta += float(M_PI) / velocity;
//
//	result.x = sinf(theta) * amplitude + pos.x;
//	result.y = pos.y;
//
//	return result;
//}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//************************* 宣言 *************************//

#pragma region シーン
	enum Scene {
		START,
		PLAY,
		CLEAR,
		END

	}scene = PLAY;

#pragma endregion

#pragma region 敵

	const int maxEnemy = 100;

	Enemy enemies[maxEnemy];

	for (int i = 0; i < maxEnemy; i++) {
		enemies[i].pos = { 800.0f, 360.0f };
		enemies[i].enemyType = i % 3;
		enemies[i].theta = 0;
		enemies[i].amplitude = 150.0f;
		enemies[i].radius = 40.0f;
	}

	Enemy enemiesBig[maxEnemy];

	for (int i = 0; i < maxEnemy; i++) {
		enemiesBig[i].pos = { 800.0f, 360.0f };
		enemiesBig[i].enemyType = i % 3;
		enemiesBig[i].theta = 0;
		enemiesBig[i].amplitude = 150.0f;
		enemiesBig[i].radius = 60.0f;
	}

	int stage2pos = 1280;
	int stage3pos = 2560;
	int stage4pos = 3840;
	int stage5pos = 5120;
	float moveFast = 15;
	float moveNormal = 25;
	float moveSlow = 50;

	enemies[64].theta = (float)M_PI / 4.0f; // ずらしたやつ
	enemies[65].theta = (float)M_PI / 8.0f; // ずらしたやつ
	enemies[66].theta = (float)M_PI / 32.0f; // ずらしたやつ
	enemies[67].theta = (float)M_PI / -16.0f; // ずらしたやつ
	enemies[68].theta = (float)M_PI / -6.0f; // ずらしたやつ
	enemies[69].theta = (float)M_PI / -4.0f; // ずらしたやつ

	enemiesBig[30].theta = (float)M_PI / 4.0f;
	enemiesBig[31].theta = (float)M_PI / 8.0f;
	enemiesBig[32].theta = (float)M_PI / 32.0f;
	enemiesBig[33].theta = (float)M_PI / -16.0f;
	enemiesBig[48].theta = (float)M_PI / -6.0f;
	
	enemiesBig[46].theta = (float)M_PI / 4.0f;
	enemiesBig[47].theta = (float)M_PI / 8.0f;
	enemiesBig[50].theta = (float)M_PI / 32.0f;
	enemiesBig[58].theta = (float)M_PI / -16.0f;
	enemiesBig[59].theta = (float)M_PI / -6.0f;
	
	enemiesBig[60].theta = (float)M_PI / 4.0f;
	enemiesBig[61].theta = (float)M_PI / 8.0f;
	enemiesBig[62].theta = (float)M_PI / 32.0f;
	enemiesBig[63].theta = (float)M_PI / -16.0f;
	enemiesBig[64].theta = (float)M_PI / -6.0f;
	
	enemies[83].theta = (float)M_PI / 4.0f; 
	enemies[84].theta = (float)M_PI / 8.0f; 
	enemies[85].theta = (float)M_PI / 32.0f;
	enemies[86].theta = (float)M_PI / -16.0f;
	enemies[87].theta = (float)M_PI / -6.0f;
	enemies[88].theta = (float)M_PI / -4.0f;

	enemies[50].theta = (float)M_PI / 4.0f;
	enemies[51].theta = (float)M_PI / 8.0f;
	enemies[52].theta = (float)M_PI / 32.0f;
	enemies[53].theta = (float)M_PI / -16.0f;
	enemies[54].theta = (float)M_PI / -6.0f;
	enemies[55].theta = (float)M_PI / -4.0f;
		

#pragma endregion		   

#pragma region プレイヤー

	Player player{
		player.pos = {150.0f, 360.0f},
		player.targetPos = {150.0f, 360.0f},
		player.radius = 30.0f,
		player.checkPointPos = {150.0f, 360.0f},
		player.isAlive = true,
		player.respawCoolTime = 60,
		player.transparency = 255,
		player.isBlinking = false,
		player.blinkingTime = 60,
		player.th = Novice::LoadTexture("./Resources/PlayScene/player.png"),
		player.screenPosX = 180,
		player.isAnimation = false,
		player.animeCount = 60,
	};

	// 高速移動速度
	const float dashDistance = 280.0f;

#pragma endregion 

#pragma region パーティくる

	// パーティクルの最大数
	const int maxParticles = 50;
	Particle particles[maxParticles];

	for (int i = 0; i < maxParticles; ++i) {
		particles[i].pos = { player.pos.x, player.pos.y };
		particles[i].velocity = { (rand() % 200 - 100) / 100.0f, 5.0f };
		particles[i].radius = 5.0f;
		particles[i].isActive = false;
		particles[i].lifeTime = 0;
		particles[i].baseAngle = 0.0f;
		particles[i].randomAngle = 0.0f;
	}
	const int particlesToGenerate = 5; // 生成するパーティクルの数

#pragma endregion

#pragma region チェックポイント

	//*********チェックポイント
	CheckPoint checkPoint{
		checkPoint.pos = {720.0f / 2.0f,0.0f},
	};

	Vector2 clearLine = { 1350.0f , 0.0f };

#pragma endregion

#pragma region カーソル

	// カーソルの初期設定
	Cursor cursor{
		{640.0f, 360.0f}
	};

#pragma endregion

#pragma region カメラ

	Camera camera{
		{0.0f, 0.0f},
		{0.0f, 0.0f}
	};

#pragma endregion

#pragma region コントローラー

	int leftStickX = 0;
	int leftStickY = 0;
	const float easingSpeed = 0.2f;

	// XPAD用
	XINPUT_STATE state;

#pragma endregion

	//シェイク
	Vector2 wrand = { 0.0f };
	int randMax = 0;

	//*****************************************************//


		// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);
		XInputGetState(0, &state);

		switch (scene)
		{
		case START:

			//スペース押したらシーン切り替え
			if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0) {

				scene = PLAY;
			}
			break;
		case PLAY:

#pragma region シェイク

			randMax--;
			if (randMax < 1) {
				randMax = 1;
			}

			wrand.x = float(rand() % randMax - randMax / 2);
			wrand.y = float(rand() % randMax - randMax / 2);
#pragma endregion

#pragma region 敵

			for (int i = 0; i < maxEnemy; i++) {
				//enemies[i].theta += float(M_PI) / 60.0f;
				if (enemies[i].enemyType == 0) {
					// 円状に動く敵
					/// ステージ 1
					enemies[56].pos = circleEnemy({ 1000.0f , 0.0f }, 250.0f, enemies[56].theta, -moveSlow); ///top
					enemies[57].pos = circleEnemy({ 1000.0f , 0.0f }, 250.0f, enemies[57].theta, moveSlow); ///top

					enemies[58].pos = circleEnemy({ 1000.0f , 720.0f }, 250.0f, enemies[58].theta, -moveSlow);///bottom
					enemies[59].pos = circleEnemy({ 1000.0f , 720.0f }, 250.0f, enemies[59].theta, moveSlow); ///bottom

					enemies[60].pos = circleEnemy({ 1000.0f , 360.0f }, 250.0f, enemies[60].theta, -moveSlow); ///top
					enemies[61].pos = circleEnemy({ 1000.0f , 360.0f }, 250.0f, enemies[61].theta, moveSlow); ///top
					//enemies[63].pos = circleEnemy({ 140.0f , 0.0f }, 250.0f, enemies[63].theta, moveSlow); ///top
					//enemies[60].pos = circleEnemy({ 140.0f , 720.0f }, 250.0f, enemies[60].theta, -moveSlow); ///bottom
					//enemies[61].pos = circleEnemy({ 140.0f , 720.0f }, 250.0f, enemies[61].theta, moveSlow); ///bottom

					/// ステージ 2
					enemies[1].pos = circleEnemy({ stage2pos + 150.0f, 100.0f }, 140.0f, enemies[1].theta, -moveNormal);
					enemies[2].pos = circleEnemy({ stage2pos + 150.0f, 100.0f }, 140.0f, enemies[2].theta, -moveSlow);
					enemies[3].pos = circleEnemy({ stage2pos + 150.0f, 100.0f }, 140.0f, enemies[3].theta, -moveFast);
					enemies[4].pos = circleEnemy({ stage2pos + 150.0f, 640.0f }, 140.0f, enemies[4].theta, moveFast);
					enemies[5].pos = circleEnemy({ stage2pos + 150.0f, 640.0f }, 140.0f, enemies[5].theta, moveSlow);
					enemies[6].pos = circleEnemy({ stage2pos + 150.0f, 640.0f }, 140.0f, enemies[6].theta, moveNormal);

					enemies[7].pos = circleEnemy({ stage2pos + 710.0f, 360.0f }, 140.0f, enemies[7].theta, moveNormal);
					enemies[8].pos = circleEnemy({ stage2pos + 710.0f, 360.0f }, 140.0f, enemies[8].theta, -moveNormal);
					
					enemies[70].pos = circleEnemy({ stage2pos + 420.0f, 360.0f }, 160.0f, enemies[70].theta, moveSlow);
					enemies[71].pos = circleEnemy({ stage2pos + 420.0f, 360.0f }, 160.0f, enemies[71].theta, -moveSlow);

					enemies[9].pos = circleEnemy({ stage2pos + 500.0f, 140.0f }, 140.0f, enemies[9].theta, -moveFast);
					enemies[10].pos = circleEnemy({ stage2pos + 500.0f, 140.0f }, 140.0f, enemies[10].theta, -moveNormal);
					enemies[11].pos = circleEnemy({ stage2pos + 500.0f, 140.0f }, 140.0f, enemies[11].theta, -moveSlow);
					enemies[12].pos = circleEnemy({ stage2pos + 500.0f, 580.0f }, 140.0f, enemies[12].theta, moveFast);
					enemies[13].pos = circleEnemy({ stage2pos + 500.0f, 580.0f }, 140.0f, enemies[13].theta, moveNormal);
					enemies[14].pos = circleEnemy({ stage2pos + 500.0f, 580.0f }, 140.0f, enemies[14].theta, moveSlow);

					enemies[15].pos = circleEnemy({ stage2pos + 990.0f, 220.0f }, 140.0f, enemies[15].theta, moveSlow);
					enemies[16].pos = circleEnemy({ stage2pos + 990.0f, 220.0f }, 140.0f, enemies[16].theta, -moveSlow);
					enemies[17].pos = circleEnemy({ stage2pos + 990.0f, 500.0f }, 140.0f, enemies[17].theta, moveSlow);
					enemies[18].pos = circleEnemy({ stage2pos + 990.0f, 500.0f }, 140.0f, enemies[18].theta, -moveSlow);


					///ステージ 3
					enemies[19].pos = circleEnemy({ stage3pos + 430.0f, 360.0f }, 160.0f, enemies[19].theta, moveNormal);
					enemies[20].pos = circleEnemy({ stage3pos + 430.0f, 80.0f }, 160.0f, enemies[20].theta, -moveNormal);
					enemies[21].pos = circleEnemy({ stage3pos + 710.0f, 80.0f }, 160.0f, enemies[21].theta, moveNormal);
					enemies[22].pos = circleEnemy({ stage3pos + 710.0f, 360.0f }, 160.0f, enemies[22].theta, -moveNormal);
					enemies[23].pos = circleEnemy({ stage3pos + 710.0f, 640.0f }, 160.0f, enemies[23].theta, moveNormal);
					enemies[24].pos = circleEnemy({ stage3pos + 990.0f, 640.0f }, 160.0f, enemies[24].theta, -moveNormal);
					enemies[25].pos = circleEnemy({ stage3pos + 990.0f, 360.0f }, 160.0f, enemies[25].theta, moveNormal);

					enemies[26].pos = circleEnemy({ stage3pos + 300.0f, 640.0f }, 160.0f, enemies[26].theta, moveNormal);
					enemies[27].pos = circleEnemy({ stage3pos + 150.0f, 80.0f }, 160.0f, enemies[27].theta, moveNormal);

					///ステージ 4
					enemies[28].pos = circleEnemy({ stage4pos + 270.0f, 360.0f }, 140.0f, enemies[28].theta, moveNormal);

					enemies[73].pos = circleEnemy({ stage4pos + 550.0f, 360.0f }, 140.0f, enemies[73].theta, -moveFast);
					enemies[74].pos = circleEnemy({ stage4pos + 550.0f, 360.0f }, 140.0f, enemies[74].theta, -moveNormal);
					enemies[75].pos = circleEnemy({ stage4pos + 550.0f, 360.0f }, 140.0f, enemies[75].theta, -moveSlow);

					enemies[29].pos = circleEnemy({ stage4pos + 270.0f, 80.0f }, 140.0f, enemies[29].theta, -moveNormal);
					enemies[30].pos = circleEnemy({ stage4pos + 550.0f, 80.0f }, 140.0f, enemies[30].theta, moveSlow);
					enemies[31].pos = circleEnemy({ stage4pos + 830.0f, 80.0f }, 140.0f, enemies[31].theta, -moveNormal);
					enemies[32].pos = circleEnemy({ stage4pos + 1100.0f, 80.0f }, 140.0f, enemies[32].theta, moveSlow);
					enemies[33].pos = circleEnemy({ stage4pos + 1390.0f, 80.0f }, 140.0f, enemies[33].theta, -moveNormal);
					
					enemies[72].pos = circleEnemy({ stage4pos + 1390.0f, 80.0f }, 140.0f, enemies[72].theta, moveFast);
					enemies[76].pos = circleEnemy({ stage4pos + 1390.0f, 80.0f }, 140.0f, enemies[76].theta, moveNormal);
					enemies[77].pos = circleEnemy({ stage4pos + 1390.0f, 80.0f }, 140.0f, enemies[77].theta, moveSlow);
					
					enemies[34].pos = circleEnemy({ stage4pos + 1390.0f, 360.0f }, 140.0f, enemies[34].theta, moveSlow);
					enemies[35].pos = circleEnemy({ stage4pos + 1100.0f, 360.0f }, 140.0f, enemies[35].theta, -moveNormal);
					enemies[36].pos = circleEnemy({ stage4pos + 830.0f, 360.0f }, 140.0f, enemies[36].theta, moveSlow);
					enemies[37].pos = circleEnemy({ stage4pos + 550.0f, 360.0f }, 140.0f, enemies[37].theta, -moveNormal);
					enemies[38].pos = circleEnemy({ stage4pos + 550.0f, 640.0f }, 140.0f, enemies[38].theta, moveSlow);
					enemies[39].pos = circleEnemy({ stage4pos + 830.0f, 640.0f }, 140.0f, enemies[39].theta, -moveNormal);
					enemies[40].pos = circleEnemy({ stage4pos + 1100.0f, 640.0f }, 140.0f, enemies[40].theta, moveSlow);
					enemies[41].pos = circleEnemy({ stage4pos + 1390.0f, 640.0f }, 140.0f, enemies[41].theta, -moveNormal);

					enemies[42].pos = circleEnemy({ stage4pos + 690.0f, 500.0f }, 280.0f, enemies[42].theta, moveSlow); /// the big circular
					enemies[43].pos = circleEnemy({ stage4pos + 970.0f, 500.0f }, 280.0f, enemies[43].theta, -moveSlow); /// the big circular
					enemies[44].pos = circleEnemy({ stage4pos + 1250.0f, 500.0f }, 280.0f, enemies[44].theta, moveSlow); /// the big circular
					//enemies[45].pos = circleEnemy({ stage3pos + 1530.0f, 500.0f }, 280.0f, enemies[45].theta, -moveSlow); /// the big circular

					//enemies[46].pos = circleEnemy({ stage3pos + 690.0f, 780.0f }, 280.0f, enemies[46].theta, -moveSlow); /// the big circular
					enemies[47].pos = circleEnemy({ stage4pos + 970.0f, 780.0f }, 280.0f, enemies[47].theta, -moveSlow); /// the big circular
					enemies[48].pos = circleEnemy({ stage4pos + 1250.0f, 780.0f }, 280.0f, enemies[48].theta, moveSlow); /// the big circular
					enemies[49].pos = circleEnemy({ stage4pos + 1530.0f, 780.0f }, 280.0f, enemies[49].theta, -moveSlow); /// the big circular


					/*enemies[32].pos = circleEnemy({ 960.0f, 0.0f }, 150.0f, enemies[32].theta, moveNormal);
					enemies[33].pos = circleEnemy({ 150.0f, 360.0f }, 150.0f, enemies[33].theta, moveNormal);
					enemies[34].pos = circleEnemy({ 480.0f, 540.0f }, 150.0f, enemies[34].theta, moveNormal);
					enemies[35].pos = circleEnemy({ 800.0f, 540.0f }, 150.0f, enemies[35].theta, moveNormal);
					enemies[36].pos = circleEnemy({ 960.0f, 360.0f }, 150.0f, enemies[36].theta, moveNormal);*/
					///ステージ 5
					enemies[50].pos = circleEnemy({ stage5pos + 840.0f , 360.0f }, 120.0f, enemies[50].theta, moveSlow); ///inner
					enemies[51].pos = circleEnemy({ stage5pos + 840.0f , 360.0f }, 200.0f, enemies[51].theta, -moveSlow);
					enemies[52].pos = circleEnemy({ stage5pos + 840.0f , 360.0f }, 280.0f, enemies[52].theta, moveSlow);
					//enemies[53].pos = circleEnemy({ stage4pos + 840.0f , 360.0f }, 300.0f, enemies[53].theta, -moveNormal);
					//enemies[54].pos = circleEnemy({ stage4pos + 840.0f , 360.0f }, 350.0f, enemies[54].theta, moveNormal);
					enemies[53].pos = circleEnemy({ stage5pos + 840.0f , 360.0f }, 360.0f, enemies[53].theta, -moveSlow);
					enemies[54].pos = circleEnemy({ stage5pos + 840.0f , 360.0f }, 420.0f, enemies[54].theta, moveSlow);
					enemies[55].pos = circleEnemy({ stage5pos + 840.0f , 360.0f }, 580.0f, enemies[55].theta, -moveSlow); ///outer*/
					enemies[63].pos = circleEnemy({ stage5pos + 840.0f , 360.0f }, 580.0f, enemies[63].theta, moveSlow); ///outer*/
					
					//enemies[78].pos = circleEnemy({ stage5pos + 640.0f , 360.0f }, 400.0f, enemies[78].theta, moveSlow); ///outer*/
					//enemies[79].pos = circleEnemy({ stage5pos + 640.0f , 360.0f }, 400.0f, enemies[79].theta, moveSlow); ///outer*/
					//enemies[80].pos = circleEnemy({ stage5pos + 640.0f , 360.0f }, 400.0f, enemies[80].theta, moveSlow); ///outer*/
					//enemies[81].pos = circleEnemy({ stage5pos + 640.0f , 360.0f }, 400.0f, enemies[81].theta, moveSlow); ///outer*/
					//enemies[82].pos = circleEnemy({ stage5pos + 640.0f , 360.0f }, 400.0f, enemies[82].theta, moveSlow); ///outer*/


				}
				else if (enemies[i].enemyType == 1) {
					// 縦方向に往復する敵
					/// ステージ 1

					enemies[64].pos = verticalEnemy({ 300.0f, 360.0f }, 300.0f, enemies[64].theta, moveSlow); /// theta test
					enemies[65].pos = verticalEnemy({ 380.0f, 360.0f }, 300.0f, enemies[65].theta, moveSlow); /// theta test
					enemies[66].pos = verticalEnemy({ 460.0f, 360.0f }, 300.0f, enemies[66].theta, moveSlow); /// theta test
					enemies[67].pos = verticalEnemy({ 540.0f, 360.0f }, 300.0f, enemies[67].theta, moveSlow); /// theta test
					enemies[68].pos = verticalEnemy({ 620.0f, 360.0f }, 300.0f, enemies[68].theta, moveSlow); /// theta test
					
					enemies[69].pos = verticalEnemy({ 700.0f, 360.0f }, 300.0f, enemies[69].theta, moveSlow); /// theta test
					//enemies[63].pos = verticalEnemy({ 300.0f, 360.0f }, 200.0f, enemiesBig[63].theta, -moveSlow); /// wall 3

					/// ステージ 3
					enemiesBig[1].pos = verticalEnemy({ stage3pos + 330.0f, 370.0f }, 100.0f, enemiesBig[1].theta, moveSlow); /// wall
					enemiesBig[2].pos = verticalEnemy({ stage3pos + 330.0f, 490.0f }, 100.0f, enemiesBig[2].theta, moveNormal); /// wall
					enemiesBig[3].pos = verticalEnemy({ stage3pos + 330.0f, 610.0f }, 100.0f, enemiesBig[3].theta, moveFast); /// wall
					enemiesBig[4].pos = verticalEnemy({ stage3pos + 330.0f, 730.0f }, 100.0f, enemiesBig[4].theta, moveNormal); /// wall
					enemiesBig[5].pos = verticalEnemy({ stage3pos + 330.0f, 850.0f }, 100.0f, enemiesBig[5].theta, moveSlow); /// wall

					enemiesBig[6].pos = verticalEnemy({ stage3pos + 810.0f, -140.0f }, 100.0f, enemiesBig[6].theta, moveFast); /// wall 2
					enemiesBig[7].pos = verticalEnemy({ stage3pos + 810.0f, -20.0f }, 100.0f, enemiesBig[7].theta, moveSlow); /// wall 2
					enemiesBig[8].pos = verticalEnemy({ stage3pos + 810.0f, 100.0f }, 100.0f, enemiesBig[8].theta, moveNormal); /// wall 2
					enemiesBig[9].pos = verticalEnemy({ stage3pos + 810.0f, 220.0f }, 100.0f, enemiesBig[9].theta, moveSlow); /// wall 2
					enemiesBig[10].pos = verticalEnemy({ stage3pos + 810.0f, 340.0f }, 100.0f, enemiesBig[10].theta, moveFast); /// wall 2

					enemiesBig[11].pos = verticalEnemy({ stage3pos + 1130.0f, 340.0f }, 100.0f, enemiesBig[11].theta, moveNormal); /// wall 3
					enemiesBig[12].pos = verticalEnemy({ stage3pos + 1130.0f, 460.0f }, 100.0f, enemiesBig[12].theta, moveFast); /// wall 3
					enemiesBig[13].pos = verticalEnemy({ stage3pos + 1130.0f, 580.0f }, 100.0f, enemiesBig[13].theta, moveSlow); /// wall 3
					enemiesBig[14].pos = verticalEnemy({ stage3pos + 1130.0f, 700.0f }, 100.0f, enemiesBig[14].theta, moveNormal); /// wall 3
					enemiesBig[15].pos = verticalEnemy({ stage3pos + 1130.0f, 820.0f }, 100.0f, enemiesBig[15].theta, moveSlow); /// wall 3

					enemiesBig[16].pos = verticalEnemy({ stage3pos + 1130.0f, -140.0f }, 100.0f, enemiesBig[16].theta, moveSlow); /// wall 3 up
					enemiesBig[17].pos = verticalEnemy({ stage3pos + 1130.0f, -20.0f }, 100.0f, enemiesBig[17].theta, moveFast); /// wall 3
					enemiesBig[18].pos = verticalEnemy({ stage3pos + 1130.0f, 100.0f }, 100.0f, enemiesBig[18].theta, moveFast); /// wall 3
					enemiesBig[19].pos = verticalEnemy({ stage3pos + 1130.0f, 220.0f }, 100.0f, enemiesBig[19].theta, moveNormal); /// wall 3

					/// ステージ 4
					enemiesBig[20].pos = verticalEnemy({ stage4pos + 410.0f, 430.0f }, 200.0f, enemiesBig[20].theta, moveNormal); /// wall 3
					enemiesBig[21].pos = verticalEnemy({ stage4pos + 410.0f, 430.0f }, 200.0f, enemiesBig[21].theta, -moveNormal); /// wall 3
					enemiesBig[22].pos = verticalEnemy({ stage4pos + 410.0f, 430.0f }, 100.0f, enemiesBig[22].theta, moveNormal); /// wall 3
					enemiesBig[23].pos = verticalEnemy({ stage4pos + 410.0f, 430.0f }, 100.0f, enemiesBig[23].theta, -moveNormal); /// wall 3


					/// ステージ 5
					enemiesBig[24].pos = verticalEnemy({ stage5pos + 250.0f, 220.0f }, 100.0f, enemiesBig[24].theta, moveNormal); /// wall 3
					enemiesBig[25].pos = verticalEnemy({ stage5pos + 250.0f, 220.0f }, 100.0f, enemiesBig[25].theta, -moveNormal); /// wall 3
					enemiesBig[26].pos = verticalEnemy({ stage5pos + 250.0f, 220.0f }, 200.0f, enemiesBig[26].theta, moveNormal); /// wall 3
					enemiesBig[27].pos = verticalEnemy({ stage5pos + 250.0f, 220.0f }, 200.0f, enemiesBig[27].theta, -moveNormal); /// wall 3

					enemies[83].pos = verticalEnemy({ stage5pos + 600.0f, 360.0f }, 900.0f, enemies[83].theta, moveSlow); /// wall 3
					enemies[84].pos = verticalEnemy({ stage5pos + 680.0f, 360.0f }, 900.0f, enemies[84].theta, moveSlow); /// wall 3
					enemies[85].pos = verticalEnemy({ stage5pos + 760.0f, 360.0f }, 900.0f, enemies[85].theta, moveSlow); /// wall 3
					enemies[86].pos = verticalEnemy({ stage5pos + 820.0f, 360.0f }, 900.0f, enemies[86].theta, moveSlow); /// wall 3
					enemies[87].pos = verticalEnemy({ stage5pos + 900.0f, 360.0f }, 900.0f, enemies[87].theta, moveSlow); /// wall 3
					enemies[88].pos = verticalEnemy({ stage5pos + 980.0f, 360.0f }, 900.0f, enemies[88].theta, moveSlow); /// wall 3




					/// ステージ ３
					/*enemies[21].pos = verticalEnemy({stage3pos + 160.0f, 180.0f}, 150.0f, enemies[21].theta, moveNormal);
					enemies[22].pos = verticalEnemy({ stage3pos + 640.0f, 180.0f }, 150.0f, enemies[22].theta, moveNormal);
					enemies[23].pos = verticalEnemy({ stage3pos + 800.0f, 180.0f }, 150.0f, enemies[23].theta, moveNormal);
					enemies[24].pos = verticalEnemy({ stage3pos + 960.0f, 180.0f }, 150.0f, enemies[24].theta, moveNormal); ///up
					enemies[25].pos = verticalEnemy({ stage3pos + 80.0f, 360.0f }, 150.0f, enemies[25].theta, moveNormal);
					enemies[26].pos = verticalEnemy({ stage3pos + 80.0f, 630.0f }, 100.0f, enemies[26].theta, moveNormal);
					enemies[27].pos = verticalEnemy({ stage3pos + 160.0f, 540.0f }, 150.0f, enemies[27].theta, moveNormal); ///down
					enemies[28].pos = verticalEnemy({ stage3pos + 320.0f, 540.0f }, 150.0f, enemies[28].theta, moveNormal);
					enemies[29].pos = verticalEnemy({ stage3pos + 480.0f, 540.0f }, 150.0f, enemies[29].theta, moveNormal);
					enemies[30].pos = verticalEnemy({ stage3pos + 640.0f, 540.0f }, 150.0f, enemies[30].theta, moveNormal);
					enemies[31].pos = verticalEnemy({ stage3pos + 800.0f, 540.0f }, 150.0f, enemies[31].theta, moveNormal);
					enemies[32].pos = verticalEnemy({ stage3pos + 880.0f, 400.0f }, 150.0f, enemies[32].theta, moveNormal);
					enemies[33].pos = verticalEnemy({ stage3pos + 960.0f, 400.0f }, 150.0f, enemies[33].theta, moveNormal);*/

				}
				else if (enemies[i].enemyType == 2) {
					// 横方向に往復する敵
					/// ステージ １
					/*enemies[9].pos = horizonEnemy({ 160.0f,0.0f }, 150.0f, enemies[9].theta, moveSlow);
					enemies[10].pos = horizonEnemy({ 1120.0f,0.0f }, 150.0f, enemies[10].theta, moveSlow);
					enemies[11].pos = horizonEnemy({ 960.0f,160.0f }, 300.0f, enemies[11].theta, moveSlow);
					enemies[12].pos = horizonEnemy({ 960.0f,540.0f }, 300.0f, enemies[12].theta, moveSlow);
					enemies[13].pos = horizonEnemy({ 160.0f,1280.0f }, 300.0f, enemies[13].theta, moveSlow);
					enemies[14].pos = horizonEnemy({ 1120.0f,1280.0f }, 300.0f, enemies[14].theta, moveSlow);*/
					/// ステージ 4
					enemiesBig[30].pos = horizonEnemy({ stage4pos + 150.0f, 480.0f }, 250.0f, enemiesBig[30].theta, moveSlow); /// wall 3
					enemiesBig[31].pos = horizonEnemy({ stage4pos + 150.0f, 480.0f }, 250.0f, enemiesBig[31].theta, moveSlow); /// wall 3
					enemiesBig[32].pos = horizonEnemy({ stage4pos + 150.0f, 480.0f }, 250.0f, enemiesBig[32].theta, moveSlow); /// wall 3
					enemiesBig[33].pos = horizonEnemy({ stage4pos + 150.0f, 480.0f }, 250.0f, enemiesBig[33].theta, moveSlow); /// wall 3
					enemiesBig[48].pos = horizonEnemy({ stage4pos + 150.0f, 480.0f }, 250.0f, enemiesBig[48].theta, moveSlow); /// wall 3
					//enemiesBig[49].pos = horizonEnemy({ stage4pos + 150.0f, 480.0f }, 250.0f, enemiesBig[49].theta, moveSlow); /// wall 3
					
					enemiesBig[46].pos = horizonEnemy({ stage4pos + 150.0f, 590.0f }, 250.0f, enemiesBig[46].theta, moveNormal); /// wall 3
					enemiesBig[47].pos = horizonEnemy({ stage4pos + 150.0f, 590.0f }, 250.0f, enemiesBig[47].theta, moveNormal); /// wall 3
					enemiesBig[50].pos = horizonEnemy({ stage4pos + 150.0f, 590.0f }, 250.0f, enemiesBig[50].theta, moveNormal); /// wall 3
					enemiesBig[58].pos = horizonEnemy({ stage4pos + 150.0f, 590.0f }, 250.0f, enemiesBig[58].theta, moveNormal); /// wall 3
					enemiesBig[59].pos = horizonEnemy({ stage4pos + 150.0f, 590.0f }, 250.0f, enemiesBig[59].theta, moveNormal); /// wall 3
					
					enemiesBig[60].pos = horizonEnemy({ stage4pos + 150.0f, 700.0f }, 250.0f, enemiesBig[60].theta, moveFast); /// wall 3
					enemiesBig[61].pos = horizonEnemy({ stage4pos + 150.0f, 700.0f }, 250.0f, enemiesBig[61].theta, moveFast); /// wall 3
					enemiesBig[62].pos = horizonEnemy({ stage4pos + 150.0f, 700.0f }, 250.0f, enemiesBig[62].theta, moveFast); /// wall 3
					enemiesBig[63].pos = horizonEnemy({ stage4pos + 150.0f, 700.0f }, 250.0f, enemiesBig[63].theta, moveFast); /// wall 3
					enemiesBig[64].pos = horizonEnemy({ stage4pos + 150.0f, 700.0f }, 250.0f, enemiesBig[64].theta, moveFast); /// wall 3

					enemiesBig[34].pos = horizonEnemy({ stage4pos + 830.0f, 220.0f }, 400.0f, enemiesBig[34].theta, moveNormal); /// wall 3
					enemiesBig[35].pos = horizonEnemy({ stage4pos + 830.0f, 220.0f }, 400.0f, enemiesBig[35].theta, -moveNormal); /// wall 3
					enemiesBig[36].pos = horizonEnemy({ stage4pos + 830.0f, 220.0f }, 250.0f, enemiesBig[36].theta, moveNormal); /// wall 3
					enemiesBig[37].pos = horizonEnemy({ stage4pos + 830.0f, 220.0f }, 250.0f, enemiesBig[37].theta, -moveNormal); /// wall 3
					enemiesBig[38].pos = horizonEnemy({ stage4pos + 830.0f, 220.0f }, 100.0f, enemiesBig[38].theta, moveNormal); /// wall 3
					enemiesBig[39].pos = horizonEnemy({ stage4pos + 830.0f, 220.0f }, 100.0f, enemiesBig[39].theta, -moveNormal); /// wall 3

					enemiesBig[40].pos = horizonEnemy({ stage4pos + 1090.0f, 480.0f }, 300.0f, enemiesBig[40].theta, moveNormal); /// wall 3
					enemiesBig[41].pos = horizonEnemy({ stage4pos + 1090.0f, 480.0f }, 300.0f, enemiesBig[41].theta, -moveNormal); /// wall 3
					enemiesBig[42].pos = horizonEnemy({ stage4pos + 1090.0f, 480.0f }, 200.0f, enemiesBig[42].theta, moveNormal); /// wall 3
					enemiesBig[43].pos = horizonEnemy({ stage4pos + 1090.0f, 480.0f }, 200.0f, enemiesBig[43].theta, -moveNormal); /// wall 3
					enemiesBig[44].pos = horizonEnemy({ stage4pos + 1090.0f, 480.0f }, 100.0f, enemiesBig[44].theta, moveNormal); /// wall 3
					enemiesBig[45].pos = horizonEnemy({ stage4pos + 1090.0f, 480.0f }, 100.0f, enemiesBig[45].theta, -moveSlow); /// wall 3

					enemiesBig[55].pos = horizonEnemy({ stage4pos + 1390.0f, 480.0f }, 100.0f, enemiesBig[55].theta, moveSlow); /// wall 3
					enemiesBig[56].pos = horizonEnemy({ stage4pos + 1390.0f, 480.0f }, 100.0f, enemiesBig[56].theta, moveNormal); /// wall 3
					enemiesBig[57].pos = horizonEnemy({ stage4pos + 1390.0f, 480.0f }, 100.0f, enemiesBig[57].theta, moveFast); /// wall 3


					/// ステージ ３
					/*enemies[34].pos = horizonEnemy({ stage3pos + 480.0f, 180.0f }, 500.0f, enemies[34].theta, moveNormal); /// first purple line << add 6 more
					enemies[35].pos = horizonEnemy({ stage3pos + 640.0f, 270.0f }, 350.0f, enemies[35].theta, moveFast); /// second  << add 1 more
					enemies[36].pos = horizonEnemy({ stage3pos + 160.0f, 270.0f }, 150.0f, enemies[36].theta, moveNormal); /// second right side << add 3 more
					enemies[37].pos = horizonEnemy({ stage3pos + 480.0f, 450.0f }, 150.0f, enemies[37].theta, moveNormal); /// third line << add 6 more
					enemies[38].pos = horizonEnemy({ stage3pos + 480.0f, 540.0f }, 150.0f, enemies[38].theta, moveNormal); /// forth line << add 6 more
					enemies[39].pos = horizonEnemy({ stage3pos + 1120.0f, 180.0f }, 150.0f, enemies[39].theta, moveFast); /// right one 1
					enemies[40].pos = horizonEnemy({ stage3pos + 1120.0f, 360.0f }, 150.0f, enemies[40].theta, moveFast); /// right one 2
					enemies[41].pos = horizonEnemy({ stage3pos + 1120.0f, 540.0f }, 150.0f, enemies[41].theta, moveFast); /// right one 3*/


				}
			}


#pragma endregion

#pragma region プレイヤー

			// スティックの入力を取得
			Novice::GetAnalogInputLeft(0, &leftStickX, &leftStickY);

			if (std::abs(leftStickX) > 8000 || std::abs(leftStickY) > 8000) {

				float magnitude = std::sqrtf(float(leftStickX * leftStickX + leftStickY * leftStickY));
				Vector2 direction = {
					(float)leftStickX / magnitude,
					(float)leftStickY / magnitude
				};

				// カーソルの位置を計算
				cursor.pos.x = player.pos.x + direction.x * dashDistance;
				cursor.pos.y = player.pos.y + direction.y * dashDistance;

				// ダッシュ
				if (Novice::IsTriggerButton(0, kPadButton10)) {

					// 目標位置を更新
					player.targetPos.x = cursor.pos.x;
					player.targetPos.y = cursor.pos.y;

					// アニメーション更新
					player.isAnimation = true;
					player.animeCount = 30;

					// ダッシュ時にパーティクルを生成
					for (int j = 0; j < particlesToGenerate; ++j) {
						for (int i = 0; i < maxParticles; ++i) {
							if (!particles[i].isActive) {
								particles[i].pos = player.pos;

								particles[i].baseAngle = atan2f(player.targetPos.y - player.pos.y, player.targetPos.x - player.pos.x) + ((float)M_PI);

								particles[i].randomAngle = particles[i].baseAngle + ((rand() % 30 - 15) * (float)M_PI / 180.0f);

								particles[i].speed = 5.0f + (rand() % 5);

								particles[i].velocity = { cosf(particles[i].randomAngle) * particles[i].speed, sinf(particles[i].randomAngle) * particles[i].speed };

								particles[i].radius = 5.0f;
								particles[i].lifeTime = 60;
								particles[i].isActive = true;

								break;
							}
						}
					}
				}
			}
			else {

				// スティックが戻ったらカーソルをプレイヤーの位置に戻す
				cursor.pos = player.pos;
			}

#pragma region キーボード

			if (keys[DIK_W]) {

				player.targetPos.y -= 10.0f;
			}
			if (keys[DIK_S]) {

				player.targetPos.y += 10.0f;
			}
			if (keys[DIK_A]) {

				player.targetPos.x -= 10.0f;
			}
			if (keys[DIK_D]) {

				player.targetPos.x += 10.0f;
			}
#pragma endregion

			// プレイヤーの位置を目標位置に向かってイージング移動
			player.pos.x += (player.targetPos.x - player.pos.x) * easingSpeed;
			player.pos.y += (player.targetPos.y - player.pos.y) * easingSpeed;

			//**************** 移動制限

			if (player.pos.x < player.radius) {
				player.pos.x = player.radius;  // 左端での制限
			}

			if (player.pos.y < player.radius) {
				player.pos.y = player.radius;  // 上端での制限
			}
			if (player.pos.y > (720.0f - player.radius)) {
				player.pos.y = 720.0f - player.radius;  // 下端での制限
			}

			//**************リスポーン
			if (!player.isAlive) {

				player.respawCoolTime--;

				// カーソル初期化
				cursor.pos = player.pos;

				// プレイヤーの位置
				player.pos = player.checkPointPos;
				player.targetPos = player.checkPointPos;

				// カメラの位置
				camera.targetPos.x = player.pos.x - 120.0f;

				player.animeCount = 0;
				player.screenPosX = 0;

			}

			if (player.respawCoolTime <= 0.0f) {

				player.respawCoolTime = 60;

				player.isAlive = true;
			}

			// *****************チェックポイント
			if (player.pos.x >= checkPoint.pos.x) {

				player.checkPointPos.x = checkPoint.pos.x;
			}

			// ****************ゴール
			if (player.pos.x >= clearLine.x) {


			}

			// *****************アニメーション

			if (player.isAnimation) {

				player.animeCount--;
				if (player.animeCount <= 30) {

					player.screenPosX = 120;
					if (player.animeCount <= 25) {
						player.screenPosX = 60;

						if (player.animeCount <= 20) {
							player.screenPosX = 0;

							if (player.animeCount <= 0) {

								player.animeCount = 0;
								player.isAnimation = false;
							}
						}
					}
				}
			}
			else {

				player.animeCount++;
				if (player.animeCount >= 7) {

					player.screenPosX = 60;
					if (player.animeCount >= 14) {
						player.screenPosX = 120;

						if (player.animeCount >= 20) {
							player.screenPosX = 180;
							player.animeCount = 30;

						}
					}
				}
			}

#pragma region パーティクル
			// パーティクルの更新処理
			for (int i = 0; i < maxParticles; ++i) {
				if (particles[i].isActive) {
					particles[i].pos.x += particles[i].velocity.x;
					particles[i].pos.y += particles[i].velocity.y;

					particles[i].velocity.y += 0.4f;

					particles[i].lifeTime--;
					if (particles[i].lifeTime <= 0) {
						particles[i].isActive = false;
					}
				}
			}

#pragma endregion

#pragma endregion

#pragma region カメラ

			// カメラをスクロールさせる
			if (player.pos.x >= 1280.0f / 2.0f) {
				camera.targetPos.x = player.pos.x - (1280.0f / 2.0f);
			}

			// プレイヤーが初期座標の半分以下に来たらカメラをデフォルトに戻す
			if (player.pos.x <= 1280.0f / 2.0f) {
				camera.targetPos.x = 0.0f;
			}

			// カメライージング
			camera.pos = Lerp(camera.pos, camera.targetPos, 0.1f);

			camera.pos.x += wrand.x;
			camera.pos.y += wrand.y;

#pragma endregion

#pragma region 当たり判定

			//for (int i = 0; i < maxEnemy; i++) {
			//	if (isCollision(player.pos, enemies[i].pos, player.radius, enemies[i].radius)) {

			//		player.isAlive = false;
			//		//randMax = 21;
			//	}
			//}

#pragma endregion

#pragma region 描画

			//**************************背景
			Novice::DrawBox(0 - (int)camera.pos.x, 0 - (int)camera.pos.y,
				1280, 720, 0.0f, BLACK, kFillModeSolid);

			//**************************敵
			// 敵の描画
			for (int i = 0; i < maxEnemy; i++) {
				Novice::DrawEllipse((int)(enemies[i].pos.x - camera.pos.x), (int)(enemies[i].pos.y - camera.pos.y),
					(int)enemies[i].radius, (int)enemies[i].radius,
					0.0f, RED, kFillModeSolid);
			}

			for (int i = 0; i < maxEnemy; i++) {
				Novice::DrawEllipse((int)(enemiesBig[i].pos.x - camera.pos.x), (int)(enemiesBig[i].pos.y - camera.pos.y),
					(int)enemiesBig[i].radius, (int)enemiesBig[i].radius,
					0.0f, RED, kFillModeSolid);
			}

			//*************************カーソルの描画
			if (player.isAlive) {
				Novice::DrawEllipse(
					(int)(cursor.pos.x - camera.pos.x), (int)(cursor.pos.y - camera.pos.y),
					10, 10,
					0.0f, RED, kFillModeSolid
				);

				// プレイヤーとカーソルの間に小さな円を2つ描画
				Vector2 midPoint1 = Lerp(player.pos, cursor.pos, 0.33f);
				Vector2 midPoint2 = Lerp(player.pos, cursor.pos, 0.66f);

				Novice::DrawEllipse(
					(int)(midPoint1.x - camera.pos.x), (int)(midPoint1.y - camera.pos.y),
					5, 5,
					0.0f, RED, kFillModeSolid
				);

				Novice::DrawEllipse(
					(int)(midPoint2.x - camera.pos.x), (int)(midPoint2.y - camera.pos.y),
					5, 5,
					0.0f, RED, kFillModeSolid
				);

			}

			//*********************チェックポイント
			Novice::DrawLine((int)checkPoint.pos.x - (int)camera.pos.x, 0, (int)checkPoint.pos.x - (int)camera.pos.x, 720, RED);

			//**********************ゴール
			Novice::DrawLine((int)clearLine.x - (int)camera.pos.x, 0, (int)clearLine.x - (int)camera.pos.x, 720, WHITE);

			//**************************プレイヤーの描画
			if (player.isAlive) {

				Novice::DrawQuad(
					((int)player.pos.x - (int)player.radius) - (int)camera.pos.x, ((int)player.pos.y - (int)player.radius) - (int)camera.pos.y,
					((int)player.pos.x + (int)player.radius) - (int)camera.pos.x, ((int)player.pos.y - (int)player.radius) - (int)camera.pos.y,
					((int)player.pos.x - (int)player.radius) - (int)camera.pos.x, ((int)player.pos.y + (int)player.radius) - (int)camera.pos.y,
					((int)player.pos.x + (int)player.radius) - (int)camera.pos.x, ((int)player.pos.y + (int)player.radius) - (int)camera.pos.y,
					player.screenPosX, 0, 60, 60, player.th, 0xFFFFFFFF);

				Novice::DrawEllipse(
					(int)(player.pos.x - camera.pos.x), (int)(player.pos.y - camera.pos.y),
					(int)player.radius, (int)player.radius,
					0.0f, 0xFFFFFF00, kFillModeWireFrame
				);
			}

			//**********************パーティクル

			for (int i = 0; i < maxParticles; ++i) {
				if (particles[i].isActive) {
					Novice::DrawEllipse(
						(int)particles[i].pos.x - (int)camera.pos.x, (int)particles[i].pos.y - (int)camera.pos.y,
						(int)particles[i].radius, (int)particles[i].radius,
						0.0f, WHITE, kFillModeSolid
					);
				}
			}



			//************************デバッグ用
			Novice::ScreenPrintf(0, 20, "PlayerPosX : %.2f", player.pos.x);

			Novice::ScreenPrintf(0, 40, "CameraPosX : %.2f", camera.pos.x);

			Novice::ScreenPrintf(0, 60, "animeCount : %d", player.animeCount);

#pragma endregion

			break;
		case CLEAR:
			break;
		case END:
			break;
		}

		// デバッグ用
		Novice::ScreenPrintf(0, 0, "scene = %d", scene);
		Novice::DrawLine(0, 360, 1280, 360, GREEN);
		Novice::DrawLine(640, 0, 640, 720, GREEN);

		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}