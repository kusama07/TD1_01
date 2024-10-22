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
    float randomAngle1;
};

struct Enemy {
    Vector2 pos;  
    int enemyType; // 敵の種類 (0: 円, 1: 縦方向, 2: 横方向)
    float theta;     
    float amplitude;
    float radius;
    int animeCount;
    int screenPosX;
};

struct Cursor {
    Vector2 pos;
    int th;
    int midCursorTh;
};

struct Camera {
    Vector2 pos; 
    Vector2 targetPos;
};

struct CheckPoint {
    Vector2 pos;
    int th;
    int w;
    int h;
    int animeCount;
    int screenPosX;
    bool isAction;
};

struct BackGround {
    Vector2 pos;
    int th;
};

bool isCollision(Vector2 aPos, Vector2 bPos, float aRadius, float bRadius) {
    float dx = aPos.x - bPos.x;
    float dy = aPos.y - bPos.y;
    float distance = sqrtf(dx * dx + dy * dy);
    return distance < (aRadius + bRadius);
}

Vector2 circleEnemy(Vector2 pos, float amplitude, float& theta, float velocity) {
    Vector2 result;

    theta += (float)M_PI / (velocity * 100.0f); 

    result.x = cosf(theta) * amplitude + pos.x;
    result.y = sinf(theta) * amplitude + pos.y;

    return result;
}

Vector2 verticalEnemy(Vector2 pos, float amplitude, float& theta, float velocity){
    Vector2 result;

    theta += float(M_PI) / (velocity * 100.0f);

    result.x = pos.x;
    result.y = sinf(theta) * amplitude + pos.y;

    return result;
}

