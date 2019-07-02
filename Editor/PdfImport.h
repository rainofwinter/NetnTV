#pragma once


class PdfImport
{
public:
	PdfImport(float width, float height);
	~PdfImport();
	void convertPdf2Img(const std::string & fileName, const std::string & outDir);
private:
	float width_;
	float height_;
};