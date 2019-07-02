#include "stdafx.h"
#include "Texture.h"
#include <sys/stat.h>
#include "FileUtils.h"
#include <boost/scope_exit.hpp>
#include <boost/bind.hpp>
#include "lz4.h"

const unsigned int cEtc1MagicNumber = 3232125;
const unsigned int cPvrtcMagicNumber = 3232126;

void readCompressParamsEnc(void);
void decompressBlockDiffFlip(unsigned int block_part1, unsigned int block_part2, uint8 *img,int width,int height,int startx,int starty);
void read_big_endian_2byte_word(unsigned short *blockadr, FILE *f);
void read_big_endian_4byte_word(unsigned int *blockadr, FILE *f);

void compressBlockDiffFlipFast				(uint8 *img, uint8 *imgdec,int width,int height,int startx,int starty, unsigned int &compressed1, unsigned int &compressed2);
void compressBlockDiffFlipFastPerceptual	(uint8 *img, uint8 *imgdec,int width,int height,int startx,int starty, unsigned int &compressed1, unsigned int &compressed2);

void compressBlockDiffFlipMedium			(uint8 *img,int width,int height,int startx,int starty, unsigned int &compressed1, unsigned int &compressed2);
void compressBlockDiffFlipMediumPerceptual	(uint8 *img,int width,int height,int startx,int starty, unsigned int &compressed1, unsigned int &compressed2);

void compressBlockDiffFlipSlow				(uint8 *img,int width,int height,int startx,int starty, unsigned int &compressed1, unsigned int &compressed2);
void compressBlockDiffFlipSlowPerceptual	(uint8 *img,int width,int height,int startx,int starty, unsigned int &compressed1, unsigned int &compressed2);

void *LibPNGDecodeImage(const char *filename, int *width, int *height);
void *LibJPEGDecodeImage(const char *filename, int *width, int *height);
void gfxLog(const std::string & str);
//-----------------------------------------------------------------------------

struct CompressEtc1Run
{
	unsigned int pos;
	unsigned short reps;
	unsigned short val;
};

unsigned char * compress2Stage(unsigned int * totalSize, const unsigned char * data, unsigned int size)
{
	unsigned short * ptr = (unsigned short *)data;
	unsigned int baseIndex = 0;
	unsigned int uShortSize = size/2;


	unsigned short curRun[] = {0, 0, 0, 0};

	std::vector<CompressEtc1Run> runs;
	std::vector<unsigned short> nonRunVals;

	while(baseIndex < uShortSize)
	{
		for (int j = 0; j < 4; ++j)
		{
			unsigned short val = ptr[baseIndex + j];

			if (curRun[j] == 0)
			{			
				//look ahead for a >4 rep run
				unsigned short numReps = 0;
				for (int k = 0; k < 65535; ++k)
				{
					unsigned int curIndex = baseIndex + j + 4*k;
					if (curIndex >= uShortSize) break;
					if (ptr[curIndex] != val) break;
					numReps++;
				}

				if (numReps > 4)
				{
					curRun[j] = numReps;
					CompressEtc1Run run;
					run.pos = baseIndex + j;
					run.reps = numReps;
					run.val = val;
					runs.push_back(run);
					--curRun[j];
				}
				else
				{
					nonRunVals.push_back(val);
				}
			}
			else 
				--curRun[j];
		}

		baseIndex += 4;
	}

	*totalSize = sizeof(unsigned int) + sizeof(unsigned int) + sizeof(CompressEtc1Run)*runs.size() + sizeof(unsigned short)*nonRunVals.size();
	unsigned char * retBuffer = (unsigned char *)malloc(*totalSize);

	unsigned int numRuns = (unsigned int)runs.size();
	unsigned char * dst = retBuffer;

	unsigned int origSize = size/8;
	memcpy(dst, &origSize, sizeof(unsigned int));
	dst += sizeof(unsigned int);
	
	memcpy(dst, &numRuns, sizeof(unsigned int));
	dst += sizeof(unsigned int);
	
	if (numRuns > 0)
	{
		memcpy(dst, &runs[0], sizeof(CompressEtc1Run)*numRuns);
		dst += sizeof(CompressEtc1Run)*numRuns;
	}
	
	if (!nonRunVals.empty()) memcpy(dst, &nonRunVals[0], sizeof(unsigned short)*nonRunVals.size());

	return retBuffer;
}

unsigned int decompress2Stage(unsigned char * bufOut, const unsigned char * data)
{
	const unsigned char * curPtr = data;
	unsigned int num8ByteBlocks = *(unsigned int *)curPtr;
	curPtr += sizeof(unsigned int);
	unsigned int numRuns = *(unsigned int *)curPtr;
	curPtr += sizeof(unsigned int);
	CompressEtc1Run * runs = (CompressEtc1Run *)curPtr;
	curPtr += sizeof(CompressEtc1Run) * numRuns;

	unsigned short * ret = (unsigned short *)bufOut;
	unsigned short * sData = (unsigned short *)curPtr;

	unsigned int cur8ByteBlock = 0;
	unsigned int curPos = 0; //in 2 byte units
	unsigned int curSrcPos = 0;
	unsigned int curRunPos = 0;

	CompressEtc1Run curRuns[4];
	for (int i = 0; i < 4; ++i)
	{
		curRuns[i].pos = 0;
		curRuns[i].reps = 0;
		curRuns[i].val = 0;
	}	

	while(cur8ByteBlock < num8ByteBlocks)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (curRunPos < numRuns && runs[curRunPos].pos == curPos)
			{				
				curRuns[i] = runs[curRunPos];
				++curRunPos;
			}

			if (curRuns[i].reps == 0)
			{
				ret[curPos] = sData[curSrcPos];
				++curSrcPos;
			}
			else
			{
				ret[curPos] = curRuns[i].val;
				--curRuns[i].reps;				
			}			
			
			++curPos;
		}		
		
		++cur8ByteBlock;
	}	

	return num8ByteBlocks * 8;
}

//-----------------------------------------------------------------------------



unsigned char * compress2Stage(unsigned int * totalSize, const unsigned char * data, unsigned int size);
unsigned char * decompressImageEtc1(const unsigned char * rgbData, const unsigned char * alphaData, int width, int height);




