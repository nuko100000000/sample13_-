//=============================================================================
//
// ���f������ [player.cpp]
// Author : GP11A132_19_��ؐM��
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
// �}�N����`
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/miku_01.obj"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(2.0f)							// �ړ���
#define	VALUE_ROTATE		(D3DX_PI * 0.02f)				// ��]��

#define PLAYER_SHADOW_SIZE	(0.4f)							// �e�̑傫��

#define	MODEL_PLAYER_PARTS	"data/MODEL/miku_02.obj"
#define	PLAYER_PARTS_MAX	(1)
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER				g_Player;						// �v���C���[
static PLAYER				g_Parts[PLAYER_PARTS_MAX];
float roty = 0.0f;


// �v���C���[�̊K�w�A�j���[�V�����f�[�^
static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
	{ D3DXVECTOR3(0.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f),         D3DXVECTOR3(1.0f, 1.0f, 1.0f), 120 },
	{ D3DXVECTOR3(0.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, -D3DX_PI / 2, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 240 },
	{ D3DXVECTOR3(0.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI / 2, 0.0f),  D3DXVECTOR3(1.0f, 1.0f, 1.0f), 120 },
	{ D3DXVECTOR3(0.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f),	     D3DXVECTOR3(1.0f, 1.0f, 1.0f), 120 },
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{	
	InitShadow();

	LoadModel(MODEL_PLAYER, &g_Player.model);

	g_Player.pos = D3DXVECTOR3(0.0f, 7.0f, -50.0f);
	g_Player.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Player.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	g_Player.spd = 0.0f;		// �ړ��X�s�[�h�N���A

	D3DXVECTOR3 pos = g_Player.pos;
	pos.y = 0.0f;
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);

	g_Player.use = true;
	g_Player.radius = 5.0f;

	// �K�w�A�j���[�V�����̏�����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = true;

		// �ʒu�E��]�E�X�P�[���̏����ݒ�
		g_Parts[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

		// �e�q�֌W
		g_Parts[i].parent = &g_Player;	// �� �����ɐe�̃A�h���X������

		// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
		g_Parts[i].tbl_adr = move_tbl;
		g_Parts[i].move_time = 0.0f;

		// �p�[�c�̓ǂݍ���
		LoadModel(MODEL_PLAYER_PARTS, &g_Parts[i].model);
	}

	return S_OK;

}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	// ���f���̉������
	UnloadModel(&g_Player.model);

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{

	CAMERA *cam = GetCamera();

	g_Player.spd *= 0.9f;

	if (g_Player.use == true)
	{
		// �ړ�����
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

		// �����������Ƀ~�N�������ړ�������
		// �����������Ƀ~�N�������������Ă��鏊

		g_Player.rot.y = roty + cam->rot.y;

		g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
		g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;


		// �J�����̎��_�ƒ����_���Z�b�g
		// �J�����̒����_���v���C���[�̍��W�ɂ��Ă݂�
		cam->at = g_Player.pos;

		cam->pos.x = cam->at.x - sinf(cam->rot.y) * cam->len;
		cam->pos.z = cam->at.z - cosf(cam->rot.y) * cam->len;

		// �e���ˏ���
		if (GetKeyboardTrigger(DIK_S))	// �v���X���ƘA�ˁI
		{
			SetBullet(g_Player.pos, g_Player.rot);
		}

		// �e���v���C���[�̈ʒu�ɍ��킹��
		D3DXVECTOR3 pos = g_Player.pos;
		pos.y = 0.0f;
		SetPositionShadow(g_Player.shadowIdx, pos);
	}

	g_Player.parent = NULL;

	// �K�w�A�j���[�V����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// �g���Ă���Ȃ珈������
		if (g_Parts[i].use == true)
		{
			// �ړ�����
			int		index = (int)g_Parts[i].move_time;
			float	time = g_Parts[i].move_time - index;
			int		size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);

			float dt = 1.0f / g_Parts[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
			g_Parts[i].move_time += dt;					// �A�j���[�V�����̍��v���Ԃɑ���

			if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�S�[���֖߂�
			{
				g_Parts[i].move_time = 0.0f;
				index = 0;
			}

			// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
			D3DXVECTOR3 vec = g_Parts[i].tbl_adr[index + 1].pos - g_Parts[i].tbl_adr[index].pos;
			g_Parts[i].pos = g_Parts[i].tbl_adr[index].pos + vec * time;

			// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
			D3DXVECTOR3 rot = g_Parts[i].tbl_adr[index + 1].rot - g_Parts[i].tbl_adr[index].rot;
			g_Parts[i].rot = g_Parts[i].tbl_adr[index].rot + rot * time;

			// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
			D3DXVECTOR3 scl = g_Parts[i].tbl_adr[index + 1].scl - g_Parts[i].tbl_adr[index].scl;
			g_Parts[i].scl = g_Parts[i].tbl_adr[index].scl + scl * time;

		}
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{	
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	if (g_Player.use == true)
	{

		// ���[���h�}�g���b�N�X�̏�����
		D3DXMatrixIdentity(&g_Player.mtxWorld);

		// �X�P�[���𔽉f
		D3DXMatrixScaling(&mtxScl, g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
		D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxScl);

		// ��]�𔽉f
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Player.rot.y + D3DX_PI, g_Player.rot.x, g_Player.rot.z);
		D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxRot);

		// �ړ��𔽉f
		D3DXMatrixTranslation(&mtxTranslate, g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
		D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&g_Player.mtxWorld);

		// ���f���`��
		DrawModel(&g_Player.model);
	}

	// �K�w�A�j���[�V����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// �g���Ă���Ȃ珈������
		if (g_Parts[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		D3DXMatrixIdentity(&g_Parts[i].mtxWorld);

		// �X�P�[���𔽉f
		D3DXMatrixScaling(&mtxScl, g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
		D3DXMatrixMultiply(&g_Parts[i].mtxWorld, &g_Parts[i].mtxWorld, &mtxScl);

		// ��]�𔽉f
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Parts[i].rot.y, g_Parts[i].rot.x, g_Parts[i].rot.z);
		D3DXMatrixMultiply(&g_Parts[i].mtxWorld, &g_Parts[i].mtxWorld, &mtxRot);

		// �ړ��𔽉f
		D3DXMatrixTranslation(&mtxTranslate, g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
		D3DXMatrixMultiply(&g_Parts[i].mtxWorld, &g_Parts[i].mtxWorld, &mtxTranslate);

		if (g_Parts[i].parent != NULL)	// �q����������e�ƌ�������
		{
			D3DXMatrixMultiply(&g_Parts[i].mtxWorld, &g_Parts[i].mtxWorld, &g_Parts[i].parent->mtxWorld);
		}

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&g_Parts[i].mtxWorld);

		// ���f���`��
		DrawModel(&g_Parts[i].model);

	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);


}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

