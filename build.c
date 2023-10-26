#define CPM_IMPLEMENTATION
#include "CPM.h"

void cpm_setup(){
  cpm_dirops(D_CREATE, "build");
}
int main(int argc, char** argv){
  StringBuilder_t *sbuilder = sb_new(sizeof(" "));
  sb_appendstr(sbuilder, "Hello, world!");
  char* res = sb_to_string(sbuilder);
  printf("%s\n", res);
}