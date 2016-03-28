#pragma once
#include <vector>
#include <queue>
#include <string>
#include "gdal_priv.h"
using namespace std;

namespace WHU{
#define DEFAULT_MIN_REG_SIZE 50
#define DEFAULT_MIN_GREYTH 35
#define DEFAULT_MAX_GREYTH 220
#define SUFFIX_PNG ".png"
#define SUFFIX_TIFF ".tif"
#define ERR_REGION_EMPTY 1
#define ERR_TIFF_FAIL 2
#define ERR_PNG_FAIL 3
#define ERR_FILE_NULL 4
#define ERR_LACK_BAND 5
	typedef struct pixel_rgb{
		int red;
		int green;
		int blue;
		pixel_rgb(int r, int g, int b) :red(r), green(g), blue(b)
		{}
		bool operator ==(const pixel_rgb &Pixel){
			return red == Pixel.red && green == Pixel.green && blue == Pixel.blue;
		}
	}pixel_rgb_t;

	typedef struct pixel_location{
		int x;
		int y;
		pixel_location(int x, int y) :x(x), y(y)
		{}
	}pixel_loc_t;


	template<typename T>
	class TranspEdge
	{
		typedef vector<pixel_loc_t> pixelArray;
		typedef vector<T> greyArray;
	public:
		TranspEdge();
		TranspEdge(int sizeX, int sizeY);
		bool InitImageSize(int sizeX, int sizeY);
		int TransparentEdge(GDALDataset*&pSrc, int minRegSize = DEFAULT_MIN_REG_SIZE);
		int TransparentEdge(const char*pFilename, char*& pOutFilename,int minRegSize = DEFAULT_MIN_REG_SIZE);
		//TranspEdge(GDALDataset*pDataset, int minRegSize = DEFAULT_MIN_REG_SIZE);
		//int Initialize();
		void Close();
		virtual ~TranspEdge();
	private:
		void ReadData(GDALDataset*pSrc, int sizeX, int sizeY, int bandCount);
		bool makeTmpFilename(const char* pFile, char*&pNew, const char*pSuffix);
		vector<pixel_loc_t>  Grey_RegionGrowth(T* pImgVal, int sizeX, int sizeY, pixel_loc_t seed, vector<T> Noises, int minRegSize, bool* visited);
		void Img2Grey(T min, T max);
		bool InitImgValArray(int sizeX, int sizeY);
		bool InitVectors(int sizeX, int sizeY);
		bool InitVisitedArray(int sizeX, int sizeY, bool defVal = false);
		bool handleTransparentBand(T* pBandVal, int sizeX, int sizeY, vector<WHU::pixel_loc_t>* pPointArray);
		bool Grey_IsNoise(greyArray*pNoises, T* grey);
		template <typename T2>
		bool SetArrayVal(T2 * pArray, int size, T2 setVal){
			for (int i = 0; i < size; ++i)
				pArray[i] = setVal;
			return true;
		};

	private:
		T ** m_ppImgVal;  //图像值
		T * m_pGrey;   //经处理的中间灰度图像
		int m_sizeX;     //图像宽
		int m_sizeY;     //图像长
		int m_bandCount;    //图像波段数
		GDALDataset* m_pSrc;//原图像指针
		int m_minRegSize;//区域阈值
		greyArray  m_vNoises;//候选噪点值
		pixelArray  m_vSeeds;//区域生长种子点
		pixelArray  m_vTransRegion; //需处理区域
		//greyArray * m_pNoises;//候选噪点值
		//pixelArray * m_pSeeds;//区域生长种子点
		//pixelArray * m_pTransRegion; //需处理区域
		bool* m_visited;  //访问控制数组
		bool m_IsInited;
	};
}

//template<typename T>
//WHU::TranspEdge<T>::TranspEdge(GDALDataset*pDataset, int minRegSize)
//:m_pSrc(pDataset), m_minRegSize(minRegSize)
//{
//	
//}

