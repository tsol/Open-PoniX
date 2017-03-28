#include_next <linux/tracepoint.h>

#ifndef TP_PROTO
#define TP_PROTO(x, args...) x, ##args
#define TP_ARGS(args...)
#define TP_STRUCT__entry(args)
#define TP_fast_assign(args)
#endif

#if !defined(DECLARE_EVENT_CLASS) && !defined(TRACE_HEADER_MULTI_READ)
#define HACKED_TRACE_DEFINE_EVENT
#define DECLARE_EVENT_CLASS(name, proto, args, str, assign, print)
#define DEFINE_EVENT(temp, name, proto, args) \
        static inline void trace_##name(proto) {}
#ifndef TRACE_EVENT
#define HACKED_TRACE_TRACE_EVENT
#define TRACE_EVENT(name, proto, args, str, assign, print) \
	static inline void trace_##name(proto) {}
#endif
#elif defined(HACKED_TRACE_DEFINE_EVENT) && defined(TRACE_HEADER_MULTI_READ)
#undef DEFINE_EVENT
#define DEFINE_EVENT(temp, name, proto, args)
#ifdef HACKED_TRACE_TRACE_EVENT
#undef TRACE_EVENT
#define TRACE_EVENT(name, proto, args, str, assign, print)
#endif
#endif
