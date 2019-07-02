#pragma once

class ContentWindow : public QWidget
{
public:

	virtual bool initDocument(const std::string & fileName) = 0;
	virtual void uninitDocument() = 0;

	virtual bool resizeToFitContent(int * width, int * height) const = 0;

	virtual void setPreserveAspect(bool preserveAspect) = 0;

public slots:
	virtual void onNextPage() = 0;
	virtual void onPreviousPage() = 0;

protected:

	bool decompress(
		const QString & decompressDir, const QString & fileName) const;

	bool deleteDirFiles(const QString &dirName, bool rmdir) const;

};