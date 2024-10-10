#include <Novice.h>
#include <Vector2.h>
#include "Easing.h"
#include <math.h>
#define _USE_MATH_DEFINES
#include <XInput.h>
#include <cmath>
#include "Blend.h"

const char kWindowTitle[] = "GC1A_03_クサマリョウト_タイトル";

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

struct Enemy{
	Vector2 pos;
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

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, 1280, 720);

    // キー入力結果を受け取る箱
    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    enum Scene {
        START,
        PLAY,
        CLEAR,
        END

    }scene = PLAY;

    //*******************Enemy
    const int MaxEnemy = 100;

    Enemy enemyUpDown[MaxEnemy];

    for (int i = 0; i < MaxEnemy; i++) {
        enemyUpDown[i].pos = { 720.0f + (100 * i),300.0f };
        enemyUpDown[i].radius = 20;
    }

    Enemy enemy2;
    enemy2.pos.x = 400;
    enemy2.pos.y = 300;
    enemy2.radius = 20;

    Enemy enemy3;
    enemy3.pos.x = 700;
    enemy3.pos.y = 300;
    enemy3.radius = 20;

    float theta = (float)M_PI;
    float amplitude = 360.0f - enemy2.radius;

    //***************Player

    Player player{
        player.pos = {120.0f, 360.0f},
        player.targetPos = {120.0f, 360.0f},
        player.radius = 15.0f,
        player.checkPointPos = {120.0f, 360.0f},
        player.isAlive = true,
        player.respawCoolTime = 60,
        player.transparency = 255,
        player.isBlinking = false,
        player.blinkingTime = 60,
        player.th = Novice::LoadTexture("./Resources/PlayScene/player.png"),
        player.screenPosX = 135,
        player.isAnimation = false,
        player.animeCount = 60,
    };

    // 高速移動速度
    const float dashDistance = 300.0f;

    //*********チェックポイント
    CheckPoint checkPoint{
        checkPoint.pos = {1350.0f,0.0f},
    };

    // カーソルの初期設定
    Cursor cursor{
        {640.0f, 360.0f}
    };

    Camera camera{
        {0.0f, 0.0f},
        {0.0f, 0.0f}
    };

    int leftStickX = 0;
    int leftStickY = 0;
    const float easingSpeed = 0.2f;

    // XPAD用
    XINPUT_STATE state;

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

#pragma region enemy
            ///moving
            /// sin cos
            theta += float(M_PI) / 60.0f;

            //***************************** 動き

            ///UpDown
            for (int i = 0; i < 3; i++) {

                enemyUpDown[i].pos.y = sinf(theta) * amplitude + (720.0f / 2.0f);
            }

            ///Up Down
            enemy2.pos.y = sinf(theta) * amplitude + (720.0f / 2.0f);
            /// Circle
            /// Circle
            enemy3.pos.x = cosf(theta) * amplitude;
            enemy3.pos.y = sinf(theta) * amplitude;
            //*******************************

#pragma endregion

#pragma region Player

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
                }
            } else {

                // スティックが戻ったらカーソルをプレイヤーの位置に戻す
                cursor.pos = player.pos;
            }

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
            if(!player.isAlive) {
            
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
                
                // 点滅開始
                player.isBlinking = true;

                player.isAlive = true;
            }

            // ******************点滅
            if (player.isBlinking) {

                player.blinkingTime--;
                if (player.blinkingTime <= 0.0f) {
                    if (player.blinkingTime ) {

                        player.transparency;
                    }

                }
            }

            // *****************チェックポイント
            if (player.pos.x >= checkPoint.pos.x) {

                player.checkPointPos.x = checkPoint.pos.x;
            }

            // *****************アニメーション

            if (player.isAnimation) {

                player.animeCount--;
                if (player.animeCount <= 30) {

                    player.screenPosX = 90;
                    if (player.animeCount <= 25) {
                        player.screenPosX = 45;

                        if (player.animeCount <= 20) {
                            player.screenPosX = 0;
                         
                            if (player.animeCount <= 0) {

                                player.animeCount = 0;
                                player.isAnimation = false;
                            }
                        }
                    }
                }
            } else {

                player.animeCount++;
                if (player.animeCount >= 7) {

                    player.screenPosX = 45;
                    if (player.animeCount >= 14) {
                        player.screenPosX = 90;

                        if (player.animeCount >= 20) {
                            player.screenPosX = 135;
                            player.animeCount = 30;
                           
                        }
                    }
                }
            }


#pragma endregion

#pragma region camera

            // カメラをスクロールさせる
            if (player.pos.x >= 1280.0f / 2.0f) {
                camera.targetPos.x = player.pos.x - (1280.0f / 2.0f);
            }
            
            if (player.pos.x <= 1280.0f / 2.0f){
                camera.targetPos.x = 0.0f;
            }

            // カメライージング
            camera.pos = Lerp(camera.pos, camera.targetPos, 0.1f);

#pragma endregion

#pragma region IsCollision

            for (int i = 0; i < 3; i++) {
                if (isCollision(player.pos, enemyUpDown[i].pos, player.radius, enemyUpDown[i].radius)) {

                    player.isAlive = false;
                }
            }


#pragma endregion

            // 背景
            Novice::DrawBox(0 - (int)camera.pos.x, 0 - (int)camera.pos.y,
                1280, 720, 0.0f, BLACK, kFillModeSolid);

            // 敵
            //UpDown
            for (int i = 0; i < 3; i++) {
                Novice::DrawEllipse(
                    (int)enemyUpDown[i].pos.x - (int)camera.pos.x, (int)enemyUpDown[i].pos.y,
                    (int)enemyUpDown[i].radius, (int)enemyUpDown[i].radius, 0,
                    WHITE, kFillModeSolid);
            };

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

            //**************************プレイヤーの描画
            if (player.isAlive) {

                Novice::DrawQuad(
                    ((int)player.pos.x - (int)player.radius - 6) - (int)camera.pos.x, ((int)player.pos.y - (int)player.radius - 6) - (int)camera.pos.y,
                    ((int)player.pos.x + (int)player.radius + 6) - (int)camera.pos.x, ((int)player.pos.y - (int)player.radius - 6) - (int)camera.pos.y,
                    ((int)player.pos.x - (int)player.radius - 6) - (int)camera.pos.x, ((int)player.pos.y + (int)player.radius + 6) - (int)camera.pos.y,
                    ((int)player.pos.x + (int)player.radius + 6) - (int)camera.pos.x, ((int)player.pos.y + (int)player.radius + 6) - (int)camera.pos.y,
                    player.screenPosX, 0, 45, 45, player.th, 0xFFFFFFFF);
                
                Novice::DrawEllipse(
                    (int)(player.pos.x - camera.pos.x), (int)(player.pos.y - camera.pos.y),
                    (int)player.radius, (int)player.radius,
                    0.0f, 0x00000066, kFillModeWireFrame
                );
            }
            
            //************************デバッグ用
            Novice::ScreenPrintf(0, 20, "PlayerPosX : %.2f", player.pos.x);

            Novice::ScreenPrintf(0, 40, "CameraPosX : %.2f", camera.pos.x);

            Novice::ScreenPrintf(0, 60, "animeCount : %d", player.animeCount);

            break;
        case CLEAR:
            break;
        case END:
            break;
        }

        Novice::ScreenPrintf(0, 0, "scene = %d", scene);

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