void write_big_endian_4byte_word(unsigned int *blockadr, unsigned char * pOut)
{
	uint8 bytes[4];
	unsigned int block;

	block = blockadr[0];

	bytes[0] = (block >> 24) & 0xff;
	bytes[1] = (block >> 16) & 0xff;
	bytes[2] = (block >> 8) & 0xff;
	bytes[3] = (block >> 0) & 0xff;

	memcpy(pOut, bytes, 4);
}

void read_big_endian_4byte_word(unsigned int *blockadr, const unsigned char * pIn)
{
	uint8 bytes[4];
	unsigned int block;

	bytes[0] = pIn[0];
	bytes[1] = pIn[1];
	bytes[2] = pIn[2];
	bytes[3] = pIn[3];

	block = 0;

	block |= bytes[0];
	block = block << 8;
	block |= bytes[1];
	block = block << 8;
	block |= bytes[2];
	block = block << 8;
	block |= bytes[3];

	blockadr[0] = block;
}
int PVRTDecompressETC(const void * const pSrcData,
	const unsigned int &x,
	const unsigned int &y,
	void *pDestData,
	const int &nMode);

unsigned char * decompressImageEtc1(const unsigned char * rgbData, const unsigned char * alphaData, int width, int height)
{	
	unsigned int block_part1, block_part2;

	uint8 * img = NULL;

	BOOST_SCOPE_EXIT( (&img)) {
		if (img) free(img);
	} BOOST_SCOPE_EXIT_END


	int expandedwidth = width, expandedheight = height;
	if (width % 4 != 0) expandedwidth = (width / 4 + 1) * 4;
	if (height % 4 != 0) expandedheight = (height / 4 + 1) * 4;

	const unsigned char * curPtr;
	unsigned char * curDstPtr;

	unsigned char * ret = (unsigned char *)malloc(4*width*height);



	img = (uint8 * )malloc(3*expandedwidth*expandedheight);

	curPtr = rgbData;
	for(int y=0;y<expandedheight/4;y++)
	{
		for(int x=0;x<expandedwidth/4;x++)
		{
			read_big_endian_4byte_word(&block_part1, curPtr);
			curPtr += 4;
			read_big_endian_4byte_word(&block_part2, curPtr);
			curPtr += 4;
			decompressBlockDiffFlip(block_part1, block_part2,img,expandedwidth,expandedheight,4*x,4*y);
		}
	}

	curPtr = img;
	curDstPtr = ret;
	for(int y=0;y<height;y++)
	{
		for(int x=0;x<width;x++)
		{
			*(curDstPtr++) = *(curPtr++);
			*(curDstPtr++) = *(curPtr++);
			*(curDstPtr++) = *(curPtr++);
			*(curDstPtr++) = 0xff;
		}

		curPtr += 3*(expandedwidth - width);
	}


	if (alphaData)
	{	
		curPtr = alphaData;
		for(int y=0;y<expandedheight/4;y++)
		{
			for(int x=0;x<expandedwidth/4;x++)
			{
				read_big_endian_4byte_word(&block_part1, curPtr);
				curPtr += 4;
				read_big_endian_4byte_word(&block_part2, curPtr);
				curPtr += 4;
				decompressBlockDiffFlip(block_part1, block_part2,img,expandedwidth,expandedheight,4*x,4*y);
			}
		}

		curPtr = img;
		curDstPtr = ret;
		for(int y=0;y<height;y++)
		{
			for(int x=0;x<width;x++)
			{
				curDstPtr[3] = curPtr[0];
				curDstPtr += 4;
				curPtr += 3;				
			}
			curPtr += 3*(expandedwidth - width);
		}
	}

	return ret;
}

unsigned int nearestPow2(unsigned int num);
void getExpandedDims(int & expandedwidth, int & expandedheight, int width, int height)
{
	expandedwidth = width;
	expandedheight = height;
	if (width % 4 != 0) expandedwidth = (width / 4 + 1) * 4;
	if (height % 4 != 0) expandedheight = (height / 4 + 1) * 4;
}

void getEtc1MipMap(unsigned int & mipMapChainEtc1Size, unsigned int & numMipMaps, int width, int height, bool doMipMaps)
{
	if (doMipMaps)
	{	
		numMipMaps = 0;
		mipMapChainEtc1Size = 0;
		int w = width, h = height;
		while(1)
		{
			numMipMaps++;
			int ew, eh;
			getExpandedDims(ew, eh, w, h);
			int totblocks = ew/4 * eh/4;
			mipMapChainEtc1Size += totblocks * 8;
			if (w == 1 && h == 1) break;
			if (w > 1) w /= 2;
			if (h > 1) h /= 2;			
		}
		
	}
	else
	{
		numMipMaps = 1;
		int ew, eh;
		getExpandedDims(ew, eh, width, height);
		int totblocks = ew/4 * eh/4;
		mipMapChainEtc1Size = totblocks * 8;
	}
}

#if !defined(ANDROID) && !defined(IOS) && !defined(OSX)
#include <boost/thread.hpp>
struct Region
{
	int startX, endX, startY, endY;
};


void etcCompressThreadFunc(uint8 * img, unsigned char * buffer, Region region, uint8 * imgdec, int action, int expandedwidth, int expandedheight)
{
	unsigned int block1, block2;
	int x, y;

	for(y=region.startY;y<=region.endY;y++)
	{
		for(x=region.startX;x<=region.endX;x++)
		{

			switch(action)
			{
			case 0:
				// FAST only tests the two most likely base colors.
				compressBlockDiffFlipFast(img, imgdec, expandedwidth, expandedheight, 4*x, 4*y, block1, block2);
				break;
			case 1:
				// The MEDIUM version tests all colors in a 3x3x3 cube around the average colors
				// This increases the likelihood that the differential mode is selected.
				compressBlockDiffFlipMedium(img,expandedwidth,expandedheight,4*x,4*y, block1, block2);							
				break;
			case 2:
				// The SLOW version tests all colors in a a 5x5x5 cube around the average colors
				// It also tries the nondifferential mode for each block even if the differential succeeds.
				compressBlockDiffFlipSlow(img,expandedwidth,expandedheight,4*x,4*y, block1, block2);							
				break;
			case 3:
				// FAST with PERCEPTUAL error metric
				compressBlockDiffFlipFastPerceptual(img, imgdec, expandedwidth, expandedheight, 4*x, 4*y, block1, block2);
				break;
			case 4:
				// MEDIUM with PERCEPTUAL error metric
				compressBlockDiffFlipMediumPerceptual(img,expandedwidth,expandedheight,4*x,4*y, block1, block2);					
				break;
			case 5:
				// SLOW with PERCEPTUAL error metric
				compressBlockDiffFlipSlowPerceptual(img,expandedwidth,expandedheight,4*x,4*y, block1, block2);							
				break;
			default:
				//TODO ERROR CHECK
				break;
			}			
			unsigned char * writePtr = buffer + 8*(y*expandedwidth/4 + x);
			write_big_endian_4byte_word(&block1, writePtr);			
			write_big_endian_4byte_word(&block2, writePtr + 4);
		}
	}
}

