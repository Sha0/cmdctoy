/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <stdlib.h>
#include "depend.h"
#include "toy.h"
#include "toydef.h"
#include "list.h"

/*
 * Overview:
 *
 *   "needs" are a set of to-do items
 *   "readiness" is a gate for the arrival of "needs"
 *   "provided" is a gate for the departure of successful "needs"
 *   "readiness" must be all of:
 *     "wanted"
 *     "not going away"
 *     "provided"
 *     "ready"
 *  "provided" requires at least one of:
 *    "wanted"
 *    "needed"
 *  "going away" means no new "needs" can ever be added
 *    "wanted" count reaching zero results in "going away"
 *    "provided" count reaching > zero, then reaching zero results in "going away"
 *    "going away" can also be specifically requested
 *  "observers"
 *    receive notifications of changes to the state
 *      including their own arrival and departure
 *    are force-removed when the dependency is released in every other way
 *
 * Examples:
 *
 *   1. "wanted"
 *      -> "provided"
 *         -> "ready"
 *            -> add to-dos
 *            -> perform to-dos
 *               -> "not ready"
 *                  -> perform to-dos
 *                     -> "ready" (again)
 *                        -> add to-dos
 *                        -> perform to-dos
 *                           -> "not wanted" so "not ready" and "going away"
 *                              -> perform to-dos
 *                                 -> finished
 *   2. "wanted"
 *      -> "provided"
 *         -> "ready"
 *            -> add to-dos
 *            -> perform to-dos
 *               -> "not provided" so "not ready" and "going away"
 *                  -> "provided" (again)
 *                     -> perform to-dos
 *                        -> finished
 */

static apifunction_dependency_check_release dependency_check_release;
static apifunction_dependency_go_away dependency_go_away;
static apifunction_dependency_is_ready dependency_is_ready;
static apifunction_dependency_meets_needs dependency_meets_needs;
static apifunction_dependency_notify dependency_notify;
static apifunction_dependency_notify_one dependency_notify_one;
static apifunction_dependency_ready dependency_ready;
static apifunction_dependency_initialize_dependency initialize_dependency;
static apifunction_dependency_initialize_need initialize_need;
static apifunction_dependency_initialize_observe initialize_observe;
static apifunction_dependency_initialize_provide initialize_provide;
static apifunction_dependency_initialize_ready initialize_ready;
static apifunction_dependency_initialize_want initialize_want;
static apifunction_dependency_need need_dependency;
static apifunction_dependency_observe observe_dependency;
static apifunction_dependency_provide provide_dependency;
static apifunction_dependency_swap_need swap_need;
static apifunction_dependency_swap_observe swap_observe;
static apifunction_dependency_swap_provide swap_provide;
static apifunction_dependency_swap_ready swap_ready;
static apifunction_dependency_swap_want swap_want;
static apifunction_dependency_unneed unneed_dependency;
static apifunction_dependency_unobserve unobserve_dependency;
static apifunction_dependency_unprovide unprovide_dependency;
static apifunction_dependency_unready unready_dependency;
static apifunction_dependency_unwant unwant_dependency;
static apifunction_dependency_want want_dependency;

static struct api_dependency api_dependency_defaults =
  {
    NULL,
    &api_dependency_initialize,
    &dependency_check_release,
    &dependency_go_away,
    &dependency_is_ready,
    &dependency_meets_needs,
    &dependency_notify,
    &dependency_notify_one,
    &dependency_ready,
    &initialize_dependency,
    &initialize_need,
    &initialize_observe,
    &initialize_provide,
    &initialize_ready,
    &initialize_want,
    &need_dependency,
    &observe_dependency,
    &provide_dependency,
    &swap_need,
    &swap_observe,
    &swap_provide,
    &swap_ready,
    &swap_want,
    &unneed_dependency,
    &unobserve_dependency,
    &unprovide_dependency,
    &unready_dependency,
    &unwant_dependency,
    &want_dependency
  };

