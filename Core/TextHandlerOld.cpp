#include "stdafx.h"
#include "TextHandlerOld.h"
#include "MathStuff.h"
#include "GfxRenderer.h"
#include "Color.h"
#include "Global.h"
#include "FileUtils.h"

#include FT_GLYPH_H
#include FT_BITMAP_H
#include FT_OUTLINE_H
#include FT_SYNTHESIS_H

#include "Text.h" //TODO maybe remove. Included for LineProperties and TextProperties

using namespace std;

StringData_oldElement_old::StringData_oldElement_old()
{
	texture = 0;
	vertexBuffer = 0;
	indexBuffer = 0;
	numIndices = 0;
}

StringData_old::~StringData_old()
{
	
	BOOST_FOREACH(StringData_oldElement_old & data, renderData)
	{
		glDeleteBuffers(1, &data.vertexBuffer);
		glDeleteBuffers(1, &data.indexBuffer);
	}
	
}

///////////////////////////////////////////////////////////////////////////////

void TextHandlerOld::create()
{
	textureDim_ = 512;
	charDim_ = 18;
	texDim_ = 36;
	padding_ = 4;
	texPadding_ = 8;

	imgBuffer_ = (unsigned char *)malloc(textureDim_ * textureDim_);
	memset(imgBuffer_, 0, textureDim_ * textureDim_);

	curCharIndex_ = 0;

	init_ = false;
	
}

TextHandlerOld::TextHandlerOld(const TextFont & font)
{
	create();
	font_ = font;
	charDim_ = pointsToPixels(font.pointSize);	
}

TextHandlerOld::~TextHandlerOld()
{	
	uninit();
	free(imgBuffer_);
}


void TextHandlerOld::drawString(GfxRenderer * gl, const StringData_old * str,
	const Color & color, float opacity, const std::vector<ImageProperties> & imgProps)
{	
	//Beware regressions
	//glDepthFunc(GL_ALWAYS);

	gl->useTextProgram();
	gl->applyCurrentShaderMatrix();
	gl->setTextProgramColor(color.r, color.g, color.b, color.a);
	gl->setTextProgramOpacity(opacity);	
	
	BOOST_FOREACH(const StringData_oldElement_old & data, str->renderData)
	{
		gl->use(data.texture);
		gl->bindElementArrayBuffer(data.indexBuffer);
		gl->bindArrayBuffer(data.vertexBuffer);
		gl->enableVertexAttribArrayPosition();
		gl->vertexAttribPositionPointer(sizeof(VertexStruct), (char *)0);
		gl->enableVertexAttribArrayTexCoord();
		gl->vertexAttribTexCoordPointer(sizeof(VertexStruct), (char *)0 + sizeof(Vector3));

		glDrawElements(GL_TRIANGLES, data.numIndices, GL_UNSIGNED_SHORT, 0);

		//Not including this line results in intermittent crashes in GL driver....
		//TODO: investigate this...
		gl->disableVertexAttribArrayTexCoord();		
	}

	gl->useTextureProgram();
	gl->setTextureProgramOpacity(opacity);		

	for (int i = 0 ; i < imgTextures_.size() ; i++)
	{
		gl->use(imgTextures_[i]);

		gl->drawRect(imgProps[i].x, imgProps[i].y, imgProps[i].width, imgProps[i].height);
	}
	
}

