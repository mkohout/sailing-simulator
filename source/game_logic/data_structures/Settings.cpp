#include "Settings.h"
#include <stdlib.h>

#include "../../application_layer/resources/XMLResource.h"

using namespace Game_Logic;
using namespace Application_Layer;

using std::pair;

void Settings::AddValue(
	const string& key, const string& value)
{
	m_values.insert(
		pair<string, string>(key, value));
}

const string Settings::GetValue(const string& key) const
{
	return m_values.find(key)->second;
}

const float Settings::GetFloatValue(const string& key) const
{
	return strtof(GetValue(key).c_str(), NULL);
}

const double Settings::GetDoubleValue(const string& key) const
{
	return std::stod(GetValue(key));
}

const int Settings::GetIntValue(const string& key) const
{
	return atoi(GetValue(key).c_str());
}

void Settings::InitFromXMLResource(
                    shared_ptr<XMLResource> pXMLResource)
{
	shared_ptr<XMLElement> rootElement =
                        pXMLResource->GetRootElement();

	if (rootElement == nullptr)
	{
		return;
	}

	for (auto pChild : rootElement->GetChildren())
	{
		ProcessXMLElement(pChild);
	}
}

void Settings::ProcessXMLElement(StrongXMLElementPtr pElement)
{
	if (pElement == nullptr)
	{
		return;
	}

	AddValue(
		pElement->GetName(), pElement->GetContent());
}