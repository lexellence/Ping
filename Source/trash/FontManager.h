/**************************************************************************************\
** File: FontManager.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the FontManager class
**
\**************************************************************************************/
#ifndef FONTMANAGER_H
#define FONTMANAGER_H
#include "FontResource.h"
#include "drawtext.h"
#include <SDL2/SDL_opengl.h>
#include <vector>
#include <stack>
#include <map>

class FontManager
{
public:
	FontManager();
	~FontManager();

	unsigned int Load(const std::string& filename, FontQuality quality);
	void Unload(unsigned int index);
	void dtxBind(unsigned int index);

private:
	std::stack<unsigned int> m_availableIndexList;
	std::vector<FontResource*> m_fontPtrList;
	std::map< std::string, std::map<FontQuality,unsigned int> > m_indexMap;
};
#endif //FONTMANAGER_H