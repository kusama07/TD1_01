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

	}scene = CLEAR;

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

#pragma region Clear画面用の変数
	enum ClearScene {
		EYE,
		ENDING
	}clearScene = EYE;

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

#pragma endregion
			break;

#pragma endregion

		case PLAY:

			break;
#pragma region クリア画面


		case CLEAR:
			switch (clearScene) {
			case EYE:
#pragma region 更新処理
				clearTimer++;
				if (clearTimer > 100) {
					if (clearFadeInOut > 0) {
						clearFadeInOut -= 5;
					}

				}

				if (clearTimer > 1000) {
					clearScene = ENDING;
					clearTimer = 0;
					clearFadeInOut = 255;
					clearColor = WHITE;
					clearPlayerAnime = 0;
					clearZoom = 0.0f;
				} else if (clearTimer > 610) {
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