void TextHandlerOld::genChar(wchar_t c, unsigned int index)
{
	if (!init_) return;
	CharData_old charData_old;
	unsigned char * buffer = imgBuffer_;

	int fontsize = font_.pointSize*Global::instance().curSceneMagnification();

	FT_Set_Char_Size(ftFace_, fontsize << 6, fontsize << 6, 96, 96);
	

	FT_Error err;
	FT_UInt glyphIndex = FT_Get_Char_Index(ftFace_, c);
	err = 
		FT_Load_Glyph(ftFace_, glyphIndex, FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH );

	charData_old.index = index;
	charData_old.glyphIndex = glyphIndex;	

	if (font_.italic && !italicBaked_) FT_GlyphSlot_Oblique(ftFace_->glyph);
	if (font_.bold && !boldBaked_) ApplyBold(ftFace_->glyph);
	//FT_GlyphSlot_Embolden(ftFace_->glyph);

	FT_Glyph glyph;
	err = FT_Get_Glyph(ftFace_->glyph, &glyph);

	FT_BBox bbox;
	FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &bbox);
	
	charData_old.texWidth = bbox.xMax - bbox.xMin;//(short)(metrics.width >> 6);
	charData_old.texHeight = bbox.yMax - bbox.yMin;//(short)(metrics.height >> 6);		
	
	

	FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;


	int localCharIndex = index % numCharsPerTexture_;
	int charXIndex = localCharIndex % numCharsPerTextureRow_;
	int charYIndex = localCharIndex / numCharsPerTextureRow_;

	int x = (int)(1 + charXIndex * (texDim_ + texPadding_));
	int y = (int)(1 + charYIndex * (texDim_ + texPadding_));
	
	//FT_Glyph_Metrics metrics = ftFace_->glyph->metrics;
	//charData_old.left = (short)metrics.horiBearingX >> 6;	
	//charData_old.top = (short)metrics.horiBearingY >> 6;	
	//charData_old.advanceX = ftFace_->glyph->linearHoriAdvance >> 16;

	
	FT_Bitmap * bitmap = &bitmap_glyph->bitmap;

	if (bitmap->pixel_mode == FT_PIXEL_MODE_GRAY)
	{
	
		for (int j = y; j < y+bitmap->rows; ++j)
		{
			for (int i = x; i < x+bitmap->width; ++i)
			{
				int localX = i - x;
				int localY = j - y;
				
				buffer[(i+j*textureDim_)]  = 
					bitmap->buffer[localX + bitmap->pitch*localY];
			}
		}
	}
	else 
	{
		for (int j = y; j < y+bitmap->rows; ++j)
		{
			for (int i = x; i < x+bitmap->width; ++i)
			{
				int localX = i - x;
				int localY = j - y;

				unsigned char val = 0;
				unsigned char * ptr = &bitmap->buffer[bitmap->pitch * localY];
				ptr += localX / 8;
				if (*ptr & (128 >> (localX % 8))) val = 255;
				buffer[(i+j*textureDim_)] = val; 
				
			}
		}
	}
	FT_Done_Glyph(glyph);

	FT_Set_Char_Size(ftFace_, font_.pointSize * 64, font_.pointSize * 64, 96, 96);

	FT_Load_Glyph(ftFace_, glyphIndex, FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH );
	FT_Get_Glyph(ftFace_->glyph, &glyph);

	FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &bbox);
	
	charData_old.width = bbox.xMax - bbox.xMin;//(short)(metrics.width >> 6);
	charData_old.height = bbox.yMax - bbox.yMin;//(short)(metrics.height >> 6);	

	FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
	bitmap_glyph = (FT_BitmapGlyph)glyph;
	charData_old.left = (short)bitmap_glyph->left;
	charData_old.top = (short)bitmap_glyph->top;
	charData_old.advanceX = (short)(glyph->advance.x >> 16);

	charData_old_[c] = charData_old;
	FT_Done_Glyph(glyph);
	
}

void TextHandlerOld::genAnyNewChars(
	const std::vector<TextProperties> & properties,
	const std::wstring & str)
{
	if (!init_) return;
	int strLen = (int)str.size();
	set<wchar_t> newChars;

	int nextPropsCharIndex = 0;
	int curPropsIndex = -1;

	bool doCurProp;
	
	for (int i = 0; i < strLen; ++i)
	{
		if (i == nextPropsCharIndex)
		{
			++curPropsIndex;
			if (curPropsIndex < (int)properties.size() - 1)
				nextPropsCharIndex = properties[curPropsIndex + 1].index;
			else
				nextPropsCharIndex = -1;
			doCurProp = properties[curPropsIndex].textHandler == this;
		}

		const wchar_t & c = str[i];	
		if (doCurProp)
		{
			if (charData_old_.find(c) == charData_old_.end()) newChars.insert(c);
		}		 
	}	
	genChars(newChars);
	
}

