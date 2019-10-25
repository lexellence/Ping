/**************************************************************************************\
** File: ResourceManager.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the ResourceManager templated class
**
\**************************************************************************************/
#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

namespace d2d
{
	template<class ResourceType>
	class ResourceManager
	{
	public:
		~ResourceManager()
		{
			for (auto &resourcePtr : m_resourcePtrList)
			{
				delete resourcePtr;
				resourcePtr = nullptr;
			}
		}

		bool Load(const std::string& filename, unsigned int& handle)
		{
			auto indexIterator = m_filenameIndexMap.find(filename);

			// If no index with the same filename
			if (indexIterator == m_filenameIndexMap.end())
			{
				// If there are no unused slots
				if (m_availableIndexList.empty())
				{
					// Load and add as a new element
					m_resourcePtrList.push_back(new ResourceType(filename));
					handle = m_resourcePtrList.size() - 1;
				}
				else
				{
					// Load and put into an unused slot.
					handle = m_availableIndexList.top();
					m_availableIndexList.pop();
					assert(handle < m_resourcePtrList.size());
					m_resourcePtrList[handle] = new ResourceType(filename);
				}
				// Map the resource's index to its filename and bail.
				m_filenameIndexMap[filename] = handle;
				return true;
			}
			else
			{
				// We found an index with the same filename, so increment the   
				// reference count for the resource that already exists at that index.
				handle = indexIterator->second;
				assert(handle < m_resourcePtrList.size());
				assert(m_resourcePtrList[handle] != nullptr);
				m_resourcePtrList[handle]->IncrementReferenceCount();
			}
			return true;
		}
		bool Unload(unsigned int handle)
		{
			if (handle >= m_resourcePtrList.size() || m_resourcePtrList[handle] == nullptr)
			{
				return false;
			}

			m_resourcePtrList[handle]->DecrementReferenceCount();
			if (m_resourcePtrList[handle]->GetReferenceCount() == 0)
			{
				// Free resource
				delete m_resourcePtrList[handle];
				m_resourcePtrList[handle] = nullptr;
				m_availableIndexList.push(handle);

				// Remove filename to index map entry
				auto indexMapIterator = m_filenameIndexMap.begin();
				while (indexMapIterator != m_filenameIndexMap.end())
				{
					if ((*indexMapIterator).second == handle)
					{
						indexMapIterator = m_filenameIndexMap.erase(indexMapIterator);
					}
					else
					{
						++indexMapIterator;
					}
				}
			}
			return true;
		}
		ResourceType* GetResourcePtr(unsigned int handle) const
		{
			if (handle < m_resourcePtrList.size())
			{
				return m_resourcePtrList[handle];
			}
			return nullptr;
		}

	private:
		std::stack<unsigned int> m_availableIndexList;
		std::vector<ResourceType*> m_resourcePtrList;
		std::map<std::string, unsigned int> m_filenameIndexMap;
	};
}
#endif //RESOURCEMANAGER_H