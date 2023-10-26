//  CPM.h written by Sum1Code, licensed under the MIT license
//  Copyright (c) 2023 Sum1Code

#pragma once
#ifndef __CPM_AVAIL_

#include <dirent.h>
#include <fnmatch.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define __GET_FILE_NAME_(name) \
  (strrchr(name, '/') ? strrchr(name, '/') + 1 : name)

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

typedef enum
{
  MSG,
  WARNING,
  ERROR
} LOG_LEVEL;

#define CPMLOG(loglevel, msg) _cpmlog(loglevel, msg);

typedef enum
{
  EXECUTABLE,
  STATICLIB,
  DYNLIB
} BuildType_e;

typedef enum
{
  D_CREATE,
  D_DELETE,
} DirOps_e;

typedef struct
{
  BuildType_e type;
  char *srcs;
  char *include_dir;
  char *project_name;
  char *compiler;
  char *flags;
  char *build_dir;
} BuildProperties_t;

typedef struct{
  char* str;
  size_t strsize;
  size_t strcap;
} StringBuilder_t;

StringBuilder_t* sb_new(size_t initial_size);
void sb_append(StringBuilder_t* sb, char c);
void sb_appendstr(StringBuilder_t* sb, char* str);
void sb_free(StringBuilder_t* sb);
char* sb_to_string(StringBuilder_t* sb);

void _cpmlog(LOG_LEVEL level, const char *msg);
void cpm_target(BuildProperties_t *buildprop, BuildType_e type,
                const char *name, const char *build_dir);
char *appendToString(const char *original, const char *append);
void cpm_init(BuildProperties_t *buildprop, char *compiler);
void _cpm_srcs_create(BuildProperties_t *buildprop, ...);
void _cpm_cflags_create(BuildProperties_t *buildprop, ...);
void cpm_dirops(DirOps_e operation, char *dirpath);
char *cpm_glob_dir(const char *dirpath, const char *pattern);
bool shouldRecompile(char *srcfile, char *execfile);

#ifdef CPM_IMPLEMENTATION // WILL BE RUN AT cpm_init
void cpm_setup();
#else
void cpm_setup()
{
  CPMLOG(ERROR, "PLEASE DEFINE cpm_setup() BY ADDING (#define "
                "CPM_IMPLEMENTATION) \nTO THE VERY TOP OF BUILD SCRIPT!");
}
#endif

