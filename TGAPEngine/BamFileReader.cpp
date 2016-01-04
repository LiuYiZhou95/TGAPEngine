#include "BamFileReader.h"

namespace engine{
BamFileReader::BamFileReader()
{
    //ctor
}

BamFileReader::~BamFileReader()
{
    //dtor
}
AnimatedActor * BamFileReader::loadFile()
{ 
unsigned int i,j;
VaoInitData * vaoInit = new VaoInitData;
if (DEBUG_MODE == 1){
	std::cout << "new Bam" << std::endl;
}


file.read((char *)vaoInit,sizeof(VaoInitData));
//std::cout<<(unsigned int)vaoInit->attrNumber<<" attrNumber "<<std::endl;
//std::cout<<vaoInit->attrOffset[1] / (sizeof(float) * 3)<<std::endl;
unsigned int * buforSize = new unsigned int;
if (DEBUG_MODE == 1){
	std::cout << "new Bam" << std::endl;
}
//std::cout<<vaoInit->attrOffset[5]<<std::endl;
file.read((char *)buforSize,sizeof(unsigned int));
//std::cout<<"bufor size in bytes "<<*buforSize<<std::endl;
//std::cout<<*buforSize<<std::endl;

char * vertexBuffer;
vertexBuffer = new char[*buforSize];
if (DEBUG_MODE == 1){
	std::cout << "new Bam" << std::endl;
}
file.read(vertexBuffer,*buforSize);

//float * floatBuffer = (float*) vertexBuffer;

/*for(i=0;i<3;i++){
    std::cout<<floatBuffer[i]<<std::endl;
}*/
unsigned int * indicesCount = new unsigned int;
if (DEBUG_MODE == 1){
	std::cout << "new Bam" << std::endl;
}
file.read((char*)indicesCount,sizeof(unsigned int));
//std::cout<<" indice count "<<*indicesCount<<std::endl;
unsigned int * indices;
//std::cout<<*indicesCount<<std::endl;
indices = new unsigned int[*indicesCount];
if (DEBUG_MODE == 1){
	std::cout << "new Bam" << std::endl;
}
file.read((char *)indices,*indicesCount);
//int * indicesInt = (int*) indices;
/*for(i=0;i<6;i++){
    std::cout<<indicesInt[i]<<std::endl;
}*/
std::vector<char> positions(vertexBuffer, vertexBuffer + *buforSize);
delete buforSize; buforSize = NULL;
delete [] vertexBuffer; vertexBuffer = NULL;
std::vector<unsigned int> ind(indices, indices + *indicesCount / sizeof(unsigned int) );
delete indicesCount; indicesCount = NULL;
delete [] indices; indices = NULL;
//std::cout<<positions.size()<<std::endl;
//std::cout<<ind.size()<<std::endl;
VertexAttributes * VAO = new VertexAttributes(positions,ind,*vaoInit);
if (DEBUG_MODE == 1){
	std::cout << "new Bam" << std::endl;
}
delete vaoInit; vaoInit = NULL;

unsigned int * boneCount;
boneCount = new unsigned int;
if (DEBUG_MODE == 1){
	std::cout << "new Bam" << std::endl;
}
file.read((char*)boneCount,sizeof(unsigned int));
//std::cout<<"bone count "<<(unsigned int)*boneCount<<std::endl;
BoneStruct * boneStructArray = new BoneStruct[*boneCount];
float * floatQuaternionTemp = new float[4];
float * floatLocationTemp = new float[3];
float * matTemp = new float[16];
if (DEBUG_MODE == 1){
	std::cout << "new Bam" << std::endl;
}

//FrameStruct * framesTemp;
unsigned int maxFrame = 0;
unsigned int nameLength = 0;
for(i=0;i<*boneCount;i++){
	file.read((char*)&nameLength, sizeof(unsigned int));
	char * boneName = new char[nameLength + 1];
	file.read(boneName, nameLength);
	boneName[nameLength] = '\0';
	//std::cout << boneName << std::endl;
	if (DEBUG_MODE == 1){
		std::cout <<"Reading bone data - "<< boneName << std::endl;
	}
	//std::cout << boneName << std::endl;
	boneStructArray[i].name = boneName;
    file.read((char*)(&boneStructArray[i].childrenCount),sizeof(unsigned int));
    //std::cout<<"children count "<<boneStructArray[i].childrenCount<<std::endl;
    boneStructArray[i].childrenIndices = new unsigned int[boneStructArray[i].childrenCount];
    for(j=0;j<boneStructArray[i].childrenCount;j++){
        file.read((char *)(&boneStructArray[i].childrenIndices[j]),sizeof(unsigned int));
    }
    file.read((char*)matTemp,sizeof(float) * 16);
    glm::mat4 * invMat;
    invMat = new glm::mat4(1.0f);
    *invMat = glm::make_mat4(matTemp);
    boneStructArray[i].invMat = invMat;
    file.read((char *)(&boneStructArray[i].framesCount),sizeof(unsigned int));
    boneStructArray[i].frames = new FrameStruct[boneStructArray[i].framesCount];
    //std::cout<<" framecount is "<<boneStructArray[i].framesCount<<std::endl<<std::endl<<"################"<<std::endl;
    //std::cout<<" i "<<i<<" frameCount "<<boneStructArray[i].framesCount<<std::endl;

    for(j=0;j<boneStructArray[i].framesCount;j++){
        file.read((char*)floatQuaternionTemp,sizeof(float) * 4);//quat(w,x,y,z)
        boneStructArray[i].frames[j].quaternion = glm::quat(floatQuaternionTemp[0],floatQuaternionTemp[1],floatQuaternionTemp[2],floatQuaternionTemp[3]);
        //boneStructArray[i].frames[j].quaternion = qm::Quaternion(floatQuaternionTemp[1],floatQuaternionTemp[2],floatQuaternionTemp[3],floatQuaternionTemp[0]);

        file.read((char*)floatLocationTemp,sizeof(float) * 3);
        boneStructArray[i].frames[j].location = glm::vec3(floatLocationTemp[0],floatLocationTemp[1],floatLocationTemp[2]);
        file.read((char*)(&boneStructArray[i].frames[j].rangeMin),sizeof(unsigned int));
        file.read((char*)(&boneStructArray[i].frames[j].rangeMax),sizeof(unsigned int));
        file.read((char*)(&boneStructArray[i].frames[j].index),sizeof(unsigned int));
        if(boneStructArray[i].frames[j].index>maxFrame) maxFrame = boneStructArray[i].frames[j].index;
        //std::cout<<boneStructArray[i].frames[j].index<<std::endl;
        //std::cout<<boneStructArray[i].frames[j].rangeMin<<std::endl;
        //std::cout<<boneStructArray[i].frames[j].rangeMax<<std::endl;
    }
}
Bone * root = NULL;
if(boneCount>0){
    root = new Bone(boneStructArray,0);
}
//std::cout<<maxFrame<<std::endl;
AnimatedActor * e = new AnimatedActor(VAO,root,maxFrame);

for(i=0;i<*boneCount;i++){
        //std::cout<<" i "<<i<<" bptr "<<boneStructArray[i].frames<<std::endl;
    delete boneStructArray[i].childrenIndices; boneStructArray[i].childrenIndices = NULL;
    if(boneStructArray[i].frames == NULL) std::cout<<"frames is null"<<std::endl;
    delete[] boneStructArray[i].frames; boneStructArray[i].frames = NULL;
}
delete boneCount; boneCount = NULL;
delete[] boneStructArray; boneStructArray = NULL;
delete[] floatQuaternionTemp; floatQuaternionTemp = NULL;
delete[] floatLocationTemp;floatLocationTemp = NULL;
delete[] matTemp; matTemp = NULL;
return e;
/*for(i=0;i<vaoInit->attrNumber;i++){
    std::cout<<(unsigned int)vaoInit->attrSize[i]<<std::endl;
}
for(i=0;i<vaoInit->attrNumber;i++){
    std::cout<<vaoInit->attrType[i]<<std::endl;
}
for(i=0;i<vaoInit->attrNumber;i++){
    std::cout<<(unsigned int)vaoInit->attrNormBool[i]<<std::endl;
}
for(i=0;i<vaoInit->attrNumber;i++){
    std::cout<<vaoInit->attrOffset[i]<<std::endl;
}*/

}

Entity * BamFileReader::loadNoBoneFile()
{
	unsigned int i, j;
	
	VaoInitData * vaoInit = new VaoInitData;

	file.read((char *)vaoInit, sizeof(VaoInitData));
	//std::cout<<(unsigned int)vaoInit->attrNumber<<" attrNumber "<<std::endl;
	//std::cout<<vaoInit->attrOffset[1] / (sizeof(float) * 3)<<std::endl;
	unsigned int * buforSize = new unsigned int;
	//std::cout<<vaoInit->attrOffset[5]<<std::endl;
	file.read((char *)buforSize, sizeof(unsigned int));
	//std::cout<<"bufor size in bytes "<<*buforSize<<std::endl;
	//std::cout<<*buforSize<<std::endl;

	char * vertexBuffer;
	vertexBuffer = new char[*buforSize];
	file.read(vertexBuffer, *buforSize);

	//float * floatBuffer = (float*) vertexBuffer;

	/*for(i=0;i<3;i++){
	std::cout<<floatBuffer[i]<<std::endl;
	}*/
	unsigned int * indicesCount = new unsigned int;
	file.read((char*)indicesCount, sizeof(unsigned int));
	//std::cout<<" indice count "<<*indicesCount<<std::endl;
	unsigned int * indices;
	//std::cout<<*indicesCount<<std::endl;
	indices = new unsigned int[*indicesCount];
	file.read((char *)indices, *indicesCount);
	//int * indicesInt = (int*) indices;
	/*for(i=0;i<6;i++){
	std::cout<<indicesInt[i]<<std::endl;
	}*/
	std::vector<char> positions(vertexBuffer, vertexBuffer + *buforSize);
	delete buforSize; buforSize = NULL;
	delete [] vertexBuffer; vertexBuffer = NULL;
	std::vector<unsigned int> ind(indices, indices + *indicesCount / sizeof(unsigned int));
	delete indicesCount; indicesCount = NULL;
	delete [] indices; indices = NULL;
	//std::cout<<positions.size()<<std::endl;
	//std::cout<<ind.size()<<std::endl;
	VertexAttributes * VAO = new VertexAttributes(positions, ind, *vaoInit);
	delete vaoInit; vaoInit = NULL;

	//unsigned int * boneCount;
	//boneCount = new unsigned int;
	//file.read((char*)boneCount, sizeof(unsigned int));
	////std::cout<<"bone count "<<(unsigned int)*boneCount<<std::endl;
	//BoneStruct * boneStructArray = new BoneStruct[*boneCount];
	//float * floatQuaternionTemp = new float[4];
	//float * floatLocationTemp = new float[3];
	//float * matTemp = new float[16];

	////FrameStruct * framesTemp;
	//unsigned int maxFrame = 0;
	//for (i = 0; i<*boneCount; i++){
	//	file.read((char*)(&boneStructArray[i].childrenCount), sizeof(unsigned int));
	//	//std::cout<<"children count "<<boneStructArray[i].childrenCount<<std::endl;
	//	boneStructArray[i].childrenIndices = new unsigned int[boneStructArray[i].childrenCount];
	//	for (j = 0; j<boneStructArray[i].childrenCount; j++){
	//		file.read((char *)(&boneStructArray[i].childrenIndices[j]), sizeof(unsigned int));
	//	}
	//	file.read((char*)matTemp, sizeof(float) * 16);
	//	glm::mat4 * invMat;
	//	invMat = new glm::mat4(1.0f);
	//	*invMat = glm::make_mat4(matTemp);
	//	boneStructArray[i].invMat = invMat;
	//	file.read((char *)(&boneStructArray[i].framesCount), sizeof(unsigned int));
	//	boneStructArray[i].frames = new FrameStruct[boneStructArray[i].framesCount];
	//	//std::cout<<" framecount is "<<boneStructArray[i].framesCount<<std::endl<<std::endl<<"################"<<std::endl;
	//	//std::cout<<" i "<<i<<" frameCount "<<boneStructArray[i].framesCount<<std::endl;

	//	for (j = 0; j<boneStructArray[i].framesCount; j++){
	//		file.read((char*)floatQuaternionTemp, sizeof(float) * 4);//quat(w,x,y,z)
	//		boneStructArray[i].frames[j].quaternion = glm::quat(floatQuaternionTemp[0], floatQuaternionTemp[1], floatQuaternionTemp[2], floatQuaternionTemp[3]);
	//		//boneStructArray[i].frames[j].quaternion = qm::Quaternion(floatQuaternionTemp[1],floatQuaternionTemp[2],floatQuaternionTemp[3],floatQuaternionTemp[0]);

	//		file.read((char*)floatLocationTemp, sizeof(float) * 3);
	//		boneStructArray[i].frames[j].location = glm::vec3(floatLocationTemp[0], floatLocationTemp[1], floatLocationTemp[2]);
	//		file.read((char*)(&boneStructArray[i].frames[j].rangeMin), sizeof(unsigned int));
	//		file.read((char*)(&boneStructArray[i].frames[j].rangeMax), sizeof(unsigned int));
	//		file.read((char*)(&boneStructArray[i].frames[j].index), sizeof(unsigned int));
	//		if (boneStructArray[i].frames[j].index>maxFrame) maxFrame = boneStructArray[i].frames[j].index;
	//		//std::cout<<boneStructArray[i].frames[j].index<<std::endl;
	//		//std::cout<<boneStructArray[i].frames[j].rangeMin<<std::endl;
	//		//std::cout<<boneStructArray[i].frames[j].rangeMax<<std::endl;
	//	}
	//}
	//Bone * root = NULL;
	//if (boneCount>0){
	//	root = new Bone(boneStructArray, 0);
	//}
	//std::cout<<maxFrame<<std::endl;
	//AnimatedActor * e = new AnimatedActor(VAO, root, maxFrame);
	Entity * e = new Entity(VAO);

	//for (i = 0; i<*boneCount; i++){
	//	//std::cout<<" i "<<i<<" bptr "<<boneStructArray[i].frames<<std::endl;
	//	delete boneStructArray[i].childrenIndices; boneStructArray[i].childrenIndices = NULL;
	//	if (boneStructArray[i].frames == NULL) std::cout << "wtf its null" << std::endl;
	//	delete[] boneStructArray[i].frames; boneStructArray[i].frames = NULL;
	//}
	/*delete boneCount; boneCount = NULL;
	delete[] boneStructArray; boneStructArray = NULL;
	delete[] floatQuaternionTemp; floatQuaternionTemp = NULL;
	delete[] floatLocationTemp; floatLocationTemp = NULL;
	delete[] matTemp; matTemp = NULL;*/
	return e;
	/*for(i=0;i<vaoInit->attrNumber;i++){
	std::cout<<(unsigned int)vaoInit->attrSize[i]<<std::endl;
	}
	for(i=0;i<vaoInit->attrNumber;i++){
	std::cout<<vaoInit->attrType[i]<<std::endl;
	}
	for(i=0;i<vaoInit->attrNumber;i++){
	std::cout<<(unsigned int)vaoInit->attrNormBool[i]<<std::endl;
	}
	for(i=0;i<vaoInit->attrNumber;i++){
	std::cout<<vaoInit->attrOffset[i]<<std::endl;
	}*/

}


Scene * BamFileReader::loadScene(const char filepath[], Renderer * renderer){
	typedef struct ObjectTreeElement{
		unsigned char id;
		unsigned char * childrenIds;
		unsigned char childrenCount;
	};
	file.open(filepath, std::ios::in | std::ios::binary);
	if (file.is_open()) std::cout << "otwarto plik " << filepath << std::endl;

	unsigned char * objectCount = new unsigned char;
	file.read((char *)objectCount, sizeof(unsigned char));
	std::cout << "objectCount " << (unsigned int)*objectCount << std::endl;
	struct ObjectTreeElement * objectTree = new struct ObjectTreeElement[*objectCount];
	for (int i = 0; i < *objectCount; i++){
		unsigned char objectId = 0;
		file.read((char *)&objectId, sizeof(unsigned char));
		std::cout << "objectId " << (unsigned int)objectId << std::endl;
		unsigned char childrenCount = 0;
		file.read((char * )&childrenCount, sizeof(unsigned char));
		std::cout << "childrenCount " << (unsigned int)childrenCount << std::endl;
		objectTree[objectId].childrenCount = childrenCount;
		objectTree[objectId].id = objectId;
		if (childrenCount > 0)
			objectTree[objectId].childrenIds = new unsigned char[childrenCount];
		for (int j = 0; j < childrenCount; j++){
			unsigned char childId = 0;
			file.read((char*)&childId, sizeof(unsigned char));
			std::cout << "childId " << (unsigned int)childId << std::endl;
			objectTree[objectId].childrenIds[j] = childId;
		}
	}

	Entity ** entityArray = new Entity * [*objectCount];

	for (int i = 0; i < *objectCount; i++){
		struct Header header = this->loadHeader();
		if (header.configurationFlags & 0x04){//object has no bones
			entityArray[i] = this->loadNoBoneFile();
		}
		else{
			entityArray[i] = this->loadFile();
		}
		entityArray[i]->name = header.entityName;
		entityArray[i]->buildId = header.id;
		entityArray[i]->applyTranslation(header.location);
		std::cout << header.location.x << " " << header.location.y << " " << header.location.z << std::endl;
		entityArray[i]->applyRotation(header.rotation);
		std::cout << header.rotation.x << " " << header.rotation.y << " " << header.rotation.z << std::endl;
		entityArray[i]->applyScale(header.scale);
		SingleMatrixMaterial * entitymaterial = new SingleMatrixMaterial(header.vertexShaderStr.c_str(), header.fragmentShaderStr.c_str());
		entityArray[i]->entityMaterial = entitymaterial;

	}
	
	for (int i = 0; i < *objectCount; i++){//iteracja po objectTree
		unsigned int entityArrayId = 0;//pozycja w tablicy gotowego obiejtu o danym id
		for (int g = 0; g < *objectCount; g++){
			if (entityArray[g]->buildId == i){
				entityArrayId = g;
			}
		}
		for (int j = 0; j < objectTree[i].childrenCount; j++){//itaracja po potomkach w objectTree
			unsigned int childId = objectTree[i].childrenIds[j];
			for (int k = 0; k < *objectCount; k++){//iteracja po elementach entityArray w celu znalezienia potomka
				if (entityArray[k]->buildId == childId){//potomek znaleziony ma pozycje k w tabeli
					entityArray[entityArrayId]->addChild(entityArray[k]);
					entityArray[k]->setParent(entityArray[entityArrayId]);
					std::cout << entityArray[entityArrayId]->buildId << " has child " << entityArray[k]->buildId << std::endl;
				}
			}
		}
	}


	for (int i = 0; i < *objectCount; i++){
		std::cout << i << " Has " << (unsigned int)objectTree[i].childrenCount << " children: " << std::endl;
		for (int j = 0; j < objectTree[i].childrenCount; j++){
			std::cout << (unsigned int)objectTree[i].childrenIds[j] << std::endl;
		}
	}

	Scene * newScene = new Scene();
	for (int i = 0; i < *objectCount; i++){
		if (entityArray[i]->getParent() == NULL){
			newScene->addEntity(entityArray[i]);
			std::cout << "entity #" << entityArray[i]->buildId << " added to scene" << std::endl;
		}
	}

	//cleanup---------------------------------
	for (int i = 0; i < *objectCount; i++){
		if (objectTree[i].childrenCount > 0)
			delete[] objectTree[i].childrenIds;
	}
	delete[] objectTree;
	delete objectCount;
	
	
	file.close();
	return newScene;
}

struct Header BamFileReader::loadHeader(){
	struct Header header;

