#pragma once

#include <map>
#include <string>

#include "../../application_layer/resources/XMLResource.h"

using std::map;
using std::string;

using namespace Application_Layer;

namespace Game_Logic
{
    /**
     * Class is representing a base class for storing and initializing settings
     */
    class Settings
    {
    private:
        /**
         * Map that holds all the setting values
         * that were found in the XML file
         */
        map<string, string> m_values;
        
    public:
        Settings() = default;
        
        void AddValue(const string&, const string&);

        /**
         * Returns the value belonging to the given key
         */
        const string GetValue(const string&) const;
        
        /**
         * Returns the value of the given key that
         * is expected to be a float
         */
        const float GetFloatValue(const string&) const;
        
        /**
         * Returns the value of the given key that
         * is expected to be a double
         */
        const double GetDoubleValue(const string&) const;
        
        /**
         * Returns the value of the given key that
         * is expected to be a integer
         */
        const int GetIntValue(const string&) const;
        
        /**
         * Initializes the settings by the given XML file
         */
        void InitFromXMLResource(shared_ptr<XMLResource>);
        
        /**
         * Processes a single XML element that was found
         * in the XML file
         */
        void ProcessXMLElement(StrongXMLElementPtr);
    };
}