static void dependency_check_release(struct api_dependency * api, struct dependency * dependency)
  {
    struct dependency_observe * dependency_observe;
    struct api_list * list_api;
    struct list_item * list_item;
    struct list swap;

    if (api == NULL || dependency == NULL)
      return;
    list_api = api->api_list;
    if (!list_api->list_is_empty(list_api, &dependency->need))
      return;
    if (dependency->going_away == apivalue_dependency_success)
      return;
    if (!list_api->list_is_empty(list_api, &dependency->provide))
      return;
    if (!list_api->list_is_empty(list_api, &dependency->ready))
      return;
    if (!list_api->list_is_empty(list_api, &dependency->want))
      return;
    /* Otherwise, the dependency has been released except maybe for observers */
    if (list_api->list_is_empty(list_api, &dependency->observe))
      /* Possible leak */
      return;
    /* Move the observers to a temporary list */
    swap.head.next = dependency->observe.head.next;
    swap.head.previous = dependency->observe.head.previous;
    swap.head.next->previous = &swap.head;
    swap.head.previous->next = &swap.head;
    dependency->observe.head.next = &dependency->observe.head;
    dependency->observe.head.previous = &dependency->observe.head;
    /* No more pointers appear in the dependency, at this point */
    while ((list_item = list_api->remove_item_from_list_head(list_api, &swap)) != NULL)
      {
        dependency_observe = type_with_member_at_ptr(struct dependency_observe, list_item, list_item);
        /* Final deallocation should be in the final observer */
        api->notify_one(api, dependency, dependency_observe, apivalue_dependency_observer_departed, dependency_observe);
      }
    /* The dependency might have been freed by the final observer, but there is no further read of its pointer */
  }

static void dependency_go_away(struct api_dependency * api, struct dependency * dependency)
  {
    struct api_list * list_api;

    if (api == NULL || dependency == NULL)
      return;
    list_api = api->api_list;
    if (dependency->going_away == apivalue_dependency_success)
      {
        dependency->going_away = apivalue_dependency_error_going_away;
        if (!list_api->list_is_empty(list_api, &dependency->provide) && !list_api->list_is_empty(list_api, &dependency->ready) && !list_api->list_is_empty(list_api, &dependency->want))
          api->notify(api, dependency, apivalue_dependency_error_not_ready, NULL);
        api->notify(api, dependency, apivalue_dependency_error_going_away, NULL);
      }
    api->check_release(api, dependency);
  }

static enum apivalue_dependency dependency_is_ready(struct api_dependency * api, struct dependency * dependency)
  {
    struct api_list * list_api;

    if (api == NULL || dependency == NULL)
      return apivalue_dependency_error_null_argument;
    list_api = api->api_list;
    if (dependency->going_away != apivalue_dependency_success)
      return apivalue_dependency_error_going_away;
    if (list_api->list_is_empty(list_api, &dependency->provide))
      return apivalue_dependency_error_not_yet_provided;
    if (list_api->list_is_empty(list_api, &dependency->ready))
      return apivalue_dependency_error_not_ready;
    if (list_api->list_is_empty(list_api, &dependency->want))
      return apivalue_dependency_error_not_wanted;
    return apivalue_dependency_success;
  }

static enum apivalue_dependency dependency_meets_needs(struct api_dependency * api, struct dependency * dependency)
  {
    struct api_list * list_api;

    if (api == NULL || dependency == NULL)
      return apivalue_dependency_error_null_argument;
    list_api = api->api_list;
    if (list_api->list_is_empty(list_api, &dependency->provide))
      return apivalue_dependency_error_gone;
    return apivalue_dependency_success;
  }

