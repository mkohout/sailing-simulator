#include "XMLElement.h"

using namespace Application_Layer;

void XMLElement::AddAttribute(
	const string& name, const string& value)
{
	m_attributes.insert(pair<string, string>(name, value));
}

string XMLElement::GetValueOfAttribute(const string& key) const
{
	return m_attributes.find(key)->second;
}

string XMLElement::GetValueOfAttribute(const string& key, const string& defaultValue) const
{
	if(m_attributes.find(key) != m_attributes.end())
	{
		return m_attributes.find(key)->second;
	}else
	{
		return defaultValue;
	}
}

bool XMLElement::HasAttributeNamed(const string& key)
{
	AttributeList::iterator it = m_attributes.find(key);

	return (it != m_attributes.end());
}

const string XMLElement::GetContent() const
{
	return m_content;
}

const string XMLElement::GetName() const
{
	return m_name;
}

void XMLElement::SetContent(const string& content)
{
	m_content = content;
}

void XMLElement::AddChildren(StrongXMLElementPtr pChildren)
{
	m_children.push_back(pChildren);
}

XMLElements XMLElement::GetChildren() const
{
	return m_children;
}

bool XMLElement::HasChildren() const
{
	return m_children.size() > 0;
}

const unsigned int XMLElement::GetChildrenCount() const
{
	return m_children.size();
}

const unsigned int XMLElement::GetChildrenCount(
	const string& childName) const
{
	if (!HasChildren())
	{
		return 0;
	}

	unsigned int count = 0;

	for (auto pChild : m_children)
	{
		if (pChild->GetName().compare(childName) != 0)
		{
			continue;
		}

		count++;
	}

	return count;
}

StrongXMLElementPtr XMLElement::GetFirstChildren() const
{
	return m_children.at(0);
}

StrongXMLElementPtr XMLElement::FindFirstChildNamed(
	const string& childName) const
{
	return FindChildNamed(childName, 0);
}

std::string XMLElement::GetValOfFirstChildsAttrib(const std::string& child, const std::string defaultVal, const std::string& attrib)
{
	auto v = FindFirstChildNamed(child);
	if (v != nullptr)
		return v->GetValueOfAttribute(attrib, defaultVal);
	return defaultVal;
}

StrongXMLElementPtr XMLElement::FindChildNamed(
	const string& childName, const int index) const
{
	if (!HasChildren())
	{
		return nullptr;
	}

	int currentIndex = 0;

	for (auto pChild : m_children)
	{
		if (pChild->GetName().compare(childName) == 0)
		{
			if (currentIndex != index)
			{
				currentIndex++;
				continue;
			}

			return pChild;
		}
	}

	return nullptr;
}

bool XMLElement::HasChildrenNamed(
	const string& childName) const
{
	return FindFirstChildNamed(childName) != nullptr;
}