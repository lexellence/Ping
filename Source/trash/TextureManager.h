/**************************************************************************************\
** File: TextureManager.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the TextureManager class
**
\**************************************************************************************/
#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H
#include "TextureResource.h"
#include <SDL2/SDL_opengl.h>
#include <vector>
#include <stack>
#include <map>

class TextureManager
{
public:
	TextureManager();
	~TextureManager();

	unsigned int Load(const std::string& filename);
	void Unload(unsigned int index);
	void glBind(unsigned int index);

private:
	std::stack<unsigned int> m_availableIndexList;
	std::vector<TextureResource*> m_texturePtrList;
	std::map<std::string, unsigned int> m_indexMap;
};

#endif //TEXTUREMANAGER_H