#include "Material.hpp"

using namespace DirectX;

Material::Material()
{
	m_materialName = "";
	
	m_ambient = XMVectorZero();
	m_diffuse = XMVectorZero();
	m_specular = XMVectorZero();
	m_dissolve = 1.0f;
	m_ambientTexture = nullptr;					//map_Ka
	m_diffuseTexture = nullptr;					//map_Kd
	m_specularColorTexture = nullptr;			//map_Ks
	m_emessiveTextureMap = nullptr;				//map_Ke
	m_specularHighlightComponent = nullptr;		//map_Ns
	m_alphaMap = nullptr;						//map_d
	m_bumpMap = nullptr;						//map_bump or just bump. by default, uses luminance channel
	m_displacementMap = nullptr;				//disp
	m_decalTexture = nullptr;					//decal. defautls to 'matte' channel
	m_reflectionMap = nullptr;					//refl. 
	m_illum = 0;
	m_opticalDensity = 1.0f;
	m_sharpness = 1.0f;
	m_ambientRes = nullptr;
	m_diffuseRes = nullptr;
	m_specularColorRes = nullptr;
	m_emessiveRes = nullptr;
	m_specularHighlightRes = nullptr;
	m_alphaRes = nullptr;
	m_bumpRes = nullptr;
	m_displacementRes = nullptr;
	m_decalRes = nullptr;
	m_reflectionRes = nullptr;
}

//Material::Material(const Material & other)
//{
//	_copy(other);
//}

//Material & Material::operator=(const Material & other)
//{
//	if (this != &other)
//	{
//		_cleanup();
//		_copy(other);
//	}
//
//	return *this;
//}

Material::~Material()
{
	_cleanup();
}

void Material::setMaterialName(const std::string & name)
{
	m_materialName = name;
}

const std::string & Material::getMaterialName() const
{
	return m_materialName;
}

void Material::setAmbientLevel(const DirectX::XMFLOAT3 & amb)
{
	m_ambient = XMLoadFloat3(&amb);
}

void Material::setDiffuseLevel(const DirectX::XMFLOAT3 & dif)
{
	m_diffuse = XMLoadFloat3(&dif);
}

void Material::setSpecularLevel(const DirectX::XMFLOAT3 & spec)
{
	XMFLOAT4 spec4;
	spec4.x = spec.x;
	spec4.y = spec.y;
	spec4.z = spec.z;

	m_specular = XMLoadFloat4(&spec4);
}

void Material::setEmissiveColor(const DirectX::XMFLOAT3 & em)
{
	m_emissive = XMLoadFloat3(&em);
}

void Material::setSpecularRange(const float & range)
{
	XMFLOAT4 spec;

	XMStoreFloat4(&spec, m_specular);
	spec.w = range;
	m_specular = XMLoadFloat4(&spec);
}

void Material::setDissolveLevel(const float & d)
{
	//dissolve = 1.0 - (N*v)(1.0-factor)

	m_dissolve = d;
}

void Material::setOpticalDensity(float opticalDensity)
{
	m_opticalDensity = opticalDensity;
}

void Material::setSharpness(float sharp)
{
	m_sharpness = sharp;
}

const DirectX::XMVECTOR & Material::getAmbientLevel() const
{
	return m_ambient;
}

const DirectX::XMVECTOR & Material::getDiffuseLevel() const
{
	return m_diffuse;
}

const DirectX::XMVECTOR & Material::getSpecularLevel() const
{
	return m_specular;
}

const DirectX::XMVECTOR & Material::getEmissiveColor() const
{
	return m_emissive;
}

const float & Material::getDissolveLevel() const
{
	return m_dissolve;
}

const float & Material::getOpticalDensity() const
{
	return m_opticalDensity;
}

const float & Material::getSharpness() const
{
	return m_sharpness;
}

void Material::setAmbientTexture(ID3D11ShaderResourceView * amb, ID3D11Resource* res)
{
	m_ambientTexture = amb;
	m_ambientRes = res;
}

void Material::setDiffuseTexture(ID3D11ShaderResourceView * dif, ID3D11Resource* res)
{
	m_diffuseTexture = dif;
	m_diffuseRes = res;
}

void Material::setSpecularColorTexture(ID3D11ShaderResourceView * specColor, ID3D11Resource* res)
{
	m_specularColorTexture = specColor;
	m_specularColorRes = res;
}

void Material::setEmessiveTextureMap(ID3D11ShaderResourceView * em, ID3D11Resource* res)
{
	m_emessiveTextureMap = em;
	m_emessiveRes = res;
}

void Material::setSpecularHighlightComponent(ID3D11ShaderResourceView * specHighlight, ID3D11Resource* res)
{
	m_specularHighlightComponent = specHighlight;
	m_specularHighlightRes = res;
}

