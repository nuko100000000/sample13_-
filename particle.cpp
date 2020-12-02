//=============================================================================
//
// パーティクル処理 [particle.cpp]
// Author : GP11A132_19_鈴木信歓
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "particle.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)			// テクスチャの数

#define	PARTICLE_SIZE_X		(4.0f)		// 頂点サイズ
#define	PARTICLE_SIZE_Y		(4.0f)		// 頂点サイズ
#define	VALUE_MOVE_PARTICLE	(5.0f)		// 移動速度

#define	MAX_PARTICLE		(512)		// パーティクル最大数

#define	DISP_SHADOW						// 影の表示
//#undef DISP_SHADOW

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3		pos;				// 位置
	D3DXVECTOR3		rot;				// 回転
	D3DXVECTOR3		scale;				// スケール
	D3DXVECTOR3		move;				// 移動量
	MATERIAL		material;			// マテリアル
	float			fSizeX;				// 幅
	float			fSizeY;				// 高さ
	int				nIdxShadow;			// 影ID
	int				nLife;				// 寿命
	bool			bUse;				// 使用しているかどうか


} PARTICLE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexParticle(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							g_TexNo;				// テクスチャ番号

static D3DXMATRIX				g_mtxWorldParticle;				// ワールドマトリックス

static PARTICLE					g_aParticle[MAX_PARTICLE];		// パーティクルワーク
static D3DXVECTOR3				g_posBase;						// ビルボード発生位置
static float					g_fWidthBase = 5.0f;			// 基準の幅
static float					g_fHeightBase = 10.0f;			// 基準の高さ
static bool						g_bPause = false;				// ポーズON/OFF
static float					g_roty = 0.0f;					// 移動方向
static float					g_spd = 0.0f;					// 移動スピード

static char *g_TextureName[] =
{
	"data/TEXTURE/effect000.jpg",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitParticle(void)
{
	// 頂点情報の作成
	MakeVertexParticle();

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

	// パーティクルワークの初期化
	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		g_aParticle[nCntParticle].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCntParticle].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCntParticle].scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		g_aParticle[nCntParticle].move = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_aParticle[nCntParticle].material, sizeof(g_aParticle[nCntParticle].material));
		g_aParticle[nCntParticle].material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		g_aParticle[nCntParticle].fSizeX = PARTICLE_SIZE_X;
		g_aParticle[nCntParticle].fSizeY = PARTICLE_SIZE_Y;
		g_aParticle[nCntParticle].nIdxShadow = -1;
		g_aParticle[nCntParticle].nLife = 0;
		g_aParticle[nCntParticle].bUse = false;
	}

	g_posBase = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_roty = 0.0f;
	g_spd = 0.0f;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitParticle(void)
{
	//テクスチャの解放
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// 頂点バッファの解放
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateParticle(void)
{
	PLAYER *pPlayer = GetPlayer();
	g_posBase = pPlayer->pos;

	if(GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? false: true;
	}

	if(g_bPause == false)
	{
		for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
		{
			if(g_aParticle[nCntParticle].bUse)
			{// 使用中
				g_aParticle[nCntParticle].pos.x += g_aParticle[nCntParticle].move.x;
				g_aParticle[nCntParticle].pos.z += g_aParticle[nCntParticle].move.z;

				g_aParticle[nCntParticle].pos.y += g_aParticle[nCntParticle].move.y;
				if(g_aParticle[nCntParticle].pos.y <= g_aParticle[nCntParticle].fSizeY / 2)
				{// 着地した
					g_aParticle[nCntParticle].pos.y = g_aParticle[nCntParticle].fSizeY / 2;
					g_aParticle[nCntParticle].move.y = -g_aParticle[nCntParticle].move.y * 0.75f;
				}

				g_aParticle[nCntParticle].move.x += (0.0f - g_aParticle[nCntParticle].move.x) * 0.015f;
				g_aParticle[nCntParticle].move.y -= 0.25f;
				g_aParticle[nCntParticle].move.z += (0.0f - g_aParticle[nCntParticle].move.z) * 0.015f;

#ifdef DISP_SHADOW
				if(g_aParticle[nCntParticle].nIdxShadow != -1)
				{// 影使用中
					float colA;

					// 影の位置設定
					SetPositionShadow(g_aParticle[nCntParticle].nIdxShadow, D3DXVECTOR3(g_aParticle[nCntParticle].pos.x, 0.1f, g_aParticle[nCntParticle].pos.z));

					// 影の色の設定
					colA = g_aParticle[nCntParticle].material.Diffuse.a;
					SetColorShadow(g_aParticle[nCntParticle].nIdxShadow, D3DXCOLOR(0.5f, 0.5f, 0.5f, colA));
				}
#endif

				g_aParticle[nCntParticle].nLife--;
				if(g_aParticle[nCntParticle].nLife <= 0)
				{
					g_aParticle[nCntParticle].bUse = false;
					ReleaseShadow(g_aParticle[nCntParticle].nIdxShadow);
					g_aParticle[nCntParticle].nIdxShadow = -1;
				}
				else
				{
					if(g_aParticle[nCntParticle].nLife <= 80)
					{
						g_aParticle[nCntParticle].material.Diffuse.r = 1.0f - (float)(80 - g_aParticle[nCntParticle].nLife) / 80 * 0.8f;
						g_aParticle[nCntParticle].material.Diffuse.g = 0.0f - (float)(80 - g_aParticle[nCntParticle].nLife) / 80 * 0.1f;
						g_aParticle[nCntParticle].material.Diffuse.b = 0.0f - (float)(80 - g_aParticle[nCntParticle].nLife) / 80 * 0.2f;
					}

					if(g_aParticle[nCntParticle].nLife <= 20)
					{
						// α値設定
						g_aParticle[nCntParticle].material.Diffuse.a -= 0.05f;
						if(g_aParticle[nCntParticle].material.Diffuse.a < 0.0f)
						{
							g_aParticle[nCntParticle].material.Diffuse.a = 0.0f;
						}
					}
				}
			}
		}

		// パーティクル発生
//		if((rand() % 2) == 0)
		{
			D3DXVECTOR3 pos;
			D3DXVECTOR3 move;
			float fAngle, fLength;
			int nLife;
			float fSize;

			pos = g_posBase;

			fAngle = (float)(rand() % 628 - 314) / 100.0f;
			fLength = rand() % (int)(g_fWidthBase * 200 ) / 100.0f - g_fWidthBase;
			move.x = sinf(fAngle) * fLength;
			move.y = rand() % 300 / 100.0f + g_fHeightBase;
			move.z = cosf(fAngle) * fLength;

			nLife = rand() % 100 + 150;  

			fSize = (float)(rand() % 30 + 20);

			pos.y = fSize / 2;

			// ビルボードの設定
			SetParticle(pos, move, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), fSize, fSize, nLife);
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawParticle(void)
{
	D3DXMATRIX mtxView,mtxScale,mtxTranslate;
	CAMERA *cam = GetCamera();

	// ライティングを無効に
	SetLightEnable(false);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ADD);

	// Z比較無し
	SetDepthEnable(false);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if (g_aParticle[nCntParticle].bUse)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_mtxWorldParticle);

			// ビューマトリックスを取得
			mtxView = cam->mtxView;

			g_mtxWorldParticle._11 = mtxView._11;
			g_mtxWorldParticle._12 = mtxView._21;
			g_mtxWorldParticle._13 = mtxView._31;
			g_mtxWorldParticle._21 = mtxView._12;
			g_mtxWorldParticle._22 = mtxView._22;
			g_mtxWorldParticle._23 = mtxView._32;
			g_mtxWorldParticle._31 = mtxView._13;
			g_mtxWorldParticle._32 = mtxView._23;
			g_mtxWorldParticle._33 = mtxView._33;

			// スケールを反映
			D3DXMatrixScaling(&mtxScale, g_aParticle[nCntParticle].scale.x, g_aParticle[nCntParticle].scale.y, g_aParticle[nCntParticle].scale.z);
			D3DXMatrixMultiply(&g_mtxWorldParticle, &g_mtxWorldParticle, &mtxScale);

			// 移動を反映
			D3DXMatrixTranslation(&mtxTranslate, g_aParticle[nCntParticle].pos.x, g_aParticle[nCntParticle].pos.y, g_aParticle[nCntParticle].pos.z);
			D3DXMatrixMultiply(&g_mtxWorldParticle, &g_mtxWorldParticle, &mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&g_mtxWorldParticle);

			// マテリアル設定
			SetMaterial(g_aParticle[nCntParticle].material);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
	// ライティングを有効に
	SetLightEnable(true);

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較有効
	SetDepthEnable(true);
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexParticle(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//頂点バッファの中身を埋める
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// 頂点座標の設定
		vertex[0].Position = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[1].Position = D3DXVECTOR3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[2].Position = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[3].Position = D3DXVECTOR3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);

		// 法線の設定
		vertex[0].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		// 反射光の設定
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
	}

	return S_OK;
}

