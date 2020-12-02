//=============================================================================
//
// ���b�V���n�ʂ̏��� [meshfield.h]
// Author : GP11A132_19_��ؐM��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitMeshField(D3DXVECTOR3 pos, D3DXVECTOR3 rot,
							int nNumBlockX, int nNumBlockZ, float nBlockSizeX, float nBlockSizeZ);
void UninitMeshField(void);
void UpdateMeshField(void);
void DrawMeshField(void);

