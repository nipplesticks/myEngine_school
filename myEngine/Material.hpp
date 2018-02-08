#ifndef MATERIAL_HPP
#define MATERIAL_HPP
#include "Structs.hpp"



class Material
{
public:
	Material();
	//Material(const Material& other);
	//Material& operator=(const Material& other);

	~Material();

	void setMaterialName(const std::string &name);
	const std::string& getMaterialName() const;


	void setAmbientLevel(const DirectX::XMFLOAT3 &amb);
	void setDiffuseLevel(const DirectX::XMFLOAT3 &dif);
	void setSpecularLevel(const DirectX::XMFLOAT3 &spec);
	void setEmissiveColor(const DirectX::XMFLOAT3 &em);
	void setSpecularRange(const float& range);
	void setDissolveLevel(const float &d);
	void setOpticalDensity(float opticalDensity);
	void setSharpness(float sharp);
	void setIllum(int illum);

	const DirectX::XMVECTOR& getAmbientLevel() const;
	const DirectX::XMVECTOR& getDiffuseLevel() const;
	const DirectX::XMVECTOR& getSpecularLevel() const;
	const DirectX::XMVECTOR& getEmissiveColor() const;
	const float& getDissolveLevel() const;
	const float& getOpticalDensity() const;
	const float& getSharpness() const;
	int getIllum() const;

	void setAmbientTexture(ID3D11ShaderResourceView* amb, ID3D11Resource* res);
	void setDiffuseTexture(ID3D11ShaderResourceView* dif, ID3D11Resource* res);
	void setSpecularColorTexture(ID3D11ShaderResourceView* specColor, ID3D11Resource* res);
	void setEmessiveTextureMap(ID3D11ShaderResourceView* em, ID3D11Resource* res);
	void setSpecularHighlightComponent(ID3D11ShaderResourceView* specHighlight, ID3D11Resource* res);
	void setAlphaMap(ID3D11ShaderResourceView* alphaMap, ID3D11Resource* res);
	void setBumpMap(ID3D11ShaderResourceView* bump, ID3D11Resource* res);
	void setDisplacementMap(ID3D11ShaderResourceView* disp, ID3D11Resource* res);
	void setDecalTecture(ID3D11ShaderResourceView* decal, ID3D11Resource* res);
	void setReflectionMap(ID3D11ShaderResourceView* reflect, ID3D11Resource* res);

	ID3D11ShaderResourceView* getAmbientTexture() const;
	ID3D11ShaderResourceView* getDiffuseTexture() const;
	ID3D11ShaderResourceView* getSpecularColorTexture() const;
	ID3D11ShaderResourceView* getEmessiveTextureMap() const;
	ID3D11ShaderResourceView* getSpecularHighlightComponent() const;
	ID3D11ShaderResourceView* getAlphaMap() const;
	ID3D11ShaderResourceView* getBumpMap() const;
	ID3D11ShaderResourceView* getDisplacementMap() const;
	ID3D11ShaderResourceView* getDecalTecture() const;
	ID3D11ShaderResourceView* getReflectionMap() const;

private:
	void _safeRelease(ID3D11ShaderResourceView* pResourceView);
	void _safeRelease(ID3D11Resource* pResource);
	void _copy(const Material& other);
	void _cleanup();

private:
	std::string m_materialName;
	DirectX::XMVECTOR m_ambient;		//Ka - float3
	DirectX::XMVECTOR m_diffuse;		//Kd - float 3
	DirectX::XMVECTOR m_specular;		//Ks + Ns - float4
	DirectX::XMVECTOR m_emissive;		//Ke
	float m_dissolve;					//d or Tr    if d : 0 - 1, 1 is transparent. if Tr : 0 - 1, 0 is transparent
	float m_opticalDensity;				//Ni
	float m_sharpness;					//sharpness
	/*
		Texture statements may have option parameters
	*/

	ID3D11ShaderResourceView*	m_ambientTexture;				//map_Ka
	ID3D11Resource*				m_ambientRes;
	ID3D11ShaderResourceView*	m_diffuseTexture;				//map_Kd
	ID3D11Resource*				m_diffuseRes;
	ID3D11ShaderResourceView*	m_specularColorTexture;			//map_Ks
	ID3D11Resource*				m_specularColorRes;
	ID3D11ShaderResourceView*	m_emessiveTextureMap;			//map_Ke
	ID3D11Resource*				m_emessiveRes;
	ID3D11ShaderResourceView*	m_specularHighlightComponent;	//map_Ns
	ID3D11Resource*				m_specularHighlightRes;
	ID3D11ShaderResourceView*	m_alphaMap;						//map_d
	ID3D11Resource*				m_alphaRes;
	ID3D11ShaderResourceView*	m_bumpMap;						//map_bump or just bump. by default, uses luminance channel
	ID3D11Resource*				m_bumpRes;
	ID3D11ShaderResourceView*	m_displacementMap;				//disp
	ID3D11Resource*				m_displacementRes;
	ID3D11ShaderResourceView*	m_decalTexture;					//decal. defautls to 'matte' channel
	ID3D11Resource*				m_decalRes;
	ID3D11ShaderResourceView*	m_reflectionMap;				//refl. 
	ID3D11Resource*				m_reflectionRes;
	int m_illum;												//illum


};


#endif