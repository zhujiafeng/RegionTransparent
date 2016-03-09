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
		template<typename T1,typename T2>
		T2 * ConvertDataType(T1*pBuffer, int nLength){
			CPLAssert(sizeof(T2) == sizeof(T1));
			T2 *pTmp = new T2[nLength];
			memcpy_s(pTmp, sizeof(T2)*nLength, pBuffer, sizeof(T1)*nLength);
			delete[]pBuffer;
			return pTmp;
		}
		//int TransparentEdge(GDALDataset*&pSrc,int minRegSize=DEFAULT_MIN_REG_SIZE);
		int TransparentEdge(T *&pBuffer, int& nLength);
		int TransparentEdge(T *&pBuffer, int& nLength, int sizeX, int sizeY);
		//TranspEdge(GDALDataset*pDataset, int minRegSize = DEFAULT_MIN_REG_SIZE);
		//int Initialize();
		void Close();
		virtual ~TranspEdge();
	private:
		//void ReadData(GDALDataset*pSrc, int sizeX,int sizeY,int bandCount);
		bool makeTmpFilename(const char* pFile, char*&pNew);
		vector<pixel_loc_t> * Grey_RegionGrowth(T* pImgVal, int sizeX, int sizeY, pixel_loc_t seed, vector<T>*pNoises, int minRegSize, bool* visited);
		void Img2Grey(T**ppVal,T min,T max);
		bool InitImgValArray(int sizeX,int sizeY);
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
		//T ** m_ppImgVal;  //图像值
		T * m_pGrey;   //经处理的中间灰度图像
		int m_sizeX;     //图像宽
		int m_sizeY;     //图像长
		int m_bandCount;    //图像波段数
		//GDALDataset* m_pSrc;//原图像指针
		int m_minRegSize;//区域阈值
		greyArray * m_pNoises;//候选噪点值
		pixelArray * m_pSeeds;//区域生长种子点
		pixelArray * m_pTransRegion; //需处理区域
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
	/*m_ppImgVal = new T*[4];
	for (int i = 0; i < 4; ++i){
		m_ppImgVal[i] = (T*)CPLMalloc(sizeX*sizeY);
	}*/
	m_pGrey = (T*)CPLMalloc(sizeX*sizeY);
	return true;
}

template<typename T>
bool WHU::TranspEdge<T>::InitVectors(int sizeX, int sizeY){
	m_pNoises = new greyArray();
	m_pNoises->push_back(MaxOfT<T>());
	m_pSeeds = new pixelArray();
	m_pSeeds->push_back(pixel_loc_t(0, 0));
	m_pSeeds->push_back(pixel_loc_t(sizeX - 1, 0));
	m_pSeeds->push_back(pixel_loc_t(0, sizeY - 1));
	m_pSeeds->push_back(pixel_loc_t(sizeX-1, sizeY-1));
	m_pTransRegion = new pixelArray();
	return true;
}

//template<typename T>
//void WHU::TranspEdge<T>::ReadData(GDALDataset*pSrc, int sizeX, int sizeY, int bandCount){
//		T **ppVal = m_ppImgVal;
//		for (int i = 0; i < bandCount; ++i){
//			GDALRasterBand *pBand = pSrc->GetRasterBand(i + 1);
//			pBand->RasterIO(GF_Read, 0, 0, sizeX, sizeY, ppVal[i], sizeX, sizeY, pBand->GetRasterDataType(), 0, 0);
//		}
//}

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

