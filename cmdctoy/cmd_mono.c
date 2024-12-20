/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <stdlib.h>
#include <string.h>
#include "builtins.h"
#include "command.h"
#include "toydef.h"
#include "toyscope.h"
#include "module.h"

struct cmd_monolith;

struct cmd_monolith
  {
    struct command command;
    struct live_module * live_module;
    struct top * ctx;
  };

static apifunction_command cmd_delete_identifier;
static apifunction_command cmd_find_identifier;
static apifunction_command cmd_list_identifiers;
static apifunction_command cmd_load_types;
static apifunction_command cmd_make_identifier;
static apifunction_command cmd_swap_scopes;
static func_module_event module_event;

static struct command command_delete_identifier;
static struct command command_find_identifier;
static struct command command_list_identifiers;
static struct command command_load_types;
static struct command command_make_identifier;
static struct command command_swap_scopes;
static struct live_module * live_module;

#if BUILTIN_CMD_MONO
union module builtin_module_cmd_monolith =
#else
union module module =
#endif
  {
    {
      {
        module_signature,
        "2024120200",
        1
      },
      &module_event,
      {
        NULL,
        NULL,
        NULL,
        NULL
      },
      "cmd_monolith"
    }
  };

static struct command command_delete_identifier =
  {
    NULL,
    "delete_identifier",
    &cmd_delete_identifier,
    {
      NULL,
      NULL
    }
  };

static struct command command_find_identifier =
  {
    NULL,
    "find_identifier",
    &cmd_find_identifier,
    {
      NULL,
      NULL
    }
  };

static struct command command_list_identifiers =
  {
    NULL,
    "list_identifiers",
    &cmd_list_identifiers,
    {
      NULL,
      NULL
    }
  };

static struct command command_load_types =
  {
    NULL,
    "load_types",
    &cmd_load_types,
    {
      NULL,
      NULL
    }
  };

static struct command command_make_identifier =
  {
    NULL,
    "make_identifier",
    &cmd_make_identifier,
    {
      NULL,
      NULL
    }
  };

static struct command command_swap_scopes =
  {
    NULL,
    "swap_scopes",
    &cmd_swap_scopes,
    {
      NULL,
      NULL
    }
  };

static int cmd_delete_identifier(struct api_command * api, struct command * command, int argc, char ** argv)
  {
    struct toy_scope_chain * chain;
    struct cmd_monolith * cmd;
    struct top * ctx;
    struct toy_scope_identifier * identifier;
    struct toy_scope * scope;
    struct api_stdio * stdio_api;
    struct api_stdlib * stdlib_api;
    struct api_toy_scope * toy_scope_api;
    enum apivalue_toy_scope toy_scope_rv;

    (void) api;

    cmd = type_with_member_at_ptr(struct cmd_monolith, command, command);
    ctx = cmd->ctx;
    stdio_api = ctx->api_stdio;
    stdlib_api = ctx->api_stdlib;
    toy_scope_api = ctx->api_toy_scope;
    chain = live_module->module.v1.module_pointers[0];

    if (argc != 2)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Usage:\n  delete_identifier IDENTIFIER\n");
        return EXIT_FAILURE;
      }

    /* Only delete from the top toy-scope */
    scope = chain->scopes[chain->count - 1];
    toy_scope_rv = toy_scope_api->find_identifier_in_scope(toy_scope_api, &identifier, argv[1], scope);
    if (toy_scope_rv == apivalue_toy_scope_error_not_found)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Identifier not found in the top toy-scope, but could be lower\n");
        return EXIT_FAILURE;
      }
    toy_scope_rv = toy_scope_api->remove_identifier_from_scope(toy_scope_api, identifier, scope);
    if (toy_scope_rv != apivalue_toy_scope_success)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Unable to remove identifier, with error '%d'\n", toy_scope_rv);
        return EXIT_FAILURE;
      }
    if (identifier->auto_free == 1)
      stdlib_api->free(stdlib_api, identifier);
    return EXIT_SUCCESS;
  }