/**
BGRA
*/
unsigned int compressImageEtc1Sub(unsigned char * buffer, unsigned char * imgdec, uint8 * img, int expandedwidth, int expandedheight, int action, int numThreads)
{	
	int blockW = expandedwidth/4;
	int blockH = expandedheight/4;
	int totblocks = blockW * blockH;	

	std::vector<Region> regions;

	if (blockW > blockH)
	{
		int wPerThread = blockW / numThreads;
		Region region;
		for (int t = 0; t < numThreads; ++t)
		{
			region.startX = t*wPerThread;
			region.endX = region.startX + wPerThread - 1;
			if (t == numThreads - 1)
				region.endX = blockW - 1;

			region.startY = 0;
			region.endY = blockH - 1;
			regions.push_back(region);
		}

	}
	else
	{

		int hPerThread = blockH / numThreads;
		Region region;
		for (int t = 0; t < numThreads; ++t)
		{
			region.startY = t*hPerThread;
			region.endY = region.startY + hPerThread - 1;
			if (t == numThreads - 1)
				region.endY = blockH - 1;

			region.startX = 0;
			region.endX = blockW - 1;
			regions.push_back(region);
		}

	}

	boost::thread * threads = new boost::thread[numThreads];
	for (int t = 0; t < numThreads; ++t)
	{					
		threads[t] = boost::thread(etcCompressThreadFunc,
			img, buffer, regions[t], imgdec, action, expandedwidth, expandedheight
			);
	}

	for (int t= 0; t < numThreads; ++t)
	{
		threads[t].join();
	}

	delete [] threads;

	return totblocks * 8;

}


void mipMapSizeDown(unsigned char * rgbBuffer, unsigned int width, unsigned int height)
{
	unsigned nWidth = width/2, nHeight = height/2;
	unsigned char * dst = rgbBuffer;
	for (unsigned j = 0; j < nHeight; ++j)
	{
		for (unsigned i = 0; i < nWidth; ++i)
		{

			double r = 0, g = 0, b = 0;

			unsigned ri = 2 * i, rj = 2 * j;
			unsigned index = 3*(rj*width + ri);

			r += rgbBuffer[index++];
			g += rgbBuffer[index++];
			b += rgbBuffer[index++];

			r += rgbBuffer[index++];
			g += rgbBuffer[index++];
			b += rgbBuffer[index++];

			rj += 1;
			index = 3*(rj*width + ri);

			r += rgbBuffer[index++];
			g += rgbBuffer[index++];
			b += rgbBuffer[index++];

			r += rgbBuffer[index++];
			g += rgbBuffer[index++];
			b += rgbBuffer[index++];

			r /= 4.0;
			g /= 4.0;
			b /= 4.0;

			*(dst++) = (unsigned char)r;
			*(dst++) = (unsigned char)g;
			*(dst++) = (unsigned char)b;
		}
	}
}