template<typename T>
WHU::TranspEdge<T>::TranspEdge()
:m_IsInited(false)
{

}

template<typename T>
WHU::TranspEdge<T>::TranspEdge(int sizeX, int sizeY)
{
	InitImageSize(sizeX, sizeY);
	m_IsInited = true;
}

template<typename T>
bool WHU::TranspEdge<T>::InitVisitedArray(int sizeX, int sizeY, bool defVal){
	m_visited = new bool[sizeX*sizeY];
	return SetArrayVal<bool>(m_visited, sizeX*sizeY, defVal);
}

//template<typename T>
//int WHU::TranspEdge<T>::Initialize(){
//	
//	
//	return 0;
//
//}

template<typename T>
bool WHU::TranspEdge<T>::InitImgValArray(int sizeX, int sizeY){
	m_ppImgVal = new T*[4];
	for (int i = 0; i < 4; ++i){
		m_ppImgVal[i] = (T*)CPLMalloc(sizeX*sizeY);
	}
	m_pGrey = (T*)CPLMalloc(sizeX*sizeY);
	return true;
}

template<typename T>
bool WHU::TranspEdge<T>::InitVectors(int sizeX, int sizeY){
	//m_pNoises = new greyArray();m_vSeed.
	m_vNoises.push_back(MaxOfT<T>());
	/*m_pSeeds = new pixelArray();*/
	m_vSeeds.push_back(pixel_loc_t(0, 0));
	m_vSeeds.push_back(pixel_loc_t(sizeX - 1, 0));
	m_vSeeds.push_back(pixel_loc_t(0, sizeY - 1));
	m_vSeeds.push_back(pixel_loc_t(sizeX - 1, sizeY - 1));
	//m_pTransRegion = new pixelArray();
	return true;
}

template<typename T>
void WHU::TranspEdge<T>::ReadData(GDALDataset*pSrc, int sizeX, int sizeY, int bandCount){
	T **ppVal = m_ppImgVal;
	for (int i = 0; i < bandCount; ++i){
		GDALRasterBand *pBand = pSrc->GetRasterBand(i + 1);
		pBand->RasterIO(GF_Read, 0, 0, sizeX, sizeY, ppVal[i], sizeX, sizeY, pBand->GetRasterDataType(), 0, 0);
	}
}

template<typename T>
bool WHU::TranspEdge<T>::InitImageSize(int sizeX, int sizeY){
	if (m_IsInited) return false;
	m_sizeX = sizeX;
	m_sizeY = sizeY;
	InitImgValArray(m_sizeX, m_sizeY);
	InitVisitedArray(m_sizeX, m_sizeY, false);
	InitVectors(m_sizeX, m_sizeY);
	return true;
}

template<typename T>
int WHU::TranspEdge<T>::TransparentEdge(const char*pFilename, char*& pOutFilename,int minRegSize = DEFAULT_MIN_REG_SIZE){
	GDALAllRegister();
	GDALDataset*pDataset = (GDALDataset*)GDALOpen(pFilename, GA_ReadOnly);
	int ret = TransparentEdge(pDataset, minRegSize);
	const char*pCurFilename = pDataset->GetDescription();
	cout << strlen(pCurFilename) << endl;
	strcpy_s(pOutFilename, strlen(pCurFilename)+1, pCurFilename);
	GDALClose(pDataset);
	return ret;
}