static void dependency_notify(struct api_dependency * api, struct dependency * dependency, enum apivalue_dependency type, void * pointer)
  {
    struct list_item hold;
    struct api_list * list_api;
    struct list_item * list_item;
    struct dependency_observe * dependency_observe;

    if (api == NULL || dependency == NULL)
      return;
    list_api = api->api_list;
    /* We are looping on a list in the dependency, so temporarily hold it via a fake "want" */
    (void) list_api->add_item_to_list_tail(list_api, &hold, &dependency->want);
    for (list_item = dependency->observe.head.next; list_item != &dependency->observe.head; list_item = list_item->next)
      {
        dependency_observe = type_with_member_at_ptr(struct dependency_observe, list_item, list_item);
        api->notify_one(api, dependency, dependency_observe, type, pointer);
      }
    /* And now release it */
    (void) list_api->remove_list_item(list_api, &hold);
  }

static void dependency_notify_one(struct api_dependency * api, struct dependency * dependency, struct dependency_observe * dependency_observe, enum apivalue_dependency type, void * pointer)
  {
    dependency_observe->call(api, dependency, dependency_observe, type, pointer);
  }

static enum apivalue_dependency dependency_ready(struct api_dependency * api, struct dependency * dependency, struct dependency_ready * dependency_ready)
  {
    struct api_list * list_api;
    int was_not_ready;

    if (api == NULL || dependency == NULL || dependency_ready == NULL)
      return apivalue_dependency_error_null_argument;
    list_api = api->api_list;
    if (dependency->going_away != apivalue_dependency_success)
      return apivalue_dependency_error_going_away;
    if (list_api->list_is_empty(list_api, &dependency->provide))
      return apivalue_dependency_error_not_yet_provided;
    if (list_api->list_is_empty(list_api, &dependency->want))
      return apivalue_dependency_error_not_wanted;
    was_not_ready = list_api->list_is_empty(list_api, &dependency->ready);
    list_api->add_item_to_list_tail(list_api, &dependency_ready->list_item, &dependency->ready);
    api->notify(api, dependency, apivalue_dependency_readiness_provider_arrived, dependency_ready);
    if (was_not_ready)
      api->notify(api, dependency, apivalue_dependency_now_ready, dependency_ready);
    return apivalue_dependency_success;
  }

static void initialize_dependency(struct api_dependency * api, struct dependency * dependency)
  {
    struct api_list * list_api;

    list_api = api->api_list;
    list_api->initialize_list(list_api, &dependency->need);
    list_api->initialize_list(list_api, &dependency->observe);
    list_api->initialize_list(list_api, &dependency->provide);
    list_api->initialize_list(list_api, &dependency->ready);
    list_api->initialize_list(list_api, &dependency->want);
    dependency->going_away = apivalue_dependency_success;
  }

static void initialize_need(struct api_dependency * api, struct dependency_need * dependency_need)
  {
    struct api_list * list_api;

    list_api = api->api_list;
    (void) list_api->initialize_list_item(list_api, &dependency_need->list_item);
  }

static void initialize_observe(struct api_dependency * api, struct dependency_observe * dependency_observe, apifunction_dependency_observe_call * call)
  {
    struct api_list * list_api;

    list_api = api->api_list;
    (void) list_api->initialize_list_item(list_api, &dependency_observe->list_item);
    dependency_observe->call = call;
  }

static void initialize_provide(struct api_dependency * api, struct dependency_provide * dependency_provide)
  {
    struct api_list * list_api;

    list_api = api->api_list;
    (void) list_api->initialize_list_item(list_api, &dependency_provide->list_item);
  }

static void initialize_ready(struct api_dependency * api, struct dependency_ready * dependency_ready)
  {
    struct api_list * list_api;

    list_api = api->api_list;
    (void) list_api->initialize_list_item(list_api, &dependency_ready->list_item);
  }

static void initialize_want(struct api_dependency * api, struct dependency_want * dependency_want)
  {
    struct api_list * list_api;

    list_api = api->api_list;
    (void) list_api->initialize_list_item(list_api, &dependency_want->list_item);
  }

