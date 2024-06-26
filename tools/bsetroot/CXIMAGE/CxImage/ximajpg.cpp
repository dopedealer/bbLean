/*
 * File:    ximajpg.cpp
 * Purpose: Platform Independent JPEG Image Class Loader and Writer
 * 07/Aug/2001 <ing.davide.pizzolato@libero.it>
 * CxImage version 5.50 07/Jan/2003
 */
 
#include "ximajpg.h"

#if CXIMAGE_SUPPORT_JPG

#include "ximaiter.h"
         
#include <setjmp.h>

struct ima_error_mgr {
	struct jpeg_error_mgr pub;  /* "public" fields */
	jmp_buf setjmp_buffer;      /* for return to caller */
	char* buffer;               /* error message <CSC>*/
};
typedef ima_error_mgr *ima_error_ptr;

////////////////////////////////////////////////////////////////////////////////
// Here's the routine that will replace the standard error_exit method:
////////////////////////////////////////////////////////////////////////////////
static void
ima_jpeg_error_exit (j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	ima_error_ptr myerr = (ima_error_ptr) cinfo->err;
	/* Create the message */
	myerr->pub.format_message (cinfo, myerr->buffer);
	/* Send it to stderr, adding a newline */
	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}
////////////////////////////////////////////////////////////////////////////////
bool CxImageJPG::Decode(CxFile * hFile)
{
	CImageIterator iter(this);
	/* This struct contains the JPEG decompression parameters and pointers to
	* working space (which is allocated as needed by the JPEG library).
	*/
	struct jpeg_decompress_struct cinfo;
	/* We use our private extension JPEG error handler. <CSC> */
	struct ima_error_mgr jerr;
	jerr.buffer=info.szLastError;
	/* More stuff */
	JSAMPARRAY buffer;  /* Output row buffer */
	int row_stride;     /* physical row width in output buffer */

	/* In this example we want to open the input file before doing anything else,
	* so that the setjmp() error recovery below can assume the file is open.
	* VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	* requires it in order to read binary files.
	*/

	/* Step 1: allocate and initialize JPEG decompression object */
	/* We set up the normal JPEG error routines, then override error_exit. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = ima_jpeg_error_exit;

	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		* We need to clean up the JPEG object, close the input file, and return.
		*/
		jpeg_destroy_decompress(&cinfo);
		return 0;
	}
	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */
	//jpeg_stdio_src(&cinfo, infile);
	CxFileJpg src(hFile);
    cinfo.src = &src;

	/* Step 3: read file parameters with jpeg_read_header() */
	(void) jpeg_read_header(&cinfo, TRUE);