bool compressImageEtc1(uint8 * rgbaImg,int width,int height,const char *dstfile, int action, int numThreads, bool mipmaps)
{	
	readCompressParamsEnc();
	bool doMipMaps = true;
	if (nearestPow2(width) != width) doMipMaps = false;
	if (nearestPow2(height) != height) doMipMaps = false;
	if (!mipmaps) doMipMaps = false;

	int expandedwidth, expandedheight;
	getExpandedDims(expandedwidth, expandedheight, width, height);

	uint8 * rgbBuffer = NULL;
	uint8 * alphaBuffer = NULL;
	uint8 * buffer = NULL;
	uint8 * imgdec = NULL;
	uint8 * comprBuffers[] = {NULL, NULL};
	unsigned int comprSizes[] = {0, 0};
	BOOST_SCOPE_EXIT( (&rgbBuffer) (&alphaBuffer) (&buffer) (&comprBuffers) (&imgdec)) {
		if (rgbBuffer) free(rgbBuffer);
		if (alphaBuffer) free(alphaBuffer);
		if (imgdec) free(imgdec);
		if (buffer) free(buffer);
		for (int i = 0; i < 2; ++i)
			if (comprBuffers[i]) free(comprBuffers[i]);		
	} BOOST_SCOPE_EXIT_END

	rgbBuffer = (uint8 *) malloc(expandedwidth * expandedheight * 3);
	alphaBuffer = (uint8 *) malloc(expandedwidth * expandedheight * 3);
	bool isAlpha = false;
	uint8 * curImgPtr = rgbaImg;
	uint8 * curRgbPtr = rgbBuffer;
	uint8 * curAlphaPtr = alphaBuffer;
	uint8 * lastImgPtr = NULL;

	std::vector<unsigned char> prevRowRgb(3*width, 0);	
	std::vector<unsigned char> prevRowAlpha(3*width, 0);

	//copy image data into rgbBuffer and alphaBuffer

	for (int i = 0; i < height; ++i)
	{
		unsigned curRowIndex = 0;
		for (int j = 0; j < width; ++j)
		{	
			prevRowRgb[curRowIndex + 0] = curRgbPtr[0] = curImgPtr[2];
			prevRowRgb[curRowIndex + 1] = curRgbPtr[1] = curImgPtr[1];
			prevRowRgb[curRowIndex + 2] = curRgbPtr[2] = curImgPtr[0];		

			prevRowAlpha[curRowIndex + 0] = curAlphaPtr[0] = curImgPtr[3];
			prevRowAlpha[curRowIndex + 1] = curAlphaPtr[1] = curImgPtr[3];
			prevRowAlpha[curRowIndex + 2] = curAlphaPtr[2] = curImgPtr[3];

			if (curImgPtr[3] != 255) isAlpha = true;

			lastImgPtr = curImgPtr;
			curImgPtr += 4;
			curRgbPtr += 3;
			curAlphaPtr += 3;
			curRowIndex += 3;
		}

		curRowIndex -= 3;
		for (int j = width; j < expandedwidth; ++j)
		{
			memcpy(curRgbPtr, &prevRowRgb[curRowIndex], 3);
			memcpy(curAlphaPtr, &prevRowAlpha[curRowIndex], 3);
			curRgbPtr += 3;
			curAlphaPtr += 3;
		}
	}

	for (int i = height; i < expandedheight; ++i)
	{
		unsigned curRowIndex = 0;
		for (int j = 0; j < width; ++j)
		{
			memcpy(curRgbPtr, &prevRowRgb[curRowIndex], 3);
			memcpy(curAlphaPtr, &prevRowAlpha[curRowIndex], 3);
			curRgbPtr += 3;
			curAlphaPtr += 3;
			curRowIndex += 3;
		}
		curRowIndex -= 3;
		for (int j = width; j < expandedwidth; ++j)
		{
			memcpy(curRgbPtr, &prevRowRgb[curRowIndex], 3);
			memcpy(curAlphaPtr, &prevRowAlpha[curRowIndex], 3);
			curRgbPtr += 3;
			curAlphaPtr += 3;
		}
	}

	FILE * f = fopen(dstfile, "wb");
	if (!f) return false;

	// Write magic number
	fwrite(&cEtc1MagicNumber, 1, sizeof(unsigned int), f);

	// Write version
	unsigned int version = 3;
	fwrite(&version, 1, sizeof(unsigned int), f);

	fwrite(&isAlpha, 1, 1, f);
	fwrite(&doMipMaps, 1, 1, f);

	unsigned int uWidth = width, uHeight = height;
	fwrite(&uWidth, sizeof(unsigned int), 1, f);
	fwrite(&uHeight, sizeof(unsigned int), 1, f);

	int blockW = expandedwidth/4;
	int blockH = expandedheight/4;
	int totblocks = blockW * blockH;	


	unsigned int mipMapChainEtc1Size;
	unsigned int numMipMaps;

	getEtc1MipMap(mipMapChainEtc1Size, numMipMaps, width, height, doMipMaps);
	buffer = (uint8 *)malloc(mipMapChainEtc1Size);	
	imgdec = (uint8 *)malloc(expandedwidth * expandedheight * 3);

	
	
	std::vector<unsigned char *> imgBuffers;
	imgBuffers.push_back(rgbBuffer);
	if (isAlpha) imgBuffers.push_back(alphaBuffer);

	for (int u = 0; u < (int)imgBuffers.size(); ++u)
	{
		unsigned char * imgBuffer = imgBuffers[u];
		comprBuffers[u] = (uint8 *)malloc(mipMapChainEtc1Size);
		uint8 * curBufferPtr = buffer;
		int w = width, h = height;

		for (int i = 0; i < numMipMaps; ++i)
		{
			int ew, eh;
			getExpandedDims(ew, eh, w, h);
			curBufferPtr += compressImageEtc1Sub(curBufferPtr, imgdec, imgBuffer, ew, eh, action, numThreads);		
			if (i < numMipMaps - 1) 
			{
				mipMapSizeDown(imgBuffer, ew, eh);
				if (w > 1) w /= 2;
				if (h > 1) h /= 2;
			}
		}	

		comprSizes[u] = (unsigned int)LZ4_compress((const char *)buffer, (char *)comprBuffers[u], mipMapChainEtc1Size);			
	}

	for (int i = 0; i < 2; ++i)
	{
		if (comprSizes[i] > 0) fwrite(&comprSizes[i], 1, sizeof(unsigned int), f);
	}
	
	for (int i = 0; i < 2; ++i)
	{
		if (comprSizes[i] > 0) fwrite(comprBuffers[i], 1, comprSizes[i], f);
	}

	fclose(f);
	return true;
}

#endif
























unsigned char * decompressImageEtc1(const unsigned char * rgbData, const unsigned char * alphaData, int width, int height);

