/**************************************************************************************\
** File: TextureManager.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the TextureManager class
**
\**************************************************************************************/
#include "TextureManager.h"
#include "TextureResource.h"
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL.h>
#include <vector>
#include <stack>
#include <map>
#include <iostream>

TextureManager::TextureManager()
{

}
TextureManager::~TextureManager()
{
	for (auto &texturePtr : m_texturePtrList)
	{
		delete texturePtr;
		texturePtr = nullptr;
	}
}
unsigned int TextureManager::Load(const std::string& filename)
{
	unsigned int index{ 0 };
	auto indexIterator = m_indexMap.find(filename);

	// If no index with the same filename
	if (indexIterator == m_indexMap.end())
	{
		// If there are no unused slots
		if (m_availableIndexList.empty())
		{
			// Load and add as a new element
			m_texturePtrList.push_back(new TextureResource(filename));
			index = m_texturePtrList.size() - 1;
		}
		else
		{
			// Load and put into an unused slot.
			index = m_availableIndexList.top();
			m_availableIndexList.pop();
			SDL_assert(index < m_texturePtrList.size());
			m_texturePtrList[index] = new TextureResource(filename);
			
		}
		// Map the resource's index to its filename and bail.
		m_indexMap[filename] = index;
		return index;
	}
	else
	{
		// We found an index with the same filename, so increment the   
		// reference count for the resource that already exists at that index.
		index = indexIterator->second;
		SDL_assert(index < m_texturePtrList.size());
		SDL_assert(m_texturePtrList[index] != nullptr);
		m_texturePtrList[index]->IncrementReferenceCount();

		// Send a handle back to the client
		return index;
	}
}
void TextureManager::Unload(unsigned int index)
{
	if (index < m_texturePtrList.size())
	{
		if (m_texturePtrList[index] != nullptr)
		{
			m_texturePtrList[index]->DecrementReferenceCount();
			if (m_texturePtrList[index]->GetReferenceCount() == 0)
			{
				delete m_texturePtrList[index];
				m_texturePtrList[index] = nullptr;
				m_availableIndexList.push(index);
			}
		}
	}
}
void TextureManager::glBind(unsigned int index)
{

}