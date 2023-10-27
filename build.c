#define CPM_IMPLEMENTATION
#include "CPM.h"

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
  cpm_compile(&prop);
  return 0;
}