//<DP>: Load true color images as RGB (no quantize) 
/* Step 4: set parameters for decompression */
/*  if (cinfo.jpeg_color_space!=JCS_GRAYSCALE) {
 *  cinfo.quantize_colors = TRUE;
 *  cinfo.desired_number_of_colors = 128;
 *}
 */ //</DP>

	/* Step 5: Start decompressor */
	jpeg_start_decompress(&cinfo);

	/* We may need to do some setup of our own at this point before reading
	* the data.  After jpeg_start_decompress() we have the correct scaled
	* output image dimensions available, as well as the output colormap
	* if we asked for color quantization.
	*/
	Create(cinfo.image_width, cinfo.image_height, 8*cinfo.num_components, CXIMAGE_FORMAT_JPG);

	if (cinfo.density_unit==2){
		SetXDPI((254*cinfo.X_density)/100);
		SetYDPI((254*cinfo.Y_density)/100);
	} else {
		SetXDPI(cinfo.X_density);
		SetYDPI(cinfo.Y_density);
	}

	if (cinfo.jpeg_color_space==JCS_GRAYSCALE){
		SetGrayPalette();
		head.biClrUsed =256;
	} else {
		if (cinfo.quantize_colors==TRUE){
			SetPalette(cinfo.actual_number_of_colors, cinfo.colormap[0], cinfo.colormap[1], cinfo.colormap[2]);
			head.biClrUsed=cinfo.actual_number_of_colors;
		} else {
			head.biClrUsed=0;
		}
	}

	/* JSAMPLEs per row in output buffer */
	row_stride = cinfo.output_width * cinfo.num_components;

	/* Make a one-row-high sample array that will go away when done with image */
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */
	/* Here we use the library's state variable cinfo.output_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	*/
	iter.Upset();
	while (cinfo.output_scanline < cinfo.output_height) {

		if (info.nEscape) longjmp(jerr.setjmp_buffer, 1); // <vho> - cancel decoding
		
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		// info.nProgress = (long)(100*cinfo.output_scanline/cinfo.output_height);
		//<DP> Step 6a: CMYK->RGB */ 
		if ((cinfo.num_components==4)&&(cinfo.quantize_colors==FALSE)){
			BYTE k,*dst,*src;
			dst=iter.GetRow();
			src=buffer[0];
			for(long x3=0,x4=0; x3<(long)info.dwEffWidth && x4<row_stride; x3+=3, x4+=4){
				k=src[x4+3];
				dst[x3]  =(BYTE)((k * src[x4+2])/255);
				dst[x3+1]=(BYTE)((k * src[x4+1])/255);
				dst[x3+2]=(BYTE)((k * src[x4+0])/255);
			}
		} else {
			/* Assume put_scanline_someplace wants a pointer and sample count. */
			iter.SetRow(buffer[0], row_stride);
		}
			iter.PrevRow();
	}

	/* Step 7: Finish decompression */
	(void) jpeg_finish_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	* with the stdio data source.
	*/

	//<DP> Step 7A: Swap red and blue components */ 
	if ((cinfo.num_components==3)&&(cinfo.quantize_colors==FALSE)){
		BYTE *r,*b,t,*r0;
		long x,y;
		r0=GetBits();
		for(y=0;y<head.biHeight;y++){

			if (info.nEscape) longjmp(jerr.setjmp_buffer, 1); // <vho> - cancel decoding

			r=r0;  b=r+2;
			for(x=0;x<head.biWidth;x++){
				t=*r; *r=*b; *b=t; r+=3; b+=3;
			}
			r0+=info.dwEffWidth;
		}
	} //</DP>

#if CXIMAGEJPG_SUPPORT_EXIF
	CxExifInfo info(&m_exifinfo);
	info.DecodeInfo(cinfo);
