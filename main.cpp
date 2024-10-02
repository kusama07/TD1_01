#include <Novice.h>

const char kWindowTitle[] = "GC1A_03_クサマリョウト_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	enum Scene {
		START,
		PLAY,
		CLEAR,
		END

	}scene = PLAY;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		switch (scene)
		{
		case START:
			
			//スペース押したらシーン切り替え
			if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0) {

				scene = PLAY;
			}

			break;
		case PLAY:

			break;
		case CLEAR:
			break;
		case END:
			break;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		switch (scene)
		{
		case START:
			break;
		case PLAY:
			break;
		case CLEAR:
			break;
		case END:
			break;
		}

		Novice::ScreenPrintf(0, 0, "scene = %d", scene);

		///
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
