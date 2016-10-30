#pragma once

#include "../core/stdafx.h"

#include <vector>
#include <map>
#include <string>
#include <memory>

using std::vector;
using std::map;
using std::string;
using std::pair;
using std::shared_ptr;

namespace Application_Layer
{
	class XMLElement;

	typedef shared_ptr<XMLElement> StrongXMLElementPtr;

	typedef vector<StrongXMLElementPtr> XMLElements;

	typedef map<string, string> AttributeList;

	/**
	 * Class is representing a xml element with all the children
	 */
	class XMLElement
	{
	private:
		XMLElements m_children;
		AttributeList m_attributes;

		string m_content;
		string m_name;

	public:
		XMLElement() : m_children(), m_content()
		{
		}

		~XMLElement() { }

		/**
		 * Adds the attribute by the given name-value pair
		 */
		void AddAttribute(const string&, const string&);

		/**
		 * Returns the value of the attribute that matches the given key
		 */
		string GetValueOfAttribute(const string&) const;
		string GetValueOfAttribute(const string& key, const string&) const;
		bool HasAttributeNamed(const string&);

		/**
		 * Returns the content of the current xml element if its simple text
		 */
		const string GetContent() const;

		/**
		 * Sets the name of the current xml element
		 */
		void SetName(const string& name)
		{
			m_name = name;
		}

		/**
		 * Returns the name of the current xml element
		 */
		const string GetName() const;

		/**
		 * Sets the text content of the current xml element
		 */
		void SetContent(const string&);

		void AddChildren(StrongXMLElementPtr);

		XMLElements GetChildren() const;

		/**
		 * Returns true if the element has children
		 */
		bool HasChildren() const;

		/**
		 * Returns the count of the children
		 */
		const unsigned int GetChildrenCount() const;

		/**
		 * Returns the count of the children named by the given string
		 */
		const unsigned int GetChildrenCount(const string&) const;

		/**
		 * Returns the first children - without considering the name of the element
		 */
		inline StrongXMLElementPtr GetFirstChildren() const;

		/**
		 * Returns the first child that matches the given name
		 */
		StrongXMLElementPtr FindFirstChildNamed(const string&) const;

		std::string GetValOfFirstChildsAttrib(const std::string& child, const std::string defaultVal = "", const std::string& attrib = "value");

		/**
		 * Returns the i-child named by the given string
		 */
		StrongXMLElementPtr FindChildNamed(
			const string&, const int) const;

		/**
		 * Returns true if the element has a children named by the given string
		 */
		bool HasChildrenNamed(const string&) const;
	};
}