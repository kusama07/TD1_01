#include <Novice.h>
#include <Vector2.h>

const char kWindowTitle[] = "GC1A_03_クサマリョウト_タイトル";

struct Enemy
{
	Vector2 pos;
	int radius;
};

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


	int colorW = WHITE;
	int enemyColor1 = WHITE;
	int c2colorW = WHITE;
	int c3colorW = WHITE;

	FillMode solid = kFillModeSolid;

	Enemy enemy1;
	enemy1.pos.x = 100;
	enemy1.pos.y = 300;
	enemy1.radius = 20;

	Enemy enemy2;
	enemy2.pos.x = 400;
	enemy2.pos.y = 300;
	enemy2.radius = 20;

	Enemy enemy3;
	enemy3.pos.x = 700;
	enemy3.pos.y = 300;
	enemy3.radius = 20;

	float distance1;
	float distance2;
	float distance3;

	
	float theta = 0.0f;
	float amplitude = 10.0f;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);


		switch (scene)
		{
		case START:

			//スペース押したらシーン切り替え
			if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0) {

				scene = PLAY;
			}
			int a;
			break;
		case PLAY:

#pragma region enemy
			///enemy
			Novice::DrawEllipse(
				(int)enemy1.pos.x, (int)enemy1.pos.y,
				enemy1.radius, enemy1.radius, 0,
				enemyColor1, solid);
			Novice::DrawEllipse(
				(int)enemy2.pos.x, (int)enemy2.pos.y,
				enemy2.radius, enemy2.radius, 0,
				c2colorW, solid);
			Novice::DrawEllipse(
				(int)enemy3.pos.x, (int)enemy3.pos.y,
				enemy3.radius, enemy3.radius, 0,
				c3colorW, solid);
#pragma endregion


			break;
		case CLEAR:
			break;
		case END:
			break;
		}

		Novice::ScreenPrintf(0, 0, "scene = %d", scene);
		Novice::ScreenPrintf(40, 40, "change");


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