unsigned char * LoadEtc1Image(Texture::Format * type, const char * filename, int * widthOut, int * heightOut)
{
	FILE * file = 0;
	*widthOut = 0;
	*heightOut = 0;
	unsigned char * tempBuffer = NULL;
	BOOST_SCOPE_EXIT( (&file) (&tempBuffer)) {
		if (file) fclose(file);		
		if (tempBuffer) free(tempBuffer);
	} BOOST_SCOPE_EXIT_END
	
	file = fopen(filename, "rb");
	
	unsigned int magicNumber;
	unsigned int version;	

	fread(&magicNumber, 1, sizeof(unsigned int), file);
	if (magicNumber != cEtc1MagicNumber) return NULL;

	fread(&version, 1, sizeof(unsigned int), file);

	unsigned char * data = NULL;
	unsigned int width, height;
	unsigned int expWidth, expHeight;
	unsigned int dataSize;

	bool isAlpha;

	if (version == 1)
	{		
		fread(&isAlpha, 1, 1, file);
		fread(&width, 1, sizeof(unsigned int), file);
		fread(&height, 1, sizeof(unsigned int), file);

		expWidth = width, expHeight = height;
		*widthOut = width;
		*heightOut = height;

		if (expWidth % 4 != 0) expWidth = (expWidth/4 + 1)*4;
		if (expHeight % 4 != 0) expHeight = (expHeight/4 + 1)*4;

		dataSize = (expWidth/4 * expHeight/4) * 8;
		
		if (isAlpha)
		{
			data = (unsigned char *)malloc(1 + 2*dataSize);
			if (!data) return NULL;
			*type = Texture::CompressionEtc1Alpha;
			fread(data + 1, 1, dataSize, file);
			fread(data + 1 + dataSize, 1, dataSize, file);	
		}
		else
		{
			data = (unsigned char *)malloc(1 + dataSize);
			if (!data) return NULL;
			*type = Texture::CompressionEtc1;
			fread(data + 1, 1, dataSize, file);
		}
		data[0] = false;
	}
	else if (version == 2)
	{
		fread(&isAlpha, 1, 1, file);		
		fread(&width, 1, sizeof(unsigned int), file);
		fread(&height, 1, sizeof(unsigned int), file);

		expWidth = width, expHeight = height;
		*widthOut = width;
		*heightOut = height;

		if (expWidth % 4 != 0) expWidth = (expWidth/4 + 1)*4;
		if (expHeight % 4 != 0) expHeight = (expHeight/4 + 1)*4;

		 dataSize = (expWidth/4 * expHeight/4) * 8;
						
		if (isAlpha)
		{
			data = (unsigned char *)malloc(1 + 2*dataSize);
			if (!data) return NULL;
			*type = Texture::CompressionEtc1Alpha;

			unsigned int rgbComprSize, alphaComprSize;

			fread(&rgbComprSize, 1, sizeof(unsigned int), file);
			fread(&alphaComprSize, 1, sizeof(unsigned int), file);

			tempBuffer = (unsigned char *)malloc(std::max(rgbComprSize, alphaComprSize));

			fread(tempBuffer, 1, rgbComprSize, file);
			decompress2Stage(data + 1, tempBuffer);

			fread(tempBuffer, 1, alphaComprSize, file);		
			decompress2Stage(data + 1 + dataSize, tempBuffer);
		}
		else
		{
			data = (unsigned char *)malloc(1 + dataSize);
			if (!data) return NULL;
			*type = Texture::CompressionEtc1;

			unsigned int rgbComprSize;
			fread(&rgbComprSize, 1, sizeof(unsigned int), file);

			tempBuffer = (unsigned char *)malloc(rgbComprSize);

			fread(tempBuffer, 1, rgbComprSize, file);
			decompress2Stage(data + 1, tempBuffer);
		}
		data[0] = false;
	}
	else if (version == 3)
	{
		bool doMipMaps;
		fread(&isAlpha, 1, 1, file);		
		fread(&doMipMaps, 1, 1, file);
		fread(&width, 1, sizeof(unsigned int), file);
		fread(&height, 1, sizeof(unsigned int), file);

		expWidth = width, expHeight = height;
		*widthOut = width;
		*heightOut = height;

		if (expWidth % 4 != 0) expWidth = (expWidth/4 + 1)*4;
		if (expHeight % 4 != 0) expHeight = (expHeight/4 + 1)*4;

		int blockW = expWidth/4;
		int blockH = expHeight/4;
		int totblocks = blockW * blockH;	

		unsigned int numMipMaps;

		getEtc1MipMap(dataSize, numMipMaps, width, height, doMipMaps);

		if (isAlpha)
		{
			data = (unsigned char *)malloc(1 + 2*dataSize);
			if (!data) return NULL;
			*type = Texture::CompressionEtc1Alpha;

			unsigned int rgbComprSize, alphaComprSize;

			fread(&rgbComprSize, 1, sizeof(unsigned int), file);
			fread(&alphaComprSize, 1, sizeof(unsigned int), file);

			tempBuffer = (unsigned char *)malloc(std::max(rgbComprSize, alphaComprSize));

			fread(tempBuffer, 1, rgbComprSize, file);
			LZ4_uncompress((char *)tempBuffer, (char *)data + 1, dataSize);		
			
			fread(tempBuffer, 1, alphaComprSize, file);
			LZ4_uncompress((char *)tempBuffer, (char *)data + 1 + dataSize, dataSize);			
		}
		else
		{
			data = (unsigned char *)malloc(1 + dataSize);
			if (!data) return NULL;
			*type = Texture::CompressionEtc1;

			unsigned int rgbComprSize;
			fread(&rgbComprSize, 1, sizeof(unsigned int), file);

			tempBuffer = (unsigned char *)malloc(rgbComprSize);

			fread(tempBuffer, 1, rgbComprSize, file);
			LZ4_uncompress((char *)tempBuffer, (char *)data + 1, dataSize);				
		}
		data[0] = doMipMaps;
	}

	
#ifndef ANDROID

	*type = Texture::UncompressedRgba32;
	unsigned char * ret = NULL;
	if (isAlpha)
		ret = decompressImageEtc1(data + 1, data + 1 + dataSize, width, height);
	else
		ret = decompressImageEtc1(data + 1, NULL, width, height);
	
	
	free(data);
	return ret;
#endif

	return data;
}

//-----------------------------------------------------------------------------
#include "PVRTTexture.h"
#include "PVRTDecompress.h"
#include "PVRTResourceFile.h"

#define GL_ETC1_RGB8_OES 0x8d64

#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG                      0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG                      0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG                     0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG                     0x8C03

#define GL_HALF_FLOAT_OES 0x8D61

PVRTuint32 getPvrCompressionFormat(const PVRTextureHeaderV3& sTextureHeader)
{
	PVRTuint64 PixelFormat = sTextureHeader.u64PixelFormat;
	EPVRTVariableType ChannelType = (EPVRTVariableType)sTextureHeader.u32ChannelType;
	PVRTuint32 internalformat = 0;

	PVRTuint64 PixelFormatPartHigh = PixelFormat&PVRTEX_PFHIGHMASK;

	if (PixelFormatPartHigh==0)
	{
		//Format and type == 0 for compressed textures.
		switch (PixelFormat)
		{
		case ePVRTPF_PVRTCI_2bpp_RGB:
			{
				internalformat=GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG; break;
			}
		case ePVRTPF_PVRTCI_2bpp_RGBA:
			{
				internalformat=GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG; break;
			}
		case ePVRTPF_PVRTCI_4bpp_RGB:
			{
				internalformat=GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG; break;
			}
		case ePVRTPF_PVRTCI_4bpp_RGBA:
			{
				internalformat=GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG; break;
			}
		}
	}	

	return internalformat;
}

