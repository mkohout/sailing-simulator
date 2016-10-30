#pragma once

#include <fstream>
#include <vector>
#include <string>

using std::string;

namespace Application_Layer
{
	/**
	 * Mostly returns only the absolute file path of a file requested
	 * Perform simple error check if file is existing
	 */
	class FileManager
	{
	public:
		/**
		 * Different kinds of files
		 * that are used by the engine
		 */
		typedef enum {
			ACTOR,
			OBJECT,
			MATERIAL,
			SCENE,
			SETTINGS,
			SHADER,
			TEXTURE,
			COMPILED_MESH,
            SOUND,
            SOUND_SPEC
		} FILE_TYPE;

	private:
        /**
         * The path to the project directory
         */
        string m_rootDir;
        
		/**
		 * Returns the directory where the
		 * specs and assets are located at
		 */
		const string GetRootDir() const;

		/**
		 * Returns the subpaths in the main directory
         * to the requested type of file
         * e.g. assets/data/actors for the FileType ACTOR
		 */
		const string GetPrefix(FILE_TYPE) const;
        
        /**
         * Since the detection of the root dir is
         * kinda expensive, this method will take
         * care that it will be only set once
         */
        void SetRootDir();

	public:
        /**
         * Initializes the file manager by setting
         * the root dir
         */
        void Init();
        
        /**
         * Returns true, whether a file can be found
         * at the given location
         */
		bool FileExists(const string&) const;
        
        /**
         * Returns the basename including the extension of the
         * given path
         */
        const string GetBaseName(const string&) const;

		const string GetPathToActorFile(const string&) const;

		const string GetPathToObjectFile(const string&) const;

		const string GetPathToMaterialFile(const string&) const;

		const string GetPathToSceneFile(const string&) const;

		const string GetPathToShaderFile(const string&) const;
        
        const string GetPathToSoundFile(const string&) const;
        
        const string GetPathToSoundSpecFile(const string&) const;

		const string GetPathToSettingsFile(const string&) const;

		const string GetPathToTextureFile(const string&) const;

		const string GetPathToCompiledMeshFile(const std::string&) const;

		const string GetPathToFile(FILE_TYPE, const string&) const;
	};

}