
//
//　最終更新日：2023/10/20
//


#include <WinSock2.h>
#include <WS2tcpip.h>

#include <Windows.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "global.h"
#include "RootObject.h"
#include "Model.h"
#include "Image.h"
#include "Camera.h"
#include "Input.h"
#include "Audio.h"
#include "VFX.h"

#pragma comment( lib, "ws2_32.lib" )

// ポート番号
const unsigned short SERVERPORT = 8888;
// 送受信するメッセージの最大値
const unsigned int MESSAGELENGTH = 1024;

#pragma comment(lib,"Winmm.lib")

#pragma comment( lib, "ws2_32.lib" )

//定数宣言
const char* WIN_CLASS_NAME = "SampleGame";	//ウィンドウクラス名


//プロトタイプ宣言
HWND InitApp(HINSTANCE hInstance, int screenWidth, int screenHeight, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


// エントリーポイント
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
//#if defined(DEBUG) | defined(_DEBUG)
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif

	srand((unsigned)time(NULL));
	SetCurrentDirectory("Assets");

	//初期化ファイル（setup.ini）から必要な情報を取得
	int screenWidth = GetPrivateProfileInt("SCREEN", "Width", 800, ".\\setup.ini");		//スクリーンの幅
	int screenHeight = GetPrivateProfileInt("SCREEN", "Height", 600, ".\\setup.ini");	//スクリーンの高さ
	int fpsLimit = GetPrivateProfileInt("GAME", "Fps", 60, ".\\setup.ini");				//FPS（画面更新速度）
	int isDrawFps = GetPrivateProfileInt("DEBUG", "ViewFps", 0, ".\\setup.ini");		//キャプションに現在のFPSを表示するかどうか




	//ウィンドウを作成
	HWND hWnd = InitApp(hInstance, screenWidth, screenHeight, nCmdShow);

	//Direct3D準備
	Direct3D::Initialize(hWnd, screenWidth, screenHeight);

	//カメラを準備
	Camera::Initialize();

	//入力処理（キーボード、マウス、コントローラー）の準備
	Input::Initialize(hWnd);

	//オーディオ（効果音）の準備
	Audio::Initialize();


	//ルートオブジェクト準備
	//すべてのゲームオブジェクトの親となるオブジェクト
	RootObject* pRootObject = new RootObject;
	pRootObject->Initialize();

	// WinSock初期化
	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0)
	{
		return 1;	// おーわり！
	}

	// ソケットの作成
	int sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock < 0)
	{
		// エラーコードを出力
		// 終了
		return 1;
	}


	// 接続先サーバのソケットアドレス情報設定
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVERPORT);
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr.s_addr);	// ほんとはよくない。せめて127.0.0.1を定数化

	// 接続要求
	if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0)
	{
		// エラーコードを出力
		// 終了
		return 1;
	}


	// ソケットsockをノンブロッキングソケットにする
	unsigned long cmdarg = 0x01;
	ret = ioctlsocket(sock, FIONBIO, &cmdarg);

	if (ret == SOCKET_ERROR)
	{
		// エラー処理
		return 1;
	}
	//メッセージループ（何か起きるのを待つ）
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		//メッセージあり（こっちが優先）
		if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//メッセージなし（ここでゲームの処理）
		else
		{
			//時間計測
			timeBeginPeriod(1);	//時間計測の制度を上げる
			static int FPS = 0;								//画面更新回数のカウンタ
			static DWORD lastFpsResetTime = timeGetTime();	//最後にキャプションにFPSを表示した
			static DWORD lastUpdateTime = timeGetTime();	//最後に画面を更新した時間
			DWORD nowTime = timeGetTime();					//現在の時間

			//キャプションに現在のFPSを表示する
			if (isDrawFps)
			{
				//1秒（1000ミリ秒）経ったら
				if (nowTime - lastFpsResetTime > 1000)
				{
					//FPSの値を表示
					char string[16];
					wsprintf(string, "FPS:%d", FPS);
					SetWindowText(GetActiveWindow(), string);
					FPS = 0;
					lastFpsResetTime = nowTime;
				}
			}


			//指定した時間（FPSを60に設定した場合は60分の1秒）経過していたら更新処理
			if ((nowTime - lastUpdateTime) * fpsLimit > 1000.0f)
			{
				//時間計測関連
				lastUpdateTime = nowTime;	//現在の時間（最後に画面を更新した時間）を覚えておく
				FPS++;						//画面更新回数をカウントする

				char buff[MESSAGELENGTH];	// 送受信メッセージの格納領域
				buff[0] = 'b';

				// 送信
				ret = send(sock, buff, strlen(buff), 0);

				if (ret != strlen(buff))
				{
					// エラーコードを出力
					// ぬける
					break;
				}

				// クライアントからのメッセージ受信
				ret = recv(sock, buff, sizeof(buff) - 1, 0);

				// ノンブロッキングの場合、受信データがない場合もエラー扱い
				if (ret < 0)
				{
					// エラーコードが WSAEWOULDBLOCK の場合は、受信データがなかったってこと
					if (WSAGetLastError() == WSAEWOULDBLOCK)
					{
					}
					else
					{
						// エラーコードを出力
						// ぬける
						break;
					}
				}
				else
				{
					// 終端記号の追加
					buff[ret] = '\0';
					buff[1] = '\n';

					// 出力				
					OutputDebugString(buff);

				}


				//入力（キーボード、マウス、コントローラー）情報を更新
				Input::Update();

				//全オブジェクトの更新処理
				//ルートオブジェクトのUpdateを呼んだあと、自動的に子、孫のUpdateが呼ばれる
				pRootObject->UpdateSub();

				//カメラを更新
				Camera::Update();

				//エフェクトの更新
				VFX::Update();


				//このフレームの描画開始
				Direct3D::BeginDraw();

				//全オブジェクトを描画
				//ルートオブジェクトのDrawを呼んだあと、自動的に子、孫のUpdateが呼ばれる
				pRootObject->DrawSub();

				//エフェクトの描画
				VFX::Draw();

				//描画終了
				Direct3D::EndDraw();



				
				//ちょっと休ませる
				Sleep(1);
			}
			timeEndPeriod(1);	//時間計測の制度を戻す
		}
	}

	

	//いろいろ解放
	VFX::Release();
	Audio::AllRelease();
	Model::AllRelease();
	Image::AllRelease();
	pRootObject->ReleaseSub();
	SAFE_DELETE(pRootObject);
	Direct3D::Release();

	return 0;
}