	unsigned char * entityId = new unsigned char;
	file.read((char *)entityId, sizeof(unsigned char));

	unsigned char * configurationFlags = new unsigned char;
	file.read((char *)configurationFlags, sizeof(unsigned char));


	unsigned int entityNameLength = 0;
	file.read((char*)&entityNameLength, sizeof(unsigned int));
	char * entityName = new char[entityNameLength + 1];
	file.read(entityName, entityNameLength);
	entityName[entityNameLength] = '\0';


	float * floatQuaternionEnt = new float[4];
	float * floatLocationEnt = new float[3];
	float * floatScaleEnt = new float[3];

	file.read((char*)floatLocationEnt, sizeof(float) * 3);
	glm::vec3 locationEnt = glm::vec3(floatLocationEnt[0], floatLocationEnt[1], floatLocationEnt[2]);

	file.read((char*)floatQuaternionEnt, sizeof(float) * 4);//quat(w,x,y,z)
	glm::quat rotationEnt = glm::quat(floatQuaternionEnt[0], floatQuaternionEnt[1], floatQuaternionEnt[2], floatQuaternionEnt[3]);

	file.read((char*)floatScaleEnt, sizeof(float) * 3);
	glm::vec3 scaleEnt = glm::vec3(floatScaleEnt[0], floatScaleEnt[1], floatScaleEnt[2]);

