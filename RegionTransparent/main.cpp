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
	WHU::TranspEdgePool<unsigned char> pool;
	WHU::TranspEdge<unsigned char>* p = pool.getInstance(256, 256);
	WHU::TranspEdge<unsigned char>* p2 = pool.getInstance(256, 256);
	char *c = new char[30];
	p->TransparentEdge("D:\\TU\\TE_Data\\9.png", c, 10);
	p2->TransparentEdge("D:\\TU\\TE_Data\\10.png", c, 10);
	delete []c;
	return 0;
	}
	_CrtDumpMemoryLeaks();
}