#include "mapa.h"
#include "data.h"
#include <random>
#include <ctime>

static inline unsigned int endianSwap(unsigned int ui){
    return	(ui >> 24) |
		((ui<<8) & 0x00FF0000) |
		((ui>>8) & 0x0000FF00) |
		(ui << 24);
}

static unsigned int findNth(const string &str, char needle, int *pos){
	int oc=0;
	for(unsigned int i=0;i<str.size();i++){
		if(str[i] == needle){
			pos[oc++] = i;
		}
	}
	return oc;
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
        elems.push_back(item);
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

void Data::agregarEstadisticaSM(const string &smp, int cantPisos){
	vector <string> separado = split(smp, '-');
	
	if(separado.size() != 3){
		cerr << "[STA] SMP mal formado: " << smp << endl;
	}else{
		string sm = separado[0] + "-" + separado[1];
		
		parcelaSumaPisos[sm] += cantPisos;
		parcelaSumaPisosCuadrado[sm] += cantPisos*cantPisos;
		parcelaCantidadPisos[sm] ++;
	}
}

float Data::estimarCantidadPisosSM(const string &smp){
	vector <string> separado = split(smp, '-');
	
	string sm = separado[0] + "-" + separado[1];

	if(parcelaCantidadPisos[sm] == 0){
		return 1.0f;
	}else if(parcelaCantidadPisos[sm] == 1){
		return parcelaSumaPisos[sm];
	}
	
	float mu = parcelaSumaPisos[sm]/parcelaCantidadPisos[sm];
	float sigma = sqrt(parcelaSumaPisosCuadrado[sm]/parcelaCantidadPisos[sm]-mu*mu);
	
	default_random_engine de(time(0));
	normal_distribution<> d(mu,sigma);
	
	return max((float)d(de), 0.5f);
}

int Data::cargarUsoSuelo(){
	ifstream inCSV("res/uso-suelo-2011.csv");
	if(!inCSV.is_open()){ cerr << "[CSV] Imposible abrir archivo." << endl; return -1; }
	
	int linea = 0, lineasInconsistentes=0;
	map <string, int> posCampo;
	int posSmp=0, posPisos=0;
	
	while(!inCSV.eof()){
		string csvLine;
		getline(inCSV,csvLine);
		
		if(linea == 0){
			stringstream ss(csvLine);
			string token;
			int idCampo = 0;
			
			while(getline(ss, token, ';'))
				posCampo[token] = idCampo++;
				
			posSmp = posCampo["smp"]-1;
			posPisos = posCampo["pisos"]-1;
		}else{
			int posiciones[32];
			if(findNth(csvLine, ';', posiciones) != posCampo.size()-1){
				//cerr << "[CSV] Linea " << linea << " inconsistente." << endl;
				lineasInconsistentes++;
				continue;
			}
			
			string smp   = csvLine.substr(posiciones[posSmp]+1, posiciones[posSmp+1]-posiciones[posSmp]-1);
			string strPisos = csvLine.substr(posiciones[posPisos]+1, posiciones[posPisos+1]-posiciones[posPisos]-1);
			
			if(smp.size()<11) cerr << "[CSV] SMP con pocos caracteres: " << smp << endl;
			
			stringstream ss(strPisos);
			int cantPisos=0;
			
			if(!(ss>>cantPisos)){
				cerr << "[CSV] Error en piso (linea " << linea << ")" << endl;
			}else{
				pisos[smp] = cantPisos;
				agregarEstadisticaSM(smp, cantPisos);
			}
		}
		linea++;
	}
	
	inCSV.close();
	cout << "[CSV] Analizadas " << linea << " lineas." << endl;
	cout << "[CSV] Lineas inconsistentes: " << lineasInconsistentes << endl;
	return 0;
}

int Data::cargarParcelas(){
	ifstream inSHP("res/mo_parcelascatastro_15072014.shp", ios::binary);
	if(!inSHP.is_open()){ cerr << "[SHP] Imposible abrir archivo." << endl; return -1; }
	ifstream inDBF("res/mo_parcelascatastro_15072014.dbf", ios::binary);
	if(!inDBF.is_open()){ cerr << "[DBF] Imposible abrir archivo." << endl; return -1; }
		
	shpHeader headerSHP;
	assert(sizeof(headerSHP) == 100);
	inSHP.read((char*) &headerSHP, sizeof(headerSHP));
	headerSHP.length = 2*endianSwap(headerSHP.length);
	assert(headerSHP.code == 0x0a270000);
	
	dbfFileHeader headerDBF;
	assert(sizeof(headerDBF) == 32);
	inDBF.read((char*) &headerDBF, sizeof(headerDBF));
	
	dbfFileField fieldDBF;
	assert(sizeof(fieldDBF) == 32);
	
	map <string, int> fields;
	map <string, int> fieldsStart;
	
	int id = 0, totalSize=0;
	while(inDBF.get() != 0x0D){
		inDBF.seekg(-1, ios::cur);
		inDBF.read((char*)&fieldDBF, sizeof(fieldDBF));		
		
		fields[fieldDBF.fieldName] = id++;
		fieldsStart[fieldDBF.fieldName] = totalSize;
		totalSize += (int)fieldDBF.fieldLen;
	}	

	inDBF.seekg(headerDBF.posFirstRecord, ios::beg);

	int smpInterpolado = 0;
	
	default_random_engine de(time(0));
	normal_distribution<float> altoPisos(6, 1);
	
	while(inSHP.peek() != EOF){
	
		int rNumber, rLength, rType;
		inSHP.read((char*)&rNumber, 4);
		inSHP.read((char*)&rLength, 4);
		inSHP.read((char*)&rType, 4);
		
		rNumber = endianSwap(rNumber);
		rLength = endianSwap(rLength)*2;
		
		if(rType != 5) {cerr << "[SHP] Error, tipo no poligono." << endl; return -1;}
		
		double box[4];
		int numParts, numPoints;
		inSHP.read((char*)&box, 4*8);
		inSHP.read((char*)&numParts, 4);
		inSHP.read((char*)&numPoints, 4);
				
		int parts[numParts];
		inSHP.read((char*)&parts, numParts*4);
		
		Point points[numPoints];
		inSHP.read((char*)&points, numPoints*sizeof(Point));
	
		Poligono poly;

		for(int i=0;i<numPoints-1;i++)
			poly.puntos.push_back((Point){points[i].x-93844, 19200-(points[i].y-91726)});
		
		char data[totalSize+2];
		inDBF.read((char*)&data, totalSize+1);
		data[totalSize+1] = 0;

		string smp(data+fieldsStart["SMP"]+1);
		smp = smp.substr(0, smp.find(' '));
		transform(smp.begin(), smp.end(), smp.begin(), ::toupper);
		
		if(pisos.count(smp) == 0){
			smpInterpolado++;
			poly.cantPisos = estimarCantidadPisosSM(smp);
			poly.altura = poly.cantPisos*altoPisos(de);
		}else{
			if(pisos[smp] == 0){
				pisos[smp] = 1;
				//Suelen ser estacionamientos
			}
			poly.cantPisos = pisos[smp];
			poly.altura = poly.cantPisos*altoPisos(de);
		}			
		parcelas.push_back(poly);
	}
	
	assert(inDBF.get() == 0x1A);
	assert(inDBF.peek() == EOF);
	
	inDBF.close();
	inSHP.close();
	
	cout << "[SHP] Cargados " << parcelas.size() << " poligonos." << endl;
	cout << "[SHP] Interpoladas " << smpInterpolado << " parcelas." << endl;
	
	return 0;
}

int Data::cargarSHP(vector <Poligono> *resultado, const string &filename){
	ifstream inSHP(filename, ios::binary);
	if(!inSHP.is_open()){ cerr << "[SHP] Imposible abrir archivo." << endl; return -1; }
		
	shpHeader headerSHP;
	assert(sizeof(headerSHP) == 100);
	inSHP.read((char*) &headerSHP, sizeof(headerSHP));
	headerSHP.length = 2*endianSwap(headerSHP.length);
	assert(headerSHP.code == 0x0a270000);

	int cantPoligonos = 0;
	
	while(inSHP.peek() != EOF){
	
		int rNumber, rLength, rType;
		inSHP.read((char*)&rNumber, 4);
		inSHP.read((char*)&rLength, 4);
		inSHP.read((char*)&rType, 4);
		
		rNumber = endianSwap(rNumber);
		rLength = endianSwap(rLength)*2;
		
		if(rType != 5) {cerr << "[SHP] Error, tipo no poligono." << endl; return -1;}
		
		double box[4];
		int numParts, numPoints;
		inSHP.read((char*)&box, 4*8);
		inSHP.read((char*)&numParts, 4);
		inSHP.read((char*)&numPoints, 4);
		
		/*if(numParts != 1){
			cout << "Partes: " << numParts << endl;
		}*/
		
		int parts[numParts];
		inSHP.read((char*)&parts, numParts*4);
		
		Point points[numPoints];
		inSHP.read((char*)&points, numPoints*sizeof(Point));
		
		Poligono poly;
		poly.altura = 0;
		
		for(int i=0;i<numPoints-1;i++)
			poly.puntos.push_back((Point){points[i].x-93844, 19200-(points[i].y-91726)});

		resultado->push_back(poly);
		
		cantPoligonos++;
	}
	
	inSHP.close();
	
	cout << "[SHP] Cargados " << cantPoligonos << " poligonos." << endl;
	
	return 0;
}

int Data::load(){
	if(cargarUsoSuelo() != 0) return -1;
	if(cargarParcelas() != 0) return -1;	
	if(cargarSHP(&manzanas, "res/manzanero_130212.shp") != 0) return -1;
	//if(cargarSHP(&verde, "res/espacios-verdes-privados.shp") != 0) return -1;
	//if(cargarSHP(&verde, "res/espacios-verdes-publicos.shp") != 0) return -1;
	return 0;
}

Data::Data(){
}

Data::~Data(){
	
}
