#include "XMLResource.h"

using namespace Application_Layer;

void XMLResource::VInitFromFile(const string& filename)
{
	m_doc.LoadFile(filename.c_str());

	Process();
}

void XMLResource::Process()
{
	const tinyxml2::XMLElement* rootElement =
		m_doc.FirstChildElement(m_rootIdentifier);

	ParseXMLElement(rootElement);
}

void XMLResource::ParseXMLElement(
	const tinyxml2::XMLElement* pElement)
{
	const tinyxml2::XMLElement* element =
		pElement->FirstChildElement();
	StrongXMLElementPtr pRootElement =
		StrongXMLElementPtr(new XMLElement());

	m_pRootElement = shared_ptr<XMLElement>(pRootElement);

	ParseXMLElement(element, pRootElement);
}

void XMLResource::ParseXMLElement(
	const tinyxml2::XMLElement* pCurrentElement,
	StrongXMLElementPtr pXMLElement)
{
	if (pCurrentElement == nullptr)
	{
		return;
	}

	do
	{
		StrongXMLElementPtr xmlElement =
			StrongXMLElementPtr(new XMLElement());
        
		xmlElement->SetName(pCurrentElement->Name());

		if (pCurrentElement->GetText() != nullptr)
		{
			xmlElement->SetContent(
				pCurrentElement->GetText());
		}

		AssignAttributes(pCurrentElement, xmlElement);

		// Parse Children
		if (!pCurrentElement->NoChildren())
		{
			ParseXMLElement(
				pCurrentElement->FirstChildElement(), xmlElement);
		}

		pXMLElement->AddChildren(xmlElement);
	} while ((pCurrentElement = pCurrentElement->NextSiblingElement()));
}

void XMLResource::AssignAttributes(
	const tinyxml2::XMLElement* pCurrentElement,
	StrongXMLElementPtr pXMLElement)
{

	if (pCurrentElement->FirstAttribute() == nullptr)
	{
		return;
	}

	const tinyxml2::XMLAttribute* attr =
		pCurrentElement->FirstAttribute();

	do
	{
		pXMLElement->AddAttribute(
			attr->Name(), attr->Value());
	} while ((attr = attr->Next()));
}