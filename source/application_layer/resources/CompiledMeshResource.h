#pragma once

#include <string>

#include "IResource.h"

#include "../../game_view/rendering/Geometry.h"
#include "../../game_view/rendering/Material.h"

using namespace Game_View;

namespace Application_Layer
{
	
	class CompiledMeshResource : public IResource
	{
		/**
		* Geometry the obj file is representing
		*/
		StrongGeometryPtr m_pGeometry;

		static const TYPE s_resourceType = COMPILED_MESH;
	public:
		CompiledMeshResource(StrongGeometryPtr a_Geo = 0)
			: m_pGeometry(a_Geo)
		{
			
		}

		virtual ~CompiledMeshResource();

		static bool CanParseFile(const string&);

		virtual void VInitFromFile(const string&);

		void WriteToFile(const string&) const;

		StrongGeometryPtr GetCreatedGeometry() const
		{
			if (!m_pGeometry->HasVertices())
			{
				return nullptr;
			}

			return m_pGeometry;
		}

		virtual void VDispose();

		/**
		* Returns the type of the current resource
		*/
		virtual TYPE VGetType() const
		{
			return s_resourceType;
		}
	};

}