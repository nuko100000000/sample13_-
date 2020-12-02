//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : GP11A132_19_鈴木信歓
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "enemy.h"
#include "shadow.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY		"data/MODEL/miku_02.obj"		// 読み込むモデル名

#define	VALUE_MOVE		(5.0f)							// 移動量
#define	VALUE_ROTATE	(D3DX_PI * 0.02f)				// 回転量

#define ENEMY_SHADOW_SIZE	(0.4f)						// 影の大きさ

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[ENEMY_MAX];

// 線形補間のプログラミングを教えちゃうよー！
// エネミーの線形移動用の移動座標テーブル
static D3DXVECTOR3	g_MoveTbl[] = {
	D3DXVECTOR3(-50.0f, 7.0f, 0.0f),
	D3DXVECTOR3(-200.0f, 7.0f, 0.0f),
	D3DXVECTOR3(-200.0f, 7.0f, 200.0f),
};


// エネミーの線形移動用の移動スピードテーブル
static float	g_MoveSpd[] = {
	0.01f,
	0.1f,
	0.005f,
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	InitShadow();

	for (int i = 0; i < ENEMY_MAX; i++)
	{

		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);

		g_Enemy[i].pos = D3DXVECTOR3(-50.0f + i * 30.0f, 7.0f, 20.0f);
		g_Enemy[i].rot = D3DXVECTOR3(0.0f, D3DX_PI, 0.0f);
		g_Enemy[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].spd = 0.0f;		// 移動スピードクリア

		D3DXVECTOR3 pos = g_Enemy[i].pos;
		pos.y = 0.0f;
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);
		g_Enemy[i].radius = 10.0f;
		g_Enemy[i].use = true;
		g_Enemy[i].time = 0.0f;

		
	}

	return S_OK;

	
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{	

	// モデルの解放処理
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		UnloadModel(&g_Enemy[i].model);
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == true)		// このエネミーが使われている？
		{								// Yes
			if (i == 0)
			{	// エネミー０番だけテーブルに従って座標移動（線形補間）
				int nowNo = (int)g_Enemy[i].time;			// 整数分であるテーブル番号を取り出している
				int maxNo = (sizeof(g_MoveTbl) / sizeof(D3DXVECTOR3)); // 登録テーブル数を数えている
				int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				D3DXVECTOR3	pos = g_MoveTbl[nextNo] - g_MoveTbl[nowNo];// XYZ移動量を計算している
				float nowTime = g_Enemy[i].time - nowNo;	// 時間部分である少数を取り出している
				pos *= nowTime;								// 現在の移動量を計算している

				// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				g_Enemy[i].pos = g_MoveTbl[nowNo] + pos;

				g_Enemy[i].time += g_MoveSpd[nowNo];	// 時間を進めている
				if ((int)g_Enemy[i].time >= maxNo)		// 登録テーブル最後まで移動したか？
				{
					g_Enemy[i].time -= maxNo;			// ０番目にリセットしつつも小数部分を引き継いでいる
				}
			}
			// 影もプレイヤーの位置に合わせる
			D3DXVECTOR3 pos = g_Enemy[i].pos;
			pos.y = 0.0f;
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);


			// 回転させる
			if (i & 1)
			{
				g_Enemy[i].rot.y += 0.05f;
			}
			else
			{
				g_Enemy[i].rot.y -= 0.05f;
			}
		}
	}

}



//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

		for (int i = 0; i < ENEMY_MAX; i++)
		{
			if (g_Enemy[i].use == false) continue;
			{
				// ワールドマトリックスの初期化
				D3DXMatrixIdentity(&g_Enemy[i].mtxWorld);

				// スケールを反映
				D3DXMatrixScaling(&mtxScl, g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
				D3DXMatrixMultiply(&g_Enemy[i].mtxWorld, &g_Enemy[i].mtxWorld, &mtxScl);

				// 回転を反映
				D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Enemy[i].rot.y, g_Enemy[i].rot.x, g_Enemy[i].rot.z);
				D3DXMatrixMultiply(&g_Enemy[i].mtxWorld, &g_Enemy[i].mtxWorld, &mtxRot);

				// 移動を反映
				D3DXMatrixTranslation(&mtxTranslate, g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
				D3DXMatrixMultiply(&g_Enemy[i].mtxWorld, &g_Enemy[i].mtxWorld, &mtxTranslate);

				// ワールドマトリックスの設定
				SetWorldMatrix(&g_Enemy[i].mtxWorld);

				// モデル描画
				DrawModel(&g_Enemy[i].model);
			}
		}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);

	
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy()
{
	return &(g_Enemy[0]);
}
