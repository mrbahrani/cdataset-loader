add_executable(load_mnist load_mnist.c)
target_include_directories(load_mnist PUBLIC ../include)
target_link_libraries(load_mnist PRIVATE cdatasetLoader)