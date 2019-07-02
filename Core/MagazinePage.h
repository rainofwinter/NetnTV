#pragma once

class Reader;
class Writer;
class XmlReader;
class XmlWriter;
class Scene;
class MagazinePage;
typedef boost::shared_ptr<MagazinePage> MagazinePageSPtr;

class MagazinePage
{
public:
	MagazinePage() {}
	
	MagazinePage(Scene * scene, const std::string & thumb)
	{
		this->scene_ = scene;
		this->thumbnailFile_ = thumb;
	}

	Scene * scene() const {return scene_;}
	void setScene(Scene * scene) {scene_ = scene;}
	const std::string & thumbnailFile() const {return thumbnailFile_;}
	void setThumbnailFile(const std::string & file) {thumbnailFile_ = file;}

	virtual unsigned char version() const {return 1;}
	
	virtual void write(Writer & writer) const;

	virtual void read(Reader & reader, unsigned char);

	void writeXml(XmlWriter & w) const;
	void readXml(XmlReader & r, xmlNode * node);
private:
	Scene * scene_;
	std::string thumbnailFile_;

	

};