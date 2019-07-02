#include "stdafx.h"
#include "Material.h"
#include "Texture.h"
#include "FileUtils.h"
#include "Reader.h"
#include "Writer.h"
#include "GfxRenderer.h"

using namespace std;

Material::Material()
{
	diffuseMap_ = normalMap_ = specularMap_ = 0;
	diffuseColor_ = Color(0.75f, 0.75f, 0.75f, 1);
	specularPower_ = 1.0f;
}

Material::~Material()
{
	uninit();
}


void Material::referencedFiles(std::vector<std::string> * refFiles) const
{
	if (diffuseMap_) refFiles->push_back(diffuseMap_->fileName());
	if (specularMap_) refFiles->push_back(specularMap_->fileName());
	if (normalMap_) refFiles->push_back(normalMap_->fileName());
}

int Material::setReferencedFiles(
	const std::vector<std::string> & refFiles, int index)
{
	if (diffuseMap_) diffuseMap_->setFileName(refFiles[index++]);
	if (specularMap_) specularMap_->setFileName(refFiles[index++]);
	if (normalMap_) normalMap_->setFileName(refFiles[index++]);

	return index;
}

void Material::init(GfxRenderer * gl)
{
}

void Material::uninit()
{	
}

void Material::SetDiffuseMap(Texture * texture)
{
	diffuseMap_ = texture;
}

void Material::setSpecularMap(Texture * texture)
{
	specularMap_ = texture;
}

void Material::setNormalMap(Texture * texture)
{
	normalMap_ = texture;
}

void Material::setSpecularPower(float power)
{
	specularPower_ = power;
}

void Material::Use(GfxRenderer * gl) const
{
	gl->useMeshProgram(1.0f, this);

	glActiveTexture(GL_TEXTURE3);
	if (specularMap_)
	{		
		glBindTexture(GL_TEXTURE_2D, specularMap_->texture_);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, gl->defaultPhongSpecularMap());
	}

	glActiveTexture(GL_TEXTURE2);
	if (normalMap_)
	{		
		glBindTexture(GL_TEXTURE_2D, normalMap_->texture_);
	}

	if (diffuseMap_ && diffuseMap_->needSeparateAlpha())
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, diffuseMap_->textureAlpha_);
	}

	glActiveTexture(GL_TEXTURE0);
	if (diffuseMap_)
	{				
		glBindTexture(GL_TEXTURE_2D, diffuseMap_->texture_);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, gl->defaultDiffuseMap());
	}	
		
}

bool Material::isEquivalent(const Material & material) const
{
	string fileName, testFileName;

	if (diffuseMap_)
	{
		if (material.diffuseMap_)
		{
			fileName = diffuseMap_->fileName();
			testFileName = material.diffuseMap_->fileName();
			if (!boost::iequals(fileName.c_str(), testFileName.c_str())) return false;
		}
		else return false;
	}
	else
	{
		if (diffuseColor_ != material.diffuseColor_) return false;
	}

	if (specularMap_)
	{
		if (material.specularMap_)
		{
			fileName = specularMap_->fileName();
			testFileName = material.specularMap_->fileName();
			if (!boost::iequals(fileName.c_str(), testFileName.c_str())) return false;
		}
		else return false;
	}

	if (normalMap_)
	{
		if (material.normalMap_)
		{
			fileName = normalMap_->fileName();
			testFileName = material.normalMap_->fileName();
			if (!boost::iequals(fileName.c_str(), testFileName.c_str())) return false;
		}
		else return false;
	}

	if (specularPower_ != material.specularPower_) return false;


	return true;
}

void Material::write(Writer & writer) const
{
	writer.write(diffuseColor_);
	writer.write(diffuseMap_);	
	writer.write(normalMap_);	
	writer.write(specularMap_);	
	writer.write(specularPower_);
	
}
void Material::read(Reader & reader, unsigned char)
{
	reader.read(diffuseColor_);
	reader.read(diffuseMap_);	
	reader.read(normalMap_);	
	reader.read(specularMap_);	
	reader.read(specularPower_);

	if (specularPower_ < 1.0f) specularPower_ = 1.0f;
}