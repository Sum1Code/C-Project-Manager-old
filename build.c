#include "CPM.h"

void generate_objects(){
  StringBuilder_t* builder = sb_new(1);
  

  sb_appendstr(builder, cpm_glob_dir("raylib/src", "*.c"));
  StringBuilder_t* rayobjs = sb_copy(builder);
  sb_patsubst(rayobjs, ".c", ".o");
  StringSplice_t* objs = sb_split_at(rayobjs, " ");
  StringSplice_t* splice = sb_split_at(builder, " ");
  
  for (size_t i = 0; i < splice->splice_count; i++){
    BuildProperties_t buildprop;
    cpm_init(&buildprop, "clang");
    cpm_srcs(&buildprop, splice->strsplice[i]);
    cpm_target(&buildprop, STATICLIB, GET_FILE_NAME(objs->strsplice[i]), "build");
    //CPMLOG(WARNING, splice->strsplice[i]);
    cpm_compile(&buildprop);
  }
  
  
}

void build_so(){
  StringBuilder_t* builder = sb_new(1);
  BuildProperties_t buildprop;
  sb_appendstr(builder, cpm_glob_dir("build", "*.o"));
  char* srcs = sb_to_string(builder);
  cpm_init(&buildprop, "clang");
    cpm_srcs(&buildprop, srcs);
    cpm_target(&buildprop, DYNLIB, "raylib.so", "build");
    //CPMLOG(WARNING, splice->strsplice[i]);
    cpm_compile(&buildprop);
}

int main(int argc, char** argv){
  cpm_rebuild_self(argv);
  
  generate_objects();
  build_so();
  return 0;
}