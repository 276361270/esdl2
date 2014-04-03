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

#include "esdl2.h"

void dtor_Window(ErlNifEnv* env, void* obj)
{
	SDL_DestroyWindow(NIF_RES_GET(Window, obj));
}

#define WINDOW_FLAGS(F) \
	F(fullscreen, SDL_WINDOW_FULLSCREEN) \
	F(fullscreen_desktop, SDL_WINDOW_FULLSCREEN_DESKTOP) \
	F(opengl, SDL_WINDOW_OPENGL) \
	F(shown, SDL_WINDOW_SHOWN) \
	F(hidden, SDL_WINDOW_HIDDEN) \
	F(borderless, SDL_WINDOW_BORDERLESS) \
	F(resizable, SDL_WINDOW_RESIZABLE) \
	F(minimized, SDL_WINDOW_MINIMIZED) \
	F(maximized, SDL_WINDOW_MAXIMIZED) \
	F(input_grabbed, SDL_WINDOW_INPUT_GRABBED) \
	F(input_focus, SDL_WINDOW_INPUT_FOCUS) \
	F(mouse_focus, SDL_WINDOW_MOUSE_FOCUS) \
	F(foreign, SDL_WINDOW_FOREIGN) \
	F(allow_high_dpi, SDL_WINDOW_ALLOW_HIGHDPI)

NIF_LIST_TO_FLAGS_FUNCTION(list_to_window_flags, Uint32, WINDOW_FLAGS)
NIF_FLAGS_TO_LIST_FUNCTION(window_flags_to_list, Uint32, WINDOW_FLAGS)

// create_window

NIF_CALL_HANDLER(thread_create_window)
{
	SDL_Window* window;
	ERL_NIF_TERM term;

	window = SDL_CreateWindow(args[0], (long)args[1], (long)args[2], (long)args[3], (long)args[4], (long)args[5]);

	enif_free(args[0]);

	if (!window)
		return sdl_error_tuple(env);

	NIF_RES_TO_TERM(Window, window, term);

	return enif_make_tuple2(env,
		atom_ok,
		term
	);
}

NIF_FUNCTION(create_window)
{
	char* title = (char*)enif_alloc(255);
	char buf[MAX_ATOM_LENGTH];
	int x, y, w, h;
	Uint32 flags = 0;

	BADARG_IF(!enif_get_string(env, argv[0], title, 255, ERL_NIF_LATIN1));

	if (enif_is_atom(env, argv[1])) {
		BADARG_IF(!enif_get_atom(env, argv[1], buf, MAX_ATOM_LENGTH, ERL_NIF_LATIN1));

		if (!strcmp(buf, "centered"))
			x = SDL_WINDOWPOS_CENTERED;
		else if (!strcmp(buf, "undefined"))
			x = SDL_WINDOWPOS_UNDEFINED;
		else
			return enif_make_badarg(env);
	} else
		BADARG_IF(!enif_get_int(env, argv[1], &x));

	if (enif_is_atom(env, argv[2])) {
		BADARG_IF(!enif_get_atom(env, argv[2], buf, MAX_ATOM_LENGTH, ERL_NIF_LATIN1));

		if (!strcmp(buf, "centered"))
			y = SDL_WINDOWPOS_CENTERED;
		else if (!strcmp(buf, "undefined"))
			y = SDL_WINDOWPOS_UNDEFINED;
		else
			return enif_make_badarg(env);
	} else
		BADARG_IF(!enif_get_int(env, argv[2], &y));

	BADARG_IF(!enif_get_int(env, argv[3], &w));
	BADARG_IF(!enif_get_int(env, argv[4], &h));
	BADARG_IF(!list_to_window_flags(env, argv[5], &flags));

	return nif_thread_call(env, thread_create_window, 6,
		title, x, y, w, h, flags);
}

// create_window_and_renderer

NIF_CALL_HANDLER(thread_create_window_and_renderer)
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	ERL_NIF_TERM wterm, rterm;

	if (0 != SDL_CreateWindowAndRenderer((long)args[0], (long)args[1], (long)args[2], &window, &renderer))
		return sdl_error_tuple(env);

	NIF_RES_TO_TERM(Window, window, wterm);
	NIF_RES_TO_TERM(Renderer, renderer, rterm);

	return enif_make_tuple3(env,
		atom_ok,
		wterm,
		rterm
	);
}

