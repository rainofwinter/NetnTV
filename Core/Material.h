#pragma once
#include "Color.h"

class Texture;
class GfxRenderer;
class Reader;
class Writer;

class Material
{
public:
	Material();
	~Material();

	void referencedFiles(std::vector<std::string> * refFiles) const;
	int setReferencedFiles(
		const std::vector<std::string> & refFiles, int index);

	void init(GfxRenderer * gl);
	void uninit();
	
	void setDiffuseColor(const Color & color) {diffuseColor_ = color;}
	void SetDiffuseMap(Texture * texture);
	Texture * diffuseMap() const {return diffuseMap_;}

	void setSpecularMap(Texture * texture);
	Texture * specularMap() const {return specularMap_;}

	void setNormalMap(Texture * texture);
	Texture * normalMap() const {return normalMap_;}

	void setSpecularPower(float power);

	void Use(GfxRenderer * gl) const;

	bool isEquivalent(const Material & material) const;

	const float & specularPower() const {return specularPower_;}
	unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	
private:
	Color diffuseColor_;
	Texture * diffuseMap_;
	Texture * specularMap_;
	Texture * normalMap_;
	float specularPower_;
};