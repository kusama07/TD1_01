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
	float moveFast = 15;
	float moveNormal = 25;
	float moveSlow = 50;

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
					/// stage 1
					enemies[32].pos = circleEnemy({ 150.0f, 100.0f }, 140.0f, enemies[32].theta, -moveNormal);
					enemies[33].pos = circleEnemy({ 150.0f, 100.0f }, 140.0f, enemies[33].theta, -moveSlow);
					enemies[34].pos = circleEnemy({ 150.0f, 100.0f }, 140.0f, enemies[34].theta, -moveFast);
					enemies[35].pos = circleEnemy({ 150.0f, 640.0f }, 140.0f, enemies[35].theta, moveFast);
					enemies[36].pos = circleEnemy({ 150.0f, 640.0f }, 140.0f, enemies[36].theta, moveSlow);
					enemies[37].pos = circleEnemy({ 150.0f, 640.0f }, 140.0f, enemies[37].theta, moveNormal);

					enemies[38].pos = circleEnemy({ 710.0f, 360.0f }, 140.0f, enemies[38].theta, moveNormal);
					enemies[39].pos = circleEnemy({ 710.0f, 360.0f }, 140.0f, enemies[39].theta, -moveNormal);

					enemies[40].pos = circleEnemy({ 500.0f, 140.0f }, 140.0f, enemies[40].theta, -moveFast);
					enemies[41].pos = circleEnemy({ 500.0f, 140.0f }, 140.0f, enemies[41].theta, -moveNormal);
					enemies[44].pos = circleEnemy({ 500.0f, 140.0f }, 140.0f, enemies[44].theta, -moveSlow);
					enemies[42].pos = circleEnemy({ 500.0f, 580.0f }, 140.0f, enemies[42].theta, moveFast);
					enemies[43].pos = circleEnemy({ 500.0f, 580.0f }, 140.0f, enemies[43].theta, moveNormal);
					enemies[45].pos = circleEnemy({ 500.0f, 580.0f }, 140.0f, enemies[45].theta, moveSlow);

					enemies[46].pos = circleEnemy({ 990.0f, 220.0f }, 140.0f, enemies[46].theta, moveSlow);
					enemies[47].pos = circleEnemy({ 990.0f, 220.0f }, 140.0f, enemies[47].theta, -moveSlow);
					enemies[48].pos = circleEnemy({ 990.0f, 500.0f }, 140.0f, enemies[48].theta, moveSlow);
					enemies[49].pos = circleEnemy({ 990.0f, 500.0f }, 140.0f, enemies[49].theta, -moveSlow);


					///ステージ2
					enemies[1].pos = circleEnemy({ stage2pos + 430.0f, 360.0f }, 160.0f, enemies[1].theta, moveNormal);
					enemies[2].pos = circleEnemy({ stage2pos + 430.0f, 80.0f }, 160.0f, enemies[2].theta, -moveNormal);
					enemies[3].pos = circleEnemy({ stage2pos + 710.0f, 80.0f }, 160.0f, enemies[3].theta, moveNormal);
					enemies[4].pos = circleEnemy({ stage2pos + 710.0f, 360.0f }, 160.0f, enemies[4].theta, -moveNormal);
					enemies[5].pos = circleEnemy({ stage2pos + 710.0f, 640.0f }, 160.0f, enemies[5].theta, moveNormal);
					enemies[6].pos = circleEnemy({ stage2pos + 990.0f, 640.0f }, 160.0f, enemies[6].theta, -moveNormal);
					enemies[7].pos = circleEnemy({ stage2pos + 990.0f, 360.0f }, 160.0f, enemies[7].theta, moveNormal);

					enemies[8].pos = circleEnemy({ stage2pos + 300.0f, 640.0f }, 160.0f, enemies[8].theta, moveNormal);
					enemies[9].pos = circleEnemy({ stage2pos + 150.0f, 80.0f }, 160.0f, enemies[9].theta, moveNormal);

					///stage3
					enemies[10].pos = circleEnemy({ stage3pos + 270.0f, 360.0f }, 140.0f, enemies[10].theta, moveNormal);
					enemies[11].pos = circleEnemy({ stage3pos + 270.0f, 80.0f }, 140.0f, enemies[11].theta, -moveNormal);
					enemies[12].pos = circleEnemy({ stage3pos + 550.0f, 80.0f }, 140.0f, enemies[12].theta, moveSlow);
					enemies[13].pos = circleEnemy({ stage3pos + 830.0f, 80.0f }, 140.0f, enemies[13].theta, -moveNormal);
					enemies[14].pos = circleEnemy({ stage3pos + 1100.0f, 80.0f }, 140.0f, enemies[14].theta, moveSlow);
					enemies[15].pos = circleEnemy({ stage3pos + 1390.0f, 80.0f }, 140.0f, enemies[15].theta, -moveNormal);
					enemies[16].pos = circleEnemy({ stage3pos + 1390.0f, 360.0f }, 140.0f, enemies[16].theta, moveSlow);
					enemies[17].pos = circleEnemy({ stage3pos + 1100.0f, 360.0f }, 140.0f, enemies[17].theta, -moveNormal);
					enemies[18].pos = circleEnemy({ stage3pos + 830.0f, 360.0f }, 140.0f, enemies[18].theta, moveSlow);
					enemies[19].pos = circleEnemy({ stage3pos + 550.0f, 360.0f }, 140.0f, enemies[19].theta, -moveNormal);
					enemies[20].pos = circleEnemy({ stage3pos + 550.0f, 640.0f }, 140.0f, enemies[20].theta, moveSlow);
					enemies[21].pos = circleEnemy({ stage3pos + 830.0f, 640.0f }, 140.0f, enemies[21].theta, -moveNormal);
					enemies[22].pos = circleEnemy({ stage3pos + 1100.0f, 640.0f }, 140.0f, enemies[22].theta, moveSlow);
					enemies[23].pos = circleEnemy({ stage3pos + 1390.0f, 640.0f }, 140.0f, enemies[23].theta, -moveNormal);

					enemies[24].pos = circleEnemy({ stage3pos + 690.0f, 500.0f }, 280.0f, enemies[24].theta, moveSlow); /// the big circular
					enemies[25].pos = circleEnemy({ stage3pos + 970.0f, 500.0f }, 280.0f, enemies[25].theta, -moveSlow); /// the big circular
					enemies[26].pos = circleEnemy({ stage3pos + 1250.0f, 500.0f }, 280.0f, enemies[26].theta, moveSlow); /// the big circular
					enemies[27].pos = circleEnemy({ stage3pos + 1530.0f, 500.0f }, 280.0f, enemies[27].theta, -moveSlow); /// the big circular

					enemies[28].pos = circleEnemy({ stage3pos + 690.0f, 780.0f }, 280.0f, enemies[28].theta, -moveSlow); /// the big circular
					enemies[29].pos = circleEnemy({ stage3pos + 970.0f, 780.0f }, 280.0f, enemies[29].theta, moveSlow); /// the big circular
					enemies[30].pos = circleEnemy({ stage3pos + 1250.0f, 780.0f }, 280.0f, enemies[30].theta, -moveSlow); /// the big circular
					enemies[31].pos = circleEnemy({ stage3pos + 1530.0f, 780.0f }, 280.0f, enemies[31].theta, moveSlow); /// the big circular


					/*enemies[32].pos = circleEnemy({ 960.0f, 0.0f }, 150.0f, enemies[32].theta, moveNormal);
					enemies[33].pos = circleEnemy({ 150.0f, 360.0f }, 150.0f, enemies[33].theta, moveNormal);
					enemies[34].pos = circleEnemy({ 480.0f, 540.0f }, 150.0f, enemies[34].theta, moveNormal);
					enemies[35].pos = circleEnemy({ 800.0f, 540.0f }, 150.0f, enemies[35].theta, moveNormal);
					enemies[36].pos = circleEnemy({ 960.0f, 360.0f }, 150.0f, enemies[36].theta, moveNormal);*/
					///ステージ4
					enemies[50].pos = circleEnemy({ stage4pos + 840.0f , 360.0f }, 150.0f, enemies[50].theta, moveFast); ///inner
					enemies[51].pos = circleEnemy({ stage4pos + 840.0f , 360.0f }, 200.0f, enemies[51].theta, -moveSlow);
					enemies[52].pos = circleEnemy({ stage4pos + 840.0f , 360.0f }, 250.0f, enemies[52].theta, moveSlow);
					//enemies[53].pos = circleEnemy({ stage4pos + 840.0f , 360.0f }, 300.0f, enemies[53].theta, -moveNormal);
					//enemies[54].pos = circleEnemy({ stage4pos + 840.0f , 360.0f }, 350.0f, enemies[54].theta, moveNormal);
					enemies[55].pos = circleEnemy({ stage4pos + 840.0f , 360.0f }, 400.0f, enemies[55].theta, -moveSlow);
					enemies[56].pos = circleEnemy({ stage4pos + 840.0f , 360.0f }, 450.0f, enemies[56].theta, moveSlow);
					enemies[57].pos = circleEnemy({ stage4pos + 840.0f , 360.0f }, 500.0f, enemies[57].theta, -moveNormal); ///outer*/

				}
				else if (enemies[i].enemyType == 1) {
					// 縦方向に往復する敵
					/// stage1
					//enemies[32].pos = verticalEnemy({ 1530.0f, 780.0f }, 280.0f, enemies[32].theta, moveNormal);

					///ステージ2
					enemiesBig[8].pos = verticalEnemy({ stage2pos + 330.0f, 370.0f }, 100.0f, enemiesBig[8].theta, moveSlow); /// wall
					enemiesBig[9].pos = verticalEnemy({ stage2pos + 330.0f, 490.0f }, 100.0f, enemiesBig[9].theta, moveNormal); /// wall
					enemiesBig[10].pos = verticalEnemy({ stage2pos + 330.0f, 610.0f }, 100.0f, enemiesBig[10].theta, moveFast); /// wall
					enemiesBig[11].pos = verticalEnemy({ stage2pos + 330.0f, 730.0f }, 100.0f, enemiesBig[11].theta, moveNormal); /// wall
					enemiesBig[12].pos = verticalEnemy({ stage2pos + 330.0f, 850.0f }, 100.0f, enemiesBig[12].theta, moveSlow); /// wall

					enemiesBig[13].pos = verticalEnemy({ stage2pos + 810.0f, -140.0f }, 100.0f, enemiesBig[13].theta, moveFast); /// wall 2
					enemiesBig[14].pos = verticalEnemy({ stage2pos + 810.0f, -20.0f }, 100.0f, enemiesBig[14].theta, moveSlow); /// wall 2
					enemiesBig[15].pos = verticalEnemy({ stage2pos + 810.0f, 100.0f }, 100.0f, enemiesBig[15].theta, moveNormal); /// wall 2
					enemiesBig[16].pos = verticalEnemy({ stage2pos + 810.0f, 220.0f }, 100.0f, enemiesBig[16].theta, moveSlow); /// wall 2
					enemiesBig[17].pos = verticalEnemy({ stage2pos + 810.0f, 340.0f }, 100.0f, enemiesBig[17].theta, moveFast); /// wall 2

					enemiesBig[18].pos = verticalEnemy({ stage2pos + 1130.0f, 340.0f }, 100.0f, enemiesBig[18].theta, moveNormal); /// wall 3
					enemiesBig[19].pos = verticalEnemy({ stage2pos + 1130.0f, 460.0f }, 100.0f, enemiesBig[19].theta, moveFast); /// wall 3
					enemiesBig[20].pos = verticalEnemy({ stage2pos + 1130.0f, 580.0f }, 100.0f, enemiesBig[20].theta, moveSlow); /// wall 3
					enemiesBig[21].pos = verticalEnemy({ stage2pos + 1130.0f, 700.0f }, 100.0f, enemiesBig[21].theta, moveNormal); /// wall 3
					enemiesBig[22].pos = verticalEnemy({ stage2pos + 1130.0f, 820.0f }, 100.0f, enemiesBig[22].theta, moveSlow); /// wall 3

					enemiesBig[23].pos = verticalEnemy({ stage2pos + 1130.0f, -140.0f }, 100.0f, enemiesBig[23].theta, moveSlow); /// wall 3 up
					enemiesBig[24].pos = verticalEnemy({ stage2pos + 1130.0f, -20.0f }, 100.0f, enemiesBig[24].theta, moveFast); /// wall 3
					enemiesBig[25].pos = verticalEnemy({ stage2pos + 1130.0f, 100.0f }, 100.0f, enemiesBig[25].theta, moveFast); /// wall 3
					enemiesBig[26].pos = verticalEnemy({ stage2pos + 1130.0f, 220.0f }, 100.0f, enemiesBig[26].theta, moveNormal); /// wall 3

					///stage3
					enemiesBig[31].pos = verticalEnemy({ stage3pos + 410.0f, 430.0f }, 200.0f, enemiesBig[31].theta, moveNormal); /// wall 3
					enemiesBig[32].pos = verticalEnemy({ stage3pos + 410.0f, 430.0f }, 200.0f, enemiesBig[32].theta, -moveNormal); /// wall 3
					enemiesBig[33].pos = verticalEnemy({ stage3pos + 410.0f, 430.0f }, 100.0f, enemiesBig[33].theta, moveNormal); /// wall 3
					enemiesBig[34].pos = verticalEnemy({ stage3pos + 410.0f, 430.0f }, 100.0f, enemiesBig[34].theta, -moveNormal); /// wall 3


					///stage4
					enemiesBig[27].pos = verticalEnemy({ stage4pos + 250.0f, 220.0f }, 100.0f, enemiesBig[27].theta, moveNormal); /// wall 3
					enemiesBig[28].pos = verticalEnemy({ stage4pos + 250.0f, 220.0f }, 100.0f, enemiesBig[28].theta, -moveNormal); /// wall 3
					enemiesBig[29].pos = verticalEnemy({ stage4pos + 250.0f, 220.0f }, 200.0f, enemiesBig[29].theta, moveNormal); /// wall 3
					enemiesBig[30].pos = verticalEnemy({ stage4pos + 250.0f, 220.0f }, 200.0f, enemiesBig[30].theta, -moveNormal); /// wall 3

					enemiesBig[38].pos = verticalEnemy({ stage4pos + 250.0f, 220.0f }, 300.0f, enemiesBig[38].theta, moveSlow); /// wall 3
					enemiesBig[45].pos = verticalEnemy({ stage4pos + 250.0f, 220.0f }, 300.0f, enemiesBig[45].theta, -moveSlow); /// wall 3




					///ステージ３
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
					/// ステージ１
					/*enemies[9].pos = horizonEnemy({ 160.0f,0.0f }, 150.0f, enemies[9].theta, moveSlow);
					enemies[10].pos = horizonEnemy({ 1120.0f,0.0f }, 150.0f, enemies[10].theta, moveSlow);
					enemies[11].pos = horizonEnemy({ 960.0f,160.0f }, 300.0f, enemies[11].theta, moveSlow);
					enemies[12].pos = horizonEnemy({ 960.0f,540.0f }, 300.0f, enemies[12].theta, moveSlow);
					enemies[13].pos = horizonEnemy({ 160.0f,1280.0f }, 300.0f, enemies[13].theta, moveSlow);
					enemies[14].pos = horizonEnemy({ 1120.0f,1280.0f }, 300.0f, enemies[14].theta, moveSlow);*/
					///stage 3
					enemiesBig[1].pos = horizonEnemy({ stage3pos + 250.0f, 480.0f }, 250.0f, enemiesBig[1].theta, moveNormal); /// wall 3
					enemiesBig[2].pos = horizonEnemy({ stage3pos + 250.0f, 480.0f }, 250.0f, enemiesBig[2].theta, -moveNormal); /// wall 3
					enemiesBig[3].pos = horizonEnemy({ stage3pos + 250.0f, 480.0f }, 100.0f, enemiesBig[3].theta, moveNormal); /// wall 3
					enemiesBig[4].pos = horizonEnemy({ stage3pos + 250.0f, 480.0f }, 100.0f, enemiesBig[4].theta, -moveNormal); /// wall 3

					enemiesBig[5].pos = horizonEnemy({ stage3pos + 830.0f, 220.0f }, 400.0f, enemiesBig[5].theta, moveNormal); /// wall 3
					enemiesBig[6].pos = horizonEnemy({ stage3pos + 830.0f, 220.0f }, 400.0f, enemiesBig[6].theta, -moveNormal); /// wall 3
					enemiesBig[7].pos = horizonEnemy({ stage3pos + 830.0f, 220.0f }, 250.0f, enemiesBig[7].theta, moveNormal); /// wall 3
					enemiesBig[35].pos = horizonEnemy({ stage3pos + 830.0f, 220.0f }, 250.0f, enemiesBig[35].theta, -moveNormal); /// wall 3
					enemiesBig[36].pos = horizonEnemy({ stage3pos + 830.0f, 220.0f }, 100.0f, enemiesBig[36].theta, moveNormal); /// wall 3
					enemiesBig[37].pos = horizonEnemy({ stage3pos + 830.0f, 220.0f }, 100.0f, enemiesBig[37].theta, -moveNormal); /// wall 3

					enemiesBig[39].pos = horizonEnemy({ stage3pos + 1090.0f, 480.0f }, 300.0f, enemiesBig[39].theta, moveNormal); /// wall 3
					enemiesBig[40].pos = horizonEnemy({ stage3pos + 1090.0f, 480.0f }, 300.0f, enemiesBig[40].theta, -moveNormal); /// wall 3
					enemiesBig[41].pos = horizonEnemy({ stage3pos + 1090.0f, 480.0f }, 200.0f, enemiesBig[41].theta, moveNormal); /// wall 3
					enemiesBig[42].pos = horizonEnemy({ stage3pos + 1090.0f, 480.0f }, 200.0f, enemiesBig[42].theta, -moveNormal); /// wall 3
					enemiesBig[43].pos = horizonEnemy({ stage3pos + 1090.0f, 480.0f }, 100.0f, enemiesBig[43].theta, moveNormal); /// wall 3
					enemiesBig[44].pos = horizonEnemy({ stage3pos + 1090.0f, 480.0f }, 100.0f, enemiesBig[44].theta, -moveSlow); /// wall 3


					/// ステージ３
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