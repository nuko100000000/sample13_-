//=============================================================================
//
// 弾処理 [tree.h]
// Author : GP11A132_19_鈴木信歓
//
//=============================================================================
#pragma once

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct BULLET
{
	D3DXMATRIX	mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3 pos;			// 位置
	D3DXVECTOR3 scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	int			nIdxShadow;		// 影ID
	bool		bUse;			// 使用しているかどうか
	float		spd;			// スピード
	D3DXVECTOR3	rot;			// 角度
	float		radius;			// 範囲
	int			type;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

int SetBullet(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
BULLET *GetBullet(void);


