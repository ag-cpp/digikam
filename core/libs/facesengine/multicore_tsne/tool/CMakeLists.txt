
MACRO(TSNE_TOOLS_BUILD)
set(_filename ${ARGV0})
string(REPLACE "." ";" _temp ${_filename})
list(GET _temp 0 _target)

set(${_target}_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/${_target}
)

add_executable(${_target} ${${_target}_SRCS})

set_target_properties(${_target} PROPERTIES OUTPUT_NAME ${_target})

target_link_libraries(${_target}
                      tsne_multicore
)
ENDMACRO()

TSNE_TOOLS_BUILD(project.cpp)