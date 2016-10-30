#pragma once

#include <string>
#include <memory>

#include "../core/stdafx.h"
#include "IResource.h"
#include "XMLElement.h"

#include "../extern/tinyxml2/tinyxml2.h"

#define XML_ROOT_ELEMENT "root"

using std::shared_ptr;
using std::string;

namespace Application_Layer
{
	/**
	 * Resource class to read random xml files
	 */
	class XMLResource : public IResource
	{
	private:
		const char* m_rootIdentifier;
		tinyxml2::XMLDocument m_doc;

		/**
		 * Pointer to the root element of the current
		 * xml resource
		 */
		StrongXMLElementPtr m_pRootElement;

	public:
		static const TYPE s_resourceType = XML;

		XMLResource()
		{
			m_rootIdentifier = XML_ROOT_ELEMENT;
		}

		/**
		 * Constructor if the root element differs from
		 * the standard root element
		 */
		XMLResource(const char* rootIdentifier)
		{
			m_rootIdentifier = rootIdentifier;
		}

		virtual ~XMLResource() { }
        
        virtual void VDispose() { }

		/**
		 * Start initialization of the given file
		 */
		virtual void VInitFromFile(const string&);

		/**
		 * Starts the processing of the xml with the root element
		 */
		void Process();

		/**
		 * Parses the given tinyxml-element
		 */
		void ParseXMLElement(const tinyxml2::XMLElement*);

		/**
		 * Parses the given tinyxml-element and adds the children to the given data element
		 */
		void ParseXMLElement(
			const tinyxml2::XMLElement*, StrongXMLElementPtr);

		/**
		 * Assigns the attributes of the given tinyxml-element
		 */
		void AssignAttributes(
			const tinyxml2::XMLElement*, StrongXMLElementPtr);

		StrongXMLElementPtr GetRootElement()
		{
			return m_pRootElement;
		}

		/**
		 * Returns the type of the current resource
		 */
		virtual TYPE VGetType() const
		{
			return s_resourceType;
		}
	};
}