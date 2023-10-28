#define CPM_IMPLEMENTATION
#include "CPM.h"

void cpm_setup(){
  cpm_dirops(D_DELETE, "builds");
  cpm_dirops(D_CREATE, "builds");
}

int main(int argc, char **argv){
  cpm_rebuild_self(argv);
  BuildProperties_t prop;
  cpm_init(&prop, "clang");
  cpm_srcs(&prop, cpm_glob_dir("srcs", "*.c"));
  cpm_target(&prop, EXECUTABLE, "tgt", "builds");
  cpm_flags(&prop, "-O3", "-fsanitize=address", "-Wall", "-Wextra", "-pedantic");

  cpm_compile(&prop);
  int processes[3];
  processes[0] = cpm_compile_async(&prop);
  processes[1] = cpm_compile_async(&prop);
  processes[2] = cpm_compile_async(&prop);

  //cpm_compile_async_poll_many(processes)
  return 0;
}