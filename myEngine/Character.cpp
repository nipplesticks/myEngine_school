#include "Character.h"

Character::Character(std::string modelName, Model* model) : Entity()
{ 
	/*
	model->settings(true, true);
	model->initModel("models/" + modelName + ".obj");
	*/
}

Character::Character() : Entity()
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

	VERTEX* vertices = t.getModel()->getTerrainMesh();

	//Calculate the value of which you would find the intersected triangles vertices
	//in the 1D array

	XMMATRIX terrainWorldMatrix = t.getWorldMatrix();
	XMMATRIX worldInverse = XMMatrixInverse(nullptr, terrainWorldMatrix);
	XMVECTOR playerPosInTerrainModelSpace = XMVector3TransformNormal(XMVectorSet(playerPosX, playerPosY, playerPosZ, 1), worldInverse);

	int vertexArraySize = t.getNrOfVertices();
	int terrainModelWidth = t.getWidith();
	int terrainModelHeight = t.getHeight();

	float pMPosX = XMVectorGetX(playerPosInTerrainModelSpace);
	float pMPosY = XMVectorGetY(playerPosInTerrainModelSpace);
	float pMPosZ = XMVectorGetZ(playerPosInTerrainModelSpace);

	//int actualVerticePos = static_cast<int>((pMPosX - 1)*(6 * (terrainModelWidth - 1) + ((pMPosZ - 1) * 6)));

	float x = pMPosX;
	float z = pMPosZ;

	//If vertexIndex is 2, we have found the 3rd vertex of the current triangle, 
	//if it is 1, we have found the second and if it is 0, we have found the first.

	if (x >= 0.0f && z >= 0.0f && x < (float)terrainModelWidth && z < (float)terrainModelHeight)
	{
		float offsetX = x - (int)x;
		float offsetZ = z - (int)z;

		float heightValue = (t.getHightValues()[(int)x][(int)z] - 128) / t.getDeltaHight();

		XMVECTOR worldPosTerrain = XMVector3TransformNormal(XMVectorSet(0, heightValue, 0, 1), terrainWorldMatrix);
		printf("\r pY: %f", XMVectorGetY(worldPosTerrain));

		setPosition(playerPosX, XMVectorGetY(worldPosTerrain), playerPosZ);
	}

	//////////TEMPORARY UNTILL ANGLE CALCULATION HAS BEEN FIXED///////////////////////////
	//float terrainModelX = ((p0.x + p1.x + p2.x) / 3); 
	//float terrainModelY = ((p0.y + p1.y + p2.y) / 3);
	//float terrainModelZ = ((p0.z + p1.z + p2.z) / 3);


	//float p0Y = p0.y; 

	//printf("\rRAW PX: %f, PY: %F, PZ: %F", p0.x, p0.y, p0.z);


	//////////////////////////////////////////////////////////////////////////////////////

	/*//Save the verices as 2D points, do not forget that z represents y here.
	DirectX::FXMVECTOR p0p12D = { p1.x - p0.x, p1.z - p0.z};
	DirectX::FXMVECTOR p0p22D = { p2.x - p0.x, p2.z - p0.z};
	DirectX::FXMVECTOR p1p22D = { p2.x - p1.x, p2.z - p2.z};

	//Normalize the vectors so that caluclations with baycentric coordinates
	//are made possible.
	DirectX::FXMVECTOR Np0p12D = DirectX::XMVector2Normalize(p0p12D);
	DirectX::FXMVECTOR Np0p22D = DirectX::XMVector2Normalize(p0p22D);
	DirectX::FXMVECTOR Np1p22D = DirectX::XMVector2Normalize(p1p22D);

	//Find smallest and biggest y-values among the vertices and save them.
	VERTEX currentVertices[3];
	currentVertices[0] = p0;
	currentVertices[1] = p1;
	currentVertices[2] = p2;

	//Finding the vertex with the smallest Y-Value.
	int currentSmallestIndex = 0;
	for (int i = 1; i < 3; i++)
	{
	if (currentVertices[i].y < currentVertices[currentSmallestIndex].y)
	{
	currentSmallestIndex = i;
	}
	}
	VERTEX smallestYVtx = currentVertices[currentSmallestIndex];

	//Finding the vertex with the biggest Y-Value.
	int currentBiggestIndex = 0;
	for (int i = 1; i < 3; i++)
	{
	if (currentVertices[i].y > currentVertices[currentBiggestIndex].y)
	{
	currentBiggestIndex = i;
	}
	}
	VERTEX BiggestYVtx = currentVertices[currentBiggestIndex];*/
}
