//=============================================================================
//
// ���C������ [main.h]
// Author : GP11A132_19_��ؐM��
//
//=============================================================================
#pragma once


#pragma warning(push)
#pragma warning(disable:4005)

#define _CRT_SECURE_NO_WARNINGS			// scanf ��warning�h�~
#include <stdio.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <d3dx9.h>

#define DIRECTINPUT_VERSION 0x0800		// �x���Ώ�
#include "dinput.h"
#include "mmsystem.h"

#pragma warning(pop)


//*****************************************************************************
// ���C�u�����̃����N
//*****************************************************************************
#pragma comment (lib, "d3d11.lib")		
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "d3dx11.lib")	
#pragma comment (lib, "d3dx9.lib")	
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dxerr.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dinput8.lib")


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SCREEN_WIDTH	(960)			// �E�C���h�E�̕�
#define SCREEN_HEIGHT	(540)			// �E�C���h�E�̍���
#define SCREEN_CENTER_X	(SCREEN_WIDTH / 2)	// �E�C���h�E�̒��S�w���W
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT / 2)	// �E�C���h�E�̒��S�x���W
#define FIELD_WIDTH		(1280.0)
#define FIELD_HEIGHT	(1280.0)
#define FIELD_TOP		(FIELD_HEIGHT / 2)
#define FIELD_DOWN		(-FIELD_HEIGHT / 2)
#define FIELD_LEFT		(-FIELD_WIDTH / 2)
#define FIELD_RIGHT		(FIELD_WIDTH / 2)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct INTERPOLATION_DATA
{
	D3DXVECTOR3 pos;		// ���_���W(�e�̍��W�����_�Ƃ������΍��W)
	D3DXVECTOR3 rot;		// ��]
	D3DXVECTOR3 scl;		// �g��k��
	float		frame;		// ���s�t���[���� ( dt = 1.0f/frame )
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
long GetMousePosX(void);
long GetMousePosY(void);
char* GetDebugStr(void);

