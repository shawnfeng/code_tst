#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>



static __thread int t_cachedTid = 0;
static __thread char t_tidString[32] = "EMPTY";
static void cacheTid()
{
	if (t_cachedTid == 0) {
		t_cachedTid = syscall(SYS_gettid);
		int n = snprintf(t_tidString, sizeof(t_tidString), "%d", t_cachedTid);
	}
}


inline int thread_id()
{
	if (__builtin_expect(t_cachedTid == 0, 0)) {
		cacheTid();
	}
	return t_cachedTid;
}

inline const char *thread_id_str()
{
	if (__builtin_expect(t_cachedTid == 0, 0)) {
		cacheTid();
	}

	return t_tidString;
}


int main()
{
	printf("C tid:%d %s\n", thread_id(), thread_id_str());
	return 0;
}