Vector2 horizonEnemy(Vector2 pos,float amplitude, float& theta, float velocity) {
    Vector2 result;

    theta += float(M_PI) / (velocity * 100.0f);

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

#pragma region タイトル用の変数

    //画像
    int titleBackgroundGraph = Novice::LoadTexture("./Resources/Images/titleBackground.png");
    int titleLogoGraph = Novice::LoadTexture("./Resources/Images/titleLogo.png");

    //ロゴの画像の切り取り開始位置
    int titleLogoCutX = 1280 * 3;

    //フェードイン・アウト
    int titleFadeInOut = 255;
    int titleFadeTimer = 0;

    //ボタンが押されたか確認する変数
    bool gameIsStart = false;

    //タイトル画面用のタイマー
    int titleTimer = 0;


#pragma endregion

#pragma region シーン
    enum Scene {
        START,
        PLAY,
        CLEAR,
        END

    }scene = START;

#pragma endregion

#pragma region 敵

    const int maxEnemy = 100;

    Enemy enemies[maxEnemy];

    for (int i = 0; i < maxEnemy; i++) {
        enemies[i].pos = { -100.0f, 0.0f };
        enemies[i].enemyType = i % 3;
        enemies[i].theta = 0;
        enemies[i].amplitude = 150.0f;
        enemies[i].radius = 40.0f;
        enemies[i].screenPosX = 0;
        enemies[i].animeCount = rand() % 60;
    }

    Enemy enemiesBig[maxEnemy];

    for (int i = 0; i < maxEnemy; i++) {
        enemiesBig[i].pos = { -100.0f, 0.0f };
        enemiesBig[i].enemyType = i % 3;
        enemiesBig[i].theta = 0;
        enemiesBig[i].amplitude = 150.0f;
        enemiesBig[i].radius = 60.0f;
        enemiesBig[i].screenPosX = 0;
        enemiesBig[i].animeCount = rand() % 60;
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

    int enemyTh = Novice::LoadTexture("./Resources/PlayScene/enemyMedium.png");
    int enemyBigTh = Novice::LoadTexture("./Resources/PlayScene/enemyLage.png");

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
        player.animeCount = 0,
    };

    // 高速移動速度
    const float dashDistance = 280.0f;

#pragma endregion 

#pragma region パーティくる

    // パーティクルの最大数
    const int maxParticles = 100;
    Particle particles[maxParticles];

    for (int i = 0; i < maxParticles; ++i) {
        particles[i].pos = { player.pos.x, player.pos.y };
        particles[i].velocity = { (rand() % 200 - 100) / 100.0f, 5.0f };
        particles[i].radius = 10.0f;
        particles[i].isActive = false;
        particles[i].lifeTime = 0;
        particles[i].baseAngle = 0.0f;
        particles[i].randomAngle = 0.0f;
        particles[i].randomAngle1 = 0.0f;
    }
    const int particlesToGenerate = 5; // 生成するパーティクルの数

    int particlesTh = Novice::LoadTexture("./Resources/PlayScene/leaf.png");

#pragma endregion 

#pragma region チェックポイント

    //*********チェックポイント
    const int MaxCheckPoint = 3;
    CheckPoint checkPoint[MaxCheckPoint];

    for (int i = 0; i < MaxCheckPoint; i++) {
        checkPoint[i].th = Novice::LoadTexture("./Resources/PlayScene/checkpoint.png");
        checkPoint[i].w = 200 / 2;
        checkPoint[i].h = 720 / 2;
        checkPoint[i].animeCount = 45;
        checkPoint[i].screenPosX = 0;
        checkPoint[i].isAction = false;
    }
    checkPoint[0].pos = { 1430.0f,720.0f / 2.0f };
    checkPoint[1].pos = { 2550.0f,720.0f / 2.0f };
    checkPoint[2].pos = { 3860.0f,720.0f / 2.0f };

    Vector2 clearLine = { 1280.0f * 5.0f , 0.0f };
    int clearLineTh = Novice::LoadTexture("./Resources/PlayScene/goalLighting.png");

#pragma endregion

#pragma region カーソル

    // カーソルの初期設定
    Cursor cursor{
        {640.0f, 360.0f},
        Novice::LoadTexture("./Resources/PlayScene/cursor.png"),
        Novice::LoadTexture("./Resources/PlayScene/cursorMidpoint.png"),
    };
#pragma region Clear画面用の変数
    enum ClearScene {
        EYE,
        ENDING
    }clearScene = EYE;

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
    const float easingSpeed = 0.1f;

    // XPAD用
    XINPUT_STATE state;

    bool isInput = false;

#pragma endregion

    //シェイク
    Vector2 wrand = { 0.0f };
    int randMax = 0;

    int whiteOutAlpha = 0;

    //背景
    BackGround innermostBg[3];

    innermostBg[0].pos = { 0.0f, -20.0f };
    innermostBg[1].pos = { 1280.0f , -20.0f };
    innermostBg[2].pos = { -1280.0f , -20.0f };

    innermostBg[0].th = Novice::LoadTexture("./Resources/PlayScene/background1_1.png");
    innermostBg[1].th = Novice::LoadTexture("./Resources/PlayScene/background1_2.png");
    innermostBg[2].th = Novice::LoadTexture("./Resources/PlayScene/background1_2.png");

    //
    BackGround middleBg[3];

    middleBg[0].pos = { 0.0f, -20.0f };
    middleBg[1].pos = { 1280.0f , -20.0f };
    middleBg[2].pos = { -1280.0f , -20.0f };

    middleBg[0].th = Novice::LoadTexture("./Resources/PlayScene/background2_1.png");
    middleBg[1].th = Novice::LoadTexture("./Resources/PlayScene/background2_2.png");
    middleBg[2].th = Novice::LoadTexture("./Resources/PlayScene/background2_2.png");

    //
    BackGround frontmostBg[3];

    frontmostBg[0].pos = { 0.0f, -20.0f };
    frontmostBg[1].pos = {1280.0f , -20.0f};
    frontmostBg[2].pos = { 2560.0f , -20.0f };

    frontmostBg[0].th = Novice::LoadTexture("./Resources/PlayScene/background3_1.png");
    frontmostBg[1].th = Novice::LoadTexture("./Resources/PlayScene/background3_2.png");
    frontmostBg[2].th = Novice::LoadTexture("./Resources/PlayScene/background3_3.png");

    int playSceneBlackOut = 255;
    
	int clearTimer = 0;
	int clearFadeInOut = 255;
	unsigned int clearColor = WHITE;
	int isClearEnd = false;
	int clearEndTimer = 0;

	int clearPlayerAnime = 0;
	float clearZoom = 0.0f;
	int clearBackground = Novice::LoadTexture("./Resources/Images/gameClearBack.png");
	int clearEye = Novice::LoadTexture("./Resources/Images/eye.png");
	int clearPlayerGraph = Novice::LoadTexture("./Resources/Images/clearPlayer.png");
	int clearLogo = Novice::LoadTexture("./Resources/Images/clearLogo.png");
#pragma endregion

#pragma region 音
    int titleBGM = Novice::LoadAudio("./Resources/Sound/title.mp3");
    int gameBGM2 = Novice::LoadAudio("./Resources/Sound/game2.mp3");
    int gameBGM = Novice::LoadAudio("./Resources/Sound/game.mp3");
    int damageSound = Novice::LoadAudio("./Resources/Sound/damage.mp3");
    int dashSound = Novice::LoadAudio("./Resources/Sound/dash.mp3");
    int chackPointSound = Novice::LoadAudio("./Resources/Sound/chackPoint.mp3");
    int startSound = Novice::LoadAudio("./Resources/Sound/start.mp3");
    int gameClearSound = Novice::LoadAudio("./Resources/Sound/gameClear.mp3");
    int clearBGM = Novice::LoadAudio("./Resources/Sound/clear.mp3");
    int clearBGM2 = Novice::LoadAudio("./Resources/Sound/clearSound.mp3");
    int clearGrith = Novice::LoadAudio("./Resources/Sound/clearGrith.mp3");
    int clearGrithPlay = -1;
    int titlePlay = -1;
    int gamePlayBGM = -1;
    int gamePlayBGM2 = -1;
    int clearPlay = -1;
    int clearPlay2 = -1;
    int gameClearPlay = -1;
    float gameVolume = 0.3f;
#pragma endregion

    int ruleTh = Novice::LoadTexture("./Resources/PlayScene/setsumei.png");

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

#pragma region タイトル画面

#pragma region 更新処理

            //タイトル画面用のタイマーを加算
            titleTimer++;

            //音楽を流す
            if (!Novice::IsPlayingAudio(titlePlay) || titlePlay == -1) {
                //Novice::StopAudio(openingPlay);
                titlePlay = Novice::PlayAudio(titleBGM, true, gameVolume);
            }

            //ボタンが押されたか
            if (gameIsStart) {
                titleFadeTimer++;
                //音量調節
                Novice::SetAudioVolume(titlePlay, gameVolume);
                if (gameVolume > 0.0f) {
                    gameVolume -= 0.005f;
                }
                //タイトルロゴのアニメーション
                if (titleFadeTimer % 5 == 0) {
                    titleLogoCutX += 1280;
                }
                //フェードイン・アウト
                if (titleFadeInOut < 255) {
                    titleFadeInOut += 5;

                }

                //ボタンが押されてから一定時間後
                if (titleFadeTimer > 140) {
                    //タイトル用変数のリセット
                    titleLogoCutX = 1280 * 3;
                    titleFadeInOut = 255;
                    titleFadeTimer = 0;
                    gameIsStart = false;
                    titleTimer = 0;
                    gameVolume = 0.3f;
                    Novice::StopAudio(titlePlay);
                    //ゲーム画面へ
                    scene = PLAY;
                }
            } else {
                //フェードイン・アウト
                if (titleFadeInOut > 0) {
                    if (titleTimer > 50) {
                        titleFadeInOut -= 5;
                    }
                }
                //タイトルロゴのアニメーション
                if (titleLogoCutX > 0) {

                    if (titleTimer > 150 && titleTimer % 5 == 0) {
                        titleLogoCutX -= 1280;
                    }
                } else {
                    //キーが押されるとスタート
                    for (int i = 0; i < 256; i++) {
                        if (keys[i] && preKeys[i] == 0) {
                            gameIsStart = true;
                            //効果音
                            Novice::PlayAudio(startSound, false, 0.7f);
                        }
                    }

                    //ボタンが押されるとスタート
                    if (Novice::IsTriggerButton(0, kPadButton0) || Novice::IsTriggerButton(0, kPadButton1) ||
                        Novice::IsTriggerButton(0, kPadButton2) || Novice::IsTriggerButton(0, kPadButton3) ||
                        Novice::IsTriggerButton(0, kPadButton4) || Novice::IsTriggerButton(0, kPadButton5) ||
                        Novice::IsTriggerButton(0, kPadButton6) || Novice::IsTriggerButton(0, kPadButton7) ||
                        Novice::IsTriggerButton(0, kPadButton8) || Novice::IsTriggerButton(0, kPadButton9) ||
                        Novice::IsTriggerButton(0, kPadButton10) || Novice::IsTriggerButton(0, kPadButton11)) {
                        gameIsStart = true;
                        //効果音
                        Novice::PlayAudio(startSound, false, 0.7f);
                    }


                }
            }

            // 初期化
            player.pos = { 150.0f, 360.0f };
            
            playSceneBlackOut = 255;
            whiteOutAlpha = 0;
            player.isAnimation = false;
            // カーソル初期化
            cursor.pos = player.pos;
            player.targetPos = { 150.0f, 360.0f };

            // プレイヤーの位置
            player.pos = player.targetPos;

            // カメラの位置
            camera.pos = { 0.0f };
            camera.targetPos.x = player.pos.x - 120.0f;

            player.animeCount = 0;
            player.screenPosX = 0;

            for (int i = 0; i < MaxCheckPoint;i++) {
                checkPoint[i].isAction = false;
                checkPoint[i].animeCount = 0;
            }
#pragma endregion

#pragma region 描画処理
            //背景
            Novice::DrawSprite(0, 0, titleBackgroundGraph, 1.0f, 1.0f, 0.0f, WHITE);

            //タイトルロゴ
            if (titleTimer > 150) {
                Novice::DrawSpriteRect(0, 0, titleLogoCutX, 0, 1280, 720, titleLogoGraph, 1.0f / 4.0f, 1.0f, 0.0f, WHITE);
            }


            //フェードイン・アウト
            if (titleFadeInOut > 0) {
                Novice::DrawBox(0, 0, 1280, 720, 0.0f, 0x00000000 + titleFadeInOut, kFillModeSolid);
            }

#pragma endregion
            break;
#pragma endregion

        case PLAY:
#pragma region 音を流す
            //音楽を流す
            if (!Novice::IsPlayingAudio(gamePlayBGM) || gamePlayBGM == -1) {
                //Novice::StopAudio(openingPlay);
                gamePlayBGM = Novice::PlayAudio(gameBGM, true, gameVolume);
            }
            if (!Novice::IsPlayingAudio(gamePlayBGM2) || gamePlayBGM2 == -1) {
                //Novice::StopAudio(openingPlay);
                gamePlayBGM2 = Novice::PlayAudio(gameBGM2, true, gameVolume+0.3f);
            }
#pragma endregion


            playSceneBlackOut--;
            if (playSceneBlackOut <= 0) {
                playSceneBlackOut = 0;
            }
            
            if (playSceneBlackOut <= 140) {
                isInput = true;
            }

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
                if (enemies[i].enemyType == 0) {
                    // 円状に動く敵
                    /// ステージ 1
                    enemies[56].pos = circleEnemy({ 1000.0f , 0.0f }, 250.0f, enemies[56].theta, -moveSlow); ///top
                    enemies[57].pos = circleEnemy({ 1000.0f , 0.0f }, 250.0f, enemies[57].theta, moveSlow); ///top

                    enemies[58].pos = circleEnemy({ 1000.0f , 720.0f }, 250.0f, enemies[58].theta, -moveSlow);///bottom
                    enemies[59].pos = circleEnemy({ 1000.0f , 720.0f }, 250.0f, enemies[59].theta, moveSlow); ///bottom

                    enemies[60].pos = circleEnemy({ 1000.0f , 360.0f }, 250.0f, enemies[60].theta, -moveSlow); ///top
                    enemies[61].pos = circleEnemy({ 1000.0f , 360.0f }, 250.0f, enemies[61].theta, moveSlow); ///top

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
                    enemies[40].pos = circleEnemy({ stage4pos + 1100.0f, 640.0f }, 140.0f, enemies[40].theta, moveSlow);

                    enemies[48].pos = circleEnemy({ stage4pos + 1250.0f, 780.0f }, 280.0f, enemies[48].theta, moveSlow); /// the big circular

                    ///ステージ 5
                    enemies[50].pos = circleEnemy({ stage5pos + 840.0f , 360.0f }, 120.0f, enemies[50].theta, moveSlow); ///inner
                    enemies[51].pos = circleEnemy({ stage5pos + 840.0f , 360.0f }, 200.0f, enemies[51].theta, -moveSlow);
                    enemies[52].pos = circleEnemy({ stage5pos + 840.0f , 360.0f }, 280.0f, enemies[52].theta, moveSlow);
                    enemies[53].pos = circleEnemy({ stage5pos + 840.0f , 360.0f }, 360.0f, enemies[53].theta, -moveSlow);
                    enemies[54].pos = circleEnemy({ stage5pos + 840.0f , 360.0f }, 420.0f, enemies[54].theta, moveSlow);
                    enemies[55].pos = circleEnemy({ stage5pos + 840.0f , 360.0f }, 580.0f, enemies[55].theta, -moveSlow); ///outer*/
                    enemies[63].pos = circleEnemy({ stage5pos + 840.0f , 360.0f }, 580.0f, enemies[63].theta, moveSlow); ///outer*/

                } else if (enemies[i].enemyType == 1) {
                    // 縦方向に往復する敵
                    /// ステージ 1

                    enemies[64].pos = verticalEnemy({ 300.0f, 360.0f }, 300.0f, enemies[64].theta, moveSlow); /// theta test
                    enemies[65].pos = verticalEnemy({ 380.0f, 360.0f }, 300.0f, enemies[65].theta, moveSlow); /// theta test
                    enemies[66].pos = verticalEnemy({ 460.0f, 360.0f }, 300.0f, enemies[66].theta, moveSlow); /// theta test
                    enemies[67].pos = verticalEnemy({ 540.0f, 360.0f }, 300.0f, enemies[67].theta, moveSlow); /// theta test
                    enemies[68].pos = verticalEnemy({ 620.0f, 360.0f }, 300.0f, enemies[68].theta, moveSlow); /// theta test

                    enemies[69].pos = verticalEnemy({ 700.0f, 360.0f }, 300.0f, enemies[69].theta, moveSlow); /// theta test

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

                } else if (enemies[i].enemyType == 2) {
                    // 横方向に往復する敵
                    /// ステージ 4
                    enemiesBig[30].pos = horizonEnemy({ stage4pos + 150.0f, 480.0f }, 250.0f, enemiesBig[30].theta, moveSlow); /// wall 3
                    enemiesBig[31].pos = horizonEnemy({ stage4pos + 150.0f, 480.0f }, 250.0f, enemiesBig[31].theta, moveSlow); /// wall 3
                    enemiesBig[32].pos = horizonEnemy({ stage4pos + 150.0f, 480.0f }, 250.0f, enemiesBig[32].theta, moveSlow); /// wall 3
                    enemiesBig[33].pos = horizonEnemy({ stage4pos + 150.0f, 480.0f }, 250.0f, enemiesBig[33].theta, moveSlow); /// wall 3
                    enemiesBig[48].pos = horizonEnemy({ stage4pos + 150.0f, 480.0f }, 250.0f, enemiesBig[48].theta, moveSlow); /// wall 3

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
                }
            }

            for (int i = 0; i < maxEnemy; i++) {
                enemies[i].animeCount--;

                if (enemies[i].animeCount <= 0) {
                    enemies[i].animeCount = 60;

                } else if (enemies[i].animeCount <= 10) {
                    enemies[i].screenPosX = 0;
                } else if (enemies[i].animeCount <= 20) {
                    enemies[i].screenPosX = 120;
                } else if (enemies[i].animeCount <= 40) {
                    enemies[i].screenPosX = 240;
                } else if (enemies[i].animeCount <= 60) {
                    enemies[i].screenPosX = 360;
                }
            }

            for (int i = 0; i < maxEnemy; i++) {
                enemiesBig[i].animeCount--;

                if (enemiesBig[i].animeCount <= 0) {

                    enemiesBig[i].animeCount = 40;
                    enemiesBig[i].screenPosX = 0;

                } else if (enemiesBig[i].animeCount <= 10) {

                    enemiesBig[i].screenPosX = 0;
                } else if (enemiesBig[i].animeCount <= 20) {

                    enemiesBig[i].screenPosX = 200;
                } else if (enemiesBig[i].animeCount <= 30) {

                    enemiesBig[i].screenPosX = 400;
                } else if (enemiesBig[i].animeCount <= 40) {

                    enemiesBig[i].screenPosX = 600;
                }
            }
