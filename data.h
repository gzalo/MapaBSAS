#ifndef DATA_H
#define DATA_H

struct shpHeader{
	//BIG ENDIAN
	int code;
	int unused[5];
	int length;
	
	//LITTLE ENDIAN
	int version;
	int shapeType;
	
	double mbr[4];
	double zR[2];
	double mR[2];
} __attribute__((packed));

struct Point{
	double x,y;
} __attribute__((packed));

struct dbfFileHeader{
	unsigned char version;
	unsigned char update[3];
	int numRecordsTable;
	unsigned short posFirstRecord;
	unsigned short lengthDataRecord;
	unsigned char reserved1[16];
	unsigned char tableFlags;
	unsigned char codePageMark;
	unsigned short reserved2;
} __attribute__((packed));

struct dbfFileField{
	char fieldName[11];
	char fieldType;
	int fieldDisp;
	unsigned char fieldLen;
	char fieldLenDec;
	char fieldFlags;
	unsigned int nextAutoIncrement;
	char autoIncrementStep; 
	unsigned char reserved[8];
} __attribute__((packed));

struct Poligono{
	vector <Point> puntos;
	int altura;
	int cantPisos;
};

class Data{
	public:
		Data();
		~Data();
		int load();
		
		vector <Poligono> parcelas;
		vector <Poligono> manzanas;
		vector <Poligono> verde;
			
	private:
		int cargarUsoSuelo();
		int cargarParcelas();
		int cargarSHP(vector <Poligono> *resultado, const string &filename);
		
		void agregarEstadisticaSM(const string &smp, int cantPisos);
		float estimarCantidadPisosSM(const string &smp);
		map <string, float> parcelaSumaPisos;
		map <string, float> parcelaSumaPisosCuadrado;
		map <string, int> parcelaCantidadPisos;
		
		map <string, int> pisos;
};

#endif