template<typename T>
int WHU::TranspEdge<T>::TransparentEdge(GDALDataset*&pSrc, int minRegSize){
	if (!pSrc) return ERR_FILE_NULL;
	GDALDataset*pTmpSrc = pSrc;
	m_bandCount = pTmpSrc->GetRasterCount();
	if (m_bandCount != 3 && m_bandCount != 4) return ERR_LACK_BAND;
	m_vTransRegion.clear();
	SetArrayVal<bool>(m_visited, m_sizeX*m_sizeY, false);

	ReadData(pTmpSrc, m_sizeX, m_sizeY, m_bandCount);
	Img2Grey(DEFAULT_MIN_GREYTH, DEFAULT_MAX_GREYTH);

	pixelArray::iterator it = m_vSeeds.begin();
	int curMax = -1;
	while (it != m_vSeeds.end()){
		vector<pixel_loc_t> Region = Grey_RegionGrowth(m_pGrey, m_sizeX, m_sizeY, *it, m_vNoises, 10, m_visited);
		int size = Region.size();
		if (size > curMax){
			curMax = Region.size();
			m_vTransRegion.clear();
			m_vTransRegion.insert(m_vTransRegion.end(), Region.begin(), Region.end());
		}
		++it;
	}
	/*while (it != m_vSeed.end()){
		vector<pixel_loc_t> *pRegion = Grey_RegionGrowth(m_pGrey, m_sizeX, m_sizeY, *it, m_pNoises, 10, m_visited);
		if (pRegion){
			if (pCurMax == NULL) { pCurMax = pRegion; }
			else if (pCurMax->size() < pRegion->size()){
				delete pCurMax;
				pCurMax = pRegion;
			}
			else{
				delete pRegion;
			}
		}
		++it;
	}*/
	//if (pCurMax == NULL) return ERR_REGION_EMPTY;
	//m_pTransRegion->insert(m_pTransRegion->end(), pCurMax->begin(), pCurMax->end());
	//delete pCurMax;
	if (m_vTransRegion.size() == 0) return 0;//nothing to do
	if (m_bandCount == 4){
		GDALRasterBand *pBand = pTmpSrc->GetRasterBand(4);
		pBand->RasterIO(GF_Read, 0, 0, m_sizeX, m_sizeY, m_ppImgVal[3], m_sizeX, m_sizeY, pBand->GetRasterDataType(), 0, 0);
	}
	else{
		SetArrayVal<T>(m_ppImgVal[3], m_sizeX*m_sizeY, MaxOfT<T>());
	}
	handleTransparentBand(m_ppImgVal[3], m_sizeX, m_sizeY, &m_vTransRegion);
	const char*pFilename = pTmpSrc->GetDescription();
	char *pTmpTiffPath = new char[strlen(pFilename) + 1];
	char *pTmpPngPath = new char[strlen(pFilename) + 1];
	char *pCurFilename = new char[strlen(pFilename) + 1];
	strcpy_s(pCurFilename, strlen(pFilename) + 1, pFilename);
	makeTmpFilename(pFilename, pTmpTiffPath, SUFFIX_TIFF);
	makeTmpFilename(pFilename, pTmpPngPath, SUFFIX_PNG);
	GDALDataType gdt = pTmpSrc->GetRasterBand(1)->GetRasterDataType();
	GDALDriver*pTifDriv = GetGDALDriverManager()->GetDriverByName("GTiff");
	GDALDataset*pTiff = pTifDriv->Create(pTmpTiffPath, m_sizeX, m_sizeY, 4, gdt, NULL);
	if (pTiff == nullptr) {
		delete[]pTmpTiffPath;
		delete[]pTmpPngPath;
		delete[]pCurFilename;
		return ERR_TIFF_FAIL;
	}
	for (int i = 0; i < 4; ++i){
		pTiff->GetRasterBand(i + 1)->RasterIO(GF_Write, 0, 0, m_sizeX, m_sizeY, m_ppImgVal[i], m_sizeX, m_sizeY, gdt, 0, 0);
	}
	//关闭、删除原文件
	GDALDriver *pOriDiver = GetGDALDriverManager()->GetDriverByName(pTmpSrc->GetDriver()->GetDescription());
	GDALClose(pTmpSrc);
	pOriDiver->Delete(pCurFilename);
	//创建新文件
	GDALDataset*pResPng = pTiff->GetDriver()->CreateCopy(pTmpPngPath, pTiff, FALSE, NULL, NULL, NULL);
	if (pResPng == nullptr) {
		delete[]pTmpTiffPath;
		delete[]pTmpPngPath;
		delete[]pCurFilename;
		GDALClose(pTiff);
		return ERR_PNG_FAIL;
	}
	//删除临时文件
	GDALClose(pTiff);
	CPLErr retcode1 = pTifDriv->Delete(pTmpTiffPath);
	/*if (retcode2 != CE_None)
		cout<<remove(pFilename);*/
	pSrc = pResPng;
	//pSrc->FlushCache();
	//free
	delete[]pTmpTiffPath;
	delete[]pTmpPngPath;
	delete[]pCurFilename;
	return 0;
}

