#ifndef COMMON_FUNC_HPP
#define COMMON_FUNC_HPP

#define dbgprint(FORMAT, ARGS...) \
printf("%s(%0d) @%s: " FORMAT "\n", __FILE__, __LINE__, sc_time_stamp().to_string().c_str(), ##ARGS)

#define dbgmodprint(FORMAT, ARGS...) \
printf("%s(%0d) [%s] @%s : " FORMAT "\n", __FILE__, __LINE__, this->name(), sc_time_stamp().to_string().c_str(), ##ARGS)

#define dbgimgtarmodprint(FORMAT, ARGS...) \
printf("%s(%0d) [%s] @%s : " FORMAT "\n", __FILE__, __LINE__, img_target::name(), sc_time_stamp().to_string().c_str(), ##ARGS)

#endif // COMMON_FUNC_HPP
