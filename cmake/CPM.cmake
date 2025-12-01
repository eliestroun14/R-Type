# CPM (C++ Package Manager)
# Permet de télécharger et gérer les dépendances automatiquement

# Set a default error message
if(NOT CPM_SOURCE_CACHE)
    set(CPM_SOURCE_CACHE "${CMAKE_BINARY_DIR}/_deps" CACHE STRING "Path to the directory which will be used to cache the sources of cpm dependencies")
endif()

# Define the download location and version
set(CPM_DOWNLOAD_VERSION 0.38.6)
set(CPM_DOWNLOAD_LOCATION "https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake")

# Create cache directory if it doesn't exist
if(NOT (EXISTS ${CMAKE_BINARY_DIR}/cmake/CPM.cmake))
    message(STATUS "Downloading CPM.cmake v${CPM_DOWNLOAD_VERSION}")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/cmake")
    file(DOWNLOAD ${CPM_DOWNLOAD_LOCATION} "${CMAKE_BINARY_DIR}/cmake/CPM.cmake")
endif()

include(${CMAKE_BINARY_DIR}/cmake/CPM.cmake)