enum apivalue_dependency api_dependency_initialize(struct api_dependency * api)
  {
    struct api_list * list_api;

    if (api->api_list == NULL)
      return apivalue_dependency_error_null_argument;
    list_api = api->api_list;
    *api = api_dependency_defaults;
    api->api_list = list_api;
    return apivalue_dependency_success;
  }

static enum apivalue_dependency need_dependency(struct api_dependency * api, struct dependency * dependency, struct dependency_need * dependency_need)
  {
    enum apivalue_dependency is_ready;
    struct api_list * list_api;

    is_ready = api->is_ready(api, dependency);
    if (is_ready != apivalue_dependency_success)
      return is_ready;
    if (dependency_need == NULL)
      return apivalue_dependency_error_null_argument;
    list_api = api->api_list;
    list_api->add_item_to_list_tail(list_api, &dependency_need->list_item, &dependency->need);
    api->notify(api, dependency, apivalue_dependency_need_arrived, dependency_need);
    return apivalue_dependency_success;
  }

static enum apivalue_dependency observe_dependency(struct api_dependency * api, struct dependency * dependency, struct dependency_observe * dependency_observe)
  {
    struct api_list * list_api;

    if (api == NULL || dependency == NULL || dependency_observe == NULL)
      return apivalue_dependency_error_null_argument;
    if (dependency->going_away != apivalue_dependency_success)
      return apivalue_dependency_error_going_away;
    list_api = api->api_list;
    if (list_api->list_is_empty(list_api, &dependency->want))
      return apivalue_dependency_error_not_wanted;
    /* LIFO */
    list_api->add_item_to_list_head(list_api, &dependency_observe->list_item, &dependency->observe);
    api->notify(api, dependency, apivalue_dependency_observer_arrived, dependency_observe);
    return apivalue_dependency_success;
  }

static enum apivalue_dependency provide_dependency(struct api_dependency * api, struct dependency * dependency, struct dependency_provide * dependency_provide)
  {
    struct api_list * list_api;

    if (api == NULL || dependency == NULL || dependency_provide == NULL)
      return apivalue_dependency_error_null_argument;
    list_api = api->api_list;
    if (dependency->going_away != apivalue_dependency_success && list_api->list_is_empty(list_api, &dependency->need))
      return apivalue_dependency_error_gone;
    list_api->add_item_to_list_tail(list_api, &dependency_provide->list_item, &dependency->provide);
    api->notify(api, dependency, apivalue_dependency_provider_arrived, dependency_provide);
    return apivalue_dependency_success;
  }

static void swap_need(struct api_dependency * api, struct dependency * dependency, struct dependency_need * old_need, struct dependency_need * new_need)
  {
    struct api_list * list_api;

    (void) dependency;

    list_api = api->api_list;
    (void) list_api->replace_list_item(list_api, &old_need->list_item, &new_need->list_item);
  }

static void swap_observe(struct api_dependency * api, struct dependency * dependency, struct dependency_observe * old_observe, struct dependency_observe * new_observe)
  {
    struct api_list * list_api;

    (void) dependency;

    list_api = api->api_list;
    (void) list_api->replace_list_item(list_api, &old_observe->list_item, &new_observe->list_item);
  }

static void swap_provide(struct api_dependency * api, struct dependency * dependency, struct dependency_provide * old_provide, struct dependency_provide * new_provide)
  {
    struct api_list * list_api;

    (void) dependency;

    list_api = api->api_list;
    (void) list_api->replace_list_item(list_api, &old_provide->list_item, &new_provide->list_item);
  }

static void swap_ready(struct api_dependency * api, struct dependency * dependency, struct dependency_ready * old_ready, struct dependency_ready * new_ready)
  {
    struct api_list * list_api;

    (void) dependency;

    list_api = api->api_list;
    (void) list_api->replace_list_item(list_api, &old_ready->list_item, &new_ready->list_item);
  }