//template<typename T>
//int WHU::TranspEdge<T>::TransparentEdge(GDALDataset*&pSrc, int minRegSize){
//	GDALDataset*pTmpSrc = pSrc;
//	m_bandCount = pTmpSrc->GetRasterCount();
//	//reInit data
//	m_pTransRegion->clear();
//	SetArrayVal<bool>(m_visited, m_sizeX*m_sizeY, false);
//
//	ReadData(pTmpSrc,m_sizeX, m_sizeY, m_bandCount);
//	Img2Grey(m_ppImgVal, DEFAULT_MIN_GREYTH, DEFAULT_MAX_GREYTH);
//	
//	pixelArray::iterator it = m_pSeeds->begin();
//	vector<pixel_loc_t> *pCurMax = NULL;
//	while (it != m_pSeeds->end()){
//		vector<pixel_loc_t> *pRegion = Grey_RegionGrowth(m_pGrey, m_sizeX, m_sizeY, *it, m_pNoises, 10, m_visited);
//		if (pRegion){
//			if (pCurMax == NULL) { pCurMax = pRegion; }
//			else if (pCurMax->size() < pRegion->size()){
//				delete pCurMax;
//				pCurMax = pRegion;
//			}
//			else{
//				delete pRegion;
//			}
//		}
//		++it;
//	}
//	m_pTransRegion->insert(m_pTransRegion->end(), pCurMax->begin(), pCurMax->end());
//	if (m_pTransRegion->size() == 0) return 0;//nothing to do
//	if (m_bandCount == 4){
//		GDALRasterBand *pBand = pTmpSrc->GetRasterBand(4);
//		pBand->RasterIO(GF_Read, 0, 0, m_sizeX, m_sizeY, m_ppImgVal[3], m_sizeX, m_sizeY,pBand->GetRasterDataType(),0,0);
//	}
//	else{
//		SetArrayVal<T>(m_ppImgVal[3], m_sizeX*m_sizeY, MaxOfT<T>());
//	}
//	handleTransparentBand(m_ppImgVal[3], m_sizeX, m_sizeY, m_pTransRegion);
//	const char*pFilename = pTmpSrc->GetDescription();
//	char *pTmpTiffPath = new char[strlen(pFilename) + 1];
//	makeTmpFilename(pFilename, pTmpTiffPath);
//	GDALDataType gdt = pTmpSrc->GetRasterBand(1)->GetRasterDataType();
//	GDALDriver*pTifDriv = GetGDALDriverManager()->GetDriverByName("GTiff");
//	GDALDataset*pTiff = pTifDriv->Create(pTmpTiffPath, m_sizeX, m_sizeY, 4, gdt, NULL);
//	for (int i = 0; i < 4; ++i){
//		pTiff->GetRasterBand(i + 1)->RasterIO(GF_Write, 0, 0, m_sizeX, m_sizeY, m_ppImgVal[i], m_sizeX, m_sizeY, gdt, 0, 0);
//	}
//	
//	GDALDataset*pResPng = pTiff->GetDriver()->CreateCopy(pFilename, pTiff, FALSE, NULL, NULL, NULL);
//	GDALClose(pTiff);
//	CPLErr retcode = GetGDALDriverManager()->GetDriverByName("GTiff")->Delete(pTmpTiffPath);
//	if (retcode != CE_None)
//		remove(pTmpTiffPath);
//	pSrc = pResPng;
//	pSrc->FlushCache();
//	//GDALClose(pTmpSrc);
//	
//	return 0;
//}

template<typename T>
void WHU::TranspEdge<T>::Img2Grey(T**ppVal,T min,T max){
	for (int i = 0; i < m_sizeX*m_sizeY; ++i){
		T grey = (ppVal[0][i] * 38 + ppVal[1][i] * 75 + ppVal[2][i] * 15) >> 7;
		m_pGrey[i] = grey<min || grey >max ? MaxOfT<T>() :grey ;
	}
}

template<typename T>
T MaxOfT(){
	int x = sizeof(T)*8;
	int x2 = 1 << x;
	return (1 << sizeof(T)*8)-1;
}

