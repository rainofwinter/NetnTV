#ifndef ImageReader_h__
#define ImageReader_h__

class ImageReader
{
public:
	ImageReader() {}
	virtual ~ImageReader() {}

	virtual void read(const std::string & fileName) = 0; 
	virtual int width() const = 0;
	virtual int height() const = 0;
	virtual unsigned char * bits() const = 0;
};


#endif // ImageReader_h__