//ウィンドウの作成
HWND InitApp(HINSTANCE hInstance, int screenWidth, int screenHeight, int nCmdShow)
{
	//ウィンドウクラス（設計図）を作成
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);					//この構造体のサイズ
	wc.hInstance = hInstance;						//インスタンスハンドル
	wc.lpszClassName = WIN_CLASS_NAME;				//ウィンドウクラス名
	wc.lpfnWndProc = WndProc;						//ウィンドウプロシージャ
	wc.style = CS_VREDRAW | CS_HREDRAW;				//スタイル（デフォルト）
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);	//アイコン
	wc.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);	//小さいアイコン
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);	//マウスカーソル
	wc.lpszMenuName = nullptr;						//メニュー（なし）
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	//背景（白）
	RegisterClassEx(&wc);

	//ウィンドウサイズの計算
	RECT winRect = { 0, 0, screenWidth, screenHeight };
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);

	//タイトルバーに表示する内容
	char caption[64];
	GetPrivateProfileString("SCREEN", "Caption", "***", caption, 64, ".\\setup.ini");

	//ウィンドウを作成
	HWND hWnd = CreateWindow(
		WIN_CLASS_NAME,					//ウィンドウクラス名
		caption,						//タイトルバーに表示する内容
		WS_OVERLAPPEDWINDOW,			//スタイル（普通のウィンドウ）
		CW_USEDEFAULT,					//表示位置左（おまかせ）
		CW_USEDEFAULT,					//表示位置上（おまかせ）
		winRect.right - winRect.left,	//ウィンドウ幅
		winRect.bottom - winRect.top,	//ウィンドウ高さ
		nullptr,						//親ウインドウ（なし）
		nullptr,						//メニュー（なし）
		hInstance,						//インスタンス
		nullptr							//パラメータ（なし）
	);

	//ウィンドウを表示
	ShowWindow(hWnd, nCmdShow);

	return hWnd;
}


//ウィンドウプロシージャ（何かあった時によばれる関数）
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	//ウィンドウを閉じた
	case WM_DESTROY:
		PostQuitMessage(0);	//プログラム終了
		return 0;

	//マウスが動いた
	case WM_MOUSEMOVE:
		Input::SetMousePosition(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}