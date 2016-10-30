#include "ImageResource.h"
#include "FileManager.h"
#include "../../extern/soil/SOIL.h"

using namespace Application_Layer;

string ImageResource::GetFilename() const
{
    return m_filename;
}

void ImageResource::VInitFromFile(const string& filename)
{
    m_filename = filename;
}
