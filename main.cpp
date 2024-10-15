#include <Novice.h>
#include <Vector2.h>
#include "Easing.h"
#include <math.h>
#define _USE_MATH_DEFINES
#include <XInput.h>
#include <cmath>
#include "Blend.h"

const char kWindowTitle[] = "5103_���̃n�i�̈�}�̓��ɂ͐s���邱�Ƃ̂Ȃ����t���������Ă��邩��A�����ȋC�����Ǝv��Ȃ��ŁB";

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

// �G�̍\����
struct Enemy {
    Vector2 pos;  
    int enemyType; // �G�̎�� (0: �~, 1: �c����, 2: ������)
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

Vector2 circleEnemy(Vector2 pos, float amplitude, float theta) {
    Vector2 result;

    result.x = cosf(theta + (float)M_PI) * amplitude + pos.x;
    result.y = sinf(theta + (float)M_PI) * amplitude + pos.y;

    return result;
}

Vector2 verticalEnemy(Vector2 pos, float amplitude, float theta){
    Vector2 result;

    result.x = pos.x;
    result.y = sinf(theta) * amplitude + pos.x;

    return result;
}

Vector2 horizonEnemy(Vector2 pos,float amplitude, float theta) {
    Vector2 result;

    theta += float(M_PI) / 60.0f;

    result.x = sinf(theta) * amplitude + pos.x;
    result.y = pos.y;

    return result;
}

// Windows�A�v���ł̃G���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // ���C�u�����̏�����
    Novice::Initialize(kWindowTitle, 1280, 720);

    // �L�[���͌��ʂ��󂯎�锠
    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

//************************* �錾 *************************//

#pragma region �V�[��
    enum Scene {
        START,
        PLAY,
        CLEAR,
        END

    }scene = PLAY;

#pragma endregion

#pragma region �G

    const int maxEnemy = 100;

    Enemy enemies[maxEnemy];
    
    for (int i = 0; i < maxEnemy; i++) {
        enemies[i].pos = { 800.0f, 360.0f };
        enemies[i].enemyType = i % 3; 
        enemies[i].theta = 0.0f;
        enemies[i].amplitude = 150.0f;
        enemies[i].radius = 25.0f;
    }

#pragma endregion

#pragma region �v���C���[

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

    // �����ړ����x
    const float dashDistance = 300.0f;

#pragma endregion 

#pragma region �p�[�e�B����

    // �p�[�e�B�N���̍ő吔
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
    const int particlesToGenerate = 5; // ��������p�[�e�B�N���̐�

#pragma endregion 

#pragma region �`�F�b�N�|�C���g

    //*********�`�F�b�N�|�C���g
    CheckPoint checkPoint{
        checkPoint.pos = {720.0f / 2.0f,0.0f},
    };

    Vector2 clearLine = { 1350.0f , 0.0f };

#pragma endregion

#pragma region �J�[�\��

    // �J�[�\���̏����ݒ�
    Cursor cursor{
        {640.0f, 360.0f}
    };

#pragma endregion

#pragma region �J����

    Camera camera{
        {0.0f, 0.0f},
        {0.0f, 0.0f}
    };

#pragma endregion

#pragma region �R���g���[���[

    int leftStickX = 0;
    int leftStickY = 0;
    const float easingSpeed = 0.2f;

    // XPAD�p
    XINPUT_STATE state;

#pragma endregion

    //�V�F�C�N
    Vector2 wrand = { 0.0f };
    int randMax = 0;

//*****************************************************//


