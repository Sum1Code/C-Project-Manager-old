#define CPM_IMPLEMENTATION      // IMPORTANT TO IMPLEMENT cpm_setup OTHERWISE ERROR
#include "CPM.h"                // INCLUDE AFTER CPM_IMPLEMENTATION

void cpm_setup(){               // IMPLEMENT THIS ON YOUR OWN 
    cpm_dircreate("./build");   //DIRCREATE TO CREATE DIRECTORIES
    cpm_dircreate("./srcs");   
}

int main(void){
    BuildProperties_t prop;                             // MAIN BUILD PROPERTIES 
    CPMLOG(WARNING, "EXAMPLE_CONFIG_USED")  
    cpm_init(&prop);                                    // INIT BUILD PROPS AND RUNS cpm_setup()
    cpm_compiler(&prop, "clang");                       // SPECIFY COMPILER (REQUIRED)
    cpm_flags(&prop, "-g", "-Wall", "-Wextra");         // SPECIFY ADDITIONAL FLAGS (OPTIONAL)
    cpm_srcs(&prop, cpm_glob_dir("./srcs", "*.c"));     // SPECIFY SRCS (REQUIRED), GLOBBING AVAILABLE
    cpm_include(&prop, "./Include");                    // SPECIFY INCLUDE DIRS (OPTIONAL)
    cpm_target(&prop, DYNLIB, "tgt");                   // SPECIFY TARGET (REQUIRED)
    CPMLOG(MSG, "Starting Compilation");
    cpm_compile(&prop);                                 // COMPILE!
}