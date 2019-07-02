#include "stdafx.h"
#include "PdfImport.h"
#include "Utils.h"

extern "C" {
//#include "fitz.h"
}



PdfImport::PdfImport(float width, float height) : width_(width) , height_(height)
{
}

PdfImport::~PdfImport()
{
}

void PdfImport::convertPdf2Img(const std::string &fileName, const std::string & outDir)
{
	//fz_document *doc = NULL;

	//fz_context *ctx;

	//fz_var(doc);

	//ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
	//fz_set_aa_level(ctx, 8);

	//fz_try(ctx)
	//{
	//	doc = fz_open_document(ctx, const_cast<char*>(fileName.c_str()));

	//	int pageNumbers = fz_count_pages(doc);

	//	if (pageNumbers > 0)
	//	{
	//		// Create a progress dialog
	//		QProgressDialog progress(QObject::tr("Converting file"),
	//			QObject::tr("Cancel"), 0, pageNumbers, 0);
	//		progress.setWindowTitle(QObject::tr("Progress"));
	//		progress.setWindowModality(Qt::WindowModal);
	//		for(int i = 0 ; i < pageNumbers ; i++)
	//		{
	//			progress.setValue(i);
	//			qApp->processEvents();
	//			if (progress.wasCanceled())
	//			{
	//				QMessageBox::information(
	//					0, 
	//					QObject::tr("Cancel"), 
	//					QObject::tr("Cancel"));
	//				return;
	//			}

	//			int temp = 10000 + i;
	//			std::stringstream outfilename;
	//			std::stringstream outfilename_zoom;
	//			outfilename << outDir << "/f_" << temp << ".png";
	//			outfilename_zoom << outDir << "/zoom/f_" << temp << ".png";


	//			fz_page *page;
	//			fz_device *dev = NULL;

	//			fz_var(dev);
	//			fz_try(ctx)
	//			{
	//				page = fz_load_page(doc, i);
	//			}
	//			fz_catch(ctx)
	//			{
	//				fz_throw(ctx, "cannot load page %d in file '%s'", i, fileName);
	//			}

	//			fz_matrix ctm1, ctm2;
	//			fz_rect bounds, bounds2, bounds3;
	//			fz_bbox bbox1, bbox2;
	//			fz_pixmap *pix = NULL;

	//			fz_var(pix);

	//			bounds = fz_bound_page(doc, page);
	//			float origWidth = bounds.x1 - bounds.x0;
	//			float origHeight = bounds.y1 - bounds.y0;
	//			double ratio = width_/origWidth;
	//			if (ratio > height_/origHeight)
	//				ratio = height_/origHeight;
	//			ctm1 = fz_scale(ratio, ratio);
	//			ctm2 = fz_scale(ratio * 2, ratio * 2);
	//			bounds2 = fz_transform_rect(ctm1, bounds);
	//			bounds3 = fz_transform_rect(ctm2, bounds);
	//			bbox1 = fz_round_rect(bounds2);
	//			bbox2 = fz_round_rect(bounds3);
	//			//bbox = fz_round_rect(bounds);
	//			fz_try(ctx)
	//			{
	//				fz_colorspace *colorspace = fz_device_rgb;
	//				//pix = fz_new_pixmap(ctx, colorspace, origWidth /** ratio*/, origHeight /** ratio*/);
	//				pix = fz_new_pixmap_with_bbox(ctx, colorspace, bbox1);

	//				// alpha
	//				//fz_clear_pixmap(ctx, pix);
	//				fz_clear_pixmap_with_value(ctx, pix, 255);

	//				dev = fz_new_draw_device(ctx, pix);

	//				fz_run_page(doc, page, dev, ctm1, NULL);

	//				fz_free_device(dev);
	//				dev = NULL;

	//				// alpha
	//				//fz_unmultiply_pixmap(ctx, pix);

	//				char buf[512];
	//				sprintf(buf, outfilename.str().c_str());

	//				fz_write_png(ctx, pix, buf, 1);
	//				fz_drop_pixmap(ctx, pix);


	//				//pix = fz_new_pixmap_with_bbox(ctx, colorspace, bbox2);

	//				//// alpha
	//				////fz_clear_pixmap(ctx, pix);
	//				//fz_clear_pixmap_with_value(ctx, pix, 255);

	//				//dev = fz_new_draw_device(ctx, pix);

	//				//fz_run_page(doc, page, dev, ctm2, NULL);

	//				//fz_free_device(dev);
	//				//dev = NULL;

	//				//// alpha
	//				////fz_unmultiply_pixmap(ctx, pix);
	//				//sprintf(buf, outfilename_zoom.str().c_str());

	//				//fz_write_png(ctx, pix, buf, 1);
	//				//fz_drop_pixmap(ctx, pix);
	//			}
	//			fz_catch(ctx)
	//			{
	//				fz_free_device(dev);
	//				fz_drop_pixmap(ctx, pix);
	//				fz_free_page(doc, page);
	//				fz_rethrow(ctx);
	//			}
	//			fz_free_page(doc, page);

	//			fz_flush_warnings(ctx);
	//		}
	//		progress.setValue(pageNumbers);
	//	}
	//	fz_close_document(doc);
	//	doc = NULL;
	//}
	//fz_catch(ctx)
	//{
	//	QMessageBox::information(
	//					0, 
	//					QObject::tr("Error"), 
	//					QObject::tr("cannot open document"));
	//	fz_close_document(doc);
	//}
	//fz_free_context(ctx);
}
