LOCAL_PATH := $(call my-dir)
LOCAL_SHORT_COMMANDS := true

########################################################
## libevent
########################################################

include $(CLEAR_VARS)

LIBEVENT_SOURCES := \
	buffer.c \
	bufferevent.c bufferevent_filter.c \
	bufferevent_openssl.c bufferevent_pair.c bufferevent_ratelim.c \
	bufferevent_sock.c epoll.c \
	epoll_sub.c evdns.c event.c \
    event_tagging.c evmap.c \
	evrpc.c evthread.c \
	evthread_pthread.c evutil.c \
	evutil_rand.c http.c \
	listener.c log.c poll.c \
	select.c signal.c strlcpy.c

LOCAL_MODULE := libevent2
LOCAL_SRC_FILES := $(addprefix libevent2/, $(LIBEVENT_SOURCES))
LOCAL_CFLAGS := -O2 -I$(LOCAL_PATH)/libevent2 \
	-I$(LOCAL_PATH)/libevent2/include \
	-I$(LOCAL_PATH)/openssl/include

include $(BUILD_STATIC_LIBRARY)