unsigned char * loadPvrTextureFromPointer(Texture::Format * type, const void* pointer, int * widthOut, int * heightOut)
{
	//Texture setup
	PVRTextureHeaderV3 sTextureHeader;
	PVRTuint8* pTextureData=NULL;

	//Just in case header and pointer for decompression.
	PVRTextureHeaderV3 sTextureHeaderDecomp;
	void* pDecompressedData=NULL;

	//Check if it's an old header format
	if((*(PVRTuint32*)pointer)!=PVRTEX3_IDENT)
	{		
		return NULL;
	}


	//Get the header from the main pointer.
	sTextureHeader=*(PVRTextureHeaderV3*)pointer;

	//cube map?
	if(sTextureHeader.u32NumFaces>1)
	{
		return NULL;
	}

	//Check if this is a texture array.
	if(sTextureHeader.u32NumSurfaces>1)
	{
		return NULL;
	}

	//Get the texture data.
	pTextureData = (PVRTuint8*)pointer+PVRTEX3_HEADERSIZE+sTextureHeader.u32MetaDataSize;

	GLenum eTextureInternalFormat  = getPvrCompressionFormat(sTextureHeader);
	if (eTextureInternalFormat == 0) return NULL;

	//Check supported texture formats.
	bool bIsPVRTCSupported = false;
#ifdef IOS
	bIsPVRTCSupported = true;
#endif

	//Check for PVRTCI support.
	if(bIsPVRTCSupported)
	{
		switch (eTextureInternalFormat)
		{
		case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG: *type = Texture::CompressionPvrtc; break;
		default: return NULL; break;
		}	
	}
	else
	{
		*type = Texture::UncompressedRgba32;
		//Try to decompress the texture.

		//Check if it's 2bpp.
		bool bIs2bppPVRTC = (eTextureInternalFormat==GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG || eTextureInternalFormat==GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG);

		//Create a near-identical texture header for the decompressed header.
		sTextureHeaderDecomp = sTextureHeader;
		sTextureHeaderDecomp.u32ChannelType=ePVRTVarTypeUnsignedByteNorm;
		sTextureHeaderDecomp.u32ColourSpace=ePVRTCSpacelRGB;
		sTextureHeaderDecomp.u64PixelFormat=PVRTGENPIXELID4('r','g','b','a',8,8,8,8);

		//Allocate enough memory for the decompressed data. OGLES2, so only decompress one surface/face.
		pDecompressedData = malloc(PVRTGetTextureDataSize(sTextureHeaderDecomp, PVRTEX_ALLMIPLEVELS, false, true) );

		//Check the malloc.
		if (!pDecompressedData)
		{
			return NULL;
		}

		//Get the dimensions for the current MIP level.
		PVRTuint32 uiMIPWidth = sTextureHeaderDecomp.u32Width;
		PVRTuint32 uiMIPHeight = sTextureHeaderDecomp.u32Height;

		//Setup temporary variables.
		PVRTuint8* pTempDecompData = (PVRTuint8*)pDecompressedData;
		PVRTuint8* pTempCompData = (PVRTuint8*)pTextureData;				

		//Decompress all the MIP levels.
		for (PVRTuint32 uiMIPMap=0;uiMIPMap<sTextureHeader.u32MIPMapCount;++uiMIPMap)
		{
			//Get the face offset. Varies per MIP level.
			PVRTuint32 decompressedFaceOffset = PVRTGetTextureDataSize(sTextureHeaderDecomp, uiMIPMap, false, false);
			PVRTuint32 compressedFaceOffset = PVRTGetTextureDataSize(sTextureHeader, uiMIPMap, false, false);

			for (PVRTuint32 uiFace=0;uiFace<sTextureHeader.u32NumFaces;++uiFace)
			{
				//Decompress the texture data.
				PVRTDecompressPVRTC(pTempCompData,bIs2bppPVRTC?1:0,uiMIPWidth,uiMIPHeight,pTempDecompData);

				//Move forward through the pointers.
				pTempDecompData+=decompressedFaceOffset;
				pTempCompData+=compressedFaceOffset;
			}

			//Work out the current MIP dimensions.
			uiMIPWidth=PVRT_MAX(1,uiMIPWidth>>1);
			uiMIPHeight=PVRT_MAX(1,uiMIPHeight>>1);
		}				
	}



	//Initialise the width/height
	PVRTuint32 u32MIPWidth = sTextureHeader.u32Width;
	PVRTuint32 u32MIPHeight = sTextureHeader.u32Height;

	*widthOut = u32MIPWidth;
	*heightOut = u32MIPHeight;

	//Temporary data to save on if statements within the load loops.
	PVRTuint8* pTempData=NULL;
	PVRTextureHeaderV3 *psTempHeader=NULL;


	if (!bIsPVRTCSupported)
	{
		pTempData=(PVRTuint8*)pDecompressedData;
		psTempHeader=&sTextureHeaderDecomp;
	}
	else
	{
		pTempData=pTextureData;
		psTempHeader=&sTextureHeader;
	}

	PVRTuint32 uiMIPLevel=0;
	PVRTuint32 uiCurrentMIPSize=PVRTGetTextureDataSize(*psTempHeader,uiMIPLevel,false,false);


	if (bIsPVRTCSupported)
	{
		unsigned char * ret = (unsigned char *)malloc(1 + uiCurrentMIPSize);
		memcpy(ret + 1, pTempData, uiCurrentMIPSize);
		ret[0] = false;
		return ret;
	}
	else
	{
		return (unsigned char *)pDecompressedData;
	}

	return NULL;		
}


