// GLX and Xlib, found at run time instead of linked.
//
// A core linked against libGL, libGLX and libX11 does not load on a system
// without X11, although on such a system it would never call them: they are
// for sharing the frontend's GLX context on an X11 session, and for RPCS3's
// X11 paths, which only run with a window of its own. The frontend owns the
// window, and when its context is a GLX one its libGL and libX11 are already
// in the process.
//
// So the core defines these functions itself, as forwarders that look the real
// one up among the frontend's libraries the first time they are called. They
// are hidden, and satisfy the references of the rest of the core and of the
// static GLEW at link time; nothing here is exported. When the real function
// is not there, there is no X11 in the process: each answers the way the real
// one does on failure.
//
// glXGetProcAddressARB is the exception that has work to do without GLX: GLEW
// loads every OpenGL function through it, on an EGL context too. Without GLX
// it goes to eglGetProcAddress.

#if defined(__linux__) && !defined(__ANDROID__)

#pragma GCC visibility push(hidden)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#pragma GCC visibility pop

// EGL is the system's, not defined here
#include <EGL/egl.h>

#include <dlfcn.h>

namespace
{
	template <typename F>
	F real(const char* name)
	{
		return reinterpret_cast<F>(dlsym(RTLD_DEFAULT, name));
	}
}

#define LR_HIDDEN __attribute__((visibility("hidden")))

extern "C"
{
	// GLX

	LR_HIDDEN __GLXextFuncPtr glXGetProcAddressARB(const GLubyte* name)
	{
		static const auto f = real<__GLXextFuncPtr (*)(const GLubyte*)>("glXGetProcAddressARB");
		if (f)
			return f(name);
		return reinterpret_cast<__GLXextFuncPtr>(eglGetProcAddress(reinterpret_cast<const char*>(name)));
	}

	LR_HIDDEN __GLXextFuncPtr glXGetProcAddress(const GLubyte* name)
	{
		return glXGetProcAddressARB(name);
	}

	LR_HIDDEN Bool glXQueryVersion(Display* display, int* major, int* minor)
	{
		static const auto f = real<Bool (*)(Display*, int*, int*)>("glXQueryVersion");
		return f ? f(display, major, minor) : False;
	}

	LR_HIDDEN Bool glXQueryExtension(Display* display, int* error_base, int* event_base)
	{
		static const auto f = real<Bool (*)(Display*, int*, int*)>("glXQueryExtension");
		return f ? f(display, error_base, event_base) : False;
	}

	LR_HIDDEN const char* glXGetClientString(Display* display, int name)
	{
		static const auto f = real<const char* (*)(Display*, int)>("glXGetClientString");
		return f ? f(display, name) : nullptr;
	}

	LR_HIDDEN const char* glXQueryExtensionsString(Display* display, int screen)
	{
		static const auto f = real<const char* (*)(Display*, int)>("glXQueryExtensionsString");
		return f ? f(display, screen) : nullptr;
	}

	LR_HIDDEN const char* glXQueryServerString(Display* display, int screen, int name)
	{
		static const auto f = real<const char* (*)(Display*, int, int)>("glXQueryServerString");
		return f ? f(display, screen, name) : nullptr;
	}

	LR_HIDDEN Display* glXGetCurrentDisplay()
	{
		static const auto f = real<Display* (*)()>("glXGetCurrentDisplay");
		return f ? f() : nullptr;
	}

	LR_HIDDEN GLXContext glXGetCurrentContext()
	{
		static const auto f = real<GLXContext (*)()>("glXGetCurrentContext");
		return f ? f() : nullptr;
	}

	LR_HIDDEN GLXDrawable glXGetCurrentDrawable()
	{
		static const auto f = real<GLXDrawable (*)()>("glXGetCurrentDrawable");
		return f ? f() : 0;
	}

	LR_HIDDEN Bool glXMakeCurrent(Display* display, GLXDrawable drawable, GLXContext context)
	{
		static const auto f = real<Bool (*)(Display*, GLXDrawable, GLXContext)>("glXMakeCurrent");
		return f ? f(display, drawable, context) : False;
	}

	LR_HIDDEN void glXDestroyContext(Display* display, GLXContext context)
	{
		static const auto f = real<void (*)(Display*, GLXContext)>("glXDestroyContext");
		if (f)
			f(display, context);
	}

	LR_HIDDEN void glXSwapBuffers(Display* display, GLXDrawable drawable)
	{
		static const auto f = real<void (*)(Display*, GLXDrawable)>("glXSwapBuffers");
		if (f)
			f(display, drawable);
	}

	// Xlib

	LR_HIDDEN int XCloseDisplay(Display* display)
	{
		static const auto f = real<int (*)(Display*)>("XCloseDisplay");
		return f ? f(display) : 0;
	}

	LR_HIDDEN int XFlush(Display* display)
	{
		static const auto f = real<int (*)(Display*)>("XFlush");
		return f ? f(display) : 0;
	}

	LR_HIDDEN int XSync(Display* display, Bool discard)
	{
		static const auto f = real<int (*)(Display*, Bool)>("XSync");
		return f ? f(display, discard) : 0;
	}

	LR_HIDDEN int XFree(void* data)
	{
		static const auto f = real<int (*)(void*)>("XFree");
		return f ? f(data) : 0;
	}

	LR_HIDDEN XErrorHandler XSetErrorHandler(XErrorHandler handler)
	{
		static const auto f = real<XErrorHandler (*)(XErrorHandler)>("XSetErrorHandler");
		return f ? f(handler) : nullptr;
	}

	LR_HIDDEN Status XGetGeometry(Display* display, Drawable drawable, Window* root, int* x, int* y,
		unsigned int* width, unsigned int* height, unsigned int* border_width, unsigned int* depth)
	{
		static const auto f = real<Status (*)(Display*, Drawable, Window*, int*, int*,
			unsigned int*, unsigned int*, unsigned int*, unsigned int*)>("XGetGeometry");
		return f ? f(display, drawable, root, x, y, width, height, border_width, depth) : 0;
	}

	LR_HIDDEN Status XMatchVisualInfo(Display* display, int screen, int depth, int visual_class, XVisualInfo* info)
	{
		static const auto f = real<Status (*)(Display*, int, int, int, XVisualInfo*)>("XMatchVisualInfo");
		return f ? f(display, screen, depth, visual_class, info) : 0;
	}

	LR_HIDDEN XImage* XCreateImage(Display* display, Visual* visual, unsigned int depth, int format, int offset,
		char* data, unsigned int width, unsigned int height, int bitmap_pad, int bytes_per_line)
	{
		static const auto f = real<XImage* (*)(Display*, Visual*, unsigned int, int, int,
			char*, unsigned int, unsigned int, int, int)>("XCreateImage");
		return f ? f(display, visual, depth, format, offset, data, width, height, bitmap_pad, bytes_per_line) : nullptr;
	}

	LR_HIDDEN int XPutImage(Display* display, Drawable drawable, GC gc, XImage* image, int src_x, int src_y,
		int dest_x, int dest_y, unsigned int width, unsigned int height)
	{
		static const auto f = real<int (*)(Display*, Drawable, GC, XImage*, int, int,
			int, int, unsigned int, unsigned int)>("XPutImage");
		return f ? f(display, drawable, gc, image, src_x, src_y, dest_x, dest_y, width, height) : 0;
	}
}

#endif
