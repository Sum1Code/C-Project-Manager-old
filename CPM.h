//  CPM.h written by NoctaDev, licensed under the MIT license
//  Copyright (c) 2023 NoctaDev

#pragma once
#ifndef __CPM_AVAIL_
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"


typedef enum { MSG, WARNING, ERROR } LOG_LEVEL;

void _cpmlog(LOG_LEVEL level, const char *msg) {
  time_t current_time;
  struct tm *time_info;

  time(&current_time);
  time_info = localtime(&current_time);

  switch (level) {
  case MSG:
    printf("%sLOG (%02d:%02d:%02d): %s%s\n", KMAG, time_info->tm_hour,
           time_info->tm_min, time_info->tm_sec, msg, KNRM);
    break;
  case ERROR:
    printf("%sCOMPILATION HALTED: %s%s\n", KRED, msg, KNRM);
    exit(1);
    break;
  case WARNING:
    printf("%sWARNING (%02d:%02d:%02d): %s%s\n", KYEL, time_info->tm_hour,
           time_info->tm_min, time_info->tm_sec, msg, KNRM);
    break;
  default:
    printf("%sInvalid Log_level%s\n", KMAG, KNRM);
    break;
  }
}
#define CPMLOG(loglevel, msg) _cpmlog(loglevel, msg);


typedef enum {
  EXECUTABLE,
  STATICLIB,
  DYNLIB,
} BuildType_e;

typedef struct {
  BuildType_e type;
  char *srcs;
  char *include_dir;
  char *project_name;
  char *compiler;
  char *flags;
} BuildProperties_t;

void cpm_target(BuildProperties_t* buildprop, BuildType_e type,
                const char *name) {
  buildprop->project_name = (char *)name;
  buildprop->type = type;
}

char *appendToString(const char *original, const char *append) {
  int originalLength = strlen(original);
  int appendLength = strlen(append);

  // Calculate the new size needed for the combined string
  int newSize = originalLength + appendLength + 2; // +2 for space and null-terminator

  // Allocate memory for the combined string
  char *result = (char *)malloc(newSize);

  if (result == NULL) {
    perror("Memory allocation error");
    exit(EXIT_FAILURE);
  }

  // Copy the original string to the result
  strcpy(result, original);

  // Add a space
  strcat(result, " ");

  // Concatenate the append string
  strcat(result, append);

  return result;
}

#define STRING_ALLOC_SIZE 255
void cpm_init(BuildProperties_t *buildprop) {
  buildprop->compiler = (char *)malloc(STRING_ALLOC_SIZE);
  buildprop->flags = (char *)malloc(STRING_ALLOC_SIZE);
  buildprop->include_dir = (char *)malloc(STRING_ALLOC_SIZE);
  buildprop->project_name = (char *)malloc(STRING_ALLOC_SIZE);
  buildprop->srcs = (char *)malloc(STRING_ALLOC_SIZE);

  if (buildprop->compiler == NULL || buildprop->flags == NULL || buildprop->include_dir == NULL || buildprop->project_name == NULL || buildprop->srcs == NULL) {
    perror("Memory allocation error");
    exit(EXIT_FAILURE);
  }

  // Initialize all fields to empty strings (null-terminated)
  buildprop->compiler = "clang\0";
  buildprop->flags[0] = '\0';
  buildprop->include_dir = "./\0";
  buildprop->project_name[0] = '\0';
  buildprop->srcs[0] = '\0';
}


void _cpm_srcs_create(BuildProperties_t *buildprop, ...) {
  va_list args;
  va_start(args, buildprop);
  char *arg = va_arg(args, char *);
  while (arg != NULL) {
    char *newSrc = appendToString(buildprop->srcs, arg);
    free(buildprop->srcs); // Free the old srcs if they were dynamically allocated
    buildprop->srcs = newSrc; // Update srcs with the new concatenated string
    arg = va_arg(args, char *);
  }
  va_end(args);
}

void _cpm_cflags_create(BuildProperties_t *buildprop, ...) {
  va_list args;
  va_start(args, buildprop);
  char *arg = va_arg(args, char *);
  while (arg != NULL) {
    char *newflags = appendToString(buildprop->flags, arg);
    free(buildprop->flags); 
    buildprop->flags = newflags;
    arg = va_arg(args, char *);
  }
  va_end(args);
}

void cpm_compiler(BuildProperties_t* prop, char* compiler_name){
    prop->compiler = compiler_name;
}

void cpm_include(BuildProperties_t* prop, char* include ){
  prop->include_dir = include;
}

void cpm_compile(BuildProperties_t* prop){
    size_t cmd_len = sizeof(prop->compiler) + sizeof(prop->include_dir) + sizeof(prop->project_name) + sizeof(prop->flags) + sizeof(prop->srcs) + 255 * 2;
    char* cmdstr = malloc(cmd_len);
    cmdstr[0] = '\0';

    strcat(cmdstr, prop->compiler);
    strcat(cmdstr, prop->flags);
    switch(prop->type){
      case DYNLIB:
      strcat(cmdstr, " -c -fpic ");
      char* new_name = malloc(sizeof(prop->project_name) + sizeof("lib"));
      new_name[0] = '\0';
      strcat(new_name, "lib");
      strcat(new_name, prop->project_name);
      prop->project_name = new_name;
        break;
      case STATICLIB:
      strcat(cmdstr, " -c ");
        break;
      default:
        break;
    }
    strcat(cmdstr, prop->srcs);
    strcat(cmdstr, " -o ");
    strcat(cmdstr, prop->project_name);
    strcat(cmdstr, " -I");
    strcat(cmdstr, prop->include_dir);

    printf("Command: %s\n", cmdstr);
    if (system(cmdstr) != 0){
      CPMLOG(ERROR, "COMPILATION FAILED!")
    };

}

#define cpm_flags(buildprop_ptr, ...)                                         \
    _cpm_cflags_create(buildprop_ptr, __VA_ARGS__, NULL)
#define cpm_srcs(buildprop_ptr, ...)                                         \
  _cpm_srcs_create(buildprop_ptr,  __VA_ARGS__, NULL)

#define __CPM_AVAIL_
#endif
