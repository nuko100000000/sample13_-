//=============================================================================
//
// メイン処理 [main.cpp]
// Author : GP11A132_19_鈴木信歓
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "tree.h"
#include "bullet.h"
#include "score.h"
#include "sound.h"
#include "particle.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"				// ウインドウのクラス名
#define WINDOW_NAME		"影表示"	// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);
void HitCheck(void);
bool HitCheckBB(D3DXVECTOR3 mpos, float mhw, float mhh,D3DXVECTOR3 ypos, float yhw, float yhh);
bool HitCheckBC(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, float size1, float size2);


//*****************************************************************************
// グローバル変数:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPSカウンタ
char	g_DebugStr[2048] = WINDOW_NAME;		// デバッグ文字表示用

#endif


//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	// 時間計測用
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// ウィンドウの左座標
		CW_USEDEFAULT,																		// ウィンドウの上座標
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// ウィンドウ横幅
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// ウィンドウ縦幅
		NULL,
		NULL,
		hInstance,
		NULL);

	// 初期化処理(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	// フレームカウント初期化
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// メッセージループ
	while(1)
	{
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳と送出
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
				dwFrameCount = 0;							// カウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60秒ごとに実行
			{
				dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存

#ifdef _DEBUG	// デバッグ版の時だけFPSを表示する
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// 更新処理
				Draw();				// 描画処理

#ifdef _DEBUG	// デバッグ版の時だけ表示する
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// 分解能を戻す

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);

	InitCamera();

	// 入力処理の初期化
	InitInput(hInstance, hWnd);

	// サウンド処理の初期化 
	InitSound(hWnd);

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_sample001);

	// プレイヤーの初期化
	InitPlayer();

	// エネミーの初期化
	InitEnemy();

	InitBullet();

	InitTree();

	InitParticle();

	// フィールドの初期化
	InitMeshField(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 16, 16, 80.0f, 80.0f);

	// 壁の初期化
	InitMeshWall(D3DXVECTOR3(0.0f, 0.0f, FIELD_TOP), D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(D3DXVECTOR3(FIELD_LEFT, 0.0f, 0.0f), D3DXVECTOR3(0.0f, -D3DX_PI * 0.50f, 0.0f),
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(D3DXVECTOR3(FIELD_RIGHT, 0.0f, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI * 0.50f, 0.0f),
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(D3DXVECTOR3(0.0f, 0.0f, FIELD_DOWN), D3DXVECTOR3(0.0f, D3DX_PI, 0.0f),
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	// 壁(裏側用の半透明)
	InitMeshWall(D3DXVECTOR3(0.0f, 0.0f, FIELD_TOP), D3DXVECTOR3(0.0f, D3DX_PI, 0.0f),
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(D3DXVECTOR3(FIELD_LEFT, 0.0f, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI * 0.50f, 0.0f),
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(D3DXVECTOR3(FIELD_RIGHT, 0.0f, 0.0f), D3DXVECTOR3(0.0f, -D3DX_PI * 0.50f, 0.0f),
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(D3DXVECTOR3(0.0f, 0.0f, FIELD_DOWN), D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	InitScore();

	// ライトを有効化
	SetLightEnable(true);

	// 背面ポリゴンをカリング
	SetCullingMode(CULL_MODE_BACK);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	// エネミーの終了処理
	UninitEnemy();

	// 地面の終了処理
	UninitMeshField();

	// 壁の終了処理
	UninitMeshWall();

	UninitTree();

	UninitParticle();

	// プレイヤーの終了処理
	UninitPlayer();

	// 影の終了処理
	UninitShadow();

	UninitBullet();

	UninitScore();

	// カメラの終了処理
	UninitCamera();

	// サウンド終了処理
	UninitSound();


	//入力の終了処理
	UninitInput();

	UninitRenderer();
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	// 入力の更新処理
	UpdateInput();

	// カメラ更新
	UpdateCamera();

	UpdateTree();

	// プレイヤーの更新処理
	UpdatePlayer();

	// エネミーの更新処理
	UpdateEnemy();

	UpdateBullet();

	UpdateParticle();

	// 地面処理の更新
	UpdateMeshField();

	// 壁処理の更新
	UpdateMeshWall();

	// 影の更新処理
	UpdateShadow();

	HitCheck();

	UpdateScore();

}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// バックバッファクリア
	Clear();

	SetCamera();

	// 地面の描画処理
	DrawMeshField();

	// 影の描画処理
	DrawShadow();



	// エネミーの描画処理
	DrawEnemy();

	// プレイヤーの描画処理
	DrawPlayer();

	DrawBullet();

	// 壁の描画処理
	DrawMeshWall();

	DrawTree();

	DrawParticle();

	//2Dのものを描画する処理
	// Z比較なし
	SetDepthEnable(false);

	// ライティングを無効
	SetLightEnable(false);

	DrawScore();

	// ライティングを有効に
	SetLightEnable(true);
	// Z比較あり
	SetDepthEnable(true);


	// バックバッファ、フロントバッファ入れ替え
	Present();
}


long GetMousePosX(void)
{
	return g_MouseX;
}


long GetMousePosY(void)
{
	return g_MouseY;
}


#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif

//=============================================================================
// BBによる当たり判定処理
// 回転は考慮しない
// 戻り値：当たってたらtrue
//=============================================================================
// バウンディングボックスの処理
bool HitCheckBB(D3DXVECTOR3 mpos, float mhw, float mhh,
	D3DXVECTOR3 ypos, float yhw, float yhh)
{
	bool ans = false;	// 外れをセットしておく

	// バウンディングボックス(BB)の処理
	if ((mpos.x + mhw > ypos.x - yhw) &&
		(mpos.x - mhw < ypos.x + yhw) &&
		(mpos.y + mhh > ypos.y - yhh) &&
		(mpos.y - mhh < ypos.y + yhh))
	{
		// 当たった時の処理
		ans = true;		// 当たりました
	}

	return ans;
}


//=============================================================================
// BCによる当たり判定処理
// サイズは半径
// 戻り値：当たってたらtrue
//=============================================================================
bool HitCheckBC(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, float size1, float size2)
{
	float len = (size1 + size2) * (size1 + size2);	// 半径を2乗した物
	D3DXVECTOR3 temp = pos1 - pos2;
	float fLengthSq = D3DXVec3LengthSq(&temp);		// 2点間の距離（2乗した物）

	if (len > fLengthSq)
	{
		return true;
	}

	return false;
}


//=============================================================================
// 当たり判定処理
//=============================================================================
void HitCheck(void)
{
	PLAYER *player = GetPlayer();
	ENEMY *enemy = GetEnemy();
	BULLET *bullet = GetBullet();

	// プレイヤーと敵との当たり判定

	if (player->use == true)		// 死んでいたら行わない
	{
		// 敵は複数居ますよね？→for文使うよ
		for (int j = 0; j < ENEMY_MAX; j++)
		{
			if (enemy[j].use == false) continue;	// 死んでいたら行わない
			{
				bool ans = HitCheckBC(player->pos, enemy[j].pos, player->radius, enemy[j].radius);

				if (ans == true)
				{
					enemy[j].use = false;
					AddScore(100);
				}
			}
		}
	}

	if (bullet->bUse == true)		// 死んでいたら行わない
	{
		// 敵は複数居ますよね？→for文使うよ
		for (int i = 0; i < ENEMY_MAX; i++)
		{
			if (enemy[i].use == false) continue;	// 死んでいたら行わない
			{
				bool ans = HitCheckBC(bullet->pos, enemy[i].pos, bullet->radius, enemy[i].radius);

				if (ans == true)
				{
					enemy[i].use = false;
					AddScore(100);

				}
			}
		}
	}
}