    // �E�B���h�E�́~�{�^�����������܂Ń��[�v
    while (Novice::ProcessMessage() == 0) {
        // �t���[���̊J�n
        Novice::BeginFrame();

        // �L�[���͂��󂯎��
        memcpy(preKeys, keys, 256);
        Novice::GetHitKeyStateAll(keys);
        XInputGetState(0, &state);

        switch (scene)
        {
        case START:

            //�X�y�[�X��������V�[���؂�ւ�
            if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0) {

                scene = PLAY;
            }
            break;
        case PLAY:

#pragma region �V�F�C�N

            randMax--;
            if (randMax < 1) {
                randMax = 1;
            }

            wrand.x = float(rand() % randMax - randMax / 2);
            wrand.y = float(rand() % randMax - randMax / 2);
#pragma endregion

#pragma region �G

            for (int i = 0; i < maxEnemy; i++) {
                enemies[i].theta += float(M_PI) / 60.0f;
                if (enemies[i].enemyType == 0) {
                    // �~��ɓ����G
                    enemies[8].pos.x = cosf(enemies[i].theta + (i * 2 * (float)M_PI)) * enemies[i].amplitude + 150.0f; 
                    enemies[8].pos.y = sinf(enemies[i].theta + (i * 2 * (float)M_PI)) * enemies[i].amplitude + 360.0f;

                    enemies[9].pos.x = cosf(enemies[i].theta + (i * 2 * (float)M_PI)) * enemies[i].amplitude + 300.0f;
                    enemies[9].pos.y = sinf(enemies[i].theta + (i * 2 * (float)M_PI)) * enemies[i].amplitude + 0.0f;

                } else if (enemies[i].enemyType == 1) { 
                    // �c�����ɉ�������G
                    enemies[6].pos.x = 500.0f; 
                    enemies[6].pos.y = sinf(enemies[i].theta) * enemies[i].amplitude + (720.0f / 2.0f);

                    enemies[7].pos.x = 800.0f;
                    enemies[7].pos.y = sinf(enemies[i].theta) * enemies[i].amplitude + (720.0f / 2.0f);
                } else if (enemies[i].enemyType == 2) {
                    // �������ɉ�������G
                    enemies[0].pos.x = sinf(enemies[i].theta) * enemies[i].amplitude + 175.0f;
                    enemies[0].pos.y = 0.0f; 

                    enemies[1].pos.x = sinf(enemies[i].theta) * enemies[i].amplitude + 170.0f;
                    enemies[1].pos.y = 720.0f;

                    enemies[2].pos.x = sinf(enemies[i].theta) * enemies[i].amplitude + 1280.0f;
                    enemies[2].pos.y = 0.0f;

                    enemies[3].pos.x = sinf(enemies[i].theta) * enemies[i].amplitude + 1280.0f;
                    enemies[3].pos.y = 720.0f;

                    enemies[4].pos.x = sinf(enemies[i].theta) * (enemies[i].amplitude * 2) + 1130.0f;
                    enemies[4].pos.y = 200.0f;

                    enemies[5].pos.x = sinf(enemies[i].theta) * (enemies[i].amplitude * 2) + 1130.0f;
                    enemies[5].pos.y = 520.0f;

                    enemies[10].pos = horizonEnemy({ 150.0f,300.0f }, 150.0f, enemies[i].theta);
                }
            }


#pragma endregion

#pragma region �v���C���[

            // �X�e�B�b�N�̓��͂��擾
            Novice::GetAnalogInputLeft(0, &leftStickX, &leftStickY);

            if (std::abs(leftStickX) > 8000 || std::abs(leftStickY) > 8000) {

                float magnitude = std::sqrtf(float(leftStickX * leftStickX + leftStickY * leftStickY));
                Vector2 direction = {
                    (float)leftStickX / magnitude,
                    (float)leftStickY / magnitude
                };

                // �J�[�\���̈ʒu���v�Z
                cursor.pos.x = player.pos.x + direction.x * dashDistance;
                cursor.pos.y = player.pos.y + direction.y * dashDistance;

                // �_�b�V��
                if (Novice::IsTriggerButton(0, kPadButton10)) {

                    // �ڕW�ʒu���X�V
                    player.targetPos.x = cursor.pos.x;
                    player.targetPos.y = cursor.pos.y;

                    // �A�j���[�V�����X�V
                    player.isAnimation = true;
                    player.animeCount = 30;

                    // �_�b�V�����Ƀp�[�e�B�N���𐶐�
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
            } else {

                // �X�e�B�b�N���߂�����J�[�\�����v���C���[�̈ʒu�ɖ߂�
                cursor.pos = player.pos;
            }

#pragma region �L�[�{�[�h

            if (keys[DIK_W]) {

                player.pos.y -= 10.0f;
            }
            if (keys[DIK_S]) {

                player.pos.y += 10.0f;
            }
            if (keys[DIK_A]) {

                player.pos.x -= 10.0f;
            }
            if (keys[DIK_D]) {

                player.pos.x += 10.0f;
            }
#pragma endregion

            // �v���C���[�̈ʒu��ڕW�ʒu�Ɍ������ăC�[�W���O�ړ�
            player.pos.x += (player.targetPos.x - player.pos.x) * easingSpeed;
            player.pos.y += (player.targetPos.y - player.pos.y) * easingSpeed;

            //**************** �ړ�����

            if (player.pos.x < player.radius) {
                player.pos.x = player.radius;  // ���[�ł̐���
            }

            if (player.pos.y < player.radius) {
                player.pos.y = player.radius;  // ��[�ł̐���
            }
            if (player.pos.y > (720.0f - player.radius)) {
                player.pos.y = 720.0f - player.radius;  // ���[�ł̐���
            }

            //**************���X�|�[��
            if(!player.isAlive) {
            
                player.respawCoolTime--;

                // �J�[�\��������
                cursor.pos = player.pos;

                // �v���C���[�̈ʒu
                player.pos = player.checkPointPos;
                player.targetPos = player.checkPointPos;

                // �J�����̈ʒu
                camera.targetPos.x = player.pos.x - 120.0f;

                player.animeCount = 0;
                player.screenPosX = 0;

            }

            if (player.respawCoolTime <= 0.0f) {

                player.respawCoolTime = 60;

                player.isAlive = true;
            }

            // *****************�`�F�b�N�|�C���g
            if (player.pos.x >= checkPoint.pos.x) {

                player.checkPointPos.x = checkPoint.pos.x;
            }

            // ****************�S�[��
            if (player.pos.x >= clearLine.x) {


            }

            // *****************�A�j���[�V����

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

#pragma region �p�[�e�B�N��
            // �p�[�e�B�N���̍X�V����
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

#pragma region �J����

            // �J�������X�N���[��������
            if (player.pos.x >= 1280.0f / 2.0f) {
                camera.targetPos.x = player.pos.x - (1280.0f / 2.0f);
            }
            
            // �v���C���[���������W�̔����ȉ��ɗ�����J�������f�t�H���g�ɖ߂�
            if (player.pos.x <= 1280.0f / 2.0f){
                camera.targetPos.x = 0.0f;
            }

            // �J�����C�[�W���O
            camera.pos = Lerp(camera.pos, camera.targetPos, 0.1f);

            camera.pos.x += wrand.x;
            camera.pos.y += wrand.y;

#pragma endregion

#pragma region �����蔻��

            for (int i = 0; i < maxEnemy; i++) {
                if (isCollision(player.pos, enemies[i].pos, player.radius, enemies[i].radius)) {

                    player.isAlive = false;
                    //randMax = 21;
                }
            }

#pragma endregion

#pragma region �`��

            //**************************�w�i
            Novice::DrawBox(0 - (int)camera.pos.x, 0 - (int)camera.pos.y,
                1280, 720, 0.0f, BLACK, kFillModeSolid);

            //**************************�G
            // �G�̕`��
            for (int i = 0; i < maxEnemy; i++) {
                Novice::DrawEllipse((int)(enemies[i].pos.x - camera.pos.x), (int)(enemies[i].pos.y - camera.pos.y),
                    (int)enemies[i].radius, (int)enemies[i].radius,
                    0.0f, RED, kFillModeSolid);
            }

            //*************************�J�[�\���̕`��
            if (player.isAlive) {
                Novice::DrawEllipse(
                    (int)(cursor.pos.x - camera.pos.x), (int)(cursor.pos.y - camera.pos.y),
                    10, 10,
                    0.0f, RED, kFillModeSolid
                );

                // �v���C���[�ƃJ�[�\���̊Ԃɏ����ȉ~��2�`��
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

            //*********************�`�F�b�N�|�C���g
            Novice::DrawLine((int)checkPoint.pos.x - (int)camera.pos.x, 0, (int)checkPoint.pos.x - (int)camera.pos.x, 720, RED);

            //**********************�S�[��
            Novice::DrawLine((int)clearLine.x - (int)camera.pos.x, 0, (int)clearLine.x - (int)camera.pos.x, 720, WHITE);

            //**************************�v���C���[�̕`��
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

            //**********************�p�[�e�B�N��

            for (int i = 0; i < maxParticles; ++i) {
                if (particles[i].isActive) {
                    Novice::DrawEllipse(
                        (int)particles[i].pos.x - (int)camera.pos.x, (int)particles[i].pos.y - (int)camera.pos.y,
                        (int)particles[i].radius, (int)particles[i].radius,
                        0.0f, WHITE, kFillModeSolid
                    );
                }
            }


            
            //************************�f�o�b�O�p
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

        // �f�o�b�O�p
        Novice::ScreenPrintf(0, 0, "scene = %d", scene);

        /// ���`�揈�������܂�
        ///

        // �t���[���̏I��
        Novice::EndFrame();

        // ESC�L�[�������ꂽ�烋�[�v�𔲂���
        if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
            break;
        }
    }

    // ���C�u�����̏I��
    Novice::Finalize();
    return 0;
}