unsigned char * LoadPvrImage(Texture::Format * type, const char * filename, int * widthOut, int * heightOut)
{
	//Attempt to open file.
	CPVRTResourceFile TexFile(filename);

	//Check file opened successfully.
	if (!TexFile.IsOpen()) 
	{
		return NULL;
	}

	//Header size.
	PVRTuint32 u32HeaderSize=0;

	//Boolean whether to byte swap the texture data or not.
	bool bSwapDataEndianness=false;

	//Texture header to check against.
	PVRTextureHeaderV3 sTextureHeader;

	//The channel type for endian swapping.
	EPVRTVariableType u32CurrentChannelType=ePVRTVarTypeUnsignedByte;

	//Check the first word of the file and see if it's equal to the current identifier (or reverse identifier)
	if(*(PVRTuint32*)TexFile.DataPtr()!=PVRTEX_CURR_IDENT && *(PVRTuint32*)TexFile.DataPtr()!=PVRTEX_CURR_IDENT_REV)
	{
		//Swap the header bytes if necessary.
		if(!PVRTIsLittleEndian())
		{
			bSwapDataEndianness=true;
			PVRTuint32 u32HeaderSize=PVRTByteSwap32(*(PVRTuint32*)TexFile.DataPtr());

			for (PVRTuint32 i=0; i<u32HeaderSize; ++i)
			{
				PVRTByteSwap( (PVRTuint8*)( ( (PVRTuint32*)TexFile.DataPtr() )+i),sizeof(PVRTuint32) );
			}
		}

		//Get a pointer to the header.
		PVR_Texture_Header* sLegacyTextureHeader=(PVR_Texture_Header*)TexFile.DataPtr();

		//Set the header size.
		u32HeaderSize=sLegacyTextureHeader->dwHeaderSize;

		//We only really need the channel type.
		PVRTuint64 tempFormat;
		EPVRTColourSpace tempColourSpace;
		bool tempIsPreMult;

		//Map the enum to get the channel type.
		PVRTMapLegacyTextureEnumToNewFormat( (PVRTPixelType)( sLegacyTextureHeader->dwpfFlags&0xff),tempFormat,tempColourSpace, u32CurrentChannelType, tempIsPreMult);
	}
	// If the header file has a reverse identifier, then we need to swap endianness
	else if(*(PVRTuint32*)TexFile.DataPtr()==PVRTEX_CURR_IDENT_REV)
	{
		//Setup the texture header
		sTextureHeader=*(PVRTextureHeaderV3*)TexFile.DataPtr();

		bSwapDataEndianness=true;
		PVRTextureHeaderV3* pTextureHeader=(PVRTextureHeaderV3*)TexFile.DataPtr();

		pTextureHeader->u32ChannelType=PVRTByteSwap32(pTextureHeader->u32ChannelType);
		pTextureHeader->u32ColourSpace=PVRTByteSwap32(pTextureHeader->u32ColourSpace);
		pTextureHeader->u32Depth=PVRTByteSwap32(pTextureHeader->u32Depth);
		pTextureHeader->u32Flags=PVRTByteSwap32(pTextureHeader->u32Flags);
		pTextureHeader->u32Height=PVRTByteSwap32(pTextureHeader->u32Height);
		pTextureHeader->u32MetaDataSize=PVRTByteSwap32(pTextureHeader->u32MetaDataSize);
		pTextureHeader->u32MIPMapCount=PVRTByteSwap32(pTextureHeader->u32MIPMapCount);
		pTextureHeader->u32NumFaces=PVRTByteSwap32(pTextureHeader->u32NumFaces);
		pTextureHeader->u32NumSurfaces=PVRTByteSwap32(pTextureHeader->u32NumSurfaces);
		pTextureHeader->u32Version=PVRTByteSwap32(pTextureHeader->u32Version);
		pTextureHeader->u32Width=PVRTByteSwap32(pTextureHeader->u32Width);
		PVRTByteSwap((PVRTuint8*)&pTextureHeader->u64PixelFormat,sizeof(PVRTuint64));

		//Channel type.
		u32CurrentChannelType=(EPVRTVariableType)pTextureHeader->u32ChannelType;

		//Header size.
		u32HeaderSize=PVRTEX3_HEADERSIZE+sTextureHeader.u32MetaDataSize;
	}
	else
	{
		//Header size.
		u32HeaderSize=PVRTEX3_HEADERSIZE+sTextureHeader.u32MetaDataSize;
	}

	// Convert the data if needed
	if(bSwapDataEndianness)
	{
		//Get the size of the variables types.
		PVRTuint32 ui32VariableSize=0;
		switch(u32CurrentChannelType)
		{
		case ePVRTVarTypeFloat:
		case ePVRTVarTypeUnsignedInteger:
		case ePVRTVarTypeUnsignedIntegerNorm:
		case ePVRTVarTypeSignedInteger:
		case ePVRTVarTypeSignedIntegerNorm:
			{
				ui32VariableSize=4;
				break;
			}
		case ePVRTVarTypeUnsignedShort:
		case ePVRTVarTypeUnsignedShortNorm:
		case ePVRTVarTypeSignedShort:
		case ePVRTVarTypeSignedShortNorm:
			{
				ui32VariableSize=2;
				break;
			}
		case ePVRTVarTypeUnsignedByte:
		case ePVRTVarTypeUnsignedByteNorm:
		case ePVRTVarTypeSignedByte:
		case ePVRTVarTypeSignedByteNorm:
			{
				ui32VariableSize=1;
				break;
			}
		default:
			break;
		}

		//If the size of the variable type is greater than 1, then we need to byte swap.
		if (ui32VariableSize>1)
		{
			//Get the texture data.
			PVRTuint8* pu8OrigData = ( (PVRTuint8*)TexFile.DataPtr() + u32HeaderSize);

			//Get the size of the texture data.
			PVRTuint32 ui32TextureDataSize = PVRTGetTextureDataSize(sTextureHeader);

			//Loop through and byte swap all the data. It's swapped in place so no need to do anything special.
			for(PVRTuint32 i = 0; i < ui32TextureDataSize; i+=ui32VariableSize)
			{
				PVRTByteSwap(pu8OrigData+i,ui32VariableSize);
			}
		}
	}


	return loadPvrTextureFromPointer(type, TexFile.DataPtr(), widthOut, heightOut);
}

unsigned int nearestPow2(unsigned int num)
{
	unsigned int n = num > 0 ? num - 1 : 0;

	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	n++;

	return n;
}

unsigned char * decompressImagePvrtc(unsigned char * data, int bpp, unsigned char * alphaData, int alphaBpp, int expandedwidth, int expandedheight)
{
	unsigned char * pDecompressedData = (unsigned char *)malloc(expandedwidth * expandedheight * 4);
	if (!pDecompressedData) return NULL;


	int ret = PVRTDecompressPVRTC(data,bpp==2?1:0,expandedwidth,expandedheight,pDecompressedData);

	if (alphaData)
	{
		unsigned char * pDecompressedAlphaData = (unsigned char *)malloc(expandedwidth * expandedheight * 4);
		if (!pDecompressedAlphaData) 
		{
			free(pDecompressedData);
			return NULL;
		}

		int ret = PVRTDecompressPVRTC(alphaData,alphaBpp==2?1:0,expandedwidth,expandedheight,pDecompressedAlphaData);


		unsigned char * dstPtr = pDecompressedData;
		unsigned char * srcPtr = pDecompressedAlphaData;

		for (unsigned int i = 0; i < expandedwidth * expandedheight; ++i)
		{
			dstPtr[3] = srcPtr[0];
			dstPtr += 4;
			srcPtr += 4;
		}

		free(pDecompressedAlphaData);
	}

	return pDecompressedData;		
}



