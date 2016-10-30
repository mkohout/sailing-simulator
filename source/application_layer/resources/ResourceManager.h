#pragma once

#include <string>
#include <map>

#include "IResource.h"
#include "FileManager.h"

using std::map;
using std::pair;

namespace Application_Layer
{
    typedef map<string, StrongResourcePtr> LoadedResources;
    
	/*
	 * Class to manage the resources
     * needed by the engine, such as loading
     * resources in advance
	 */
	class ResourceManager
	{
	private:
		StrongResourcePtr LoadImageResource(const string&);
        
        StrongResourcePtr LoadObjResource(const string&);
        
        StrongResourcePtr LoadSoundResource(const string&);

        StrongResourcePtr LoadXMLResource(const string&);

	public:
		ResourceManager() = default;

		~ResourceManager();

		void Init();

		/*
		 * Returns a pointer to the request resource file
		 */
		StrongResourcePtr GetResource(const string&, IResource::TYPE);
	};
}