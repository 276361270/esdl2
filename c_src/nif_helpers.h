// Copyright (c) 2014, Loïc Hoguin <essen@ninenines.eu>
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#ifndef __NIF_HELPERS_H__
#define __NIF_HELPERS_H__

#include "erl_nif.h"

#define TO_STRING(i) #i

// Atoms.

#define MAX_ATOM_LENGTH 255

#define NIF_ATOM_DECL(a) ERL_NIF_TERM atom_ ## a;
#define NIF_ATOM_H_DECL(a) extern ERL_NIF_TERM atom_ ## a;
#define NIF_ATOM_INIT(a) atom_ ## a = enif_make_atom(env, #a);

// Functions.

#ifndef NIF_FUNCTION_NAME
#define NIF_FUNCTION_NAME(n) n
#endif

#define NIF_FUNCTION(f) \
	ERL_NIF_TERM NIF_FUNCTION_NAME(f)(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
#define NIF_FUNCTION_ARRAY(f, a) {#f, a, NIF_FUNCTION_NAME(f)},
#define NIF_FUNCTION_H_DECL(f, a) \
	ERL_NIF_TERM NIF_FUNCTION_NAME(f)(ErlNifEnv*, int, const ERL_NIF_TERM []);

#define BADARG_IF(cond) if (cond) return enif_make_badarg(env)

// Resources.

#ifndef NIF_RES_TYPE
#define NIF_RES_TYPE(t) t
#endif

#define NIF_RES_DECL(r) ErlNifResourceType* res_ ## r = NULL;
#define NIF_RES_H_DECL(r) \
	extern ErlNifResourceType* res_ ## r; \
	void dtor_ ## r(ErlNifEnv*, void*); \
	typedef struct { \
		NIF_RES_TYPE(r)* v; \
	} obj_ ## r;
#define NIF_RES_INIT(r) \
	res_ ## r = enif_open_resource_type(env, NULL, TO_STRING(NIF_RES_TYPE(r)), dtor_ ## r, ERL_NIF_RT_CREATE, NULL); \
	if (!res_ ## r) return -1;

#define NIF_RES_GET(r, obj) (((obj_ ## r*)obj)->v)
#define NIF_RES_TO_TERM(r, val, term) { \
	obj_ ## r* res = enif_alloc_resource(res_ ## r, sizeof(obj_ ## r)); \
	res->v = val; \
	term = enif_make_resource(env, res); \
	enif_release_resource(res); \
}

// Function generators.

#define NIF_ATOM_TO_FLAG(a, f) if (!strcmp(buf, #a)) *flags |= f; else
#define NIF_LIST_TO_FLAGS_FUNCTION(f, type, flags_list) \
	int f(ErlNifEnv* env, ERL_NIF_TERM list, type* flags) \
	{ \
		int i; \
		char buf[MAX_ATOM_LENGTH]; \
		ERL_NIF_TERM head; \
		\
		if (!enif_is_list(env, list)) \
			return 0; \
		\
		while (enif_get_list_cell(env, list, &head, &list)) { \
			if (!enif_get_atom(env, head, buf, MAX_ATOM_LENGTH, ERL_NIF_LATIN1)) \
				return 0; \
		\
			flags_list(NIF_ATOM_TO_FLAG) /* else */ return 0; \
		} \
		\
		return 1; \
	}

#define NIF_FLAG_CONS_LIST(a, f) if (flags & f) list = enif_make_list_cell(env, atom_ ## a, list);
#define NIF_FLAGS_TO_LIST_FUNCTION(f, type, flags_list) \
	ERL_NIF_TERM f(ErlNifEnv* env, type flags) \
	{ \
		ERL_NIF_TERM list = enif_make_list(env, 0); \
		flags_list(NIF_FLAG_CONS_LIST); \
		return list; \
	}

#define NIF_ENUM_TO_ATOM(a, e) if (id == e) return atom_ ## a;
#define NIF_ENUM_TO_ATOM_FUNCTION(f, type, enum_list) \
	ERL_NIF_TERM f(type id) \
	{ \
		enum_list(NIF_ENUM_TO_ATOM) \
		return atom_undefined; \
	}

#endif
