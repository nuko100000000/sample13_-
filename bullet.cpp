//=============================================================================
//
// �e���� [Bullet.cpp]
// Author : GP11A132_19_��ؐM��
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "bullet.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)				// �e�N�X�`���̐�

#define	BULLET_WIDTH		(20.0f)			// ���_�T�C�Y
#define	BULLET_HEIGHT		(20.0f)			// ���_�T�C�Y

#define	MAX_BULLET			(256)			// �e�ő吔

#define	BULLET_SPD			(3.0f)			// �X�s�[�h

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexBullet(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;					// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };		// �e�N�X�`�����

static BULLET						g_aBullet[MAX_BULLET];					// �؃��[�N
static int							g_TexNo;								// �e�N�X�`���ԍ�

static char *g_TextureName[] =
{
	"data/TEXTURE/Bullet00.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBullet(void)
{
	MakeVertexBullet();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// �e���[�N�̏�����
	for(int i = 0; i < MAX_BULLET; i++)
	{
		ZeroMemory(&g_aBullet[i].material, sizeof(g_aBullet[i].material));
		g_aBullet[i].material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		g_aBullet[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aBullet[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		g_aBullet[i].fWidth = BULLET_WIDTH;
		g_aBullet[i].fHeight = BULLET_HEIGHT;
		g_aBullet[i].bUse = false;
		g_aBullet[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aBullet[i].spd = BULLET_SPD;
		g_aBullet[i].radius = 10.0f;
	}


	

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBullet(void)
{
	for(int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if(g_Texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if(g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBullet(void)
{

	for(int i = 0; i < MAX_BULLET; i++)
	{
		if(g_aBullet[i].bUse)
		{
			// �e�̈ړ�����
			for (int i = 0; i < MAX_BULLET; i++)
			{
				if (g_aBullet[i].bUse)
				{
					g_aBullet[i].pos.x -= sinf(g_aBullet[i].rot.y) * g_aBullet[i].spd;
					g_aBullet[i].pos.z -= cosf(g_aBullet[i].rot.y) * g_aBullet[i].spd;
				}
			}

			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_aBullet[i].nIdxShadow, D3DXVECTOR3(g_aBullet[i].pos.x, 0.1f, g_aBullet[i].pos.z));
		}

		if (g_aBullet[i].pos.x < FIELD_DOWN
			|| g_aBullet[i].pos.x > FIELD_TOP
			|| g_aBullet[i].pos.z < FIELD_LEFT
			|| g_aBullet[i].pos.z > FIELD_RIGHT)
		{
			g_aBullet[i].bUse = false;
			ReleaseShadow(g_aBullet[i].nIdxShadow);
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBullet(void)
{

	// ���C�e�B���O�𖳌�
	SetLightEnable(false);

	D3DXMATRIX mtxView, mtxScale, mtxTranslate, mtxRot;

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_aBullet[i].bUse)
		{
			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_aBullet[i].mtxWorld);

			// �r���[�}�g���b�N�X���擾
			CAMERA *cam = GetCamera();
			mtxView = cam->mtxView;

			// �X�P�[���𔽉f
			D3DXMatrixScaling(&mtxScale, g_aBullet[i].scl.x,
				g_aBullet[i].scl.y,
				g_aBullet[i].scl.z);
			D3DXMatrixMultiply(&g_aBullet[i].mtxWorld, &g_aBullet[i].mtxWorld, &mtxScale);

			// ��]�𔽉f
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aBullet[i].rot.y + D3DX_PI, g_aBullet[i].rot.x, g_aBullet[i].rot.z);
			D3DXMatrixMultiply(&g_aBullet[i].mtxWorld, &g_aBullet[i].mtxWorld, &mtxRot);

			// �ړ��𔽉f
			D3DXMatrixTranslation(&mtxTranslate, g_aBullet[i].pos.x,
				g_aBullet[i].pos.y,
				g_aBullet[i].pos.z);
			D3DXMatrixMultiply(&g_aBullet[i].mtxWorld, &g_aBullet[i].mtxWorld, &mtxTranslate);


			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&g_aBullet[i].mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_aBullet[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(true);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(false);
}

//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
BULLET *GetBullet(void)
{
	return &g_aBullet[0];
}


//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexBullet(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = BULLET_WIDTH;
	float fHeight = BULLET_HEIGHT;

	// ���_���W�̐ݒ�
	vertex[0].Position = D3DXVECTOR3(-fWidth / 2.0f, 0.0f, fHeight / 2.0f);
	vertex[1].Position = D3DXVECTOR3(fWidth / 2.0f, 0.0f, fHeight / 2.0f);
	vertex[2].Position = D3DXVECTOR3(-fWidth / 2.0f, 0.0f, -fHeight / 2.0f);
	vertex[3].Position = D3DXVECTOR3(fWidth / 2.0f, 0.0f, -fHeight / 2.0f);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
	vertex[1].TexCoord = D3DXVECTOR2(1.0f, 0.0f);
	vertex[2].TexCoord = D3DXVECTOR2(0.0f, 1.0f);
	vertex[3].TexCoord = D3DXVECTOR2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// �e�̃p�����[�^���Z�b�g
//=============================================================================
int SetBullet(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	int nIdxBullet = -1;

	for(int i = 0; i < MAX_BULLET; i++)
	{
		if(!g_aBullet[i].bUse)
		{
			g_aBullet[i].pos = pos;
			g_aBullet[i].rot = rot;
			g_aBullet[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			g_aBullet[i].bUse = true;

			// �e�̐ݒ�
			g_aBullet[i].nIdxShadow = CreateShadow(g_aBullet[i].pos, 0.5f, 0.5f);

			nIdxBullet = i;

			break;
		}
	}

	return nIdxBullet;
}
