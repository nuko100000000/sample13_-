//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : GP11A132_19_��ؐM��
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "enemy.h"
#include "shadow.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY		"data/MODEL/miku_02.obj"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE		(5.0f)							// �ړ���
#define	VALUE_ROTATE	(D3DX_PI * 0.02f)				// ��]��

#define ENEMY_SHADOW_SIZE	(0.4f)						// �e�̑傫��

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_Enemy[ENEMY_MAX];

// ���`��Ԃ̃v���O���~���O���������Ⴄ��[�I
// �G�l�~�[�̐��`�ړ��p�̈ړ����W�e�[�u��
static D3DXVECTOR3	g_MoveTbl[] = {
	D3DXVECTOR3(-50.0f, 7.0f, 0.0f),
	D3DXVECTOR3(-200.0f, 7.0f, 0.0f),
	D3DXVECTOR3(-200.0f, 7.0f, 200.0f),
};


// �G�l�~�[�̐��`�ړ��p�̈ړ��X�s�[�h�e�[�u��
static float	g_MoveSpd[] = {
	0.01f,
	0.1f,
	0.005f,
};

//=============================================================================
// ����������
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

		g_Enemy[i].spd = 0.0f;		// �ړ��X�s�[�h�N���A

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
// �I������
//=============================================================================
void UninitEnemy(void)
{	

	// ���f���̉������
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		UnloadModel(&g_Enemy[i].model);
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == true)		// ���̃G�l�~�[���g���Ă���H
		{								// Yes
			if (i == 0)
			{	// �G�l�~�[�O�Ԃ����e�[�u���ɏ]���č��W�ړ��i���`��ԁj
				int nowNo = (int)g_Enemy[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
				int maxNo = (sizeof(g_MoveTbl) / sizeof(D3DXVECTOR3)); // �o�^�e�[�u�����𐔂��Ă���
				int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
				D3DXVECTOR3	pos = g_MoveTbl[nextNo] - g_MoveTbl[nowNo];// XYZ�ړ��ʂ��v�Z���Ă���
				float nowTime = g_Enemy[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���
				pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���

				// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
				g_Enemy[i].pos = g_MoveTbl[nowNo] + pos;

				g_Enemy[i].time += g_MoveSpd[nowNo];	// ���Ԃ�i�߂Ă���
				if ((int)g_Enemy[i].time >= maxNo)		// �o�^�e�[�u���Ō�܂ňړ��������H
				{
					g_Enemy[i].time -= maxNo;			// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				}
			}
			// �e���v���C���[�̈ʒu�ɍ��킹��
			D3DXVECTOR3 pos = g_Enemy[i].pos;
			pos.y = 0.0f;
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);


			// ��]������
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
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

		for (int i = 0; i < ENEMY_MAX; i++)
		{
			if (g_Enemy[i].use == false) continue;
			{
				// ���[���h�}�g���b�N�X�̏�����
				D3DXMatrixIdentity(&g_Enemy[i].mtxWorld);

				// �X�P�[���𔽉f
				D3DXMatrixScaling(&mtxScl, g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
				D3DXMatrixMultiply(&g_Enemy[i].mtxWorld, &g_Enemy[i].mtxWorld, &mtxScl);

				// ��]�𔽉f
				D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Enemy[i].rot.y, g_Enemy[i].rot.x, g_Enemy[i].rot.z);
				D3DXMatrixMultiply(&g_Enemy[i].mtxWorld, &g_Enemy[i].mtxWorld, &mtxRot);

				// �ړ��𔽉f
				D3DXMatrixTranslation(&mtxTranslate, g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
				D3DXMatrixMultiply(&g_Enemy[i].mtxWorld, &g_Enemy[i].mtxWorld, &mtxTranslate);

				// ���[���h�}�g���b�N�X�̐ݒ�
				SetWorldMatrix(&g_Enemy[i].mtxWorld);

				// ���f���`��
				DrawModel(&g_Enemy[i].model);
			}
		}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);

	
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ENEMY *GetEnemy()
{
	return &(g_Enemy[0]);
}