//=============================================================================
// マテリアルカラーの設定
//=============================================================================
void SetColorParticle(int nIdxParticle, D3DXCOLOR col)
{
	g_aParticle[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
int SetParticle(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxParticle = -1;

	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if(!g_aParticle[nCntParticle].bUse)
		{
			g_aParticle[nCntParticle].pos = pos;
			g_aParticle[nCntParticle].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_aParticle[nCntParticle].scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			g_aParticle[nCntParticle].move = move;
			g_aParticle[nCntParticle].material.Diffuse = col;
			g_aParticle[nCntParticle].fSizeX = fSizeX;
			g_aParticle[nCntParticle].fSizeY = fSizeY;
			g_aParticle[nCntParticle].nLife = nLife;
			g_aParticle[nCntParticle].bUse = true;

			nIdxParticle = nCntParticle;

#ifdef DISP_SHADOW
			// 影の設定
			g_aParticle[nCntParticle].nIdxShadow = CreateShadow(D3DXVECTOR3(pos.x, 0.1f, pos.z), 0.2f, 0.2f);		// 影の設定
			if(g_aParticle[nCntParticle].nIdxShadow != -1)
			{
				SetColorShadow(g_aParticle[nCntParticle].nIdxShadow, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));
			}
#endif

			break;
		}
	}

	return nIdxParticle;
}