void TextHandlerOld::genChars(const std::set<wchar_t> & chars)
{
	//determine the number of texture sheets needed
	int numChars = (int)chars.size();
	if (numChars == 0) return;
	
	set<wchar_t>::const_iterator iter = chars.begin();
	int count = 0;
	for (;iter != chars.end(); ++iter, ++curCharIndex_, ++count)
	{		
		bool newTexture = curCharIndex_ % numCharsPerTexture_ == 0;
		bool lastCharOnTexture = (curCharIndex_ + 1)	 % numCharsPerTexture_ == 0;
		bool lastChar = count == numChars - 1;

		if (newTexture)
		{
			//move on to new texture
			memset(imgBuffer_, 0, textureDim_ * textureDim_);
			GLuint texture;
			glGenTextures(1, &texture);
			textures_.push_back(texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		else if (count == 0)
		{
			glBindTexture(GL_TEXTURE_2D, textures_.back());
		}

		const wchar_t & c = *iter;
		genChar(c, curCharIndex_);

		if (lastCharOnTexture || lastChar)
		{
			GLenum err;
			//save current texture
		
			glBindTexture(GL_TEXTURE_2D, textures_.back());
			err = glGetError();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 
				textureDim_, textureDim_, 
				0, GL_ALPHA, GL_UNSIGNED_BYTE, imgBuffer_);		
			err = glGetError();
		}		
	}	
}

void TextHandlerOld::init()
{
	uninit();

	FT_Error err;

	boldBaked_ = false;
	italicBaked_ = false;

	std::string absFontFile = 
		getAbsFileName(Global::instance().readDirectory(), font_.fontFile);

	err = FT_New_Face(Global::instance().ftLibrary(), 
		absFontFile.c_str(), font_.faceIndex, &ftFace_);
	if (!err)
	{
		int fontsize = font_.pointSize*Global::instance().curSceneMagnification();
		FT_Set_Char_Size(ftFace_, fontsize << 6, fontsize << 6, 96, 96);

		//FT_Set_Pixel_Sizes(ftFace_, charDim_, charDim_);		
		//FT_BBox bbox = ftFace_->glyph->face->bbox;
		//int xDim = (bbox.xMax >> 6) - (bbox.xMin >> 6) + 1;
		//int yDim = (bbox.yMax >> 6) - (bbox.yMin >> 6) + 1;

		int xDim = ftFace_->size->metrics.x_ppem;
		int yDim = ftFace_->size->metrics.y_ppem;
		/*
		int lineHeight;
		if (FT_IS_SCALABLE(ftFace_))
		{		
			lineHeight = FT_MulFix( ftFace_->height,
                       ftFace_->size->metrics.y_scale >> 16 ) >> 6;
		}
		else
		{
			lineHeight = ftFace_->height >> 6;
		}*/
		texDim_ = xDim;
		if (texDim_ < yDim) texDim_ = yDim;
		//if (charDim_ < lineHeight) charDim_ = lineHeight;
		texPadding_ = (int)texDim_*0.2f;		

		numCharsPerTextureRow_ = textureDim_ / (texDim_ + texPadding_);
		numCharsPerTexture_ = numCharsPerTextureRow_ * numCharsPerTextureRow_;

		FT_Set_Char_Size(ftFace_, font_.pointSize * 64, font_.pointSize * 64, 96, 96);

		xDim = ftFace_->size->metrics.x_ppem;
		yDim = ftFace_->size->metrics.y_ppem;
		charDim_ = xDim;
		if (charDim_ < yDim) charDim_ = yDim;
		//if (charDim_ < lineHeight) charDim_ = lineHeight;
		padding_ = (int)charDim_*0.2f;		


		italicBaked_ = ftFace_->style_flags & FT_STYLE_FLAG_ITALIC;
		boldBaked_ = ftFace_->style_flags & FT_STYLE_FLAG_BOLD;
		

		if (FT_IS_SCALABLE(ftFace_))
		{
			underlinePosition_ = (float)FT_MulFix( ftFace_->underline_position,
                       ftFace_->size->metrics.y_scale)/64;

			underlineThickness_ = (float)FT_MulFix( ftFace_->underline_thickness,
                       ftFace_->size->metrics.y_scale)/64;
			if (underlineThickness_ < 1) underlineThickness_ = 1;

			lineHeight_ = (float)FT_MulFix( ftFace_->height,
                       ftFace_->size->metrics.y_scale)/64;

			descender_ = (float)FT_MulFix( ftFace_->descender,
                       ftFace_->size->metrics.y_scale)/64;
		}
		else
		{
			//Error
			underlinePosition_ = (float)ftFace_->underline_position/64;
			underlineThickness_ = (float)ftFace_->underline_thickness/64;
			lineHeight_ = (float)ftFace_->height/64;
			descender_ = (float)ftFace_->descender/64;
		}

		init_ = true;
				
	}
}

void TextHandlerOld::uninit()
{
	if (init_)
	{
		if (!textures_.empty())
		{
			glDeleteTextures((int)textures_.size(), &textures_[0]);
			textures_.clear();

			chars_.clear();
			curCharIndex_ = 0;
		}

		if (!imgTextures_.empty())
		{
			for (int i = 0 ; i < imgTextures_.size() ; i++)
			{
				delete imgTextures_[i];
			}
		}

		FT_Done_Face(ftFace_);
		init_ = false;
	}
}

void TextHandlerOld::addChars(const std::string & str)
{
	for (int i = 0; i < (int)str.size(); ++i)
	{
		chars_.insert(str[i]);
	}
}





StringData_old * TextHandlerOld::genString(
	GfxRenderer * gl,
	const std::wstring & str, 
	const std::vector<TextProperties> & properties,
	const TextProperties & thisProps,
	const std::vector<LineProperties> & lines,
	std::vector<ImageProperties> & imgProps,
	float width, float height, float letterSpacing, int align)
{	
	if (!init_) return 0;

	int curPropsIndex;
	int nextPropsCharIndex;
	TextHandlerOld * curPropTextHandlerOld;
	bool doCurProp;

	std::vector<VertexStruct> vertices;
	std::vector<GLushort> indices;

	std::vector<ImageData_old> imgData;

	StringData_old * newStr = new StringData_old;
	
	
	bool validBoundary = width > 0;

	int strLen = (int)str.size();

	//for kerning
	FT_UInt previousIndex;
	//TODO: Maybe get kerning to work properly some time
	bool use_kerning = false;//FT_HAS_KERNING(ftFace_); 	

	TextSpan_old curSpan;
	bool constructingSpan;

	float curLineY;
	
	for (int curTextureIndex = 0; curTextureIndex < (int)textures_.size();
		++curTextureIndex)
	{
		int curLineIndex = 0;
		int numLines = (int)lines.size();

		vertices.clear();
		indices.clear();
		float curX = 0;
		if (align != 0)
		{
			nextPropsCharIndex = 0;
			curPropsIndex = -1;
			float linelen = 0;
			int lineend;
			if (curLineIndex == numLines - 1)
				lineend = str.size();
			else
				lineend = lines[curLineIndex+1].index;
			for (int linei = 0 ; linei < lineend ; linei++)
			{
				if (linei == nextPropsCharIndex)
				{
					++curPropsIndex;
					if (curPropsIndex < (int)properties.size() - 1)
						nextPropsCharIndex = properties[curPropsIndex + 1].index;
					else
						nextPropsCharIndex = -1;

					curPropTextHandlerOld = properties[curPropsIndex].textHandler;
				}
				const wchar_t & c = str[linei];

				//handle ignored characters
				bool isIgnored = c == L'\t' || c == L'\r' || c == L'\b' || c == L'\n';
				//if (c == L'\t' || c == L'\r' || c == L'\b' || c == L'\n') continue;

				CharData_old & data = curPropTextHandlerOld->charData_old_[c];
				if (!isIgnored)
					linelen += data.advanceX + letterSpacing;
			}
			if (align == 1)
				curX = (width - linelen)/2;
			else if (align == 2)
				curX = width - linelen;
			if (curX < 0)
				curX = 0;
		}
		curLineY = 0;

		newStr->size = Vector2(0, 0);
		newStr->size.y = pixelSize() - descender();

		nextPropsCharIndex = 0;
		curPropsIndex = -1;

		constructingSpan = false;

		for (int i = 0; i < strLen; ++i)
		{
			if (i == nextPropsCharIndex)
			{
				++curPropsIndex;
				if (curPropsIndex < (int)properties.size() - 1)
					nextPropsCharIndex = properties[curPropsIndex + 1].index;
				else
					nextPropsCharIndex = -1;

				curPropTextHandlerOld = properties[curPropsIndex].textHandler;
				doCurProp = curPropTextHandlerOld == this && 
					properties[curPropsIndex].color == thisProps.color &&
					properties[curPropsIndex].underline == thisProps.underline;
				previousIndex = 0;

				if (doCurProp && curTextureIndex == 0)
				{
					curSpan.x = (int)curX;
					curSpan.y = (int)curLineY + lines[curLineIndex].ascenderHeight;
					curSpan.width = 0;
					constructingSpan = true;
				}
				else
				{
					if (constructingSpan)
					{
						newStr->spans.push_back(curSpan);
					}
					constructingSpan = false;
				}
			}

			//break if outside region
			int lineBottomY = curLineY + pixelSize();					

			if (validBoundary && (lineBottomY > height && height != 0))
			{
				newStr->size.y = curLineY - descender_;
				break;
			}

			const wchar_t & c = str[i];

			//handle ignored characters
			bool isIgnored = c == L'\t' || c == L'\r' || c == L'\b' || c == L'\n';
			//if (c == L'\t' || c == L'\r' || c == L'\b' || c == L'\n') continue;			

			
			CharData_old & data = curPropTextHandlerOld->charData_old_[c];



			for (int imgindex = 0 ; imgindex < imgProps.size() ; imgindex++)
			{
				if (imgProps[imgindex].index == i)
				{
					Texture* texture = new Texture;
					texture->init(gl, imgProps[imgindex].fileName, Texture::UncompressedRgba32);
					imgTextures_.push_back(texture);
					imgProps[imgindex].x = curX - 1;
					imgProps[imgindex].y = curLineY + lines[curLineIndex].ascenderHeight - data.top;
					curX += imgProps[imgindex].width + letterSpacing;

					ImageData_old id;
					id.x = imgProps[imgindex].x;
					id.y = imgProps[imgindex].y;
					id.width = imgProps[imgindex].width;
					id.height = imgProps[imgindex].height;

					imgData.push_back(id);

					break;
				}
			}


			curX += data.left;
			if (doCurProp) 
			{
				float curY = curLineY + lines[curLineIndex].ascenderHeight - data.top;

				int textureIndex = data.index / numCharsPerTexture_;
				
				int charIndex = data.index - textureIndex * numCharsPerTexture_;

				int charXIndex = charIndex % numCharsPerTextureRow_;
				int charYIndex = charIndex / numCharsPerTextureRow_;

				float texX = (float)(charXIndex * (texDim_ + texPadding_)) / textureDim_;
				float texY = (float)(charYIndex * (texDim_ + texPadding_)) / textureDim_;		
				float dx = (data.width + 2);
				float dy = (data.height + 2);
				//float texDx = (float)dx / textureDim_;
				//float texDy = (float)dy / textureDim_;
				float mag = Global::instance().curSceneMagnification();
				float texDx = (float)(data.texWidth + (2*mag)) / textureDim_;
				float texDy = (float)(data.texHeight + (2*mag)) / textureDim_;

				
				float x = curX - 1;
				float y = curY - 1;

				if (textureIndex == curTextureIndex && !isIgnored)
				{

					int baseIndex = (int)vertices.size();

					vertices.push_back(VertexStruct(
						Vector3(x, y, 0), 
						Vector2(texX, texY)));
					vertices.push_back(VertexStruct(
						Vector3(x, y + dy, 0), 
						Vector2(texX, texY + texDy)));
					vertices.push_back(VertexStruct(
						Vector3(x + dx, y + dy, 0), 
						Vector2(texX + texDx, texY + texDy)));
					vertices.push_back(VertexStruct(
						Vector3(x + dx, y, 0), 
						Vector2(texX + texDx, texY)));
					
					
					indices.push_back(baseIndex + 0);
					indices.push_back(baseIndex + 1);
					indices.push_back(baseIndex + 2);
					indices.push_back(baseIndex + 0);
					indices.push_back(baseIndex + 2);
					indices.push_back(baseIndex + 3);
				}
			}

			curX += data.advanceX - data.left + letterSpacing;

			if (doCurProp)
			{
				//apply kerning
				if (use_kerning && previousIndex && data.glyphIndex)
				{
					FT_Vector kernDelta;
					FT_Get_Kerning(ftFace_, previousIndex, data.glyphIndex, FT_KERNING_DEFAULT, &kernDelta);				
					curX += kernDelta.x >> 6;
				}
				previousIndex = data.glyphIndex;
			}

			if (constructingSpan)
			{
				curSpan.width = curX - curSpan.x;
			}

			if (curX > newStr->size.x) newStr->size.x = curX;

			//handle newlines
			if (curLineIndex + 1 < numLines &&
				lines[curLineIndex + 1].index <= i + 1) 
			{	
				curLineY += lines[curLineIndex].toNextLine;

				newStr->size.y = curLineY + 
					lines[curLineIndex + 1].ascenderHeight - descender_;

				curX = 0;
				previousIndex = 0;
				++curLineIndex;
				if (align != 0)
				{
					int tempNextPropsCharIndex = nextPropsCharIndex;
					int tempCurPropsIndex = curPropsIndex;
					float linelen = 0;
					int lineend;
					if (curLineIndex == numLines - 1)
						lineend = str.size();
					else
						lineend = lines[curLineIndex+1].index;
					TextHandlerOld * tempCurPropTextHandlerOld = curPropTextHandlerOld;
					for (int linei = lines[curLineIndex].index ; linei < lineend ; linei++)
					{
						if (linei == tempNextPropsCharIndex)
						{
							++tempCurPropsIndex;
							if (tempCurPropsIndex < (int)properties.size() - 1)
								tempNextPropsCharIndex = properties[tempCurPropsIndex + 1].index;
							else
								tempNextPropsCharIndex = -1;

							tempCurPropTextHandlerOld = properties[tempCurPropsIndex].textHandler;
						}
						const wchar_t & c = str[linei];

						//handle ignored characters
						bool isIgnored = c == L'\t' || c == L'\r' || c == L'\b' || c == L'\n';
						//if (c == L'\t' || c == L'\r' || c == L'\b' || c == L'\n') continue;

						CharData_old & data = tempCurPropTextHandlerOld->charData_old_[c];
						if (!isIgnored)
							linelen += data.advanceX + letterSpacing;
					}
					if (align == 1)
						curX = (width - linelen)/2;
					else if (align == 2)
						curX = width - linelen;
					if (curX < 0)
						curX = 0;
				}
				if (constructingSpan)
				{
					newStr->spans.push_back(curSpan);
					curSpan.x = (int)curX;
					curSpan.y = (int)curLineY + lines[curLineIndex].ascenderHeight;
					curSpan.width = 0;
				}
			}
		} //end for strlen

		if (constructingSpan)
		{
			newStr->spans.push_back(curSpan);
		}

		if (vertices.empty()) continue;

		StringData_oldElement_old data;
		data.texture = textures_[curTextureIndex];
		data.numIndices = (int)indices.size();
		glGenBuffers(1, &data.vertexBuffer);
		gl->bindArrayBuffer(data.vertexBuffer);
		
		glBufferData(GL_ARRAY_BUFFER, 
			sizeof(VertexStruct)*vertices.size(), 
			&vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &data.indexBuffer);
		gl->bindElementArrayBuffer(data.indexBuffer);	
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
			sizeof(GLushort) * indices.size(), &indices[0], GL_STATIC_DRAW);

		newStr->renderData.push_back(data);
				
	} //end for curTextureIndex

	return newStr;
}

void TextHandlerOld::resizeText()
{
	charData_old_.clear();
	init();
}

bool TextHandlerOld::asyncLoadUpdate()
{
	bool everythingLoaded = true;
	
	for (int i = 0 ; i<imgTextures_.size() ; i++)
	{
		if (!imgTextures_[i]->isFullyLoaded())
		{
			imgTextures_[i]->asyncLoadUpdate();				
		}

		everythingLoaded &= imgTextures_[i]->isLoaded();
	}
	return everythingLoaded;	
}