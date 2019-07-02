/*
 *  LibPNGDecoder.cpp
 *  MMagazinIPAD
 *
 *  Created by jkpark on 11. 3. 16..
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include "stdafx.h"

#define PNG_NO_SETJMP
#define PNG_SETJMP_NOT_SUPPORTED
#define PNG_NO_SETJMP_SUPPORTED

#include "../libpng/png.h"
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>




typedef unsigned int u32;

typedef struct
{
	char *buffer;
	u32 pos;
	u32 size;
} GFpng;

static void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	GFpng *ctx = (GFpng*)png_ptr->io_ptr;
	
	if (ctx->pos + length > ctx->size) {
		png_error(png_ptr, "Read Error");
	} else {
		memcpy(data, (char*) ctx->buffer + ctx->pos, length);
		ctx->pos += length;
	}
}
static void user_error_fn(png_structp png_ptr,png_const_charp error_msg)
{
}


void *LibPNGDecodeImage(char * buff, int buff_len, int *width, int *height)
{
	*width = 0;
	*height = 0;

	GFpng udta;
	png_struct *png_ptr = 0;
	png_info *info_ptr = 0;
	png_byte **rows = 0;
	png_byte *scan_line = 0;
	u32 i = 0, stride = 0, bpp = 0;

	char *tmp = 0, *tmp_org = 0;

	unsigned char *pixels = 0;
	int pixels_len = 0, pixels_stride = 0, pixels_byte_per_pixel = 0, alpha_index = 3;
	int bytes_per_pixel = 0;
	int pixels_stride_ = 0;
	
	unsigned char *pixels_ = 0;
	unsigned char *tmp_pixels_ = 0;	

	unsigned char * tempBuffer = NULL;
	if ((buff_len<8) || png_sig_cmp((png_byte*)buff, 0, 8) ) 
	{
		goto exit;
	}

	udta.buffer = buff;
	udta.size = buff_len;
	udta.pos = 0;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp) &udta, NULL, NULL);
	if (!png_ptr) 
	{
		goto exit;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) 
	{
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		goto exit;
	}
	
    png_set_read_fn(png_ptr, &udta, (png_rw_ptr) user_read_data);
	png_set_error_fn(png_ptr, &udta, (png_error_ptr) user_error_fn, NULL);

	png_read_info(png_ptr, info_ptr);

	/*unpaletize*/
	if (info_ptr->color_type==PNG_COLOR_TYPE_PALETTE) 
	{
		png_set_expand(png_ptr);
		png_read_update_info(png_ptr, info_ptr);
	}
	if (info_ptr->num_trans) 
	{
		png_set_tRNS_to_alpha(png_ptr);
		png_read_update_info(png_ptr, info_ptr);
	}
	if(info_ptr->bit_depth == 16)
	{
		png_set_strip_16(png_ptr);
		png_read_update_info(png_ptr, info_ptr);
	}

	*width = info_ptr->width;
	*height = info_ptr->height;

	bytes_per_pixel = (info_ptr->pixel_depth) >> 3;
	
	/*read*/
	stride = png_get_rowbytes(png_ptr, info_ptr);

	// Create RGBA buffer
	pixels_byte_per_pixel = 4;
	alpha_index = pixels_byte_per_pixel-1;
	pixels_stride = pixels_byte_per_pixel * (*width);
	pixels_len = pixels_stride * info_ptr->height;
	pixels = (unsigned char *)malloc(pixels_len);
	if(pixels == NULL) goto exit;
	
	png_set_interlace_handling(png_ptr);
	
	


	if (bytes_per_pixel == pixels_byte_per_pixel)
	{
		rows = (png_bytepp) malloc(sizeof(png_bytep) * info_ptr->height);

		for (i=0; i<info_ptr->height; i++) 
			rows[i] = pixels + i * stride;

		png_read_image(png_ptr, rows);
	}
	else
	{
		tempBuffer = (unsigned char *)malloc(bytes_per_pixel * (*width) * info_ptr->height);
		if (tempBuffer == NULL) goto exit;

		rows = (png_bytepp) malloc(sizeof(png_bytep) * info_ptr->height);

		for (i=0; i<info_ptr->height; i++) 
			rows[i] = tempBuffer + i * stride;

		png_read_image(png_ptr, rows);
		
		tmp = (char *)pixels;
		tmp_pixels_ = tempBuffer;
		for(i=0; i<info_ptr->height; i++)
		{
			if (bytes_per_pixel == 3)
			{
				unsigned char *tmp2 = (unsigned char *)tmp;
				int k = 0, m = 0;
				for(k=0; k < stride; k += bytes_per_pixel)
				{
					for(m=0; m<bytes_per_pixel; m++) {
						tmp2[m] = tmp_pixels_[k + m];
					}

					tmp2[alpha_index] = 0xff;
					tmp2 += pixels_byte_per_pixel;
				}
			}
			else if(bytes_per_pixel == 2)
			{
				unsigned char *tmp2 = (unsigned char *)tmp;
				unsigned short *short_tmp;
				unsigned short buf_value;
				unsigned char a,r,g,b;
				int k = 0, m = 0;
				for(k=0; k < stride; k += bytes_per_pixel)
				{
					short_tmp = (unsigned short*)&tmp_pixels_[k];
					buf_value = *short_tmp;
					buf_value = ((buf_value & 0xff00) >> 8) | ((buf_value & 0xff) << 8);

					r = g = b = (buf_value & 0xff00) >> 8;
					a = (buf_value & 0x00ff);

					tmp2[0] = r;
					tmp2[1] = g;
					tmp2[2] = b;
					tmp2[3] = a;

					//tmp2[alpha_index] = tmp_pixels_[k+1];
					tmp2 += pixels_byte_per_pixel;
					//LOGE("k = %04d, buf_value = 0x%04x, a = %d, r = %d, g = %d, b = %d", k, buf_value, a, r, g, b);
				}
			}

			tmp_pixels_ += stride;
			tmp += pixels_stride;
		}
	}
	
	png_read_end(png_ptr, NULL);
	
	if(rows) free(rows);
	if(scan_line) free(scan_line);
	if (tempBuffer) free(tempBuffer);

	png_destroy_info_struct(png_ptr,(png_infopp) & info_ptr);
	png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);

	
	return pixels;
exit:

	if(png_ptr)
	{
		png_destroy_info_struct(png_ptr,(png_infopp) & info_ptr);
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
	}
	
	if(pixels) free(pixels);
	
	return NULL;
}

void *LibPNGDecodeImage(const char *filename, int *width, int *height)
{
	*width = 0;
	*height = 0;

	FILE *fp = 0;

	char *buff = 0, *tmp = 0, *tmp_org = 0;
	int buff_len = 0;

	unsigned char *pixels = 0;
	int pixels_len = 0, pixels_stride = 0, pixels_byte_per_pixel = 0, alpha_index = 3;
	int bytes_per_pixel = 0;
	int pixels_stride_ = 0;

	unsigned char *pixels_ = 0;
	unsigned char *tmp_pixels_ = 0;

	unsigned char * tempBuffer = NULL;
	fp = fopen(filename, "rb");
	if(!fp) 
	{
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	buff_len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if(buff_len <= 0) 
	{
		fclose(fp);
		return NULL;
	}

	buff = (char *)malloc(buff_len);
	if(buff == NULL)
	{
		fclose(fp);
		return NULL;
	}

	fread(buff, 1, buff_len, fp);

	void * ret = LibPNGDecodeImage(buff, buff_len, width, height);

	fclose(fp);
	free(buff);
	return ret;		
}