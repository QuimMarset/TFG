
set ( PROJECT_LINK_LIBS libLLVMLiveVarsPass.so )
link_directories(../../LiveVarsAnalysis/build/LiveVarsPass/)

add_library(LLVMDFGraphPass MODULE DFGraphPass.h DFGraphPass.cpp)
target_link_libraries(LLVMDFGraphPass ${PROJECT_LINK_LIBS} )