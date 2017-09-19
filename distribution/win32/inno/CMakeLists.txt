# Generate InnoSetup installer
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/package-binary-inno.cpack.in ${CMAKE_CURRENT_BINARY_DIR}/package-binary-inno.cpack)
ADD_CUSTOM_TARGET(package-binary-inno
	${CMAKE_CPACK_COMMAND}
	--config ${CMAKE_CURRENT_BINARY_DIR}/package-binary-inno.cpack
	)

