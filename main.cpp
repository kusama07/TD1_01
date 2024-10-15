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

	theta += float(M_PI) / velocity; ///

	result.x = cosf(theta + (float)M_PI) * amplitude + pos.x;
	result.y = sinf(theta + (float)M_PI) * amplitude + pos.y;

	return result;
}

Vector2 verticalEnemy(Vector2 pos, float amplitude, float &theta, float velocity) {
	Vector2 result;

	theta += float(M_PI) / velocity; ///

	result.x = pos.x;
	result.y = sinf(theta) * amplitude + pos.y;

	return result;
}

Vector2 horizonEnemy(Vector2 pos, float amplitude, float &theta, float velocity) {
	Vector2 result;

	theta += float(M_PI) / velocity; ///

	result.x = sinf(theta) * amplitude + pos.x;
	result.y = pos.y;

	return result;
}

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
		enemies[i].theta = 0.0f;
		enemies[i].amplitude = 150.0f;
		enemies[i].radius = 25.0f;
	}

	//int stage2x = 1280;

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
	const float dashDistance = 300.0f;

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
					///ステージ１
					/*enemies[0].pos = circleEnemy({ 320.0f, 0.0f }, 150.0f, enemies[i].theta);
					enemies[1].pos = circleEnemy({ 640.0f, 180.0f }, 150.0f, enemies[i].theta);
					enemies[2].pos = circleEnemy({ 960.0f, 0.0f }, 150.0f, enemies[i].theta);
					enemies[3].pos = circleEnemy({ 150.0f, 360.0f }, 150.0f, enemies[i].theta);
					enemies[4].pos = circleEnemy({ 480.0f, 540.0f }, 150.0f, enemies[i].theta);
					enemies[5].pos = circleEnemy({ 800.0f, 540.0f }, 150.0f, enemies[i].theta);
					enemies[6].pos = circleEnemy({ 960.0f, 360.0f }, 150.0f, enemies[i].theta);*/
					///ステージ２
					/*enemies[15].pos = circleEnemy({ 640.0f , 360.0f }, 100.0f, enemies[i].theta, 300);
					enemies[16].pos = circleEnemy({ 640.0f , 360.0f }, 200.0f, enemies[i].theta, 300);
					enemies[17].pos = circleEnemy({ 640.0f , 360.0f }, 300.0f, enemies[i].theta, 300);
					enemies[18].pos = circleEnemy({ 640.0f , 360.0f }, 400.0f, enemies[i].theta, 300);
					enemies[19].pos = circleEnemy({ 640.0f , 360.0f }, 500.0f, enemies[i].theta, 300);
					enemies[20].pos = circleEnemy({ 640.0f , 360.0f }, 600.0f, enemies[i].theta, 300);*/

				}
				else if (enemies[i].enemyType == 1) {
					// 縦方向に往復する敵
					///ステージ１
					/*enemies[7].pos = verticalEnemy({ 480.0f, 360.0f }, 150.0f, enemies[i].theta);
					enemies[8].pos = verticalEnemy({ 640.0f, 360.0f }, 150.0f, enemies[i].theta);*/
					///ステージ３
					enemies[21].pos = verticalEnemy({ 160.0f, 180.0f }, 150.0f, enemies[i].theta, 600);
					enemies[22].pos = verticalEnemy({ 640.0f, 180.0f }, 150.0f, enemies[i].theta, 600);
					enemies[23].pos = verticalEnemy({ 800.0f, 180.0f }, 150.0f, enemies[i].theta, 600);
					enemies[24].pos = verticalEnemy({ 960.0f, 180.0f }, 150.0f, enemies[i].theta, 600); ///up
					enemies[25].pos = verticalEnemy({ 80.0f, 360.0f }, 150.0f, enemies[i].theta, 600);
					enemies[26].pos = verticalEnemy({ 80.0f, 630.0f }, 100.0f, enemies[i].theta, 600);
					enemies[27].pos = verticalEnemy({ 160.0f, 540.0f }, 150.0f, enemies[i].theta, 600); ///down
					enemies[28].pos = verticalEnemy({ 320.0f, 540.0f }, 150.0f, enemies[i].theta, 600);
					enemies[29].pos = verticalEnemy({ 480.0f, 540.0f }, 150.0f, enemies[i].theta, 600);
					enemies[30].pos = verticalEnemy({ 640.0f, 540.0f }, 150.0f, enemies[i].theta, 600);
					enemies[31].pos = verticalEnemy({ 800.0f, 540.0f }, 150.0f, enemies[i].theta, 600);
					enemies[32].pos = verticalEnemy({ 880.0f, 400.0f }, 150.0f, enemies[i].theta, 600);
					enemies[33].pos = verticalEnemy({ 960.0f, 400.0f }, 150.0f, enemies[i].theta, 600);

				}
				else if (enemies[i].enemyType == 2) {
					// 横方向に往復する敵
					/*enemies[9].pos = horizonEnemy({ 160.0f,0.0f }, 150.0f, enemies[i].theta);
					enemies[10].pos = horizonEnemy({ 1120.0f,0.0f }, 150.0f, enemies[i].theta);
					enemies[11].pos = horizonEnemy({ 960.0f,160.0f }, 300.0f, enemies[i].theta);
					enemies[12].pos = horizonEnemy({ 960.0f,540.0f }, 300.0f, enemies[i].theta);
					enemies[13].pos = horizonEnemy({ 160.0f,1280.0f }, 300.0f, enemies[i].theta);
					enemies[14].pos = horizonEnemy({ 1120.0f,1280.0f }, 300.0f, enemies[i].theta);*/

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

			for (int i = 0; i < maxEnemy; i++) {
				if (isCollision(player.pos, enemies[i].pos, player.radius, enemies[i].radius)) {

					player.isAlive = false;
					//randMax = 21;
				}
			}

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