#endif //CXIMAGEJPG_SUPPORT_EXIF

	/* Step 8: Release JPEG decompression object */
	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(&cinfo);

	/* At this point you may want to check to see whether any corrupt-data
	* warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	*/

	/* And we're done! */
	return 1;
}
////////////////////////////////////////////////////////////////////////////////
bool CxImageJPG::Encode(CxFile * hFile)
{
	if (head.biClrUsed!=0 && !IsGrayScale()){
		strcpy(info.szLastError,"JPEG can save only RGB or GreyScale images");
		return false;
	}   

	/* This struct contains the JPEG compression parameters and pointers to
	* working space (which is allocated as needed by the JPEG library).
	* It is possible to have several such structures, representing multiple
	* compression/decompression processes, in existence at once.  We refer
	* to any one struct (and its associated working data) as a "JPEG object".
	*/
	struct jpeg_compress_struct cinfo;
	/* This struct represents a JPEG error handler.  It is declared separately
	* because applications often want to supply a specialized error handler
	* (see the second half of this file for an example).  But here we just
	* take the easy way out and use the standard error handler, which will
	* print a message on stderr and call exit() if compression fails.
	* Note that this struct must live as long as the main JPEG parameter
	* struct, to avoid dangling-pointer problems.
	*/
	//struct jpeg_error_mgr jerr;
	/* We use our private extension JPEG error handler. <CSC> */
	struct ima_error_mgr jerr;
	jerr.buffer=info.szLastError;
	/* More stuff */
	int row_stride;     /* physical row width in image buffer */
	JSAMPARRAY buffer;      /* Output row buffer */

	/* Step 1: allocate and initialize JPEG compression object */
	/* We have to set up the error handler first, in case the initialization
	* step fails.  (Unlikely, but it could happen if you are out of memory.)
	* This routine fills in the contents of struct jerr, and returns jerr's
	* address which we place into the link field in cinfo.
	*/
	//cinfo.err = jpeg_std_error(&jerr); <CSC>
	/* We set up the normal JPEG error routines, then override error_exit. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = ima_jpeg_error_exit;

	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		* We need to clean up the JPEG object, close the input file, and return.
		*/
		strcpy(info.szLastError, jerr.buffer); //<CSC>
		jpeg_destroy_compress(&cinfo);
		return 0;
	}
	
	/* Now we can initialize the JPEG compression object. */
	jpeg_create_compress(&cinfo);
	/* Step 2: specify data destination (eg, a file) */
	/* Note: steps 2 and 3 can be done in either order. */
	/* Here we use the library-supplied code to send compressed data to a
	* stdio stream.  You can also write your own code to do something else.
	* VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	* requires it in order to write binary files.
	*/

	//jpeg_stdio_dest(&cinfo, outfile);
	CxFileJpg dest(hFile);
    cinfo.dest = &dest;

	/* Step 3: set parameters for compression */
	/* First we supply a description of the input image.
	* Four fields of the cinfo struct must be filled in:
	*/
	cinfo.image_width = GetWidth();     // image width and height, in pixels
	cinfo.image_height = GetHeight();

	if (IsGrayScale()){
		cinfo.input_components = 1;         // # of color components per pixel
		cinfo.in_color_space = JCS_GRAYSCALE; /* colorspace of input image */
	} else {
		cinfo.input_components = 3;     // # of color components per pixel
		cinfo.in_color_space = JCS_RGB; /* colorspace of input image */
	}

	/* Now use the library's routine to set default compression parameters.
	* (You must set at least cinfo.in_color_space before calling this,
	* since the defaults depend on the source color space.)
	*/
	jpeg_set_defaults(&cinfo);
	/* Now you can set any non-default parameters you wish to.
	* Here we just illustrate the use of quality (quantization table) scaling:
	*/
	jpeg_set_quality(&cinfo, info.nQuality, TRUE /* limit to baseline-JPEG values */);

	cinfo.density_unit=1;
	cinfo.X_density=(unsigned short)GetXDPI();
	cinfo.Y_density=(unsigned short)GetYDPI();

	/* Step 4: Start compressor */
	/* TRUE ensures that we will write a complete interchange-JPEG file.
	* Pass TRUE unless you are very sure of what you're doing.
	*/
	jpeg_start_compress(&cinfo, TRUE);

	/* Step 5: while (scan lines remain to be written) */
	/*           jpeg_write_scanlines(...); */
	/* Here we use the library's state variable cinfo.next_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	* To keep things simple, we pass one scanline per call; you can pass
	* more if you wish, though.
	*/
	row_stride = info.dwEffWidth;   /* JSAMPLEs per row in image_buffer */

	//<DP> "8+row_stride" fix heap deallocation problem during debug???
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, 8+row_stride, 1);

	CImageIterator iter(this);

	iter.Upset();
	while (cinfo.next_scanline < cinfo.image_height) {
		// info.nProgress = (long)(100*cinfo.next_scanline/cinfo.image_height);
		iter.GetRow(buffer[0], row_stride);
		if (head.biClrUsed==0){             // swap R & B for RGB images
			RGBtoBGR(buffer[0], row_stride);    // Lance : 1998/09/01 : Bug ID: EXP-2.1.1-9
		}
		iter.PrevRow();
		(void) jpeg_write_scanlines(&cinfo, buffer, 1);
	}

	/* Step 6: Finish compression */
	jpeg_finish_compress(&cinfo);

	/* Step 7: release JPEG compression object */
	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_compress(&cinfo);

	/* And we're done! */
	return true;
}
////////////////////////////////////////////////////////////////////////////////
#endif // CXIMAGE_SUPPORT_JPG

