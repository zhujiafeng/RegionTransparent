#ifndef GDAL_UTILS_H
#define GDAL_UTILS_H
#include <gdal_priv.h>

GDALDataset* GetDataset(const char*pFilename, GDALAccess access = GA_ReadOnly){
	GDALAllRegister();
	return  (GDALDataset*)GDALOpen(pFilename, access);
}
template<typename T>
T **ReadData(const char*pFilename, GDALAccess access = GA_ReadOnly){
	GDALDataset *pDataset = GetDataset(pFilename, access);
	return ReadData<T>(pDataset,access);
}

template<typename T>
T **ReadData(GDALDataset* pDataset,GDALAccess access = GA_ReadOnly){
	int sizeX = pDataset->GetRasterXSize();
	int sizeY = pDataset->GetRasterYSize();
	int bandCount = pDataset->GetRasterCount();
	T **ppVal = new T*[bandCount];
	for (int i = 0; i < bandCount; ++i){
		ppVal[i] = (T*)CPLMalloc(sizeX*sizeY);
		GDALRasterBand *pBand = pDataset->GetRasterBand(i + 1);
		pBand->RasterIO(GF_Read, 0, 0, sizeX, sizeY, ppVal[i], sizeX, sizeY, pBand->GetRasterDataType(), 0, 0);
	}
	return ppVal;
}

template<typename T>
bool WriteData(const char *pFilename, T**pVal, int bandCount, int sizeX, int sizeY){
	WriteData<T>(GetDataset(pFilename, GA_Update));
}

template<typename T>
bool WriteData(GDALDataset *pDataset,T**pVal,int bandCount,int sizeX,int sizeY){
	for (int i = 0; i < bandCount; ++i){
		GDALRasterBand*pBand = pDataset->GetRasterBand(i + 1);
		pBand->RasterIO(GF_Write, 0, 0, sizeX, sizeY, pBand, sizeX, sizeY, pBand->GetRasterDataType, 0, 0);
	}
}

GDALDataset* CreateDataset(const char*pFilename, const char* pFormat, int bandCount, int sizeX, int sizeY, GDALDataType dataType){
	GDALAllRegister();
	GDALDriver*pDriver = GetGDALDriverManager()->GetDriverByName(pFormat);
	return pDriver->Create(pFilename, sizeX, sizeY, bandCount, dataType, NULL);
}


#endif