# Copy files from source directory to destination directory, substituting any
# variables.  Create destination directory if it does not exist.
message(STATUS "Configuring directory ${DEST_DIR}")
make_directory(${DEST_DIR})

file(GLOB_RECURSE templateFiles RELATIVE ${SRC_DIR} ${SRC_DIR}/*)
foreach(templateFile ${templateFiles})
    set(srcTemplatePath ${SRC_DIR}/${templateFile})
    if(NOT IS_DIRECTORY ${srcTemplatePath})
        message(STATUS "Configuring file ${templateFile} to ${DEST_DIR}/${templateFile}")
        configure_file(
                ${srcTemplatePath}
                ${DEST_DIR}/${templateFile}
                COPYONLY)
    endif(NOT IS_DIRECTORY ${srcTemplatePath})
endforeach(templateFile)
