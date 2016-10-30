#pragma once

#include "IResource.h"
#include <boost/filesystem.hpp>

namespace Application_Layer
{
	/**
	 * File that holds image data such as tif, jpg, bmp
	 */
	class ImageResource : public IResource
	{
    private:
        string m_filename;
        
	public:
		static const TYPE s_resourceType = IMAGE;

		ImageResource() = default;
        
        virtual void VDispose()
        {
        }

		virtual ~ImageResource()
        {
        }

		virtual void VInitFromFile(const string&);
        
        string GetFilename() const;
        
        string GetBasename() const
        {
            boost::filesystem::path pathname(m_filename);
            
            return pathname.filename().string();
        }

		virtual TYPE VGetType() const
		{
			return s_resourceType;
		}
	};
}