/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#ifndef INC_DEPEND
#define INC_DEPEND

#include "list.h"

enum apivalue_dependency
  {
    apivalue_dependency_success,
    apivalue_dependency_error_going_away,
    apivalue_dependency_error_gone,
    apivalue_dependency_error_not_ready,
    apivalue_dependency_error_not_wanted,
    apivalue_dependency_error_not_yet_provided,
    apivalue_dependency_error_null_argument,
    apivalue_dependency_need_arrived,
    apivalue_dependency_need_departed,
    apivalue_dependency_not_needed,
    apivalue_dependency_now_ready,
    apivalue_dependency_observer_arrived,
    apivalue_dependency_observer_departed,
    apivalue_dependency_provider_arrived,
    apivalue_dependency_provider_departed,
    apivalue_dependency_readiness_provider_arrived,
    apivalue_dependency_readiness_provider_departed,
    apivalue_dependency_want_arrived,
    apivalue_dependency_want_departed,
    apivalue_dependency_zero = 0
  };

struct api_dependency;
struct dependency;
struct dependency_need;
struct dependency_observe;
struct dependency_provide;
struct dependency_ready;
struct dependency_want;

typedef void apifunction_dependency_check_release(struct api_dependency *, struct dependency *);
typedef void apifunction_dependency_go_away(struct api_dependency *, struct dependency *);
typedef enum apivalue_dependency apifunction_dependency_api_initialize(struct api_dependency *);
typedef void apifunction_dependency_initialize_dependency(struct api_dependency *, struct dependency *);
typedef void apifunction_dependency_initialize_need(struct api_dependency *, struct dependency_need *);
typedef void apifunction_dependency_initialize_provide(struct api_dependency *, struct dependency_provide *);
typedef void apifunction_dependency_initialize_ready(struct api_dependency *, struct dependency_ready *);
typedef void apifunction_dependency_initialize_want(struct api_dependency *, struct dependency_want *);
typedef enum apivalue_dependency apifunction_dependency_is_ready(struct api_dependency *, struct dependency *);
typedef enum apivalue_dependency apifunction_dependency_meets_needs(struct api_dependency *, struct dependency *);
typedef enum apivalue_dependency apifunction_dependency_need(struct api_dependency *, struct dependency *, struct dependency_need *);
typedef void apifunction_dependency_notify(struct api_dependency *, struct dependency *, enum apivalue_dependency, void *);
typedef void apifunction_dependency_notify_one(struct api_dependency *, struct dependency *, struct dependency_observe *, enum apivalue_dependency, void *);
typedef void apifunction_dependency_observe_call(struct api_dependency *, struct dependency *, struct dependency_observe *, enum apivalue_dependency, void *);
typedef void apifunction_dependency_initialize_observe(struct api_dependency *, struct dependency_observe *, apifunction_dependency_observe_call *);
typedef enum apivalue_dependency apifunction_dependency_ready(struct api_dependency *, struct dependency *, struct dependency_ready *);
typedef enum apivalue_dependency apifunction_dependency_observe(struct api_dependency *, struct dependency *, struct dependency_observe *);
typedef enum apivalue_dependency apifunction_dependency_provide(struct api_dependency *, struct dependency *, struct dependency_provide *);
typedef void apifunction_dependency_swap_need(struct api_dependency *, struct dependency *, struct dependency_need *, struct dependency_need *);
typedef void apifunction_dependency_swap_observe(struct api_dependency *, struct dependency *, struct dependency_observe *, struct dependency_observe *);
typedef void apifunction_dependency_swap_provide(struct api_dependency *, struct dependency *, struct dependency_provide *, struct dependency_provide *);
typedef void apifunction_dependency_swap_ready(struct api_dependency *, struct dependency *, struct dependency_ready *, struct dependency_ready *);
typedef void apifunction_dependency_swap_want(struct api_dependency *, struct dependency *, struct dependency_want *, struct dependency_want *);
typedef void apifunction_dependency_unneed(struct api_dependency *, struct dependency *, struct dependency_need *);
typedef void apifunction_dependency_unobserve(struct api_dependency *, struct dependency *, struct dependency_observe *);
typedef void apifunction_dependency_unprovide(struct api_dependency *, struct dependency *, struct dependency_provide *);
typedef void apifunction_dependency_unready(struct api_dependency *, struct dependency *, struct dependency_ready *);
typedef void apifunction_dependency_unwant(struct api_dependency *, struct dependency *, struct dependency_want *);
typedef enum apivalue_dependency apifunction_dependency_want(struct api_dependency *, struct dependency *, struct dependency_want *);

extern apifunction_dependency_api_initialize api_dependency_initialize;

struct dependency
  {
    /* LIFO */
    struct list observe;
    /* FIFO */
    struct list need;
    struct list provide;
    struct list ready;
    struct list want;
    /* Shut down */
    enum apivalue_dependency going_away;
  };

struct api_dependency
  {
    struct api_list * api_list;
    apifunction_dependency_api_initialize * api_initialize;
    apifunction_dependency_check_release * check_release;
    apifunction_dependency_go_away * go_away;
    apifunction_dependency_is_ready * is_ready;
    apifunction_dependency_meets_needs * meets_needs;
    apifunction_dependency_notify * notify;
    apifunction_dependency_notify_one * notify_one;
    apifunction_dependency_ready * dependency_ready;
    apifunction_dependency_initialize_dependency * initialize;
    apifunction_dependency_initialize_need * initialize_need;
    apifunction_dependency_initialize_observe * initialize_observe;
    apifunction_dependency_initialize_provide * initialize_provide;
    apifunction_dependency_initialize_ready * initialize_ready;
    apifunction_dependency_initialize_want * initialize_want;
    apifunction_dependency_need * need;
    apifunction_dependency_observe * observe;
    apifunction_dependency_provide * provide;
    apifunction_dependency_swap_need * swap_need;
    apifunction_dependency_swap_observe * swap_observe;
    apifunction_dependency_swap_provide * swap_provde;
    apifunction_dependency_swap_ready * swap_ready;
    apifunction_dependency_swap_want * swap_want;
    apifunction_dependency_unneed * unneed;
    apifunction_dependency_unobserve * unobserve;
    apifunction_dependency_unprovide * unprovide;
    apifunction_dependency_unready * unready;
    apifunction_dependency_unwant * unwant;
    apifunction_dependency_want * want;
  };

struct dependency_need
  {
    struct list_item list_item;
  };

struct dependency_observe
  {
    apifunction_dependency_observe_call * call;
    struct list_item list_item;
  };

struct dependency_provide
  {
    struct list_item list_item;
  };

struct dependency_ready
  {
    struct list_item list_item;
  };

struct dependency_want
  {
    struct list_item list_item;
  };

#endif /* INC_DEPEND */
