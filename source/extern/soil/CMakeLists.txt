SET(SOURCE	SOIL.c
		image_DXT.c
		stb_image_aug.c
		image_helper.c)

add_library(extern_soil STATIC ${SOURCE})