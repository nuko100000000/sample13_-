//=============================================================================
//
// �e���� [tree.h]
// Author : GP11A132_19_��ؐM��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct BULLET
{
	D3DXMATRIX	mtxWorld;		// ���[���h�}�g���b�N�X
	D3DXVECTOR3 pos;			// �ʒu
	D3DXVECTOR3 scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	float		fWidth;			// ��
	float		fHeight;		// ����
	int			nIdxShadow;		// �eID
	bool		bUse;			// �g�p���Ă��邩�ǂ���
	float		spd;			// �X�s�[�h
	D3DXVECTOR3	rot;			// �p�x
	float		radius;			// �͈�
	int			type;
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

int SetBullet(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
BULLET *GetBullet(void);


