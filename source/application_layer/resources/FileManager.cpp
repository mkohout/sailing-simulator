#include "FileManager.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <sstream>

#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

using namespace Application_Layer;

void FileManager::Init()
{
    SetRootDir();
}

bool FileManager::FileExists(const string &filepath) const
{
    return boost::filesystem::exists(filepath);
}

const string FileManager::GetRootDir() const
{
    return m_rootDir;
}

const string FileManager::GetBaseName(const string& filename) const
{
    return boost::filesystem::basename(filename) +
            boost::filesystem::extension(filename);
}

void FileManager::SetRootDir()
{
    m_rootDir =
        std::string(STRINGIZE(CMAKE_PROJECT_PATH)) + "/";
}

const string FileManager::GetPathToActorFile(
	const string &filename) const
{
	return GetPathToFile(ACTOR, filename);
}

const string FileManager::GetPathToObjectFile(
	const string &filename) const
{
	return GetPathToFile(OBJECT, filename);
}

const string FileManager::GetPathToMaterialFile(
	const string &filename) const
{
	return GetPathToFile(MATERIAL, filename);
}

const string FileManager::GetPathToSceneFile(
	const string &filename) const
{
	return GetPathToFile(SCENE, filename);
}

const string FileManager::GetPathToShaderFile(
	const string &filename) const
{
	return GetPathToFile(SHADER, filename);
}

const string FileManager::GetPathToSettingsFile(
	const string &filename) const
{
	return GetPathToFile(SETTINGS, filename);
}

const string FileManager::GetPathToFile(
	FILE_TYPE fileType, const string &filename) const
{
	return GetRootDir() + GetPrefix(fileType) + filename;
}

const string FileManager::GetPathToTextureFile(
	const string& filename) const
{
	return GetPathToFile(TEXTURE, filename);
}

const string FileManager::GetPathToSoundFile(
                                        const string& filename) const
{
    return GetPathToFile(SOUND, filename);
}

const string FileManager::GetPathToSoundSpecFile(
                                               const string& filename) const
{
    return GetPathToFile(SOUND_SPEC, filename);
}

const string FileManager::GetPathToCompiledMeshFile(
                const std::string& filename) const
{
	return GetPathToFile(COMPILED_MESH, filename);
}

const string FileManager::GetPrefix(FILE_TYPE fileType) const
{
	switch (fileType)
	{
	case ACTOR:
		return "assets/data/actors/";
		break;

	case MATERIAL:
		return "assets/material/";

	case OBJECT:
		return "assets/objects/";
		break;

	case SCENE:
		return "assets/data/scenes/";
		break;

	case SETTINGS:
		return "assets/data/settings/";
		break;

	case TEXTURE:
		return "assets/textures/";
		break;

	case SHADER:
		return "assets/shader/";
		break;
            
    case SOUND:
        return "assets/sounds/";
        break;
            
    case SOUND_SPEC:
        return "assets/data/sounds/";
        break;

	case COMPILED_MESH:
		return "assets/compiled_meshes/";
		break;

	}

	return "specs/";
}
