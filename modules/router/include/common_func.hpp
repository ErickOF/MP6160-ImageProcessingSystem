#ifndef COMMON_FUNC
#define COMMON_FUNC

#define dbgprint(FORMAT, ARGS...) \
printf("%s(%0d) : " FORMAT "\n", __FILE__, __LINE__, ##ARGS)

#define dbgmodprint(FORMAT, ARGS...) \
printf("%s(%0d) [%s] : " FORMAT "\n", __FILE__, __LINE__, this->name(), ##ARGS)

#define dbgimgtarmodprint(FORMAT, ARGS...) \
printf("%s(%0d) [%s] : " FORMAT "\n", __FILE__, __LINE__, img_target::name(), ##ARGS)

#endif // COMMON_FUNC
