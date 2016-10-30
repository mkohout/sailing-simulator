#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <memory>

using std::string;
using std::shared_ptr;

namespace Application_Layer
{

	/**
	 * Basic interface for a resource
	 */
	class IResource
	{
	public:
		typedef enum {
			IMAGE,
			OBJ,
			XML,
			COMPILED_MESH,
            SOUND
		} TYPE;

		virtual void VInitFromFile(const string&) = 0;

		virtual TYPE VGetType() const = 0;
        
        virtual void VDispose() = 0;
	};

	typedef shared_ptr<IResource> StrongResourcePtr;
}