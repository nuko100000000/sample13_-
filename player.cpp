//=============================================================================
//
// モデル処理 [player.cpp]
// Author : GP11A132_19_鈴木信歓
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "player.h"
#include "shadow.h"
#include "renderer.h"
#include "bullet.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/miku_01.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(2.0f)							// 移動量
#define	VALUE_ROTATE		(D3DX_PI * 0.02f)				// 回転量

#define PLAYER_SHADOW_SIZE	(0.4f)							// 影の大きさ

#define	MODEL_PLAYER_PARTS	"data/MODEL/miku_02.obj"
#define	PLAYER_PARTS_MAX	(1)
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER				g_Player;						// プレイヤー
static PLAYER				g_Parts[PLAYER_PARTS_MAX];
float roty = 0.0f;


// プレイヤーの階層アニメーションデータ
static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
	{ D3DXVECTOR3(0.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f),         D3DXVECTOR3(1.0f, 1.0f, 1.0f), 120 },
	{ D3DXVECTOR3(0.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, -D3DX_PI / 2, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 240 },
	{ D3DXVECTOR3(0.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI / 2, 0.0f),  D3DXVECTOR3(1.0f, 1.0f, 1.0f), 120 },
	{ D3DXVECTOR3(0.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f),	     D3DXVECTOR3(1.0f, 1.0f, 1.0f), 120 },
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{	
	InitShadow();

	LoadModel(MODEL_PLAYER, &g_Player.model);

	g_Player.pos = D3DXVECTOR3(0.0f, 7.0f, -50.0f);
	g_Player.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Player.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	g_Player.spd = 0.0f;		// 移動スピードクリア

	D3DXVECTOR3 pos = g_Player.pos;
	pos.y = 0.0f;
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);

	g_Player.use = true;
	g_Player.radius = 5.0f;

	// 階層アニメーションの初期化
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = true;

		// 位置・回転・スケールの初期設定
		g_Parts[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

		// 親子関係
		g_Parts[i].parent = &g_Player;	// ← ここに親のアドレスを入れる

		// 階層アニメーション用のメンバー変数の初期化
		g_Parts[i].tbl_adr = move_tbl;
		g_Parts[i].move_time = 0.0f;

		// パーツの読み込み
		LoadModel(MODEL_PLAYER_PARTS, &g_Parts[i].model);
	}

	return S_OK;

}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	// モデルの解放処理
	UnloadModel(&g_Player.model);

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{

	CAMERA *cam = GetCamera();

	g_Player.spd *= 0.9f;

	if (g_Player.use == true)
	{
		// 移動処理
		if (GetKeyboardPress(DIK_LEFT))
		{
			//g_Player.spd = VALUE_MOVE;
			g_Player.spd = VALUE_MOVE;
			roty = D3DX_PI / 2;
		}
		if (GetKeyboardPress(DIK_RIGHT))
		{
			/*g_Player.spd = VALUE_MOVE;
			g_Player.pos.x += g_Player.spd;*/
			g_Player.rot.y = 0.0f + cam->rot.y;

			g_Player.spd = VALUE_MOVE;
			roty = -D3DX_PI / 2;
		}
		if (GetKeyboardPress(DIK_UP))
		{
			g_Player.spd = VALUE_MOVE;
			//g_Player.pos.z += g_Player.spd;
			roty = D3DX_PI;
		}
		if (GetKeyboardPress(DIK_DOWN))
		{
			//g_Player.spd = VALUE_MOVE;
			//g_Player.pos.z -= g_Player.spd;
			g_Player.spd = VALUE_MOVE;
			//g_Player.rot.y = 0.0f + cam->rot.y;
			roty = 0.0f;
		}

		// 押した方向にミクちゃんを移動させる
		// 押した方向にミクちゃんを向かせている所

		g_Player.rot.y = roty + cam->rot.y;

		g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
		g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;


		// カメラの視点と注視点をセット
		// カメラの注視点をプレイヤーの座標にしてみる
		cam->at = g_Player.pos;

		cam->pos.x = cam->at.x - sinf(cam->rot.y) * cam->len;
		cam->pos.z = cam->at.z - cosf(cam->rot.y) * cam->len;

		// 弾発射処理
		if (GetKeyboardTrigger(DIK_S))	// プレスだと連射！
		{
			SetBullet(g_Player.pos, g_Player.rot);
		}

		// 影もプレイヤーの位置に合わせる
		D3DXVECTOR3 pos = g_Player.pos;
		pos.y = 0.0f;
		SetPositionShadow(g_Player.shadowIdx, pos);
	}

	g_Player.parent = NULL;

	// 階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// 使われているなら処理する
		if (g_Parts[i].use == true)
		{
			// 移動処理
			int		index = (int)g_Parts[i].move_time;
			float	time = g_Parts[i].move_time - index;
			int		size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);

			float dt = 1.0f / g_Parts[i].tbl_adr[index].frame;	// 1フレームで進める時間
			g_Parts[i].move_time += dt;					// アニメーションの合計時間に足す

			if (index > (size - 2))	// ゴールをオーバーしていたら、ゴールへ戻す
			{
				g_Parts[i].move_time = 0.0f;
				index = 0;
			}

			// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
			D3DXVECTOR3 vec = g_Parts[i].tbl_adr[index + 1].pos - g_Parts[i].tbl_adr[index].pos;
			g_Parts[i].pos = g_Parts[i].tbl_adr[index].pos + vec * time;

			// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
			D3DXVECTOR3 rot = g_Parts[i].tbl_adr[index + 1].rot - g_Parts[i].tbl_adr[index].rot;
			g_Parts[i].rot = g_Parts[i].tbl_adr[index].rot + rot * time;

			// scaleを求める S = StartX + (EndX - StartX) * 今の時間
			D3DXVECTOR3 scl = g_Parts[i].tbl_adr[index + 1].scl - g_Parts[i].tbl_adr[index].scl;
			g_Parts[i].scl = g_Parts[i].tbl_adr[index].scl + scl * time;

		}
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{	
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	if (g_Player.use == true)
	{

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_Player.mtxWorld);

		// スケールを反映
		D3DXMatrixScaling(&mtxScl, g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
		D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxScl);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Player.rot.y + D3DX_PI, g_Player.rot.x, g_Player.rot.z);
		D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxRot);

		// 移動を反映
		D3DXMatrixTranslation(&mtxTranslate, g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
		D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&g_Player.mtxWorld);

		// モデル描画
		DrawModel(&g_Player.model);
	}

	// 階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// 使われているなら処理する
		if (g_Parts[i].use == false) continue;

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_Parts[i].mtxWorld);

		// スケールを反映
		D3DXMatrixScaling(&mtxScl, g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
		D3DXMatrixMultiply(&g_Parts[i].mtxWorld, &g_Parts[i].mtxWorld, &mtxScl);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Parts[i].rot.y, g_Parts[i].rot.x, g_Parts[i].rot.z);
		D3DXMatrixMultiply(&g_Parts[i].mtxWorld, &g_Parts[i].mtxWorld, &mtxRot);

		// 移動を反映
		D3DXMatrixTranslation(&mtxTranslate, g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
		D3DXMatrixMultiply(&g_Parts[i].mtxWorld, &g_Parts[i].mtxWorld, &mtxTranslate);

		if (g_Parts[i].parent != NULL)	// 子供だったら親と結合する
		{
			D3DXMatrixMultiply(&g_Parts[i].mtxWorld, &g_Parts[i].mtxWorld, &g_Parts[i].parent->mtxWorld);
		}

		// ワールドマトリックスの設定
		SetWorldMatrix(&g_Parts[i].mtxWorld);

		// モデル描画
		DrawModel(&g_Parts[i].model);

	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);


}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

