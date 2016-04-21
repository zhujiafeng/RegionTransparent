using namespace std;
#include <iostream>
#include "TranspEdge.h"
#include "DEMResampling.h"

int main(void){
	/*const char *filename = "D:\\TU\\DEM\\H49C001002.asc";
	GDALAllRegister();
	GDALDataset* pOri = (GDALDataset*)GDALOpen(filename, GA_ReadOnly);
	GDALDriver *pDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
	int sizeX = pOri->GetRasterXSize();
	int sizeY = pOri->GetRasterYSize();
	double oriGeo[6], desGeo[6];
	pOri->GetGeoTransform(oriGeo);
	memcpy_s(desGeo, sizeof(double)* 6, oriGeo, sizeof(double)* 6);
	desGeo[1] *= 2;
	desGeo[5] *= 2;
	GDALDataset*pNew=pDriver->Create("D:\\TU\\DEM\\test.tif", sizeX/4, sizeY/4, 1, pOri->GetRasterBand(1)->GetRasterDataType(), nullptr);
	pNew->SetGeoTransform(desGeo);
	WHU::DemResample<float> ds;
	ds.DemResampling(pOri, pNew);*/
	{
	//	//WHU::TranspEdgePool<unsigned char>* pIns = WHU::TranspEdgePool<unsigned char>::getInstance();
	char *c = new char[30];
	//WHU::TranspEdge<unsigned char>* pWorker = pIns->getWorker(256, 256);
	//pWorker->TransparentEdge("D:\\TU\\TE_Data\\1.png", c, 10);
	////pIns->TransparentEdge("D:\\TU\\TE_Data\\10.png", c, 10);
	//
	//return 0;
		const WHU::TranspEdgePool<unsigned char>* pIns = WHU::TranspEdgePool<unsigned char>::getInstance();
		const WHU::TranspEdgePool<unsigned char>* pIns2 = WHU::TranspEdgePool<unsigned char>::getInstance();
		WHU::TranspEdge<unsigned char>*p=pIns->getWorker(256, 256);
		WHU::TranspEdge<unsigned char>*p2 = pIns->getWorker(256, 256);
		WHU::TranspEdgePool<unsigned char>::releaseWorker(p);
		WHU::TranspEdge<unsigned char>*p3 = pIns->getWorker(256, 256);
		p->TransparentEdge("D:\\TU\\TE_Data\\5.png", c, 10);

		delete[]c;
	}
	_CrtDumpMemoryLeaks();
	return 0;
}