template<typename T>
void WHU::TranspEdge<T>::Img2Grey(T min, T max){
	for (int i = 0; i < m_sizeX*m_sizeY; ++i){
		T grey = (m_ppImgVal[0][i] * 38 + m_ppImgVal[1][i] * 75 + m_ppImgVal[2][i] * 15) >> 7;
		m_pGrey[i] = grey<min || grey >max ? MaxOfT<T>() : grey;
	}
}

template<typename T>
T MaxOfT(){
	int x = sizeof(T)* 8;
	int x2 = 1 << x;
	return (1 << sizeof(T)* 8) - 1;
}

template<typename T>
vector<WHU::pixel_loc_t> WHU::TranspEdge<T>::Grey_RegionGrowth(T* pImgVal, int sizeX, int sizeY, pixel_loc_t seed, vector<T> Noises, int minRegSize, bool* visited){
	/*pImgVal 图像数据     已经经过灰度变换、阈值处理的灰度图像
	sizeX,sizeY 图像尺寸
	seed 初始噪点
	pNoises 样本噪点集合。采用绝对符合的方法。
	minRegSize 区域尺寸阈值
	visited 访问控制数组
	*/
	pixelArray Array;
	std::queue<pixel_loc_t> pixQueue;
	pixQueue.push(seed);
	int curIdx = seed.x + seed.y*sizeX;
	visited[curIdx] = true;
	/*double regAvg[RGB_BAND_COUNT] = { pImgVal[0][curIdx], pImgVal[1][curIdx], pImgVal[2][curIdx] };
	double tmpAvg[RGB_BAND_COUNT] = { pImgVal[0][curIdx], pImgVal[1][curIdx], pImgVal[2][curIdx] };*/
	while (!pixQueue.empty()){
		pixel_loc_t cur = pixQueue.front();
		pixQueue.pop();
		Array.push_back(cur);
		/*int validCount = 0;*/
		int tmpCurIdx = cur.y*sizeX + cur.x;
		//left
		if (cur.x - 1 >= 0){
			int tmpIdx = cur.y*sizeX + cur.x - 1;
			if (!visited[tmpIdx] && Grey_IsNoise(&Noises, &pImgVal[tmpIdx]))
			{
				pixQueue.push(pixel_loc_t(cur.x - 1, cur.y));
				/*for (int i = 0; i < RGB_BAND_COUNT; ++i)
				tmpAvg[i] = (tmpAvg[i] * (pArray->size() + validCount) + pImgVal[i][tmpIdx]) / (pArray->size() + validCount + 1);*/
				/*++validCount;*/
				visited[tmpIdx] = true;
			}

		}
		//down
		if (cur.y + 1 < sizeY){
			int tmpIdx = (cur.y + 1)*sizeX + cur.x;
			if (!visited[tmpIdx] && Grey_IsNoise(&Noises, &pImgVal[tmpIdx])){
				pixQueue.push(pixel_loc_t(cur.x, cur.y + 1));
				/*for (int i = 0; i < RGB_BAND_COUNT; ++i)
				tmpAvg[i] = (tmpAvg[i] * (pArray->size() + validCount) + pImgVal[i][tmpIdx]) / (pArray->size() + validCount + 1);*/
				/*++validCount;*/
				visited[tmpIdx] = true;
			}

		}
		//right
		if (cur.x + 1 < sizeX){
			int tmpIdx = (cur.y)*sizeX + cur.x + 1;
			if (!visited[tmpIdx] && Grey_IsNoise(&Noises, &pImgVal[tmpIdx])){
				pixQueue.push(pixel_loc_t(cur.x + 1, cur.y));
				/*for (int i = 0; i < RGB_BAND_COUNT; ++i)
				tmpAvg[i] = (tmpAvg[i] * (pArray->size() + validCount) + pImgVal[i][tmpIdx]) / (pArray->size() + validCount + 1);*/
				/*++validCount;*/
				visited[tmpIdx] = true;
			}
		}
		//up
		if (cur.y - 1 >= 0){
			int tmpIdx = (cur.y - 1)*sizeX + cur.x;
			if (!visited[tmpIdx] && Grey_IsNoise(&Noises, &pImgVal[tmpIdx])){
				pixQueue.push(pixel_loc_t(cur.x, cur.y - 1));
				/*for (int i = 0; i < RGB_BAND_COUNT; ++i)
				tmpAvg[i] = (tmpAvg[i] * (pArray->size() + validCount) + pImgVal[i][tmpIdx]) / (pArray->size() + validCount + 1);*/
				/*++validCount;*/
				visited[tmpIdx] = true;
			}
		}
		/*for (int i = 0; i < RGBA_BAND_COUNT; ++i)
		regAvg[i] = tmpAvg[i];*/
	}
	/*delete pixQueue;
	pixQueue = NULL;*/
	/*if (minRegSize == -1 || pArray->size() >= minRegSize)
		return pArray;
	else{
		delete pArray;
		return NULL;
	}*/
	return Array;
}

