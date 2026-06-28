add_library(usecase INTERFACE)
target_include_directories(usecase INTERFACE include)
target_link_libraries(usecase INTERFACE domain)
