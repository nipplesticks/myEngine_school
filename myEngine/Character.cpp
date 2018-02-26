#include "Character.h"

Character::Character(std::string modelName, Model* model) : Entity()
{ 
	
}

Character::Character()
{

}

Character::~Character()
{

}

using namespace DirectX;
void Character::collisionHandling(const Terrain& t, int mapWidth)
{
	DirectX::XMFLOAT3 worldPlayerPos = getPosition();

	float playerPosX = worldPlayerPos.x;
	float playerPosY = worldPlayerPos.y;
	float playerPosZ = worldPlayerPos.z;

	XMMATRIX terrainWorldMatrix = t.getWorldMatrix();
	XMMATRIX worldInverse = XMMatrixInverse(nullptr, terrainWorldMatrix);
	XMVECTOR playerPosInTerrainModelSpace = XMVector3TransformNormal(XMVectorSet(playerPosX, playerPosY, playerPosZ, 1), worldInverse);

	int vertexArraySize = t.getNrOfVertices();
	int terrainModelWidth = t.getWidith();
	int terrainModelHeight = t.getHeight();

	float pMPosX = XMVectorGetX(playerPosInTerrainModelSpace);
	float pMPosY = XMVectorGetY(playerPosInTerrainModelSpace);
	float pMPosZ = XMVectorGetZ(playerPosInTerrainModelSpace);

	float x = pMPosX;
	float z = pMPosZ;

	if (x >= 0.0f && z >= 0.0f && x < (float)terrainModelWidth && z < (float)terrainModelHeight)
	{
		float heightValue = (t.getHightValues()[(int)x][(int)z] - 128) / t.getDeltaHight();

		XMVECTOR worldPosTerrain = XMVector3TransformNormal(XMVectorSet(0, heightValue, 0, 1), terrainWorldMatrix);
		setPosition(playerPosX, XMVectorGetY(worldPosTerrain), playerPosZ);
	}
}