static int cmd_find_identifier(struct api_command * api, struct command * command, int argc, char ** argv)
  {
    struct toy_scope_chain * chain;
    struct cmd_monolith * cmd;
    struct top * ctx;
    struct toy_scope_identifier * identifier;
    struct api_stdio * stdio_api;
    struct api_toy_scope * toy_scope_api;
    enum apivalue_toy_scope toy_scope_rv;

    (void) api;

    cmd = type_with_member_at_ptr(struct cmd_monolith, command, command);
    ctx = cmd->ctx;
    stdio_api = ctx->api_stdio;
    toy_scope_api = ctx->api_toy_scope;
    chain = live_module->module.v1.module_pointers[0];

    if (argc != 2)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Usage:\n  find_identifier IDENTIFIER\n");
        return EXIT_FAILURE;
      }

    toy_scope_rv = toy_scope_api->find_identifier_in_scope_chain(toy_scope_api, &identifier, argv[1], chain);
    if (toy_scope_rv == apivalue_toy_scope_error_not_found)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Identifier not found in the top toy-scope\n");
        return EXIT_FAILURE;
      }
    (void) stdio_api->fprintf(stdio_api, stdout, "Identifier found at %p\n", (void *) identifier);
    return EXIT_SUCCESS;
  }

static int cmd_list_identifiers(struct api_command * api, struct command * command, int argc, char ** argv)
  {
    struct api_btree * btree_api;
    struct btree_node * btree_node;
    struct toy_scope_chain * chain;
    struct cmd_monolith * cmd;
    struct top * ctx;
    size_t i;
    struct toy_scope_identifier * identifier;
    struct toy_scope * scope;
    size_t scope_count;
    struct api_stdio * stdio_api;

    (void) api;
    (void) argc;
    (void) argv;

    cmd = type_with_member_at_ptr(struct cmd_monolith, command, command);
    ctx = cmd->ctx;
    btree_api = ctx->api_btree;
    stdio_api = ctx->api_stdio;

    (void) stdio_api->fprintf(stdio_api, stdout, "Identifiers within the toy-scope are:\n");
    chain = live_module->module.v1.module_pointers[0];
    scope_count = chain->count;
    for (i = 0; i < scope_count; ++i)
      {
        (void) stdio_api->fprintf(stdio_api, stdout, "  Scope #%lu:\n", (unsigned long int) i);
        scope = chain->scopes[i];
        for (btree_node = btree_api->ordered_visit(btree_api, &scope->btree, NULL, apivalue_btree_direction_more); btree_node != NULL; btree_node = btree_api->ordered_visit(btree_api, &scope->btree, btree_node, apivalue_btree_direction_more))
          {
            identifier = type_with_member_at_ptr(struct toy_scope_identifier, btree_node, btree_node);
            (void) stdio_api->fprintf(stdio_api, stdout, "    %s\n", identifier->name);
          }
      }
    return EXIT_SUCCESS;
  }

static int cmd_load_types(struct api_command * api, struct command * command, int argc, char ** argv)
  {
    struct toy_scope_chain * chain;
    struct cmd_monolith * cmd;
    struct top * ctx;
    size_t i;
    struct toy_scope_identifier * identifier;
    int rv;
    struct toy_scope * scope;
    struct sd_type * sd_type;
    struct api_stdio * stdio_api;
    struct type * struct_type_ptr;
    struct api_toy_scope * toy_scope_api;
    enum apivalue_toy_scope toy_scope_rv;
    struct api_type * type_api;
    size_t type_count;

    (void) api;
    (void) argv;

    /* Assume success until proven otherwise */
    rv = EXIT_SUCCESS;

    cmd = type_with_member_at_ptr(struct cmd_monolith, command, command);
    ctx = cmd->ctx;
    stdio_api = ctx->api_stdio;
    toy_scope_api = ctx->api_toy_scope;
    type_api = ctx->api_type;
    chain = live_module->module.v1.module_pointers[0];
    scope = chain->scopes[0];

    if (argc != 1)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Usage:\n  load_types  Load \"sd types\" into the bottom toy-scope\n");
        return EXIT_FAILURE;
      }

    type_count = type_api->type_count;
    /* First, find the 'struct type *' type */
    for (i = 0; i < type_count; ++i)
      {
        sd_type = type_api->types + i;
        if (strcmp(sd_type->identifier, "sd_pointer_to_struct_type") == 0)
          break;
      }
    if (i == type_count)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Couldn't find the 'struct type *' type\n");
        return EXIT_FAILURE;
      }
    struct_type_ptr = sd_type->type;
    /* Populate */
    for (i = 0; i < type_count; ++i)
      {
        sd_type = type_api->types + i;
        toy_scope_rv = toy_scope_api->allocate_identifier(toy_scope_api, &identifier, sd_type->identifier, struct_type_ptr);
        if (toy_scope_rv != apivalue_toy_scope_success)
          {
            (void) stdio_api->fprintf(stdio_api, stderr, "Allocating identifier failed with error '%d'\n", toy_scope_rv);
            rv = EXIT_FAILURE;
            continue;
          }
        /* Copy the 'struct type *' into the identifier's newly-allocated storage */
        memcpy(identifier->value, &sd_type->type, sizeof sd_type->type);
        /* Add it to the bottom scope */
        toy_scope_rv = toy_scope_api->add_identifier_to_scope(toy_scope_api, identifier, scope);
        if (toy_scope_rv != apivalue_toy_scope_success)
          {
            (void) stdio_api->fprintf(stdio_api, stderr, "Adding identifier failed with error '%d'\n", toy_scope_rv);
            rv = EXIT_FAILURE;
            continue;
          }
      }
    return rv;
  }

