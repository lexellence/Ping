/**************************************************************************************\
** File: FontManager.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the FontManager class
**
\**************************************************************************************/
#include "FontManager.h"
#include "FontResource.h"
#include "drawtext.h"
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL.h>
#include <vector>
#include <stack>
#include <map>
#include <iostream>

FontManager::FontManager()
{

}
FontManager::~FontManager()
{

}
unsigned int FontManager::Load(const std::string& filename, FontQuality quality)
{
	// Store the handle that will get returned
	unsigned int index{ 0 };

	// Search our resource list for one with the same filename
	auto nameIndexIterator = m_indexMap.find(filename);

	// If we found an index with the same filename
	if (nameIndexIterator != m_indexMap.end())
	{
		// If we find an index that also has the same quality
		auto qualityIndexIterator = nameIndexIterator->second.find(quality);
		if (qualityIndexIterator != nameIndexIterator->second.end())
		{
			// Increment the reference count for the resource that already exists and bail.
			index = qualityIndexIterator->second;
			SDL_assert(index < m_fontPtrList.size());
			SDL_assert(m_fontPtrList[index] != nullptr);
			m_fontPtrList[index]->IncrementReferenceCount();
			return index;
		}
	}
	// If we make it to here, we didn't find a matching resource

	// If there are no unused slots
	if (m_availableIndexList.empty())
	{
		// Load and add as a new element
		m_fontPtrList.push_back(new FontResource(filename, quality));
		index = m_fontPtrList.size() - 1;
	}
	else
	{
		// Load and put into an unused slot.
		index = m_availableIndexList.top();
		m_availableIndexList.pop();
		SDL_assert(index < m_fontPtrList.size());
		m_fontPtrList[index] = new FontResource(filename, quality);
	}

	// Map the resource's index to its filename/quality and bail.
	m_indexMap[filename][quality] = index;
	return index;
}
void FontManager::Unload(unsigned int index)
{
	if (index < m_fontPtrList.size())
	{
		if (m_fontPtrList[index] != nullptr)
		{
			m_fontPtrList[index]->DecrementReferenceCount();
			if (m_fontPtrList[index]->GetReferenceCount() == 0)
			{
				delete m_fontPtrList[index];
				m_fontPtrList[index] = nullptr;
				m_availableIndexList.push(index);
			}
		}
	}
}
void FontManager::dtxBind(unsigned int index)
{
	if (index < m_fontPtrList.size())
	{
		if (m_fontPtrList[index] != nullptr)
		{
			m_fontPtrList[index]->dtxBind();
		}
	}
}