/**************************************************************************************\
** File: FontResource.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the FontResource class
**
\**************************************************************************************/
#ifndef FONTRESOURCE_H
#define FONTRESOURCE_H
#include "Resource.h"
#include "drawtext.h"

namespace d2d
{
	class FontResource : public Resource
	{
	public:
		FontResource(const std::string& filename);
		~FontResource();
		struct dtx_font * GetDTXFontPtr() const;
		static int GetDTXFontSize();

	private:
		struct dtx_font *m_dtxFontPtr{};
		static const int m_dtxFontSize{ 192 };
	};
}
#endif //FONTRESOURCE_H