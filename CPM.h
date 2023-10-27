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

#define GET_FILE_NAME(name) \
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

typedef struct StringBuilder
{
  char *str;
  size_t strsize;
  size_t strcap;
} StringBuilder_t;

typedef struct StringSplice
{
  char **strsplice;
  char delim;
  size_t splice_count;
} StringSplice_t;

StringBuilder_t *sb_new(size_t initial_size);
void sb_append(StringBuilder_t *sb, char c);
void sb_appendstr(StringBuilder_t *sb, char *str);
void sb_free(StringBuilder_t *sb);
char *sb_to_string(StringBuilder_t *sb);
StringSplice_t *sb_split_at(StringBuilder_t *builder, char *delim);
StringBuilder_t* sb_copy(StringBuilder_t* sb);

void _cpmlog(LOG_LEVEL level, const char *msg);
void cpm_target(BuildProperties_t *buildprop, BuildType_e type, const char *name, const char *build_dir);
void cpm_init(BuildProperties_t *buildprop, char *compiler);
void _cpm_srcs_create(BuildProperties_t *buildprop, ...);
void _cpm_cflags_create(BuildProperties_t *buildprop, ...);
void cpm_dirops(DirOps_e operation, char *dirpath);
char *cpm_glob_dir(char *dirpath, const char *pattern);
bool shouldRecompile(char *srcfile, char *execfile);

bool cpm_setup_warning_once = false;


#define sb_append_strspace(string_builder_ptr, str) \
  sb_appendstr(string_builder_ptr, str);            \
  sb_append(string_builder_ptr, ' ');

#ifdef CPM_IMPLEMENTATION // WILL BE RUN AT cpm_init
void cpm_setup();
#else
void cpm_setup()
{
  if (!cpm_setup_warning_once)
  {
    CPMLOG(WARNING, "using empty cpm_setup(), define by adding (#define CPM_IMPLEMENTATION) before including cpm.h and then define cpm_setup in your build.c \n");
    cpm_setup_warning_once = true;
  }
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
  buildprop->include_dir[0] = '\0';
  buildprop->project_name[0] = '\0';
  buildprop->srcs[0] = '\0';
}

void _cpm_srcs_create(BuildProperties_t *buildprop, ...)
{
  va_list args;
  va_start(args, buildprop);
  char *arg = va_arg(args, char *);
  StringBuilder_t *builder = sb_new(1);
  while (arg != NULL)
  {
    sb_appendstr(builder, arg);
    // char *newSrc = appendToString(buildprop->srcs, arg);
    // free(buildprop
    //          ->srcs);         // Free the old srcs if they were dynamically allocated
    // buildprop->srcs = newSrc; // Update srcs with the new concatenated string
    arg = va_arg(args, char *);
  }
  va_end(args);
  buildprop->srcs = sb_to_string(builder);
}

void _cpm_cflags_create(BuildProperties_t *buildprop, ...)
{
  va_list args;
  va_start(args, buildprop);
  StringBuilder_t *resbuild = sb_new(1);
  char *arg = va_arg(args, char *);
  while (arg != NULL)
  {
    sb_append_strspace(resbuild, arg);
    arg = va_arg(args, char *);
  }
  va_end(args);
  buildprop->flags = sb_to_string(resbuild);
}

void sb_patsubst(StringBuilder_t* sb, const char* text_to_replace, const char* replacement) {
    size_t text_len = strlen(text_to_replace);
    size_t replace_len = strlen(replacement);

    char* start = sb->str;
    while ((start = strstr(start, text_to_replace)) != NULL) {
        // Calculate the length of the portion before the match
        size_t prefix_len = start - sb->str;

        // Calculate the length of the portion after the match
        size_t suffix_len = sb->strsize - prefix_len - text_len;

        // Allocate a temporary buffer for the modified string
        char* temp_buffer = (char*)malloc(prefix_len + replace_len + suffix_len + 1);

        // Copy the prefix
        strncpy(temp_buffer, sb->str, prefix_len);
        temp_buffer[prefix_len] = '\0';

        // Copy the replacement
        strcat(temp_buffer, replacement);

        // Copy the suffix
        strcat(temp_buffer, start + text_len);

        // Update StringBuilder's buffer
        free(sb->str);
        sb->str = temp_buffer;
        sb->strsize = prefix_len + replace_len + suffix_len;

        // Move 'start' ahead to avoid an infinite loop
        start = sb->str + prefix_len + replace_len;
    }
}

StringBuilder_t* sb_copy(StringBuilder_t* sb){
  StringBuilder_t* cpy = malloc(sizeof(StringBuilder_t));
  cpy->str = malloc(sb->strcap);
  cpy->strcap = sb->strcap;
  cpy->strsize = sb->strsize;
  strncpy(cpy->str, sb->str, sb->strsize);
  return cpy;
}