template<typename T>
vector<WHU::pixel_loc_t> * WHU::TranspEdge<T>::Grey_RegionGrowth(T* pImgVal, int sizeX, int sizeY, pixel_loc_t seed, vector<T>*pNoises, int minRegSize, bool* visited){
	/*pImgVal 图像数据     已经经过灰度变换、阈值处理的灰度图像
	sizeX,sizeY 图像尺寸
	seed 初始噪点
	pNoises 样本噪点集合。采用绝对符合的方法。
	minRegSize 区域尺寸阈值
	visited 访问控制数组
	*/
	pixelArray*pArray = new pixelArray;
	std::queue<pixel_loc_t> *pixQueue = new std::queue<pixel_loc_t>;
	pixQueue->push(seed);
	int curIdx = seed.x + seed.y*sizeX;
	visited[curIdx] = true;
	/*double regAvg[RGB_BAND_COUNT] = { pImgVal[0][curIdx], pImgVal[1][curIdx], pImgVal[2][curIdx] };
	double tmpAvg[RGB_BAND_COUNT] = { pImgVal[0][curIdx], pImgVal[1][curIdx], pImgVal[2][curIdx] };*/
	while (!pixQueue->empty()){
		pixel_loc_t cur = pixQueue->front();
		pixQueue->pop();
		pArray->push_back(cur);
		/*int validCount = 0;*/
		int tmpCurIdx = cur.y*sizeX + cur.x;
		//left
		if (cur.x - 1 >= 0){
			int tmpIdx = cur.y*sizeX + cur.x - 1;
			if (!visited[tmpIdx] && Grey_IsNoise(pNoises, &pImgVal[tmpIdx]))
			{
				pixQueue->push(pixel_loc_t(cur.x - 1, cur.y));
				/*for (int i = 0; i < RGB_BAND_COUNT; ++i)
				tmpAvg[i] = (tmpAvg[i] * (pArray->size() + validCount) + pImgVal[i][tmpIdx]) / (pArray->size() + validCount + 1);*/
				/*++validCount;*/
				visited[tmpIdx] = true;
			}

		}
		//down
		if (cur.y + 1 < sizeY){
			int tmpIdx = (cur.y + 1)*sizeX + cur.x;
			if (!visited[tmpIdx] && Grey_IsNoise(pNoises, &pImgVal[tmpIdx])){
				pixQueue->push(pixel_loc_t(cur.x, cur.y + 1));
				/*for (int i = 0; i < RGB_BAND_COUNT; ++i)
				tmpAvg[i] = (tmpAvg[i] * (pArray->size() + validCount) + pImgVal[i][tmpIdx]) / (pArray->size() + validCount + 1);*/
				/*++validCount;*/
				visited[tmpIdx] = true;
			}

		}
		//right
		if (cur.x + 1 < sizeX){
			int tmpIdx = (cur.y)*sizeX + cur.x + 1;
			if (!visited[tmpIdx] && Grey_IsNoise(pNoises, &pImgVal[tmpIdx])){
				pixQueue->push(pixel_loc_t(cur.x + 1, cur.y));
				/*for (int i = 0; i < RGB_BAND_COUNT; ++i)
				tmpAvg[i] = (tmpAvg[i] * (pArray->size() + validCount) + pImgVal[i][tmpIdx]) / (pArray->size() + validCount + 1);*/
				/*++validCount;*/
				visited[tmpIdx] = true;
			}
		}
		//up
		if (cur.y - 1 >= 0){
			int tmpIdx = (cur.y - 1)*sizeX + cur.x;
			if (!visited[tmpIdx] && Grey_IsNoise(pNoises, &pImgVal[tmpIdx])){
				pixQueue->push(pixel_loc_t(cur.x, cur.y - 1));
				/*for (int i = 0; i < RGB_BAND_COUNT; ++i)
				tmpAvg[i] = (tmpAvg[i] * (pArray->size() + validCount) + pImgVal[i][tmpIdx]) / (pArray->size() + validCount + 1);*/
				/*++validCount;*/
				visited[tmpIdx] = true;
			}
		}
		/*for (int i = 0; i < RGBA_BAND_COUNT; ++i)
		regAvg[i] = tmpAvg[i];*/
	}
	delete pixQueue;
	pixQueue = NULL;
	if (minRegSize==-1 || pArray->size() >= minRegSize)
		return pArray;
	else
		return NULL;
}