#pragma endregion

#pragma region プレイヤー

            if (isInput) {

                // 初期化
                leftStickX = 0;
                leftStickY = 0;

                // キーボード入力のチェック（8方向）
                if (keys[DIK_LEFT] || keys[DIK_A]) {
                    leftStickX = -1;
                } else if (keys[DIK_RIGHT] || keys[DIK_D]) {
                    leftStickX = 1;
                }

                if (keys[DIK_UP] || keys[DIK_W]) {
                    leftStickY = -1;
                } else if (keys[DIK_DOWN] || keys[DIK_S]) {
                    leftStickY = 1;
                }

                // キーボード入力に基づいた方向ベクトルを正規化
                Vector2 directionKey = { (float)leftStickX, (float)leftStickY };
                float keyMagnitude = std::sqrtf(directionKey.x * directionKey.x + directionKey.y * directionKey.y);

                if (keyMagnitude != 0.0f) {
                    directionKey.x /= keyMagnitude;
                    directionKey.y /= keyMagnitude;
                }

                // キーボードによるカーソル位置の計算
                cursor.pos.x = player.pos.x + directionKey.x * dashDistance;
                cursor.pos.y = player.pos.y + directionKey.y * dashDistance;

                // ゲームパッドのスティック入力を取得（int型で取得）
                int leftStickXi = 0;
                int leftStickYi = 0;
                Novice::GetAnalogInputLeft(0, &leftStickXi, &leftStickYi);

                // int型をfloat型に変換
                float leftStickXf = static_cast<float>(leftStickXi);
                float leftStickYf = static_cast<float>(leftStickYi);

                // 入力ベクトルの大きさを計算
                float stickMagnitude = std::sqrtf(leftStickXf * leftStickXf + leftStickYf * leftStickYf);

                // デッドゾーン設定
                if (stickMagnitude > 8000) {
                    // 入力ベクトルを正規化（360度対応）
                    float normX = leftStickXf / stickMagnitude;
                    float normY = leftStickYf / stickMagnitude;

                    // スティックによるカーソル位置の計算
                    cursor.pos.x = player.pos.x + normX * dashDistance;
                    cursor.pos.y = player.pos.y + normY * dashDistance;
                }

                // プレイヤーが生きている場合のダッシュ処理
                if (player.isAlive) {
                    // 有効な入力があるかを確認
                    bool isValidDirection = (keyMagnitude > 0.0f || stickMagnitude > 8000);

                    // キーボードのスペースキーまたはゲームパッドのボタンでダッシュ
                    if (isValidDirection && ((keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0) || Novice::IsTriggerButton(0, kPadButton10))) {
                        // ダッシュ音を再生
                        Novice::PlayAudio(dashSound, false, 0.7f);

                        // 目標位置を更新
                        player.targetPos.x = cursor.pos.x;
                        player.targetPos.y = cursor.pos.y;

                        // アニメーションの開始
                        player.isAnimation = true;
                        player.animeCount = 30;

                        // パーティクル生成
                        for (int j = 0; j < particlesToGenerate; ++j) {
                            for (int i = 0; i < maxParticles; ++i) {
                                if (!particles[i].isActive) {
                                    particles[i].pos = player.pos;

                                    particles[i].randomAngle1 = ((rand() % 200) * (float)M_PI / 180.0f);
                                    
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

                // スティックがニュートラルの時、カーソルをプレイヤーの位置に戻す
                if (stickMagnitude <= 8000 && keyMagnitude == 0.0f) {
                    cursor.pos = player.pos;
                }

            }

#pragma region キーボード

           
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

                player.isAlive = true;
            }

            // *****************チェックポイント

            for (int i = 0; i < MaxCheckPoint; i++) {
                if (player.pos.x >= checkPoint[i].pos.x && !checkPoint[i].isAction) {

                    player.checkPointPos = checkPoint[i].pos;
                    checkPoint[i].isAction = true;
                    //効果音
                    Novice::PlayAudio(damageSound, false, 0.6f);
                }

                if (checkPoint[i].isAction) {

                    checkPoint[i].animeCount--;

                    if (checkPoint[i].animeCount <= 0) {

                        checkPoint[i].animeCount = 0;
                        checkPoint[i].screenPosX = 600;
                    } else if (checkPoint[i].animeCount <= 15) {

                        checkPoint[i].screenPosX = 400;
                    } else if (checkPoint[i].animeCount <= 30) {

                        checkPoint[i].screenPosX = 200;
                    } else if (checkPoint[i].animeCount <= 45) {

                        checkPoint[i].screenPosX = 0;
                    }
                }
            }

            // ****************ゴール
            if (player.pos.x >= clearLine.x - 60.0f) {

                player.pos = Lerp(player.pos, { clearLine.x + 100.0f,player.pos.y }, 0.1f);
                if (!Novice::IsPlayingAudio(gameClearPlay) || gameClearPlay == -1) {
                   
                    gameClearPlay = Novice::PlayAudio(gameClearSound, true, gameVolume + 0.3f);
                }
                isInput = false;

                whiteOutAlpha += 2;
                if (whiteOutAlpha >= 255) {
                    Novice::StopAudio(gamePlayBGM);
                    Novice::StopAudio(gamePlayBGM2);
                    whiteOutAlpha = 255;
                    scene = CLEAR;
                }

                
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
            } else {

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
            if (player.pos.x <= clearLine.x - (1280.0f / 2.0f)) {
                if (player.pos.x >= 1280.0f / 2.0f) {
                    camera.targetPos.x = player.pos.x - (1280.0f / 2.0f);
                }
            }

            // プレイヤーが初期座標の半分以下に来たらカメラをデフォルトに戻す
            if (player.pos.x <= 1280.0f / 2.0f){
                camera.targetPos.x = 0.0f;
            }

            // カメライージング
            camera.pos = Lerp(camera.pos, camera.targetPos, 0.1f);

            camera.pos.x += wrand.x;
            camera.pos.y += wrand.y;

#pragma endregion

#pragma region 当たり判定

            for (int i = 0; i < maxEnemy; i++) {
                if (isCollision(player.pos, enemies[i].pos, player.radius, enemies[i].radius)
                    || isCollision(player.pos, enemiesBig[i].pos, player.radius, enemiesBig[i].radius)) {

                    player.isAlive = false;
                    //効果音
                    Novice::PlayAudio(chackPointSound, false, 0.9f);
                    // シェイク
                    randMax = 20;

                    for (int j = 0; j < particlesToGenerate; ++j) {
                        for (int l = 0; l < maxParticles; ++l) {
                            if (!particles[l].isActive) {
                                particles[l].pos = player.pos;

                                particles[i].randomAngle1 = ((rand() % 100) * (float)M_PI / 180.0f);

                                particles[l].randomAngle = (rand() % 10 - 1) + ((rand() % 30 - 15) * (float)M_PI / 180.0f);

                                particles[l].speed = 5.0f + (rand() % 5);

                                particles[l].velocity = { cosf(particles[l].randomAngle) * particles[l].speed, sinf(particles[l].randomAngle) * particles[l].speed };

                                particles[l].radius = 5.0f;
                                particles[l].lifeTime = 60;
                                particles[l].isActive = true;

                                break;
                            }
                        }
                    }
                }
            }

#pragma endregion

#pragma region 描画

            //**************************背景

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 2; j++) {
                    Novice::DrawSprite(
                        (int)innermostBg[i].pos.x + (j * 2560) - (int)(camera.pos.x * 0.25f),
                        (int)innermostBg[i].pos.y - (int)camera.pos.y, 
                        innermostBg[i].th, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF
                    );
                }
            }
            

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 2; j++) {
                    Novice::DrawSprite(
                        (int)middleBg[i].pos.x + (j * 2560) - (int)(camera.pos.x * 0.5f),
                        (int)middleBg[i].pos.y - (int)camera.pos.y,
                        middleBg[i].th, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF
                    );
                }
            }

            Novice::DrawSprite(
                (int)frontmostBg[0].pos.x - (int)camera.pos.x, 
                (int)frontmostBg[0].pos.y - (int)camera.pos.y,  
                frontmostBg[0].th, 1.0f, 1.0f, 0.0f, 0xFFFFFFEE
            );

            Novice::DrawSprite(
                (int)frontmostBg[0].pos.x + 3840 - (int)camera.pos.x,  
                (int)frontmostBg[0].pos.y - (int)camera.pos.y,  
                frontmostBg[0].th, 1.0f, 1.0f, 0.0f, 0xFFFFFFEE
            );

            Novice::DrawSprite(
                (int)frontmostBg[1].pos.x - (int)camera.pos.x, 
                (int)frontmostBg[1].pos.y - (int)camera.pos.y, 
                frontmostBg[1].th, 1.0f, 1.0f, 0.0f, 0xFFFFFFEE
            );

            Novice::DrawSprite(
                (int)frontmostBg[2].pos.x - (int)camera.pos.x, 
                (int)frontmostBg[2].pos.y - (int)camera.pos.y,  
                frontmostBg[2].th, 1.0f, 1.0f, 0.0f, 0xFFFFFFEE
            );

            Novice::DrawSprite(0 - (int)camera.pos.x, 0 - (int)camera.pos.y, ruleTh, 1, 1, 0, 0xFFFFFFFF);

            //**************************クリアライト
            Novice::DrawSprite(((int)clearLine.x - 300) - (int)camera.pos.x, 0 - (int)camera.pos.y, clearLineTh, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF);

            //*********************チェックポイント

            for (int i = 0; i < MaxCheckPoint; i++) {
                Novice::DrawQuad(
                    ((int)checkPoint[i].pos.x - (int)checkPoint[i].w) - (int)camera.pos.x, ((int)checkPoint[i].pos.y - (int)checkPoint[i].h) - (int)camera.pos.y,
                    ((int)checkPoint[i].pos.x + (int)checkPoint[i].w) - (int)camera.pos.x, ((int)checkPoint[i].pos.y - (int)checkPoint[i].h) - (int)camera.pos.y,
                    ((int)checkPoint[i].pos.x - (int)checkPoint[i].w) - (int)camera.pos.x, ((int)checkPoint[i].pos.y + (int)checkPoint[i].h) - (int)camera.pos.y,
                    ((int)checkPoint[i].pos.x + (int)checkPoint[i].w) - (int)camera.pos.x, ((int)checkPoint[i].pos.y + (int)checkPoint[i].h) - (int)camera.pos.y,
                    checkPoint[i].screenPosX, 0, 200, 720, checkPoint[i].th, 0xFFFFFFFF);
            }

            //**************************敵
            // 敵の描画
            for (int i = 0; i < maxEnemy; i++) {
                Novice::DrawQuad(
                    ((int)enemies[i].pos.x - (int)enemies[i].radius) - (int)camera.pos.x, ((int)enemies[i].pos.y - (int)enemies[i].radius) - (int)camera.pos.y,
                    ((int)enemies[i].pos.x + (int)enemies[i].radius) - (int)camera.pos.x, ((int)enemies[i].pos.y - (int)enemies[i].radius) - (int)camera.pos.y,
                    ((int)enemies[i].pos.x - (int)enemies[i].radius) - (int)camera.pos.x, ((int)enemies[i].pos.y + (int)enemies[i].radius) - (int)camera.pos.y,
                    ((int)enemies[i].pos.x + (int)enemies[i].radius) - (int)camera.pos.x, ((int)enemies[i].pos.y + (int)enemies[i].radius) - (int)camera.pos.y,
                    enemies[i].screenPosX, 0, 120, 120, enemyTh, 0xFFFFFFFF);

                Novice::DrawQuad(
                    ((int)enemiesBig[i].pos.x - (int)enemiesBig[i].radius) - (int)camera.pos.x, ((int)enemiesBig[i].pos.y - (int)enemiesBig[i].radius) - (int)camera.pos.y,
                    ((int)enemiesBig[i].pos.x + (int)enemiesBig[i].radius) - (int)camera.pos.x, ((int)enemiesBig[i].pos.y - (int)enemiesBig[i].radius) - (int)camera.pos.y,
                    ((int)enemiesBig[i].pos.x - (int)enemiesBig[i].radius) - (int)camera.pos.x, ((int)enemiesBig[i].pos.y + (int)enemiesBig[i].radius) - (int)camera.pos.y,
                    ((int)enemiesBig[i].pos.x + (int)enemiesBig[i].radius) - (int)camera.pos.x, ((int)enemiesBig[i].pos.y + (int)enemiesBig[i].radius) - (int)camera.pos.y,
                    enemiesBig[i].screenPosX, 0, 200, 200, enemyBigTh, 0xFFFFFFFF);
            }

            //*************************カーソルの描画
            if (player.isAlive && isInput) {

                Novice::DrawSprite(((int)cursor.pos.x - 20) - (int)camera.pos.x, ((int)cursor.pos.y - 20) - (int)camera.pos.y,
                    cursor.th, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF
                );

                // プレイヤーとカーソルの間に小さな円を2つ描画
                Vector2 midPoint1 = Lerp(player.pos, cursor.pos, 0.33f);
                Vector2 midPoint2 = Lerp(player.pos, cursor.pos, 0.66f);


                Novice::DrawSprite(((int)midPoint1.x - 10) - (int)camera.pos.x, ((int)midPoint1.y - 10) - (int)camera.pos.y,
                    cursor.midCursorTh, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF
                );

                Novice::DrawSprite(((int)midPoint2.x - 10) - (int)camera.pos.x, ((int)midPoint2.y - 10) - (int)camera.pos.y,
                    cursor.midCursorTh, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF
                );

            }

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
            }

            //**********************パーティクル

            for (int i = 0; i < maxParticles; ++i) {
                if (particles[i].isActive) {
                   

                    Novice::DrawSprite((int)particles[i].pos.x - (int)particles[i].radius - (int)camera.pos.x, (int)particles[i].pos.y - (int)particles[i].radius - (int)camera.pos.y, particlesTh, 1.0f, 1.0f, particles[i].randomAngle1, 0xFFFFFFFF);
                }
            }

            if (playSceneBlackOut > 0) {
                Novice::DrawBox(0, 0, 1280, 720, 0.0f, 0x00000000 + playSceneBlackOut, kFillModeSolid);
            }

            //*********************:ホワイト
            Novice::DrawBox(0, 0, 1280, 720, 0.0f, GetColor(255, 255, 255, whiteOutAlpha), kFillModeSolid);

