#ifndef RegisterNum_h
#define RegisterNum_h

////////////////////////////////////////////////////////////////////
//
//�@c++ ����ς����� hlsl �����蓮�ŕς���
//
////////////////////////////////////////////////////////////////////


// --- c++ ---
#ifdef __cplusplus

// �萔
#define MAX_INSTANCE					512
#define GAUSSIAN_DOWNSAMPLING_COUNT		4

// �萔�o�b�t�@
#define _cameraConstant		0		// �J�����̃f�[�^
#define _objectConstant		1		// 3d ��Ԃɏo���S�Ẵ��f�������� 
#define _lightConstant		2		// ���C�g�̃f�[�^

#define _luminanceExtractionConstant	5	// ���P�x���o�p
#define _gaussianConstant				5	// �K�E�V�A���t�B���^�p
#define _gaussianAvgConstant			5	// �K�E�V�A���t�B���^���ϗp

// �e�N�X�`���ԍ�
#define _deffuseTexture		0
#define _normalTexture		1
#define _specularTexture	2
#define _emissiveTexture	3
#define _textureNum			4

#define _spriteTexture		0
#define _dissolveTexture	1


// --- hlsl ---
#else

// �萔
#define MAX_INSTANCE					512
#define GAUSSIAN_DOWNSAMPLING_COUNT		4

// �萔�o�b�t�@
#define _cameraConstant		b0		// �J�����̃f�[�^
#define _objectConstant		b1		// 3d ��Ԃɏo���S�Ẵ��f��������
#define _lightConstant		b2		// ���C�g�̃f�[�^

#define _luminanceExtractionConstant	b5	// ���P�x���o�p
#define _gaussianConstant				b5	// �K�E�V�A���t�B���^�p
#define _gaussianAvgConstant			b5	// �K�E�V�A���t�B���^���ϗp

// �e�N�X�`���ԍ�
#define _deffuseTexture		t0
#define _normalTexture		t1
#define _specularTexture	t2
#define _emissiveTexture	t3

#define _spriteTexture		t0
#define _dissolveTexture	t1


// �T���v���[
#define POINT				0
#define LINEAR				1
#define ANISOTROPIC			2
#define SAMPLER_NUM			4

#endif


#endif