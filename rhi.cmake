function(rhi_symlink_shaders ABSOLUTE_TARGET_SHADER_DIRECTORY)
    if(NOT UNIX)
        file(TO_NATIVE_PATH ${ABSOLUTE_TARGET_SHADER_DIRECTORY}/rhi DST_PATH)
        file(TO_NATIVE_PATH ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/shaders/rhi SRC_PATH)
        if(NOT EXISTS ${DST_PATH})
            message(STATUS "Creating symlink/junction between ${DST_PATH} and ${SRC_PATH}")
            execute_process(COMMAND cmd.exe /c mklink /D /J "${DST_PATH}" "${SRC_PATH}" >NUL)
        endif()
    endif()
endfunction()

function(rhi_download_and_extract_zip URL DST_FOLDER FOLDER_NAME)
    if(NOT EXISTS ${CMAKE_BINARY_DIR}/download/${FOLDER_NAME}.zip)
        message(STATUS "Downloading ${URL} and unpacking to ${DST_FOLDER}/${FOLDER_NAME}.")
        file(
            DOWNLOAD
            ${URL}
            ${CMAKE_BINARY_DIR}/download/${FOLDER_NAME}.zip
        )
    else()
        message(STATUS "${CMAKE_BINARY_DIR}/download/${FOLDER_NAME}.zip already exists. No download required.")
    endif()
    if(NOT EXISTS ${DST_FOLDER}/${FOLDER_NAME})
        message(STATUS "Extracting ${CMAKE_BINARY_DIR}/download/${FOLDER_NAME}.zip to ${DST_FOLDER}/${FOLDER_NAME}.")
        file(
            ARCHIVE_EXTRACT
            INPUT ${CMAKE_BINARY_DIR}/download/${FOLDER_NAME}.zip
            DESTINATION ${DST_FOLDER}/${FOLDER_NAME}
        )
    else()
        message(STATUS "${CMAKE_BINARY_DIR}/download/${FOLDER_NAME}.zip is already extracted to ${DST_FOLDER}/${FOLDER_NAME}.")
    endif()
endfunction()

function(rhi_deploy_files SRC DST)
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        set(DEPLOY_FILES_DST ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/${DST})
        message(STATUS "Copying ${SRC} to ${DEPLOY_FILES_DST}.")
        file(COPY ${SRC} DESTINATION ${DEPLOY_FILES_DST})
    endforeach()
endfunction()

function(rhi_deploy_d3d12 DST)
    message(STATUS "Deploying D3D12 Agility SDK to <executable_location>/${DST}/.")
    rhi_deploy_files(${rhi_SOURCE_DIR}/thirdparty/d3d12_agility_sdk/build/native/bin/x64/D3D12Core.dll ${DST})
    rhi_deploy_files(${rhi_SOURCE_DIR}/thirdparty/d3d12_agility_sdk/build/native/bin/x64/d3d12SDKLayers.dll ${DST})
endfunction()
