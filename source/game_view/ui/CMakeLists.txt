SET(UI_HEADERS	ui/ANDController.h
		ui/IController.h
		ui/Sensor.h
		ui/KeyboardSensor.h
		ui/MouseSensor.h
		ui/ORController.h
		ui/KeyMapping.h
		ui/Interactive.h
		PARENT_SCOPE
		)

SET(UI_SOURCE	ui/ANDController.cpp
		ui/KeyboardSensor.cpp
		ui/MouseSensor.cpp
		ui/ORController.cpp
		ui/KeyMapping.cpp
		ui/Interactive.cpp
		PARENT_SCOPE
		)