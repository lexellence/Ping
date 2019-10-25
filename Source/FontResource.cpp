/**************************************************************************************\
** File: FontResource.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the FontResource class
**
\**************************************************************************************/
#include "pch.h"
#include "FontResource.h"
#include "drawtext.h"

namespace d2d
{
	FontResource::FontResource(const std::string& filename)
		: m_dtxFontPtr{ dtx_open_font(filename.c_str(), m_dtxFontSize) }
	{
		if (m_dtxFontPtr == nullptr)
		{
			SetLoadSuccess(false);
			std::cerr << "Error: libdrawtext failed to open font " << filename << std::endl;
		}
		else
		{
			SetLoadSuccess(true);
		}
	}
	FontResource::~FontResource()
	{
		if (m_dtxFontPtr != nullptr)
		{
			dtx_close_font(m_dtxFontPtr);
		}
	}
	struct dtx_font * FontResource::GetDTXFontPtr() const
	{
		return m_dtxFontPtr;
	}
	int FontResource::GetDTXFontSize()
	{
		return m_dtxFontSize;
	}
}