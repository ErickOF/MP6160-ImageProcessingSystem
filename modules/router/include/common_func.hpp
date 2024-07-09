#ifndef COMMON_FUNC
#define COMMON_FUNC

#define dbgprint(FORMAT, ARGS...) \
printf("%s -> %0d : " FORMAT "\n", __FILE__, __LINE__, ##ARGS)

#endif // COMMON_FUNC
