#define CPM_IMPLEMENTATION
#include "../CPM.h"
//TESTING SOME SOURCEFILE
void cpm_setup(){
  cpm_dirops(D_DELETE, "build");
  cpm_dirops(D_CREATE, "build");
}

int main(int argc, char **argv){
  cpm_rebuild_self(argv);
  BuildProperties_t prop;
  cpm_init(&prop, "clang");
  cpm_srcs(&prop, cpm_glob_dir("srcs", "*.c"));
  cpm_target(&prop, EXECUTABLE, "tgt.so", "build");
  cpm_flags(&prop, "-g", "-Wall", "-Wextra");
  int pid1 = cpm_compile_async(&prop);
  
  int pid2 = cpm_compile_async(&prop);
  
  int pid3 = cpm_compile_async(&prop);
  cpm_compile_async_poll(pid1);
  cpm_compile_async_poll(pid2);
  cpm_compile_async_poll(pid3);
  return 0;
}