//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : GP11A132_19_鈴木信歓
//
//=============================================================================
#pragma once


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

int SetParticle(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fSizeX, float fSizeY, int nLife);
void SetColorParticle(int nIdxParticle, D3DXCOLOR col);

