/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#ifndef INC_MODULE
#define INC_MODULE

#include "depend.h"
#include "toy.h"
#include "toyio.h"
#include "list.h"

#define module_signature "MoDuLe"

enum apivalue_module_event_type
  {
    apivalue_module_event_type_loaded,
    apivalue_module_event_type_thread_started,
    apivalue_module_event_type_thread_stop_requested,
    apivalue_module_event_type_thread_stopped,
    apivalue_module_event_type_unload_requested,
    apivalue_module_event_type_unload,
    apivalue_module_event_type_zero = 0
  };

enum apivalue_module
  {
    apivalue_module_pointer_count = 4,
    apivalue_module_serial_length = 10,
    apivalue_module_signature_length = sizeof module_signature - 1,
    apivalue_module_zero = 0
  };

struct live_module;
union module;
struct module_api;
struct module_module;
struct module_required;
struct module_v1;

typedef void func_initialize_module_system(struct top *);
typedef int func_module_live_module_from_module(struct top *, union module *, struct dependency_want *, struct live_module **);
typedef int func_module_event(enum apivalue_module_event_type, void *);
typedef int func_module_load(struct top *, struct live_module *);
typedef void func_module_request_shutdown(struct top *);
typedef int func_module_start_thread(struct top *);
typedef int func_module_thread_started(struct top *, struct live_module *);
typedef void func_module_unload(struct top *, struct live_module *);
typedef void func_module_unload_all(struct top *);

extern func_initialize_module_system initialize_module_system;

struct module_api
  {
    struct api_dependency * api_dependency;
    struct api_list * api_list;
    struct api_stdio * api_stdio;
    struct api_stdlib * api_stdlib;
    func_module_live_module_from_module * live_module_from_module;
    func_module_load * load;
    func_module_request_shutdown * request_shutdown;
    func_module_start_thread * start_thread;
    func_module_thread_started * thread_started;
    func_module_unload * unload;
    func_module_unload_all * unload_all;
    struct list * module_list;
  };

struct module_required
  {
    char signature[apivalue_module_signature_length];
    char module_serial[apivalue_module_serial_length];
    unsigned int api_version;
  };

struct module_v1
  {
    struct module_required required;
    func_module_event * event_func;
    void * module_pointers[apivalue_module_pointer_count];
    char * nice_name;
  };

union module
  {
    struct module_v1 v1;
    struct module_required required;
  };

struct live_module
  {
    union module module;
    union module * origin;
    struct top * ctx;
    struct dependency dependency;
    int loaded;
  };

#endif /* INC_MODULE */