void cpm_compile(BuildProperties_t *prop)
{
  StringBuilder_t *sbuild = sb_new(1);
  sb_append_strspace(sbuild, prop->compiler);
  sb_append_strspace(sbuild, prop->flags);
  switch (prop->type)
  {
  case DYNLIB:
    sb_append_strspace(sbuild, "-shared -fpic -c");
    break;
  case STATICLIB:
    sb_append_strspace(sbuild, "-c");
    break;
  case EXECUTABLE:
    break;
  default:
    CPMLOG(ERROR, "build type not supported");
    break;
  }
  sb_append_strspace(sbuild, "-o");
  sb_appendstr(sbuild, prop->build_dir);
  sb_appendstr(sbuild, "/");
  sb_append_strspace(sbuild, prop->project_name);
  sb_append_strspace(sbuild, prop->srcs);
  char* cmdstr = sb_to_string(sbuild);
  CPMLOG(MSG, cmdstr);
  system(cmdstr);
}

#define cpm_flags(buildprop_ptr, ...) \
  _cpm_cflags_create(buildprop_ptr, __VA_ARGS__, NULL)
#define cpm_srcs(buildprop_ptr, ...) \
  _cpm_srcs_create(buildprop_ptr, __VA_ARGS__, NULL)

char *cpm_glob_dir(char *dirpath, const char *pattern)
{
  DIR *dir;
  struct dirent *entry;
  StringBuilder_t *resbuild = sb_new(1);
  if ((dir = opendir(dirpath)) == NULL)
  {
    perror("opendir");
    return NULL;
  }

  while ((entry = readdir(dir)) != NULL)
  {
    if (fnmatch(pattern, entry->d_name, 0) == 0)
    {
      sb_appendstr(resbuild, dirpath);
      sb_append(resbuild, '/');
      sb_appendstr(resbuild, entry->d_name);
      sb_append(resbuild, ' ');
    }
  }

  closedir(dir);
  char *result = sb_to_string(resbuild);
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
    cpm_target(&prop, EXECUTABLE, GET_FILE_NAME(argv[0]), "./");     \
    cpm_srcs(&prop, __FILE__);                                       \
    cpm_compile(&prop);                                              \
    CPMLOG(WARNING, "RUNNING NEW BUILDER\n---------------------\n"); \
    if (system(argv[0]) != 0)                                        \
      CPMLOG(ERROR, "CANNOT RUN NEW BUILDER");                       \
    exit(0);                                                         \
  }

// SB IMPL
StringBuilder_t *sb_new(size_t initial_size)
{
  StringBuilder_t *sb_res = malloc(sizeof(StringBuilder_t));
  if (!sb_res)
    CPMLOG(ERROR, "String builder failed to create");
  sb_res->str = calloc(initial_size, sizeof(char));
  sb_res->strcap = initial_size;
  sb_res->strsize = 0;
  return sb_res;
}
void sb_append(StringBuilder_t *sb, char c)
{
  sb->str[sb->strsize] = c;
  ++sb->strsize;
  if (sb->strsize == sb->strcap)
  {
    char *new_str = realloc(sb->str, sb->strcap * 2);
    if (!new_str)
      CPMLOG(ERROR, "FAILED TO REALLOC STRING");
    memset(new_str + sb->strcap, 0, sb->strcap);
    sb->str = new_str;
    sb->strcap *= 2;
  }
}
void sb_appendstr(StringBuilder_t *sb, char *str)
{
  for (size_t strsize = 0; strsize < strlen(str); strsize++)
  {
    sb_append(sb, str[strsize]);
  }
}
void sb_free(StringBuilder_t *sb)
{
  free(sb->str);
  free(sb);
}

/// WARNING: sb_retstr WILL RETURN THE STRING CONTAINED BUT CONSUME THE STRING BUILDER
char *sb_to_string(StringBuilder_t *sb)
{
  char *ret = malloc(sb->strsize);
  ret = strcpy(ret, sb->str);
  sb_free(sb);
  return ret;
}

#define STRING_SPLICE_INIT_SIZE 5
/// Borrows StringBuilder and create a unique StringSplice that should be immutable
StringSplice_t *sb_split_at(StringBuilder_t *builder, char *delim)
{
  StringSplice_t *splice = malloc(sizeof(StringSplice_t));
  splice->splice_count = 0;
  splice->delim = 0;
  splice->strsplice = (char **)calloc(STRING_SPLICE_INIT_SIZE, sizeof(char *));
  char *src_cpy = strdup(builder->str);
  char *tok = strtok(src_cpy, delim);
  while (tok)
  {
    CPMLOG(MSG, tok);
    ++splice->splice_count;
    size_t copytoken_size = strlen(tok);
    splice->strsplice[splice->splice_count - 1] = malloc(copytoken_size * sizeof(char));
    splice->strsplice[splice->splice_count - 1] = strdup(tok);
    tok = strtok(NULL, delim);
  }
  return splice;
}

#define __CPM_AVAIL_
#endif