static int cmd_make_identifier(struct api_command * api, struct command * command, int argc, char ** argv)
  {
    struct toy_scope_chain * chain;
    struct cmd_monolith * cmd;
    struct top * ctx;
    struct toy_scope_identifier * identifier;
    struct toy_scope * scope;
    struct api_stdio * stdio_api;
    struct api_toy_scope * toy_scope_api;
    enum apivalue_toy_scope toy_scope_rv;

    (void) api;

    cmd = type_with_member_at_ptr(struct cmd_monolith, command, command);
    ctx = cmd->ctx;
    stdio_api = ctx->api_stdio;
    toy_scope_api = ctx->api_toy_scope;

    if (argc == 1)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Usage:\n  make_identifier IDENTIFIER\n");
        return EXIT_FAILURE;
      }

    chain = live_module->module.v1.module_pointers[0];
    scope = chain->scopes[1];

    /* Allocate the identifier */
    toy_scope_rv = toy_scope_api->allocate_identifier(toy_scope_api, &identifier, argv[1], NULL);
    if (toy_scope_rv != apivalue_toy_scope_success)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Allocating identifier failed with error '%d'\n", toy_scope_rv);
        return EXIT_FAILURE;
      }

    /* Add it to the top scope */
    toy_scope_rv = toy_scope_api->add_identifier_to_scope(toy_scope_api, identifier, scope);
    if (toy_scope_rv != apivalue_toy_scope_success)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Adding identifier failed with error '%d'\n", toy_scope_rv);
        return EXIT_FAILURE;
      }
    return EXIT_SUCCESS;
  }

static int cmd_swap_scopes(struct api_command * api, struct command * command, int argc, char ** argv)
  {
    struct toy_scope_chain * chain;
    struct cmd_monolith * cmd;
    struct top * ctx;
    struct toy_scope * scope;
    struct api_stdio * stdio_api;

    (void) api;
    (void) argv;
    (void) command;

    cmd = type_with_member_at_ptr(struct cmd_monolith, command, command);
    ctx = cmd->ctx;
    stdio_api = ctx->api_stdio;

    if (argc != 1)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Usage:\n  swap_scopes  Switches which toy-scope is at the top of the toy-scope-chain\n");
        return EXIT_FAILURE;
      }

    chain = live_module->module.v1.module_pointers[0];
    scope = chain->scopes[0];
    chain->scopes[0] = chain->scopes[1];
    chain->scopes[1] = scope;
    return EXIT_SUCCESS;
  }

