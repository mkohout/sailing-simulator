#pragma once

#define DEBUG 1
#define OPEN_GL 1
#define FMOD 1

#define g_pGameApp Application_Layer::GameCore::GetInstance()->GetGameApp()

#define SAFE_DELETE(p) { if(p) { delete(p); (p) = nullptr; } }