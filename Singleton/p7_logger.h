#pragma once

#include <vector>
#include <utility>
#include <cassert>
#include <iostream>
#include <exception>
#include <vector>
#include <P7_Trace.h>
#include <P7_Telemetry.h>
#include <misc.h>

class p7_error : public std::exception{};

class DLL_PUBLIC p7_beam
{
public:
   p7_beam(tUINT8 tid);
   bool add(tINT64 i_llValue);
private:
   tUINT8 m_tid;
};

/*! Обертка над библиотекой Р7. Содержит в себе один Trace для вывода сообщений
 * и один Telemetry для ведения телеметрии. При конструировании получает опции
 * для клиента Р7.*/
class DLL_PUBLIC p7_logger
{
public:
   p7_logger(char const* opts);
   p7_logger(p7_logger const&) = delete;
   p7_logger(p7_logger&&) = default;
   p7_logger& operator=(p7_logger const&) = delete;
   p7_logger& operator=(p7_logger&&) = default;
   ~p7_logger();

/*! В эту функцию должны быть имена всех модулей, которые будут использованы
 * в дальнейшем. Адресация модулей при использовании класса p7_logger будет совпадать
 * с позицией модулей в этом векторе. Если функция будет вызываться несколько раз,
 * нумерация будет продолжена.*/
   void                 register_module(const std::vector<const char*>& module_names);
   void                 register_thread(char const* name);

/*! Будьте внимательны, module_index не должен превышать размер вектора,
 * переданный при инициализации модулей. */
   IP7_Trace::hModule&  module(size_t module_index);
   IP7_Trace&           trace();
   IP7_Telemetry&       telemetry();
   p7_beam              create_beam(const tXCHAR  *i_pName,  tINT64    i_llMin,
                                    tINT64         i_llMax,  tINT64    i_llAlarm);
   void                 set_verbosity(size_t module_idx, eP7Trace_Level const& level);
   void                 set_verbosity(eP7Trace_Level const& level);

private:
   IP7_Client*       m_client = nullptr;
   IP7_Trace *       m_trace = nullptr;
   IP7_Telemetry *   m_telemetry = nullptr;
   std::vector<IP7_Trace::hModule> m_modules;
   stTelemetry_Conf  m_telemetry_conf     = {};
};

/*! Класс инициализирует синглетон логгера p7_logger при создании и деинициализирует при удалении. */
struct DLL_PUBLIC p7_logger_raii
{
   p7_logger_raii(char const* opts);
   ~p7_logger_raii();

   static p7_logger& instance();

private:
   void deinit();

private:
   static p7_logger* m_instance;
};

#ifdef USE_P7_LOG
#define P7_LOG(log_level, module_enum, format, ...)\
   p7_logger::instance().trace().P7_DELIVER(0,\
                                            log_level,\
                                            p7_logger::instance().module(module_enum),\
                                            format,\
                                            ##__VA_ARGS__)
#else // USE_P7_LOG
#define P7_LOG(...)
#endif // USE_P7_LOG

#define P7_LOG_TRACE(module_enum, format, ...) P7_LOG(EP7TRACE_LEVEL_TRACE,\
                                                      static_cast<size_t>(module_enum),\
                                                      format,\
                                                      ##__VA_ARGS__)
#define P7_LOG_DEBUG(module_enum, format, ...) P7_LOG(EP7TRACE_LEVEL_DEBUG,\
                                                      static_cast<size_t>(module_enum),\
                                                      format,\
                                                      ##__VA_ARGS__)
#define P7_LOG_INFO(module_enum, format, ...) P7_LOG(EP7TRACE_LEVEL_INFO,\
                                                      static_cast<size_t>(module_enum),\
                                                      format,\
                                                      ##__VA_ARGS__)
#define P7_LOG_WARNING(module_enum, format, ...) P7_LOG(EP7TRACE_LEVEL_WARNING,\
                                                      static_cast<size_t>(module_enum),\
                                                      format,\
                                                      ##__VA_ARGS__)
#define P7_LOG_ERROR(module_enum, format, ...) P7_LOG(EP7TRACE_LEVEL_ERROR,\
                                                      static_cast<size_t>(module_enum),\
                                                      format,\
                                                      ##__VA_ARGS__)
#define P7_LOG_CRITICAL(module_enum, format, ...) P7_LOG(EP7TRACE_LEVEL_CRITICAL,\
                                                      static_cast<size_t>(module_enum),\
                                                      format,\
                                                      ##__VA_ARGS__)