void Material::setAlphaMap(ID3D11ShaderResourceView * alphaMap, ID3D11Resource* res)
{
	m_alphaMap = alphaMap;
	m_alphaRes = res;
}

void Material::setBumpMap(ID3D11ShaderResourceView * bump, ID3D11Resource* res)
{
	m_bumpMap = bump;
	m_bumpRes = res;
}

void Material::setDisplacementMap(ID3D11ShaderResourceView * disp, ID3D11Resource* res)
{
	m_displacementMap = disp;
	m_displacementRes = res;
}

void Material::setDecalTecture(ID3D11ShaderResourceView * decal, ID3D11Resource* res)
{
	m_decalTexture = decal;
	m_decalRes = res;
}

void Material::setReflectionMap(ID3D11ShaderResourceView * reflect, ID3D11Resource* res)
{
	m_reflectionMap = reflect;
	m_reflectionRes = res;
}

void Material::setIllum(int illum)
{
	m_illum = illum;
}

ID3D11ShaderResourceView * Material::getAmbientTexture() const
{
	return m_ambientTexture;
}

ID3D11ShaderResourceView * Material::getDiffuseTexture() const
{
	return m_diffuseTexture;
}

ID3D11ShaderResourceView * Material::getSpecularColorTexture() const
{
	return m_specularColorTexture;
}

ID3D11ShaderResourceView * Material::getEmessiveTextureMap() const
{
	return m_emessiveTextureMap;
}

ID3D11ShaderResourceView * Material::getSpecularHighlightComponent() const
{
	return m_specularHighlightComponent;
}

ID3D11ShaderResourceView * Material::getAlphaMap() const
{
	return m_alphaMap;
}

ID3D11ShaderResourceView * Material::getBumpMap() const
{
	return m_bumpMap;
}

ID3D11ShaderResourceView * Material::getDisplacementMap() const
{
	return m_displacementMap;
}

ID3D11ShaderResourceView * Material::getDecalTecture() const
{
	return m_decalTexture;
}

ID3D11ShaderResourceView * Material::getReflectionMap() const
{
	return m_reflectionMap;
}

int Material::getIllum() const
{
	return m_illum;
}

void Material::_safeRelease(ID3D11ShaderResourceView * pResourceView)
{
	//if (pResourceView != nullptr)
	//	pResourceView->Release();
}

void Material::_safeRelease(ID3D11Resource * pResource)
{
	//if (pResource != nullptr)
	//	pResource->Release();
}
//
//void Material::_copy(const Material & other)
//{
//	m_materialName = other.m_materialName;
//
//	m_ambient = other.m_ambient;
//	m_diffuse = other.m_diffuse;
//	m_specular = other.m_specular;
//	m_dissolve = other.m_dissolve;
//	m_ambientTexture = new ID3D11ShaderResourceView(other.m_ambientTexture);
//	m_diffuseTexture = nullptr;
//	m_specularColorTexture = nullptr;
//	m_emessiveTextureMap = nullptr;
//	m_specularHighlightComponent = nullptr;
//	m_alphaMap = nullptr;
//	m_bumpMap = nullptr;
//	m_displacementMap = nullptr;
//	m_decalTexture = nullptr;
//	m_reflectionMap = nullptr;
//	m_illum = 0;
//	m_opticalDensity = 1.0f;
//	m_sharpness = 1.0f;
//	m_ambientRes = nullptr;
//	m_diffuseRes = nullptr;
//	m_specularColorRes = nullptr;
//	m_emessiveRes = nullptr;
//	m_specularHighlightRes = nullptr;
//	m_alphaRes = nullptr;
//	m_bumpRes = nullptr;
//	m_displacementRes = nullptr;
//	m_decalRes = nullptr;
//	m_reflectionRes = nullptr;
//}

void Material::_cleanup()
{
	_safeRelease(m_ambientTexture);
	_safeRelease(m_diffuseTexture);
	_safeRelease(m_specularColorTexture);
	_safeRelease(m_emessiveTextureMap);
	_safeRelease(m_specularHighlightComponent);
	_safeRelease(m_alphaMap);
	_safeRelease(m_bumpMap);
	_safeRelease(m_displacementMap);
	_safeRelease(m_decalTexture);
	_safeRelease(m_reflectionMap);

	_safeRelease(m_ambientRes);
	_safeRelease(m_diffuseRes);
	_safeRelease(m_specularColorRes);
	_safeRelease(m_emessiveRes);
	_safeRelease(m_specularHighlightRes);
	_safeRelease(m_alphaRes);
	_safeRelease(m_bumpRes);
	_safeRelease(m_displacementRes);
	_safeRelease(m_decalRes);
	_safeRelease(m_reflectionRes);
}
