#define CPM_IMPLEMENTATION      // IMPORTANT TO IMPLEMENT cpm_setup OTHERWISE ERROR
#include "CPM.h"                // INCLUDE AFTER CPM_IMPLEMENTATION

void cpm_setup(){               // IMPLEMENT THIS ON YOUR OWN 
    cpm_dircreate("./build");   //DIRCREATE TO CREATE DIRECTORIES
    cpm_dircreate("./srcs");   
}

void compile_objects(){
    BuildProperties_t prop;
    CPMLOG(MSG, "Building Object files");
    cpm_init(&prop);
    cpm_target(&prop, STATICLIB, "bb");
}

int main(int argc, char** argv){
    cpm_rebuild_self(argc, argv);                        // rebuild myself if source code changed
    BuildProperties_t prop;                              // MAIN BUILD PROPERTIES 
    CPMLOG(WARNING, "EXAMPLE_CONFIG_USED")  
    cpm_init(&prop);                                    // INIT BUILD PROPS AND RUNS cpm_setup()
    cpm_compiler(&prop, "clang");                       // SPECIFY COMPILER (REQUIRED)
    cpm_flags(&prop, "-g", "-Wall", "-Wextra");         // SPECIFY ADDITIONAL FLAGS (OPTIONAL)
    cpm_srcs(&prop, cpm_glob_dir("./srcs", "*.c"));     // SPECIFY SRCS (REQUIRED), GLOBBING AVAILABLE
    cpm_include(&prop, "./Include");                    // SPECIFY INCLUDE DIRS (OPTIONAL)
    cpm_target(&prop, EXECUTABLE, "build/tgt");                   // SPECIFY TARGET (REQUIRED)
    CPMLOG(MSG, "Starting compilation");
    cpm_compile(&prop);                                  // COMPILE!
}