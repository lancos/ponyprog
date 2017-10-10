# Generate NSIS installer
SET(bin_dir             .)
SET(lib_dir             .)
SET(lang_dir            ./lang/)
SET(help_dir            .)
SET(license_dir         .)

SET(CPACK_GENERATOR NSIS ZIP)

# set(CPACK_PACKAGE_INSTALL_DIRECTORY "${ROBOMONGO_DISPLAY_FULL_NAME}")
 
FUNCTION(INSTALL_QT_LIBS)
    FOREACH(module ${ARGV})
        SET(target_name Qt5::${module})
        SET(module_name Qt5${module})

        # Get full path to some known Qt library (i.e. /path/to/lib/libQt5Widgets.so.5.5.1)
        GET_TARGET_PROPERTY(target_path Qt5::Core LOCATION)

        # Get folder path of library (i.e. /path/to/lib)
        GET_FILENAME_COMPONENT(qt_lib_dir ${target_path} DIRECTORY)

        # Install single DLL to lib directory
        INSTALL(FILES ${qt_lib_dir}/${module_name}${CMAKE_SHARED_LIBRARY_SUFFIX}
                DESTINATION ${lib_dir}
                )
    ENDFOREACH()
ENDFUNCTION()

INSTALL(PROGRAMS "${CURRENT_BUILD_DIR}/ponyprog.exe" 
        DESTINATION ${bin_dir}
        COMPONENT "application" 
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ
        )

INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/icons/"    
        DESTINATION ${bin_dir}
        COMPONENT "application" 
        DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ 
        FILES_MATCHING PATTERN "*.png"
        )

INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/lang/" 
        DESTINATION ${lang_dir}
        COMPONENT "lang files" 
        DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ 
        FILES_MATCHING PATTERN "*.utf"
        ) 

INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/distribution/innosetup/" 
        DESTINATION ${help_dir}
        COMPONENT "help files" 
        DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ 
        FILES_MATCHING PATTERN "*.jpg" PATTERN "*.html"
        ) 
        
INSTALL(FILES "${PROJECT_SOURCE_DIR}/InpOutLib/Win32/inpout32.dll" 
        DESTINATION  ${bin_dir}
        COMPONENT "driver" 
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ
        ) 
     
INSTALL(FILES "${PROJECT_SOURCE_DIR}/InpOutLib/InstallDriver/InstallDriver.exe" 
        DESTINATION  ${bin_dir}
        COMPONENT "driver" 
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ
        ) 
        
INSTALL_QT_LIBS(Core Gui Widgets PrintSupport Multimedia)
        
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/package-binary-nsis.cpack.in ${CMAKE_CURRENT_BINARY_DIR}/package-binary-nsis.cpack)
ADD_CUSTOM_TARGET(package-binary-nsis
	${CMAKE_CPACK_COMMAND}
	--config ${CMAKE_CURRENT_BINARY_DIR}/package-binary-nsis.cpack
	)


# INCLUDE(CPack)
