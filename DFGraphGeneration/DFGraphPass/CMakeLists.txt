
set ( PROJECT_LINK_LIBS libLLVMLiveVarsPass.so )
link_directories(../../LiveVarsAnalysis/build/LiveVarsPass/)

include_directories(../../DFGraphComponents)
file(GLOB SOURCES ../../DFGraphComponents/*.cpp)

add_library(LLVMDFGraphPass MODULE DFGraphPass.h DFGraphPass.cpp ${SOURCES})
target_link_libraries(LLVMDFGraphPass ${PROJECT_LINK_LIBS} )