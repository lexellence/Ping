/**************************************************************************************\
** File: ResourceReference.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the ResourceReference, TextureResource, FontResource classes
**
\**************************************************************************************/
#ifndef RESOURCEREFERENCE_H
#define RESOURCEREFERENCE_H

namespace d2d
{
	// Automatically load/unload resource to/from current Renderer on ctor/dtor
	class ResourceReference
	{
	public:
		ResourceReference(std::string filename);
		virtual ~ResourceReference() = default;
		unsigned int GetID() const;
		std::string GetFilename() const;
		
	protected:
		std::string m_filename;
		unsigned int m_id{};
	};

	// Automatically load/unload texture to/from Renderer on ctor/dtor
	class TextureReference : public ResourceReference
	{
	public:
		TextureReference(std::string filename);
		virtual ~TextureReference();
	};

	// Automatically load/unload font to/from Renderer on ctor/dtor
	class FontReference : public ResourceReference
	{
	public:
		FontReference(std::string filename);
		virtual ~FontReference();
	};
}
#endif //RESOURCEREFERENCE_H