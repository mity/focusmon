
add_definitions(-D_WIN32_IE=0x0501 -D_WIN32_WINNT=0x0600 -DWINVER=_WIN32_WINNT)
add_executable(focusmon main.c)
target_link_libraries(focusmon psapi)