static int module_event(enum apivalue_module_event_type type, void * event_data)
  {
    struct api_command * command_api;
    struct cmd_monolith (* commands)[6];
    struct top * ctx;
    size_t i;
    size_t j;
    struct api_list * list_api;
    int rv;
    struct primary_scope_chain
      {
        struct toy_scope_chain chain;
        struct toy_scope scopes[2];
        struct toy_scope * scope_ptrs[2];
        struct top * ctx;
      }
      * primary_scope_chain;
    struct api_stdio * stdio_api;
    struct api_stdlib * stdlib_api;
    struct api_toy_scope * toy_scope_api;

    switch (type)
      {
        case apivalue_module_event_type_loaded:
        if (live_module != NULL)
          return EXIT_FAILURE;
        live_module = event_data;
        return EXIT_SUCCESS;

        case apivalue_module_event_type_thread_started:
        ctx = event_data;
        command_api = ctx->api_command;
        list_api = ctx->api_list;
        stdio_api = ctx->api_stdio;
        stdlib_api = ctx->api_stdlib;
        toy_scope_api = ctx->api_toy_scope;

        /* Primary scope chain */
        primary_scope_chain = stdlib_api->malloc(stdlib_api, sizeof *primary_scope_chain);
        if (primary_scope_chain == NULL)
          {
            (void) stdio_api->fprintf(stdio_api, stderr, "Out of memory while allocating primary scope chain\n");
            rv = EXIT_FAILURE;
            goto err_primary_scope_chain;
          }
        live_module->module.v1.module_pointers[0] = primary_scope_chain;
        toy_scope_api->initialize_scope(toy_scope_api, primary_scope_chain->scopes + 0);
        toy_scope_api->initialize_scope(toy_scope_api, primary_scope_chain->scopes + 1);
        primary_scope_chain->scope_ptrs[0] = primary_scope_chain->scopes + 0;
        primary_scope_chain->scope_ptrs[1] = primary_scope_chain->scopes + 1;
        primary_scope_chain->chain.count = 2;
        primary_scope_chain->chain.scopes = primary_scope_chain->scope_ptrs;
        primary_scope_chain->ctx = ctx;

        /* Commands */
        commands = stdlib_api->malloc(stdlib_api, sizeof *commands);
        if (commands == NULL)
          {
            (void) stdio_api->fprintf(stdio_api, stderr, "Out of memory while registering 'delete_identifier', 'find_identifier', 'list_identifiers', 'load_types', 'make_identifier', 'swap_scopes' commands\n");
            rv = EXIT_FAILURE;
            goto err_commands;
          }
        live_module->module.v1.module_pointers[1] = commands;
        (*commands)[0].command = command_delete_identifier;
        (*commands)[0].ctx = ctx;
        (*commands)[1].command = command_find_identifier;
        (*commands)[1].ctx = ctx;
        (*commands)[2].command = command_list_identifiers;
        (*commands)[2].ctx = ctx;
        (*commands)[3].command = command_make_identifier;
        (*commands)[3].ctx = ctx;
        (*commands)[4].command = command_swap_scopes;
        (*commands)[4].ctx = ctx;
        (*commands)[5].command = command_load_types;
        (*commands)[5].ctx = ctx;
        for (i = 0; i < countof(*commands); ++i)
          {
            (*commands)[i].command.live_module = live_module;
            list_api->initialize_list_item(list_api, &(*commands)[i].command.list_item);
            rv = command_api->add(command_api, &(*commands)[i].command);
            if (rv != EXIT_SUCCESS)
              {
                (void) stdio_api->fprintf(stdio_api, stderr, "Error '%d' while attempting to register '%s' command\n", rv, (*commands)[i].command.name);
                for (j = 0; j < i; ++j)
                  (void) command_api->remove(command_api, &(*commands)[i].command);
                return rv;
              }
          }
        return rv;

        stdlib_api->free(stdlib_api, commands);
        err_commands:

        stdlib_api->free(stdlib_api, primary_scope_chain);
        err_primary_scope_chain:

        return rv;

        case apivalue_module_event_type_thread_stop_requested:
        case apivalue_module_event_type_thread_stopped:
        case apivalue_module_event_type_unload_requested:
        return EXIT_SUCCESS;

        case apivalue_module_event_type_unload:
        /* TODO: This clean-up belongs to thread-stopped, but that's not yet implemented */

        primary_scope_chain = live_module->module.v1.module_pointers[0];
        if (primary_scope_chain != NULL)
          {
            primary_scope_chain->ctx->api_stdlib->free(primary_scope_chain->ctx->api_stdlib, primary_scope_chain);
            live_module->module.v1.module_pointers[0] = NULL;
          }

        commands = live_module->module.v1.module_pointers[1];
        if (commands == NULL)
          return EXIT_SUCCESS;
        /* Just take from the first command */
        ctx = (*commands)[0].ctx;
        command_api = ctx->api_command;
        stdio_api = ctx->api_stdio;
        stdlib_api = ctx->api_stdlib;
        for (i = 0; i < countof(*commands); ++i)
          {
            rv = command_api->remove(command_api, &(*commands)[i].command);
            if (rv != EXIT_SUCCESS)
              {
                (void) stdio_api->fprintf(stdio_api, stderr, "Error '%d' while attempting to deregister '%s' command\n", rv, (*commands)[i].command.name);
                /* Oh well.  Leak */
                return rv;
              }
          }
        stdlib_api->free(stdlib_api, commands);
        live_module->module.v1.module_pointers[1] = NULL;
        live_module = NULL;
        return rv;
      }
    return EXIT_FAILURE;
  }
