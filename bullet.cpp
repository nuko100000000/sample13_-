//=============================================================================
//
// 弾処理 [Bullet.cpp]
// Author : GP11A132_19_鈴木信歓
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "bullet.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)				// テクスチャの数

#define	BULLET_WIDTH		(20.0f)			// 頂点サイズ
#define	BULLET_HEIGHT		(20.0f)			// 頂点サイズ

#define	MAX_BULLET			(256)			// 弾最大数

#define	BULLET_SPD			(3.0f)			// スピード

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexBullet(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;					// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };		// テクスチャ情報

static BULLET						g_aBullet[MAX_BULLET];					// 木ワーク
static int							g_TexNo;								// テクスチャ番号

static char *g_TextureName[] =
{
	"data/TEXTURE/Bullet00.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet(void)
{
	MakeVertexBullet();

	// テクスチャ生成
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

	// 弾ワークの初期化
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
// 終了処理
//=============================================================================
void UninitBullet(void)
{
	for(int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if(g_Texture[nCntTex] != NULL)
		{// テクスチャの解放
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if(g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBullet(void)
{

	for(int i = 0; i < MAX_BULLET; i++)
	{
		if(g_aBullet[i].bUse)
		{
			// 弾の移動処理
			for (int i = 0; i < MAX_BULLET; i++)
			{
				if (g_aBullet[i].bUse)
				{
					g_aBullet[i].pos.x -= sinf(g_aBullet[i].rot.y) * g_aBullet[i].spd;
					g_aBullet[i].pos.z -= cosf(g_aBullet[i].rot.y) * g_aBullet[i].spd;
				}
			}

			// 影の位置設定
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
// 描画処理
//=============================================================================
void DrawBullet(void)
{

	// ライティングを無効
	SetLightEnable(false);

	D3DXMATRIX mtxView, mtxScale, mtxTranslate, mtxRot;

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_aBullet[i].bUse)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aBullet[i].mtxWorld);

			// ビューマトリックスを取得
			CAMERA *cam = GetCamera();
			mtxView = cam->mtxView;

			// スケールを反映
			D3DXMatrixScaling(&mtxScale, g_aBullet[i].scl.x,
				g_aBullet[i].scl.y,
				g_aBullet[i].scl.z);
			D3DXMatrixMultiply(&g_aBullet[i].mtxWorld, &g_aBullet[i].mtxWorld, &mtxScale);

			// 回転を反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aBullet[i].rot.y + D3DX_PI, g_aBullet[i].rot.x, g_aBullet[i].rot.z);
			D3DXMatrixMultiply(&g_aBullet[i].mtxWorld, &g_aBullet[i].mtxWorld, &mtxRot);

			// 移動を反映
			D3DXMatrixTranslation(&mtxTranslate, g_aBullet[i].pos.x,
				g_aBullet[i].pos.y,
				g_aBullet[i].pos.z);
			D3DXMatrixMultiply(&g_aBullet[i].mtxWorld, &g_aBullet[i].mtxWorld, &mtxTranslate);


			// ワールドマトリックスの設定
			SetWorldMatrix(&g_aBullet[i].mtxWorld);

			// マテリアル設定
			SetMaterial(g_aBullet[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(true);

	// αテストを無効に
	SetAlphaTestEnable(false);
}

//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
BULLET *GetBullet(void)
{
	return &g_aBullet[0];
}


//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexBullet(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = BULLET_WIDTH;
	float fHeight = BULLET_HEIGHT;

	// 頂点座標の設定
	vertex[0].Position = D3DXVECTOR3(-fWidth / 2.0f, 0.0f, fHeight / 2.0f);
	vertex[1].Position = D3DXVECTOR3(fWidth / 2.0f, 0.0f, fHeight / 2.0f);
	vertex[2].Position = D3DXVECTOR3(-fWidth / 2.0f, 0.0f, -fHeight / 2.0f);
	vertex[3].Position = D3DXVECTOR3(fWidth / 2.0f, 0.0f, -fHeight / 2.0f);

	// 拡散光の設定
	vertex[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	vertex[0].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
	vertex[1].TexCoord = D3DXVECTOR2(1.0f, 0.0f);
	vertex[2].TexCoord = D3DXVECTOR2(0.0f, 1.0f);
	vertex[3].TexCoord = D3DXVECTOR2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 弾のパラメータをセット
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

			// 影の設定
			g_aBullet[i].nIdxShadow = CreateShadow(g_aBullet[i].pos, 0.5f, 0.5f);

			nIdxBullet = i;

			break;
		}
	}

	return nIdxBullet;
}
