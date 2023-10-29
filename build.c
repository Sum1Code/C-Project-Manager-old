#include "CPM.h"

void regen() {
  cpm_dirops(D_DELETE, "build");
  cpm_dirops(D_CREATE, "build/raysrc");
}

void buildobjects() {
  StringBuilder_t *sb = sb_new();
  sb_appendstr(sb, cpm_glob_dir("./raysrc", "*.c"));
  StringSplice_t *srcs = sb_split_at(sb, " ");
  sb_patsubst(sb, ".c", ".o");
  StringSplice_t *objs = sb_split_at(sb, " ");
  sb_free(sb);

  pid_t compilation_proc[srcs->splice_count];
  for (size_t i = 0; i < srcs->splice_count; i++) {
    compilation_proc[i] = cpm_quick_compile_async(
        "clang", srcs->strsplice[i], "build", OBJECT, objs->strsplice[i],
        "-DPLATFORM_DESKTOP -DUSE_WAYLAND -D_GNU_SOURCE "
        "-I./raysrc/external/glfw/include ");
  }

  cpm_compile_async_poll_many(compilation_proc);
}

void cpm_build_raylib() {
  cpm_quick_compile("clang", cpm_glob_dir("./build/raysrc", "*.o"), "build",
                    DYNLIB, "libraylib.so", NULL);
}
void run() { system("LD_LIBRARY_PATH=./build ./build/main"); }
int main(int argc, char **argv) {
  cpm_rebuild_self(argv);
  regen();
  buildobjects();
  cpm_build_raylib();
  cpm_quick_compile("clang", "./srcs/main.c", "build", EXECUTABLE, "main",
                    "-I./raysrc -Lbuild -lraylib -lm");
  run();
}
