macro (set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
  set_property (TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY}
    ${XCODE_VALUE})
endmacro (set_xcode_property)

macro (enable_cpp11 TARGET)
	set_xcode_property(${TARGET} CLANG_CXX_LANGUAGE_STANDARD "c++0x")
	set_xcode_property(${TARGET} CLANG_CXX_LIBRARY "libc++")
endmacro (enable_cpp11)
