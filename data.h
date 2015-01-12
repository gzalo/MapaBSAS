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

class Data{
	public:
		Data();
		~Data();
		int load();
		
		vector <pair<int,int> > lineas;
				
		vector <pair<float,float> > puntos;
		vector <int> alturas;
		
	private:
	
		int cargarUsoSuelo();
		int cargarParcelas();
		
		map <string, int> pisos;
};

#endif