void _cpmlog(LOG_LEVEL level, const char *msg)
{
  time_t current_time;
  struct tm *time_info;

  time(&current_time);
  time_info = localtime(&current_time);

  switch (level)
  {
  case MSG:
    printf("%sLOG (%02d:%02d:%02d): %s%s\n", KMAG, time_info->tm_hour,
           time_info->tm_min, time_info->tm_sec, msg, KNRM);
    break;
  case ERROR:
    printf("%sCOMPILATION HALTED: %s%s\n", KRED, msg, KNRM);
    perror("HALTED at:");
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

void cpm_target(BuildProperties_t *buildprop, BuildType_e type,
                const char *name, const char *build_dir)
{
  buildprop->project_name = (char *)name;
  buildprop->type = type;
  buildprop->build_dir = (char *)build_dir;
}

char *appendToString(const char *original, const char *append)
{
  int originalLength = strlen(original);
  int appendLength = strlen(append);

  // Calculate the new size needed for the combined string
  int newSize =
      originalLength + appendLength + 2; // +2 for space and null-terminator

  // Allocate memory for the combined string
  char *result = (char *)malloc(newSize);

  if (result == NULL)
  {
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
void cpm_init(BuildProperties_t *buildprop, char *compiler)
{
  cpm_setup();
  buildprop->compiler = (char *)malloc(STRING_ALLOC_SIZE);
  buildprop->flags = (char *)malloc(STRING_ALLOC_SIZE);
  buildprop->include_dir = (char *)malloc(STRING_ALLOC_SIZE);
  buildprop->project_name = (char *)malloc(STRING_ALLOC_SIZE);
  buildprop->srcs = (char *)malloc(STRING_ALLOC_SIZE);

  if (buildprop->compiler == NULL || buildprop->flags == NULL ||
      buildprop->include_dir == NULL || buildprop->project_name == NULL ||
      buildprop->srcs == NULL)
  {
    perror("Memory allocation error");
    exit(EXIT_FAILURE);
  }

  // Initialize all fields to empty strings (null-terminated)
  buildprop->compiler = compiler;
  buildprop->flags[0] = '\0';
  buildprop->include_dir = "./\0";
  buildprop->project_name[0] = '\0';
  buildprop->srcs[0] = '\0';
}

void _cpm_srcs_create(BuildProperties_t *buildprop, ...)
{
  va_list args;
  va_start(args, buildprop);
  char *arg = va_arg(args, char *);
  while (arg != NULL)
  {
    char *newSrc = appendToString(buildprop->srcs, arg);
    free(buildprop
             ->srcs);         // Free the old srcs if they were dynamically allocated
    buildprop->srcs = newSrc; // Update srcs with the new concatenated string
    arg = va_arg(args, char *);
  }
  va_end(args);
}

void _cpm_cflags_create(BuildProperties_t *buildprop, ...)
{
  va_list args;
  va_start(args, buildprop);
  char *arg = va_arg(args, char *);
  while (arg != NULL)
  {
    char *newflags = appendToString(buildprop->flags, arg);
    free(buildprop->flags);
    buildprop->flags = newflags;
    arg = va_arg(args, char *);
  }
  va_end(args);
}

void cpm_include(BuildProperties_t *prop, char *include)
{
  prop->include_dir = include;
}

void cpm_compile(BuildProperties_t *prop)
{
  size_t cmd_len = sizeof(prop->compiler) + sizeof(prop->include_dir) +
                   sizeof(prop->project_name) + sizeof(prop->flags) +
                   sizeof(prop->srcs) + 1024 * 2;
  char *cmdstr = (char *)malloc(cmd_len * 5);
  cmdstr[0] = '\0';

  strcat(cmdstr, prop->compiler);
  strcat(cmdstr, prop->flags);
  switch (prop->type)
  {
  case DYNLIB:
  {
    strcat(cmdstr, " -c -fpic ");
    char *new_name = (char *)malloc(sizeof(prop->project_name) + sizeof("lib"));
    new_name[0] = '\0';
    strcat(new_name, "lib");
    strcat(new_name, prop->project_name);
    prop->project_name = new_name;
  }
  break;

  case STATICLIB:
  {
    strcat(cmdstr, " -c ");
  }
  break;
  default:
    break;
  }
  strcat(cmdstr, prop->srcs);
  strcat(cmdstr, " -o ");
  strcat(cmdstr, prop->build_dir);
  strcat(cmdstr, "/");
  strcat(cmdstr, prop->project_name);
  strcat(cmdstr, " -I");
  strcat(cmdstr, prop->include_dir);

  CPMLOG(MSG, cmdstr);
  if (system(cmdstr) != 0)
  {
    CPMLOG(ERROR, "COMPILATION FAILED AT THE ERROR ON TOP!")
  };
}

#define cpm_flags(buildprop_ptr, ...) \
  _cpm_cflags_create(buildprop_ptr, __VA_ARGS__, NULL)
#define cpm_srcs(buildprop_ptr, ...) \
  _cpm_srcs_create(buildprop_ptr, __VA_ARGS__, NULL)

char *cpm_glob_dir(const char *dirpath, const char *pattern)
{
  DIR *dir;
  struct dirent *entry;

  if ((dir = opendir(dirpath)) == NULL)
  {
    perror("opendir");
    return NULL;
  }

  // Initialize a buffer to store the results
  char *result = (char *)malloc(5);
  result[0] = '\0';
  size_t resultSize = 0;

  while ((entry = readdir(dir)) != NULL)
  {
    if (fnmatch(pattern, entry->d_name, 0) == 0)
    {
      // Calculate the new size of the result string
      size_t newResultSize =
          resultSize + strlen(entry->d_name) + 1; // +1 for the space

      // Allocate memory for the updated result string
      char *newResult = (char *)realloc(result, newResultSize);
      if (!newResult)
      {
        perror("realloc");
        free(result);
        closedir(dir);
        return NULL;
      }

      if (newResult)
      result = newResult;

      // Append the matched file name and a space
      strcat(result, dirpath);
      strcat(result, "/");
      strcat(result, entry->d_name);
      strcat(result, " ");
      resultSize = newResultSize;
    }
  }

  closedir(dir);

  return result;
}

void cpm_dirops(DirOps_e operation, char *dirpath)
{
  char command[256];
  switch (operation)
  {
  case D_CREATE:

    snprintf(command, sizeof(command), "mkdir -p %s", dirpath);

    if (system(command) == 0)
    {
      CPMLOG(MSG, command);
    }
    else
    {
      CPMLOG(ERROR, "DIRECTORY CREATION FAILED!");
    }
    break;
  case D_DELETE:
    snprintf(command, sizeof(command), "rm -rf %s", dirpath);

    if (system(command) == 0)
    {
      CPMLOG(MSG, command);
    }
    else
    {
      CPMLOG(ERROR, "DIRECTORY DELETION FAILED!");
    }
    break;
  }
}

bool shouldRecompile(char *srcfile, char *execfile)
{
  struct stat sourceInfo, executableInfo;

  if (stat(srcfile, &sourceInfo) == -1)
    CPMLOG(ERROR, "Failed to stat source file");

  if (stat(execfile, &executableInfo) == -1)
    CPMLOG(ERROR, "Failed to stat executable file");

  return sourceInfo.st_mtime > executableInfo.st_mtime;
}

#define cpm_rebuild_self(argv)                                       \
  if (shouldRecompile(__FILE__, argv[0]))                            \
  {                                                                  \
    char *change_name_cmd = malloc(255);                             \
    sprintf(change_name_cmd, "mv %s %s.old", argv[0], argv[0]);      \
    CPMLOG(WARNING, "changing current executable to old");           \
    system(change_name_cmd);                                         \
    CPMLOG(WARNING, "recompiling build system");                     \
    BuildProperties_t prop;                                          \
    cpm_init(&prop, "cc");                                           \
    cpm_target(&prop, EXECUTABLE, __GET_FILE_NAME_(argv[0]), "./");  \
    cpm_srcs(&prop, __FILE__);                                       \
    cpm_compile(&prop);                                              \
    CPMLOG(WARNING, "RUNNING NEW BUILDER\n---------------------\n"); \
    if (system(argv[0]) != 0)                                        \
      CPMLOG(ERROR, "CANNOT RUN NEW BUILDER");                       \
    exit(0);                                                         \
  }

// SB IMPL
StringBuilder_t* sb_new(size_t initial_size){
  StringBuilder_t* sb_res = malloc(sizeof(StringBuilder_t));
  if(!sb_res) CPMLOG(ERROR, "String builder failed to create");
  sb_res->str = calloc(initial_size, sizeof(char));
  sb_res->strcap = initial_size;
  sb_res->strsize = 0;
  return sb_res;
}
void sb_append(StringBuilder_t* sb, char c){
  sb->str[sb->strsize] = c;
  ++sb->strsize;
  if (sb->strsize == sb->strcap){
    char* new_str = realloc(sb->str, sb->strcap * 2);
    if (!new_str) CPMLOG(ERROR, "FAILED TO REALLOC STRING");
    memset(new_str + sb->strcap, 0, sb->strcap);
    sb->str = new_str;
    sb->strcap *= 2;
  }
}
void sb_appendstr(StringBuilder_t* sb, char* str){
  for (size_t strsize = 0 ; strsize < strlen(str); strsize++){
    sb_append(sb, str[strsize]);
  }
}
void sb_free(StringBuilder_t* sb){
  free(sb->str);
  free(sb);
}

/// WARNING: sb_retstr WILL RETURN THE STRING CONTAINED BUT CONSUME THE STRING BUILDER
char* sb_to_string(StringBuilder_t* sb){
  char* ret = malloc(sb->strsize);
  ret = strncpy(ret, sb->str, sb->strsize);
  sb_free(sb);
  return ret; 
}


#define __CPM_AVAIL_
#endif
