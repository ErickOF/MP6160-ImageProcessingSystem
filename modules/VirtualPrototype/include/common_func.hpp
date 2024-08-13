#ifndef COMMON_FUNC_HPP
#define COMMON_FUNC_HPP

#define dbgprint(FORMAT, ARGS...) \
printf("%s(%0d) @%s: " FORMAT "\n", __FILE__, __LINE__, sc_core::sc_time_stamp().to_string().c_str(), ##ARGS);

#define dbgmodprint(DBG_ACT, FORMAT, ARGS...) \
if (DBG_ACT) printf("%s(%0d) [%s] @%s : " FORMAT "\n", __FILE__, __LINE__, this->name(), sc_core::sc_time_stamp().to_string().c_str(), ##ARGS)

#define dbgimgtarmodprint(DBG_ACT, FORMAT, ARGS...) \
if (DBG_ACT) printf("%s(%0d) [%s] @%s : " FORMAT "\n", __FILE__, __LINE__, img_target::name(), sc_core::sc_time_stamp().to_string().c_str(), ##ARGS)

#define checkprintenable(DBG_ACT) \
if (DBG_ACT) \
{ \
  printf("%s(%0d) [%s] @%s : " "Prints enabled for this module" "\n", __FILE__, __LINE__, this->name(), sc_core::sc_time_stamp().to_string().c_str()); \
} \
else \
{ \
  printf("%s(%0d) [%s] @%s : " "Prints are not enabled for this module" "\n", __FILE__, __LINE__, this->name(), sc_core::sc_time_stamp().to_string().c_str()); \
}

#define checkprintenableimgtar(DBG_ACT) \
if (DBG_ACT) \
{ \
  printf("%s(%0d) [%s] @%s : " "Prints enabled for this module" "\n", __FILE__, __LINE__, img_target::name(), sc_core::sc_time_stamp().to_string().c_str()); \
} \
else \
{ \
  printf("%s(%0d) [%s] @%s : " "Prints are not enabled for this module" "\n", __FILE__, __LINE__, img_target::name(), sc_core::sc_time_stamp().to_string().c_str()); \
}

#endif // COMMON_FUNC_HPP