	unsigned int entityVertexLength = 0;
	file.read((char*)&entityVertexLength, sizeof(unsigned int));
	char * entityVertexString = new char[entityVertexLength + 1];
	if (entityVertexLength > 0)
		file.read(entityVertexString, entityVertexLength);
	entityVertexString[entityVertexLength] = '\0';

	unsigned int entityFragmentLength = 0;
	file.read((char*)&entityFragmentLength, sizeof(unsigned int));
	char * entityFragmentString = new char[entityFragmentLength + 1];
	if (entityFragmentLength > 0)
		file.read(entityFragmentString, entityFragmentLength);
	entityFragmentString[entityFragmentLength] = '\0';


	header.id = *entityId;
	header.configurationFlags = *configurationFlags;
	header.entityName = std::string(entityName);
	if (entityVertexLength <= 0){
		header.vertexShaderStr = std::string("vertex.vert");
	}
	else{
		header.vertexShaderStr = std::string(entityVertexString);
	}
	if (entityFragmentLength <= 0){
		header.fragmentShaderStr = std::string("fragment.frag");
	}
	else{
		header.fragmentShaderStr = std::string(entityFragmentString);
	}
	header.location = locationEnt;
	header.rotation = rotationEnt;
	header.scale = scaleEnt;

	delete[] floatScaleEnt;
	delete[] floatLocationEnt;
	delete[] floatQuaternionEnt;
	delete[] entityName;
	delete[] entityFragmentString;
	delete[] entityVertexString;
	delete configurationFlags;
	delete entityId;

	return header;
}
}
