#include <Novice.h>

const char kWindowTitle[] = "このハナの一枝の内には尽きることのない言葉がこもっているから、生半可な気持ちと思わないで。";

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

	}scene = START;

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


	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);


		switch (scene)
		{
#pragma region タイトル画面

		case START:

#pragma region 更新処理

			//タイトル画面用のタイマーを加算
			titleTimer++;


			//ボタンが押されたか
			if (gameIsStart) {
				titleFadeTimer++;
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
					}


				}
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

			Novice::ScreenPrintf(0, 30, "%d", gameIsStart);

#pragma endregion
			break;

#pragma endregion

		case PLAY:

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
