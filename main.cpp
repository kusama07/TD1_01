#include <Novice.h>
#include <Vector2.h>
#include <cmath>
#include <XInput.h>
#include "Easing.h"

const char kWindowTitle[] = "GC1A_03_クサマリョウト_タイトル";

struct Player {
    Vector2 pos;
    Vector2 targetPos;  
    float radius;
};

struct Cursor {
    Vector2 pos;
};

struct Camera {
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
    } scene = PLAY;

    Player player{
        {120.0f, 360.0f},  
        {120.0f, 360.0f}, 
        15.0f             
    };

    // カーソルの初期設定
    Cursor cursor{
        {640.0f, 360.0f} 
    };

    Camera camera{
        {0.0f, 0.0f} 
    };

    int leftStickX = 0;
    int leftStickY = 0;

    // 高速移動速度
    const float dashDistance = 300.0f;
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

        switch (scene) {
        case START:
            // スペース押したらシーン切り替え
            if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0) {
                scene = PLAY;
            }
            break;


        case PLAY:

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

            //**************スクロール

            // カメラをスクロールさせる
            if (player.pos.x >= 1280.0f / 2.0f) {
                camera.pos.x = player.pos.x - (1280.0f / 2.0f);
            }

            //**********************

            // 背景
            Novice::DrawBox(0 - (int)camera.pos.x, 0 - (int)camera.pos.y, 1280, 720, 0.0f, BLACK, kFillModeSolid);

            // カーソルの描画
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

            // プレイヤーの描画
            Novice::DrawEllipse(
                (int)(player.pos.x - camera.pos.x), (int)(player.pos.y - camera.pos.y),
                (int)player.radius, (int)player.radius,
                0.0f, WHITE, kFillModeSolid
            );

            Novice::ScreenPrintf(0, 20, "PlayerPosX : %.2f", player.pos.x);

#pragma endregion

#pragma region IsCollision
#pragma endregion

            break;

        case CLEAR:
            break;
        case END:
            break;
        }

        Novice::ScreenPrintf(0, 0, "scene = %d", scene);

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
