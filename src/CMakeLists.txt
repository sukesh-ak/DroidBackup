add_executable(${DROIDBACKUP_PROJECT_NAME}
        ${DROIDBACKUP_SRC_DIR}/main.cpp)

target_compile_features(${DROIDBACKUP_PROJECT_NAME} PUBLIC
        cxx_std_20)

target_include_directories(${DROIDBACKUP_PROJECT_NAME} PUBLIC
        ${PROJECT_BINARY_DIR}
        ${DROIDBACKUP_INC_DIR}
        cxxopts abackup)

#target_link_libraries(${DROIDBACKUP_PROJECT_NAME} PUBLIC cxxopts)