template<typename T>
bool WHU::TranspEdge<T>::handleTransparentBand(T* pBandVal, int sizeX, int sizeY, vector<WHU::pixel_loc_t>* pPointArray){
	if (!pPointArray || pPointArray->size() == 0) return false;
	vector<WHU::pixel_loc_t>::iterator it = pPointArray->begin();
	while (it != pPointArray->end()){
		pBandVal[sizeX*it->y + it->x] = 0;
		++it;
	}
	return true;
}

template<typename T>
void WHU::TranspEdge<T>::Close(){
	/*if (m_ppImgVal){
		for (int i = 0; i < 4; ++i){
			if (m_ppImgVal[i])
				CPLFree(m_ppImgVal[i]);
			m_ppImgVal[i] = NULL;
		}
		delete[]m_ppImgVal;
		m_ppImgVal = NULL;
	}*/
	if (m_pGrey){
		CPLFree(m_pGrey);
		m_pGrey = NULL;
	}
	if (m_pNoises){
		delete m_pNoises;//候选噪点值
		m_pNoises = NULL;
	}
	if (m_pSeeds){
		delete m_pSeeds;//区域生长种子点
		m_pSeeds = NULL;
	}
	if (m_pTransRegion){
		delete m_pTransRegion; //需处理区域
		m_pTransRegion = NULL;
	}
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
int WHU::TranspEdge<T>::TransparentEdge(T*&pBuffer, int& nLength){
	return TransparentEdge(pBuffer,nLength,m_sizeX,m_sizeY);
}

template<typename T>
int WHU::TranspEdge<T>::TransparentEdge(T*&pBuffer, int& nLength, int sizeX, int sizeY){
	m_sizeX = sizeX;
	m_sizeY = sizeY;
	m_pTransRegion->clear();
	SetArrayVal<bool>(m_visited, m_sizeX*m_sizeY, false);
	m_bandCount = nLength / (sizeX*sizeY);
	T *ppVal[4];
	for (int i = 0; i < m_bandCount; ++i){
		ppVal[i] = pBuffer + i*(sizeX*sizeY);
	}
	Img2Grey(ppVal, DEFAULT_MIN_GREYTH, DEFAULT_MAX_GREYTH);
	pixelArray::iterator it = m_pSeeds->begin();
	vector<pixel_loc_t> *pCurMax = NULL;
	while (it != m_pSeeds->end()){
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
	}
	m_pTransRegion->insert(m_pTransRegion->end(), pCurMax->begin(), pCurMax->end());
	if (m_pTransRegion->size() == 0) return 0;//nothing to do
	if (m_bandCount == 4){
		ppVal[3] = pBuffer + 3 * (m_sizeX*m_sizeY);
		handleTransparentBand(ppVal[3], m_sizeX, m_sizeY, m_pTransRegion);
	}
	else{
		T *pNewBuffer = (T*)CPLMalloc(m_sizeX*m_sizeY*4);
		memcpy_s(pNewBuffer, m_sizeX*m_sizeY*3, pBuffer, m_sizeX*m_sizeY*3);
		SetArrayVal<T>(pNewBuffer + 3 * m_sizeX*m_sizeY, m_sizeX*m_sizeY, MaxOfT<T>());//此处算法可优化
		handleTransparentBand(pNewBuffer + 3 * m_sizeX*m_sizeY, m_sizeX, m_sizeY, m_pTransRegion);
		CPLFree(pBuffer);
		pBuffer = pNewBuffer;
		nLength = m_sizeX*m_sizeY * 4;
	}
	return 0;
}

template<typename T>
bool WHU::TranspEdge<T>::makeTmpFilename(const char* pFile, char*&pNew){
	string str = string(pFile);
	if (!pNew) pNew = new char[str.length() + 1];
	size_t pos = str.find_last_of('.');
	strcpy_s(pNew, str.length() + 1, str.replace(pos, size_t(4), ".tif").c_str());
	return true;
}