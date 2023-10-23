#define CPM_IMPLEMENTATION
#include "CPM.h"

void cpm_setup(){
    cpm_dircreate("./build");
    cpm_dircreate("./srcs");
}

int main(void){
    BuildProperties_t prop;
    cpm_init(&prop);
    cpm_compiler(&prop, "clang");
    cpm_srcs(&prop, cpm_glob_dir("./srcs", "*.c"));
    cpm_target(&prop, EXECUTABLE, "build/tgt");
    cpm_compile(&prop);
}