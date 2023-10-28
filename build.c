#include "CPM.h"

void regen(){
  cpm_dirops(D_CREATE, "build");
}

void buildobjects(){
  cpm_quick_compile("clang", cpm_glob_dir("./srcs", "*.c"), "build", EXECUTABLE, "main", "");
  pid_t addobj = cpm_quick_compile_async("clang", "./srcs/add.c", "build", OBJECT, "add.o", NULL);
  cpm_compile_async_poll(addobj);
}

int main(int argc, char** argv){
  cpm_rebuild_self(argv)
  regen();
  buildobjects();
}