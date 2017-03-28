#ifndef __TRACEPOINT_COMPAT_H
#define __TRACEPOINT_COMPAT_H

#define DECLARE_EVENT_CLASS(name, proto, args, str, assign, print)
#define TP_PROTO(x, args...) x, ##args
#define TP_ARGS(args...)
#define TP_STRUCT__entry(args)
#define TP_fast_assign(args)
#define DECLARE_TRACE(name, proto, args) \
	static inline void trace_##name(proto) {}
#define TRACE_EVENT(name, proto, args, str, assign, print) \
	static inline void trace_##name(proto) {}
#define DEFINE_EVENT(temp, name, proto, args) \
	static inline void trace_##name(proto) {}

#endif /* __TRACEPOINT_COMPAT_H */