static void swap_want(struct api_dependency * api, struct dependency * dependency, struct dependency_want * old_want, struct dependency_want * new_want)
  {
    struct api_list * list_api;

    (void) dependency;

    list_api = api->api_list;
    (void) list_api->replace_list_item(list_api, &old_want->list_item, &new_want->list_item);
  }

static void unneed_dependency(struct api_dependency * api, struct dependency * dependency, struct dependency_need * dependency_need)
  {
    struct api_list * list_api;

    if (api == NULL || dependency == NULL || dependency_need == NULL)
      return;
    list_api = api->api_list;
    list_api->remove_list_item(list_api, &dependency_need->list_item);
    api->notify(api, dependency, apivalue_dependency_need_departed, dependency_need);
    /* If there are no more needs, notify about that fact */
    if (list_api->list_is_empty(list_api, &dependency->need))
      api->notify(api, dependency, apivalue_dependency_not_needed, NULL);
    api->check_release(api, dependency);
  }

static void unobserve_dependency(struct api_dependency * api, struct dependency * dependency, struct dependency_observe * dependency_observe)
  {
    struct api_list * list_api;

    if (api == NULL || dependency == NULL || dependency_observe == NULL)
      return;
    list_api = api->api_list;
    list_api->remove_list_item(list_api, &dependency_observe->list_item);
    api->notify(api, dependency, apivalue_dependency_observer_departed, dependency_observe);
  }

static void unprovide_dependency(struct api_dependency * api, struct dependency * dependency, struct dependency_provide * dependency_provide)
  {
    struct api_list * list_api;

    if (api == NULL || dependency == NULL || dependency_provide == NULL)
      return;
    list_api = api->api_list;
    list_api->remove_list_item(list_api, &dependency_provide->list_item);
    api->notify(api, dependency, apivalue_dependency_provider_departed, dependency_provide);
    if (list_api->list_is_empty(list_api, &dependency->provide))
      api->go_away(api, dependency);
  }

static void unready_dependency(struct api_dependency * api, struct dependency * dependency, struct dependency_ready * dependency_ready)
  {
    struct api_list * list_api;

    if (api == NULL || dependency == NULL || dependency_ready == NULL)
      return;
    list_api = api->api_list;
    list_api->remove_list_item(list_api, &dependency_ready->list_item);
    api->notify(api, dependency, apivalue_dependency_readiness_provider_departed, dependency_ready);
    /* Check for change of readiness */
    if (!list_api->list_is_empty(list_api, &dependency->ready))
      return;
    if (dependency->going_away != apivalue_dependency_success)
      return;
    if (list_api->list_is_empty(list_api, &dependency->provide))
      return;
    if (list_api->list_is_empty(list_api, &dependency->want))
      return;
    /* Otherwise, change detected */
    api->notify(api, dependency, apivalue_dependency_error_not_ready, dependency_ready);
  }

static void unwant_dependency(struct api_dependency * api, struct dependency * dependency, struct dependency_want * dependency_want)
  {
    struct api_list * list_api;

    if (api == NULL || dependency == NULL || dependency_want == NULL)
      return;
    list_api = api->api_list;
    list_api->remove_list_item(list_api, &dependency_want->list_item);
    api->notify(api, dependency, apivalue_dependency_want_departed, dependency_want);
    if (list_api->list_is_empty(list_api, &dependency->want))
      api->go_away(api, dependency);
  }

static enum apivalue_dependency want_dependency(struct api_dependency * api, struct dependency * dependency, struct dependency_want * dependency_want)
  {
    struct api_list * list_api;

    if (api == NULL || dependency == NULL || dependency_want == NULL)
      return apivalue_dependency_error_null_argument;
    if (dependency->going_away != apivalue_dependency_success)
      return apivalue_dependency_error_going_away;
    list_api = api->api_list;
    list_api->add_item_to_list_tail(list_api, &dependency_want->list_item, &dependency->want);
    api->notify(api, dependency, apivalue_dependency_want_arrived, dependency_want);
    return apivalue_dependency_success;
  }