NIF_FUNCTION(create_window_and_renderer)
{
	int w, h;
	Uint32 flags = 0;

	BADARG_IF(!enif_get_int(env, argv[0], &w));
	BADARG_IF(!enif_get_int(env, argv[1], &h));
	BADARG_IF(!list_to_window_flags(env, argv[2], &flags));

	return nif_thread_call(env, thread_create_window_and_renderer, 3,
		w, h, flags);
}

// get_window_brightness

NIF_CALL_HANDLER(thread_get_window_brightness)
{
	return enif_make_double(env, SDL_GetWindowBrightness(args[0]));
}

NIF_FUNCTION(get_window_brightness)
{
	void* window_res;

	BADARG_IF(!enif_get_resource(env, argv[0], res_Window, &window_res));

	return nif_thread_call(env, thread_get_window_brightness, 1,
		NIF_RES_GET(Window, window_res));
}

// get_window_display_index

NIF_CALL_HANDLER(thread_get_window_display_index)
{
	int i = SDL_GetWindowDisplayIndex(args[0]);

	if (i < 0)
		return sdl_error_tuple(env);

	return enif_make_tuple2(env,
		atom_ok,
		enif_make_int(env, i)
	);
}

NIF_FUNCTION(get_window_display_index)
{
	void* window_res;

	BADARG_IF(!enif_get_resource(env, argv[0], res_Window, &window_res));

	return nif_thread_call(env, thread_get_window_display_index, 1,
		NIF_RES_GET(Window, window_res));
}

// get_window_flags

NIF_CALL_HANDLER(thread_get_window_flags)
{
	return window_flags_to_list(env, SDL_GetWindowFlags(args[0]));
}

NIF_FUNCTION(get_window_flags)
{
	void* window_res;

	BADARG_IF(!enif_get_resource(env, argv[0], res_Window, &window_res));

	return nif_thread_call(env, thread_get_window_flags, 1,
		NIF_RES_GET(Window, window_res));
}

// get_window_grab

NIF_CALL_HANDLER(thread_get_window_grab)
{
	if (SDL_GetWindowGrab(args[0]))
		return atom_true;

	return atom_false;
}

NIF_FUNCTION(get_window_grab)
{
	void* window_res;

	BADARG_IF(!enif_get_resource(env, argv[0], res_Window, &window_res));

	return nif_thread_call(env, thread_get_window_grab, 1,
		NIF_RES_GET(Window, window_res));
}

// get_window_id

NIF_CALL_HANDLER(thread_get_window_id)
{
	return enif_make_uint(env, SDL_GetWindowID(args[0]));
}

NIF_FUNCTION(get_window_id)
{
	void* window_res;

	BADARG_IF(!enif_get_resource(env, argv[0], res_Window, &window_res));

	return nif_thread_call(env, thread_get_window_id, 1,
		NIF_RES_GET(Window, window_res));
}

// get_window_maximum_size

NIF_CALL_HANDLER(thread_get_window_maximum_size)
{
	int w, h;

	SDL_GetWindowMaximumSize(args[0], &w, &h);

	return enif_make_tuple2(env,
		enif_make_int(env, w),
		enif_make_int(env, h)
	);
}

NIF_FUNCTION(get_window_maximum_size)
{
	void* window_res;

	BADARG_IF(!enif_get_resource(env, argv[0], res_Window, &window_res));

	return nif_thread_call(env, thread_get_window_maximum_size, 1,
		NIF_RES_GET(Window, window_res));
}

// get_window_minimum_size

NIF_CALL_HANDLER(thread_get_window_minimum_size)
{
	int w, h;

	SDL_GetWindowMinimumSize(args[0], &w, &h);

	return enif_make_tuple2(env,
		enif_make_int(env, w),
		enif_make_int(env, h)
	);
}

NIF_FUNCTION(get_window_minimum_size)
{
	void* window_res;

	BADARG_IF(!enif_get_resource(env, argv[0], res_Window, &window_res));

	return nif_thread_call(env, thread_get_window_minimum_size, 1,
		NIF_RES_GET(Window, window_res));
}

// get_window_position

NIF_CALL_HANDLER(thread_get_window_position)
{
	int x, y;

	SDL_GetWindowPosition(args[0], &x, &y);

	return enif_make_tuple2(env,
		enif_make_int(env, x),
		enif_make_int(env, y)
	);
}

NIF_FUNCTION(get_window_position)
{
	void* window_res;

	BADARG_IF(!enif_get_resource(env, argv[0], res_Window, &window_res));

	return nif_thread_call(env, thread_get_window_position, 1,
		NIF_RES_GET(Window, window_res));
}