template<typename T>
bool WHU::TranspEdge<T>::handleTransparentBand(T* pBandVal, int sizeX, int sizeY, vector<WHU::pixel_loc_t>* pPointArray){
	if (!pPointArray || pPointArray->size() == 0) return false;
	vector<WHU::pixel_loc_t>::iterator it = pPointArray->begin();
	while (it != pPointArray->end()){
		pBandVal[sizeX*it->y + it->x] = 0;//set to transparent
		++it;
	}
	return true;
}

template<typename T>
void WHU::TranspEdge<T>::Close(){
	if (m_ppImgVal){
		for (int i = 0; i < 4; ++i){
			if (m_ppImgVal[i])
				CPLFree(m_ppImgVal[i]);
			m_ppImgVal[i] = NULL;
		}
		delete[]m_ppImgVal;
		m_ppImgVal = NULL;
	}
	if (m_pGrey){
		CPLFree(m_pGrey);
		m_pGrey = NULL;
	}
	//if (m_pNoises){
	//	delete m_pNoises;//候选噪点值
	//	m_pNoises = NULL;
	//}
	//if (m_pSeeds){
	//	delete m_pSeeds;//区域生长种子点
	//	m_pSeeds = NULL;
	//}
	//if (m_pTransRegion){
	//	delete m_pTransRegion; //需处理区域
	//	m_pTransRegion = NULL;
	//}
	if (m_visited){
		delete[] m_visited;  //访问控制数组
		m_visited = NULL;
	}
}

template<typename T>
WHU::TranspEdge<T>::~TranspEdge(){
	Close();
}

template<typename T>
bool WHU::TranspEdge<T>::Grey_IsNoise(vector<T>*pNoises, T *grey){
	vector<T>::iterator it = pNoises->begin();
	while (it != pNoises->end()){
		if (*it == *grey) return true;
		else ++it;
	}
	return false;
}

template<typename T>
bool WHU::TranspEdge<T>::makeTmpFilename(const char* pFile, char*&pNew, const char *pSuffix){
	string str = string(pFile);
	if (!pNew) pNew = new char[str.length() + 1];
	size_t pos = str.find_last_of('.');
	strcpy_s(pNew, str.length() + 1, str.replace(pos, size_t(4), pSuffix).c_str());
	return true;
}