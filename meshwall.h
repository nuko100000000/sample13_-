//=============================================================================
//
// メッシュ壁の処理 [meshwall.h]
// Author : GP11A132_19_鈴木信歓
//
//=============================================================================
#pragma once


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMeshWall(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col,
						int nNumBlockX, int nNumBlockY, float fSizeBlockX, float fSizeBlockY);
void UninitMeshWall(void);
void UpdateMeshWall(void);
void DrawMeshWall(void);

