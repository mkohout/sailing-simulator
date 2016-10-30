SET(CORE_HEADERS	core/stdafx.h
			core/GameCore.h
			core/PlayerInputThread.h
			core/Thread.h
			core/WorldSimulationThread.h
			core/PhysicSimulationThread.h
			core/SoundThread.h
			PARENT_SCOPE
					)

SET(CORE_SOURCE		core/GameCore.cpp
			core/PlayerInputThread.cpp
			core/PhysicSimulationThread.cpp
			core/WorldSimulationThread.cpp
			core/SoundThread.cpp
			PARENT_SCOPE
			)