#pragma endregion

			break;
#pragma region クリア画面


		case CLEAR:
			switch (clearScene) {
			case EYE:
#pragma region 更新処理
                //音
                if (!Novice::IsPlayingAudio(clearPlay) || clearPlay == -1) {
                    //Novice::StopAudio(openingPlay);
                    clearPlay = Novice::PlayAudio(clearBGM, true, gameVolume);
                }
				clearTimer++;
				if (clearTimer > 100) {
					if (clearFadeInOut > 0) {
						clearFadeInOut -= 5;
					}

				}

				if (clearTimer > 1000) {
					clearScene = ENDING;
                    Novice::StopAudio(clearGrithPlay);
                    Novice::StopAudio(clearPlay);
                    Novice::StopAudio(clearPlay2);
                    Novice::StopAudio(gameClearPlay);
					clearTimer = 0;
					clearFadeInOut = 255;
					clearColor = WHITE;
					clearPlayerAnime = 0;
					clearZoom = 0.0f;
				} else if (clearTimer > 610) {
                    if (!Novice::IsPlayingAudio(clearPlay2) || clearPlay2 == -1) {
                        //Novice::StopAudio(openingPlay);
                        clearPlay2 = Novice::PlayAudio(clearBGM2, true, gameVolume);
                    }
					if (clearZoom > 1.f) {
						clearZoom -= 0.055f;
					} else {
						clearZoom += 0.025f;
					}
					if (clearTimer < 800) {
						if (clearPlayerAnime < 600) {
							clearPlayerAnime += 100;
						} else {
							clearPlayerAnime = 0;
						}

					} else {
						clearPlayerAnime = 600;
						if (clearTimer % 8 == 0) {
							clearColor = RED;
						} else if (clearTimer % 8 == 2) {
							clearColor = BLUE;
						} else if (clearTimer % 8 == 4) {
							clearColor = WHITE;
						} else if (clearTimer % 8 == 6) {
							clearColor = 0xffff00ff;
						}
					}



				} else if (clearTimer > 600) {
					if (clearPlayerAnime < 600) {
						clearPlayerAnime += 100;
					}
					clearZoom = 0.f;
				} else if (clearTimer > 300) {
					clearZoom += 0.005f;
                    if (!Novice::IsPlayingAudio(clearGrithPlay) || clearGrithPlay == -1) {
                        //Novice::StopAudio(openingPlay);
                        clearGrithPlay = Novice::PlayAudio(clearGrith, true, gameVolume);
                    }
				}

#pragma endregion

#pragma region 描画処理
				Novice::DrawSprite(0 - static_cast<int>(clearZoom * 640.0f), 0 - static_cast<int>(clearZoom * 360.0f),
					clearBackground, 1.0f + clearZoom, 1.0f + clearZoom, 0.0f, WHITE);

				//プレイヤ
				Novice::DrawSpriteRect(480 - static_cast<int>(clearZoom * 150.0f), 220 - static_cast<int>(clearZoom * 150.0f),
					clearPlayerAnime, 0, 300, 300, clearPlayerGraph, 1.f / 3.f + clearZoom / 3.f, 1.f + clearZoom, 0.f, clearColor);
				if (clearTimer > 950) {
					Novice::DrawBox(0, 0, 1280, 720, 0.0f, BLACK, kFillModeSolid);
				} else if (clearTimer > 800) {
					Novice::DrawSprite(0 - static_cast<int>(clearZoom * 640.0f), 0 - static_cast<int>(clearZoom * 360.0f),
						clearEye, 1.0f + clearZoom, 1.0f + clearZoom, 0.0f, 0xffffff55);
				} else if (clearTimer > 600) {

				} else if (clearTimer > 300) {
					Novice::DrawSprite(0 - static_cast<int>(clearZoom * 640.0f), 0 - static_cast<int>(clearZoom * 360.0f),
						clearEye, 1.0f + clearZoom, 1.0f + clearZoom, 0.0f, 0xffffff55);
				}
				if (clearScene == EYE) {
					Novice::DrawBox(0, 0, 1280, 720, 0.0f, 0xffffff00 + clearFadeInOut, kFillModeSolid);
				} else {
					Novice::DrawBox(0, 0, 1280, 720, 0.0f, 0x00000000 + clearFadeInOut, kFillModeSolid);
				}
#pragma endregion
				break;
			case ENDING:
#pragma region 更新処理
				clearTimer++;

				//ボタンが押されたか
				if (isClearEnd) {
					clearEndTimer++;
					//フェードイン・アウト
					if (clearFadeInOut < 255) {
						clearFadeInOut += 5;

					}

					//ボタンが押されてから一定時間後
					if (clearEndTimer > 140) {
						//Clear用変数のリセット
						clearScene = EYE;

						clearTimer = 0;
						clearFadeInOut = 255;
						clearColor = WHITE;
						isClearEnd = false;
						clearEndTimer = 0;
						clearPlayerAnime = 0;
						clearZoom = 0.0f;

						//タイトル画面へ
						scene = START;
					}
				} else {
					//フェードイン・アウト
					if (clearFadeInOut > 0) {
						if (clearTimer > 50) {
							clearFadeInOut -= 5;
						}
					}

					//キーが押されるとタイトル
					for (int i = 0; i < 256; i++) {
						if (keys[i] && preKeys[i] == 0) {
							isClearEnd = true;
						}
					}

					//ボタンが押されるとタイトル
					if (Novice::IsTriggerButton(0, kPadButton0) || Novice::IsTriggerButton(0, kPadButton1) ||
						Novice::IsTriggerButton(0, kPadButton2) || Novice::IsTriggerButton(0, kPadButton3) ||
						Novice::IsTriggerButton(0, kPadButton4) || Novice::IsTriggerButton(0, kPadButton5) ||
						Novice::IsTriggerButton(0, kPadButton6) || Novice::IsTriggerButton(0, kPadButton7) ||
						Novice::IsTriggerButton(0, kPadButton8) || Novice::IsTriggerButton(0, kPadButton9) ||
						Novice::IsTriggerButton(0, kPadButton10) || Novice::IsTriggerButton(0, kPadButton11)) {
						isClearEnd = true;
					}



				}
#pragma endregion

#pragma region 描画処理
				Novice::DrawSprite(0, 0, titleBackgroundGraph, 1.0f, 1.0f, 0.0f, 0xff0000ff);
				Novice::DrawSprite(0, 0, clearLogo, 1.0f, 1.0f, 0.0f, 0xff9090ff);
				//フェードイン・アウト
				if (titleFadeInOut > 0) {
					Novice::DrawBox(0, 0, 1280, 720, 0.0f, 0x00000000 + clearFadeInOut, kFillModeSolid);
				}
#pragma endregion				
				break;
			}
			break;
#pragma endregion
		case END:
			break;
		}

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