#include "KeyMapping.h"

#include <glfw/glfw3.h>

using namespace Game_View;

bool KeyMapping::s_initialized = false;
map<string, int> KeyMapping::s_mapping;
map<int, string> KeyMapping::s_reversed;


void KeyMapping::InitMapping()
{
    s_mapping.insert(KeyPair("up", GLFW_KEY_UP));
    s_mapping.insert(KeyPair("down", GLFW_KEY_DOWN));
    s_mapping.insert(KeyPair("left", GLFW_KEY_LEFT));
    s_mapping.insert(KeyPair("right", GLFW_KEY_RIGHT));
    s_mapping.insert(KeyPair(",", GLFW_KEY_COMMA));
    s_mapping.insert(KeyPair(".", GLFW_KEY_PERIOD));
    s_mapping.insert(KeyPair("m", GLFW_KEY_M));
    s_mapping.insert(KeyPair("u", GLFW_KEY_U));
    s_mapping.insert(KeyPair("i", GLFW_KEY_I));
    s_mapping.insert(KeyPair("o", GLFW_KEY_O));
    s_mapping.insert(KeyPair("p", GLFW_KEY_P));
    s_mapping.insert(KeyPair("k", GLFW_KEY_K));
    s_mapping.insert(KeyPair("w", GLFW_KEY_W));
    s_mapping.insert(KeyPair("a", GLFW_KEY_A));
    s_mapping.insert(KeyPair("s", GLFW_KEY_S));
    s_mapping.insert(KeyPair("d", GLFW_KEY_D));
    s_mapping.insert(
            KeyPair("mouseButtonLeft", GLFW_MOUSE_BUTTON_LEFT));
    s_mapping.insert(
            KeyPair("mouseButtonRight", GLFW_MOUSE_BUTTON_RIGHT));
    s_mapping.insert(
            KeyPair("mouseButtonMiddle", GLFW_MOUSE_BUTTON_MIDDLE));
    
    InitReversedMapping();
}

void KeyMapping::InitReversedMapping()
{
    for(auto& mapEntry : s_mapping)
    {
        s_reversed.insert(
            pair<int, string>(mapEntry.second, mapEntry.first));
    }
}

int KeyMapping::GetGLFWKeyForString(const string& keyString)
{
    if(s_initialized==false)
    {
        InitMapping();
    }
    
    auto findIt = s_mapping.find(keyString);
    
    if(findIt==s_mapping.end())
    {
        return -1;
    }
    
    return s_mapping.at(keyString);
}

string KeyMapping::GetKeyStringForGLFW(const int& glfwKey)
{
    if(s_initialized==false)
    {
        InitMapping();
    }
    
    auto findIt = s_reversed.find(glfwKey);
    
    if(findIt==s_reversed.end())
    {
        return "";
    }
    
    return s_reversed.at(glfwKey);
}