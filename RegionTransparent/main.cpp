using namespace std;
#include <iostream>
#include "TranspEdge.h"
#define _DEBUG
#ifdef _DEBUG
#include "MemoryLeak.h"
#endif
int main(void){
	/*_CrtSetBreakAlloc(149);
	_CrtSetBreakAlloc(476);
	_CrtSetBreakAlloc(471);
	_CrtSetBreakAlloc(470);

	_CrtSetBreakAlloc(469);
	_CrtSetBreakAlloc(464);
	_CrtSetBreakAlloc(463);
	_CrtSetBreakAlloc(462);
	_CrtSetBreakAlloc(457);
	_CrtSetBreakAlloc(456);*//*
	_CrtSetBreakAlloc(470);
	_CrtSetBreakAlloc(463);
	_CrtSetBreakAlloc(456);*/
		typedef unsigned char T;
		const char*pFilename = "D:\\TU\\TE_Data\\C69.jpg";
		//TransparentEdge<T>(pSrc, 50);
		WHU::TranspEdge<T> te;
		te.InitImageSize(256, 256);
		//te.TransparentEdge(pSrc);
		te.TransparentEdge(pFilename);
		//int *p = new int;
		
		te.Close();
		_CrtDumpMemoryLeaks();
	return 0;
}


//cout << pSrc->GetDescription();
//int sizeX = pSrc->GetRasterXSize();
//int sizeY = pSrc->GetRasterYSize();
//int bandCount = pSrc->GetRasterCount();
//T **ppVal = ReadData<T>(pSrc);
//pDes = CreateDataset(pTmp, "GTiff", 1, sizeX, sizeY, GDT_Byte);
//T*pGrey = ImageToGrey<T, 3>(ppVal, sizeX, sizeY);
//Grey_ThresholdingNormailzed<T>(pGrey, sizeX*sizeY, 35, 220, 255);
//vector<pixelPointArray*> *pRegions = new vector<pixelPointArray*>();
//vector<T> *pNoises = new vector<T>();
//pNoises->push_back(255);
//vector<pixel_loc_t> *pSeeds = new vector<pixel_loc_t>();
//pSeeds->push_back(pixel_loc_t(0, 0));
//pSeeds->push_back(pixel_loc_t(0, sizeY - 1));
//pSeeds->push_back(pixel_loc_t(sizeX - 1, 0));
//pSeeds->push_back(pixel_loc_t(sizeX - 1, sizeY - 1));
//
//vector<pixel_loc_t>::const_iterator it = pSeeds->begin();
//bool *visited = new bool[sizeX*sizeY];
//for (int i = 0; i < sizeX*sizeY; ++i) visited[i] = false;
//while (it != pSeeds->end()){
//	vector<pixel_loc_t> *pRegion = Grey_RegionGrowth<T>(pGrey, sizeX, sizeY, *it, pNoises, 50, visited);
//	if (pRegion){
//		pRegions->push_back(pRegion);
//	}
//	++it;
//}
//
//int maxSize = (*pRegions)[0]->size();
//int maxIdx = 0;
//for (int i = 1; i < pRegions->size(); ++i){
//	if ((*pRegions)[i]->size() >maxSize){
//		delete (*pRegions)[maxIdx];
//		maxSize = (*pRegions)[i]->size();
//		maxIdx = i;
//	}
//}
//vector<pixel_loc_t>*pFinalRegion = (*pRegions)[maxIdx];
//delete pRegions;
//
//T* pAlphaArray = NULL;
//if (bandCount == RGB_BAND_COUNT){
//	pAlphaArray = (T*)CPLMalloc(sizeX*sizeY);
//	initBandArray<T>(pAlphaArray, sizeX, sizeY);
//	handleTransparentBand(pAlphaArray, sizeX, sizeY, pFinalRegion);
//}
//else if (bandCount == RGBA_BAND_COUNT){
//	pAlphaArray = ppVal[bandCount - 1];
//	handleTransparentBand(pAlphaArray, sizeX, sizeY, pFinalRegion);
//}
//GDALDataset*pTiff = CreateDataset("D:\\TU\\regionPNG\\tmp.tif", "GTiff", 4, sizeX, sizeY, GDT_Byte);
//for (int i = 0; i < 3; ++i){
//	pTiff->GetRasterBand(i + 1)->RasterIO(GF_Write, 0, 0, sizeX, sizeY, ppVal[i], sizeX, sizeY, GDT_Byte, 0, 0);
//}
//pTiff->GetRasterBand(4)->RasterIO(GF_Write, 0, 0, sizeX, sizeY, pAlphaArray, sizeX, sizeY, GDT_Byte, 0, 0);
//pTiff->GetDriver()->CreateCopy("D:\\TU\\regionPNG\\tmp.png", pTiff, FALSE, NULL, NULL, NULL);
//pTiff->FlushCache();
//GDALClose(pTiff);