unsigned char * LoadPvrtcImage(Texture::Format * type, const char * filename, int * widthOut, int * heightOut)
{
	FILE * file = 0;
	*widthOut = 0;
	*heightOut = 0;
	unsigned char * tempBuffer = NULL, * tempAlphaBuffer = NULL;
	BOOST_SCOPE_EXIT( (&file) (&tempBuffer) (&tempAlphaBuffer)) {
		if (file) fclose(file);		
		if (tempBuffer) free(tempBuffer);
		if (tempAlphaBuffer) free(tempAlphaBuffer);
	} BOOST_SCOPE_EXIT_END

	file = fopen(filename, "rb");

	unsigned int magicNumber;
	unsigned int version;	

	fread(&magicNumber, 1, sizeof(unsigned int), file);
	if (magicNumber != cPvrtcMagicNumber) return NULL;

	fread(&version, 1, sizeof(unsigned int), file);
	bool isAlpha = false;
	
	if (version == 2) fread(&isAlpha, 1, 1, file);

	unsigned char * data = NULL;
	unsigned int origWidth, origHeight;
	unsigned int rgbDataSize, alphaDataSize;

	fread(&origWidth, 1, sizeof(unsigned int), file);
	fread(&origHeight, 1, sizeof(unsigned int), file);

	int expandedwidth = nearestPow2(origWidth);
	int expandedheight = nearestPow2(origHeight);
	if (expandedwidth > expandedheight)expandedheight = expandedwidth;
	else expandedwidth = expandedheight;	

	
	*widthOut = origWidth;
	*heightOut = origHeight;

	unsigned char bpp = 4, alphaBpp = 4;
	if (!isAlpha)
	{
		*type = Texture::CompressionPvrtc;

		rgbDataSize = Texture::getTextureDataSize(*type, origWidth, origHeight);
		data = (unsigned char *)malloc(1 + rgbDataSize);
		if (!data) return NULL;

		unsigned int comprSize;
		
		fread(&comprSize, 1, sizeof(unsigned int), file);
		if (version == 2) fread(&bpp, 1, 1, file);
		tempBuffer = (unsigned char *)malloc(comprSize);

		fread(tempBuffer, 1, comprSize, file);
		unsigned int decSize = decompress2Stage(data + 1, tempBuffer);

		free(tempBuffer); tempBuffer = NULL;
	}
	else
	{
		unsigned int uw = nearestPow2(origWidth);
		unsigned int uh = nearestPow2(origHeight);
		if (uw > uh) uh = uw; else uw = uh;		

		

		unsigned int comprSize, alphaComprSize;
		fread(&comprSize, 1, sizeof(unsigned int), file);
		fread(&bpp, 1, 1, file);
		fread(&alphaComprSize, 1, sizeof(unsigned int), file);
		fread(&alphaBpp, 1, 1, file);

		if (alphaBpp == 4)
			*type = Texture::CompressionPvrtcAlpha4Bpp;
		else
			*type = Texture::CompressionPvrtcAlpha2Bpp;

		rgbDataSize = uw * uh * 4 / 8;
		if (rgbDataSize < 32) rgbDataSize = 32;
		alphaDataSize = uw * uh * alphaBpp / 8;
		if (alphaDataSize < 32) alphaDataSize = 32;

		data = (unsigned char *)malloc(1 + rgbDataSize + alphaDataSize);
		if (!data) return NULL;

		tempBuffer = (unsigned char *)malloc(comprSize);
		tempAlphaBuffer = (unsigned char *)malloc(alphaComprSize);

		fread(tempBuffer, 1, comprSize, file);
		unsigned int decSize = decompress2Stage(data + 1, tempBuffer);

		fread(tempAlphaBuffer, 1, alphaComprSize, file);
		decompress2Stage(data + 1 + rgbDataSize, tempAlphaBuffer);

		free(tempBuffer); tempBuffer = NULL;
		free(tempAlphaBuffer); tempAlphaBuffer = NULL;
	}	

#ifndef IOS

	*type = Texture::UncompressedRgba32;
	unsigned char * ret = decompressImagePvrtc(data + 1, bpp, isAlpha?data + 1 + rgbDataSize:NULL, alphaBpp, expandedwidth, expandedheight);
	free(data);
	unsigned char * dstPtr = ret;
	unsigned char * srcPtr = ret;
	for (unsigned int y = 0; y < origHeight; ++y)
	{
		memmove(dstPtr, srcPtr,  4*origWidth);

		dstPtr += 4*origWidth;
		srcPtr += 4*expandedwidth;		
	}

	ret = (unsigned char *)realloc(ret, origWidth * origHeight * 4);
	return ret;	

#endif
	data[0] = false;
	return data;
}


//-----------------------------------------------------------------------------

void doReadImage(const std::string & fileNameStr, 
	Texture::Format * type, unsigned char ** bits, int * width, int * height)
{

	const char * filename = fileNameStr.c_str();
	struct stat buffer;
	if ( stat( filename, &buffer ) ) 
	{
		//file doesn't exist
		*bits = (unsigned char *)malloc(1);
		*width = 0;
		*height = 0;
		return;
	}	
	*type = Texture::UncompressedRgba32;
	*bits = NULL;
	char ext[10] = {0,};

	int i = 0, j = 0;
	int len = 0;

	if(filename == NULL) return;

	len = strlen(filename);
	if(!len) return;

	for(i=len-1; i>=0; i--)
	{
		if(filename[i] == '.') 
		{
			strcpy(ext, &filename[i+1]);
			break;
		}
	}

	len = strlen(ext);

	if(!len) return;
	for(i=0; i<len; i++)
	{
		if(ext[i] >='a' && ext[i] <= 'z') ext[i] = 'A' + (ext[i] - 'a');
	}

	if(!strcmp(ext, "JPG"))
		*bits = (unsigned char *)LibJPEGDecodeImage(filename, width, height);
	else if(!strcmp(ext, "PNG"))
		*bits = (unsigned char *)LibPNGDecodeImage(filename, width, height);
	else if (!strcmp(ext, "ETC1"))			
		*bits = (unsigned char *)LoadEtc1Image(type, filename, width, height);	
	else if (!strcmp(ext, "PVRTC"))
		*bits = (unsigned char*)LoadPvrtcImage(type, filename, width, height);
	else if (!strcmp(ext, "PVR"))
		*bits = (unsigned char*)LoadPvrImage(type, filename, width, height);
	else
		LOGE("_DecodeImage IMAGE_TYPE_UNKNOWN file = %s", filename);
		
}

