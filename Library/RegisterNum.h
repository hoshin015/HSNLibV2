#ifndef RegisterNum_h
#define RegisterNum_h

////////////////////////////////////////////////////////////////////
//
//　c++ 側を変えたら hlsl 側も手動で変える
//
////////////////////////////////////////////////////////////////////


// --- c++ ---
#ifdef __cplusplus

// 定数
#define MAX_INSTANCE					512
#define GAUSSIAN_DOWNSAMPLING_COUNT		4

// 定数バッファ
#define _cameraConstant		0		// カメラのデータ
#define _objectConstant		1		// 3d 空間に出す全てのモデルが持つ 
#define _lightConstant		2		// ライトのデータ

#define _luminanceExtractionConstant	5	// 高輝度抽出用
#define _gaussianConstant				5	// ガウシアンフィルタ用
#define _gaussianAvgConstant			5	// ガウシアンフィルタ平均用

// テクスチャ番号
#define _deffuseTexture		0
#define _normalTexture		1
#define _specularTexture	2
#define _emissiveTexture	3
#define _textureNum			4

#define _spriteTexture		0
#define _dissolveTexture	1


// --- hlsl ---
#else

// 定数
#define MAX_INSTANCE					512
#define GAUSSIAN_DOWNSAMPLING_COUNT		4

// 定数バッファ
#define _cameraConstant		b0		// カメラのデータ
#define _objectConstant		b1		// 3d 空間に出す全てのモデルが持つ
#define _lightConstant		b2		// ライトのデータ

#define _luminanceExtractionConstant	b5	// 高輝度抽出用
#define _gaussianConstant				b5	// ガウシアンフィルタ用
#define _gaussianAvgConstant			b5	// ガウシアンフィルタ平均用

// テクスチャ番号
#define _deffuseTexture		t0
#define _normalTexture		t1
#define _specularTexture	t2
#define _emissiveTexture	t3

#define _spriteTexture		t0
#define _dissolveTexture	t1


// サンプラー
#define POINT				0
#define LINEAR				1
#define ANISOTROPIC			2
#define SAMPLER_NUM			4

#endif


#endif