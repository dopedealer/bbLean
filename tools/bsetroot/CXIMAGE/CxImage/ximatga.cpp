/*
 * File:	ximatga.cpp
 * Purpose:	Platform Independent TGA Image Class Loader and Writer
 * 05/Jan/2001 <ing.davide.pizzolato@libero.it>
 * CxImage version 5.50 07/Jan/2003
 */

#include "ximatga.h"

#if CXIMAGE_SUPPORT_TGA

#include "ximaiter.h"

// Definitions for image types.
#define TGA_Null 0
#define TGA_Map 1
#define TGA_RGB 2
#define TGA_Mono 3
#define TGA_RLEMap 9
#define TGA_RLERGB 10
#define TGA_RLEMono 11
#define TGA_CompMap 32
#define TGA_CompMap4 33

////////////////////////////////////////////////////////////////////////////////
bool CxImageTGA::Decode(CxFile *hFile)
{
	if (hFile == NULL) return false;

	TGAHEADER tgaHead;

  try
  {
	if (hFile->Read(&tgaHead,sizeof(tgaHead),1)==0)
		throw "Not a TGA";

	bool bCompressed;
	switch (tgaHead.ImageType){
	case TGA_Map:
	case TGA_RGB:
	case TGA_Mono:
		bCompressed = false;
		break;
	case TGA_RLEMap:
	case TGA_RLERGB:
	case TGA_RLEMono:
		bCompressed = true;
		break;
	default:
		throw "Unknown TGA image type";
	}

	if (tgaHead.ImageWidth==0 || tgaHead.ImageHeight==0 || tgaHead.PixelDepth==0 || tgaHead.CmapLength>256)
		throw "bad TGA header";

	if (tgaHead.IdLength>0) hFile->Seek(tgaHead.IdLength,SEEK_CUR); //skip descriptor

	Create(tgaHead.ImageWidth, tgaHead.ImageHeight, tgaHead.PixelDepth, CXIMAGE_FORMAT_TGA);
#if CXIMAGE_SUPPORT_ALPHA	// <vho>
	if (tgaHead.PixelDepth==32) AlphaCreate(); // Image has alpha channel
#endif //CXIMAGE_SUPPORT_ALPHA

	if (!IsValid()) throw "TGA Create failed";
	
	if (info.nEscape) throw "Cancelled"; // <vho> - cancel decoding

	if (tgaHead.CmapType != 0){ // read the palette
		rgb_color pal[256];
		hFile->Read(pal,tgaHead.CmapLength*sizeof(rgb_color), 1);
		for (int i=0;i<tgaHead.CmapLength; i++) SetPaletteIndex((BYTE)i,pal[i].b,pal[i].g,pal[i].r);
	}

	if (tgaHead.ImageType == TGA_Mono || tgaHead.ImageType == TGA_RLEMono)
		SetGrayPalette();

	// Bits 4 & 5 of the Image Descriptor byte control the ordering of the pixels.
	bool bXReversed = ((tgaHead.ImagDesc & 16) == 16);
	bool bYReversed = ((tgaHead.ImagDesc & 32) == 32);

    CImageIterator iter(this);
	BYTE* pDest;
    for (int y=0; y < tgaHead.ImageHeight; y++){

		if (info.nEscape) throw "Cancelled"; // <vho> - cancel decoding

		if (hFile == NULL || hFile->Eof()) throw "corrupted TGA";

		if (bYReversed) pDest = iter.GetRow(tgaHead.ImageHeight-y-1);
		else pDest = iter.GetRow(y);

		if (bCompressed) ExpandCompressedLine(pDest,&tgaHead,hFile,tgaHead.ImageWidth,y);
		else ExpandUncompressedLine  (pDest,&tgaHead,hFile,tgaHead.ImageWidth,y,0);
    }

	if (bXReversed) Mirror();

  } catch (const char *message) {
	strncpy(info.szLastError,message,255);
	return FALSE;
  }
    return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CxImageTGA::Encode(CxFile * hFile)
{
	if (hFile==NULL || pDib==NULL) return false;

	if (head.biBitCount<8){
		strcpy(info.szLastError,"Bit depth must be 8 or 24");
		return false;
	}

	TGAHEADER tgaHead;

    tgaHead.IdLength = 0;				// Image ID Field Length
    tgaHead.CmapType = GetPalette()!=0; // Color Map Type
    tgaHead.ImageType = (head.biBitCount == 8) ? (BYTE)TGA_Map : (BYTE)TGA_RGB; // Image Type

    tgaHead.CmapIndex=0;				// First Entry Index
    tgaHead.CmapLength=(head.biBitCount == 8) ? 256 : 0;   // Color Map Length
    tgaHead.CmapEntrySize=(head.biBitCount == 8) ? (BYTE)24 : (BYTE)0; // Color Map Entry Size

    tgaHead.X_Origin=0;					// X-origin of Image
    tgaHead.Y_Origin=0;					// Y-origin of Image
    tgaHead.ImageWidth=(WORD)head.biWidth;		// Image Width
    tgaHead.ImageHeight=(WORD)head.biHeight;	// Image Height
    tgaHead.PixelDepth=(BYTE)head.biBitCount;	// Pixel Depth
    tgaHead.ImagDesc=0;					// Image Descriptor

	if (pAlpha && head.biBitCount==24) tgaHead.PixelDepth=32; 

	hFile->Write(&tgaHead,sizeof(TGAHEADER),1);

	if (head.biBitCount==8){
		rgb_color pal[256];
		RGBQUAD* ppal = GetPalette();
		for (int i=0;i<256; i++){
			pal[i].r = ppal[i].rgbBlue;
			pal[i].g = ppal[i].rgbGreen;
			pal[i].b = ppal[i].rgbRed;
		}
		hFile->Write(&pal,256*sizeof(rgb_color),1);
	}
	
	CImageIterator iter(this);
	BYTE* pDest;
	if (pAlpha==0 || head.biBitCount==8){
		for (int y=0; y < tgaHead.ImageHeight; y++){
			pDest = iter.GetRow(y);
			hFile->Write(pDest,tgaHead.ImageWidth * (head.biBitCount >> 3),1);
		}
	} else {
		pDest = (BYTE*)malloc(4*tgaHead.ImageWidth);
		RGBQUAD c;
		for (int y=0; y < tgaHead.ImageHeight; y++){
			for(int x=0, x4=0;x<tgaHead.ImageWidth;x++, x4+=4){
				c=GetPixelColor(x,y);
				pDest[x4+0]=c.rgbBlue;
				pDest[x4+1]=c.rgbGreen;
				pDest[x4+2]=c.rgbRed;
#if CXIMAGE_SUPPORT_ALPHA	// <vho>
				pDest[x4+3]=(BYTE)((AlphaGet(x,y)*info.nAlphaMax)/255);
#else
				pDest[x4+3]=0;
#endif //CXIMAGE_SUPPORT_ALPHA
			}
			hFile->Write(pDest,4*tgaHead.ImageWidth,1);
		}
		free(pDest);
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
void CxImageTGA::ExpandCompressedLine(BYTE* pDest,TGAHEADER* ptgaHead,CxFile *hFile,int width, int y)
{
	try{
	BYTE rle;
	for (int x=0; x<width; ){
		hFile->Read(&rle,1,1);
		if (rle & 128) { // RLE-Encoded packet
			rle -= 127; // Calculate real repeat count.
			if ((x+rle)>width) rle=width-x; 
			switch (ptgaHead->PixelDepth)
			{
			case 32: {
				RGBQUAD color;
				hFile->Read(&color,4,1);
				for (int ix = 0; ix < rle; ix++){
					memcpy(&pDest[3*ix],&color,3);
#if CXIMAGE_SUPPORT_ALPHA	// <vho>
					AlphaSet(ix+x,y,color.rgbReserved);
#endif //CXIMAGE_SUPPORT_ALPHA
				}
				break;
					 } 
			case 24: {
				rgb_color triple;
				hFile->Read(&triple,3,1);
				for (int ix = 0; ix < rle; ix++) memcpy(&pDest[3*ix],&triple,3);
				break;
					 }
			case 15:
			case 16: {
				WORD pixel;
				hFile->Read(&pixel,2,1);
				rgb_color triple;
				triple.r = (BYTE)(( pixel & 0x1F ) * 8);     // red
				triple.g = (BYTE)(( pixel >> 2 ) & 0x0F8);   // green
				triple.b = (BYTE)(( pixel >> 7 ) & 0x0F8);   // blue
				for (int ix = 0; ix < rle; ix++){
					memcpy(&pDest[3*ix],&triple,3);
				}
				break;
					 }
			case 8: {
				BYTE pixel;
				hFile->Read(&pixel,1,1);
				for (int ix = 0; ix < rle; ix++) pDest[ix] = pixel;
					}
			}
		} else { // Raw packet
			rle += 1; // Calculate real repeat count.
			ExpandUncompressedLine(pDest,ptgaHead,hFile,rle,y,x);
		}
		if (head.biBitCount == 24)	pDest += rle*3;	else pDest += rle;
		x += rle;
	}
	} catch(...){	}
}
////////////////////////////////////////////////////////////////////////////////
void CxImageTGA::ExpandUncompressedLine(BYTE* pDest,TGAHEADER* ptgaHead,CxFile *hFile,int width, int y, int xoffset)
{
	try{
	switch (ptgaHead->PixelDepth){
	case 8:
		hFile->Read(pDest,width,1);
		break;
	case 15:
	case 16:{
		BYTE* dst=pDest;
		WORD pixel;
		for (int x=0; x<width; x++){
			hFile->Read(&pixel,2,1);
			*dst++ = (BYTE)(( pixel & 0x1F ) * 8);     // blue
			*dst++ = (BYTE)(( pixel >> 2 ) & 0x0F8);   // green
			*dst++ = (BYTE)(( pixel >> 7 ) & 0x0F8);   // red
		}
		break;
			}
	case 24:
		hFile->Read(pDest,3*width,1);
		break;
	case 32:{
		BYTE* dst=pDest;
		for (int x=0; x<width; x++){
			RGBQUAD pixel;
			hFile->Read(&pixel,4,1);
			*dst++ = pixel.rgbBlue;
			*dst++ = pixel.rgbGreen;
			*dst++ = pixel.rgbRed;
#if CXIMAGE_SUPPORT_ALPHA	// <vho>
			AlphaSet(x+xoffset,y,pixel.rgbReserved); //alpha
#endif //CXIMAGE_SUPPORT_ALPHA
		}
		break;
			}
	}
	} catch(...){	}
}
////////////////////////////////////////////////////////////////////////////////
#endif 	// CXIMAGE_SUPPORT_TGA

