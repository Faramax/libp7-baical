#pragma once

#include <memory>
#include <vector>
#include <utility>
#include <cassert>
#include <iostream>
#include <exception>
#include <vector>
#include <P7_Trace.h>
#include <P7_Telemetry.h>
#include <misc.h>

class p7_error : public std::exception
{
   public:
      explicit p7_error(std::string msg)
         :msg_(std::move(msg))
      {}

   const char * what () const noexcept override{
      return msg_.c_str();
   }
private:
   std::string msg_;
};

class DLL_PUBLIC p7_beam
{
public:
   p7_beam(tUINT16 tid);
   bool add(tDOUBLE i_llValue);
private:
   tUINT16 m_tid;
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
 * нумерация будет продолжена. */
   void                 register_module(const std::vector<const char*>& module_names);
   void                 register_thread(char const* name);

/*! Будьте внимательны, module_index не должен превышать размер вектора,
 * переданный при инициализации модулей. */
   IP7_Trace::hModule   module(size_t module_index);
   IP7_Trace*           trace();
   IP7_Telemetry*       telemetry();
   p7_beam              create_beam(const tXCHAR  *i_pName,  tDOUBLE i_llMin, tDOUBLE i_dbAlarmMin,
                                    tDOUBLE i_llMax, tDOUBLE i_dbAlarmMax);
   void                 set_verbosity(size_t module_idx, eP7Trace_Level const& level);
   void                 set_verbosity(eP7Trace_Level const& level);

private:
   IP7_Client*       m_client = nullptr;
   IP7_Trace *       m_trace = nullptr;
   IP7_Telemetry *   m_telemetry = nullptr;
   std::vector<IP7_Trace::hModule> m_modules;
   stTelemetry_Conf  m_telemetry_conf     = {};
};

/*! Класс инициализирует синглетон логгера p7_logger.
 * По-умолчанию он не инициализирован. instance() возвращает нулевой указатель.
 * Макросы P7_LOG написаны так, что не отправляют лог неинициализированному
 * синглетону логгера.
 * Инициализируется методом init().
 * Деинициализироваться он будет автоматически при завершении программы.
*/
struct DLL_PUBLIC p7_logger_singleton
{
   inline static void init(char const* opts)
   {
      m_instance = {};
      m_instance = std::make_unique<p7_logger>(opts);
   }

   inline static p7_logger*           instance(){return m_instance.get();}

   inline static void                 set_verbosity(size_t module_idx, eP7Trace_Level const& level)
   {
      if(instance()) instance()->set_verbosity(module_idx, level);
   }
   inline static void                 set_verbosity(eP7Trace_Level const& level)
   {
      if(instance()) instance()->set_verbosity(level);
   }

private:
   static std::unique_ptr<p7_logger> m_instance;
};

#ifdef USE_P7_LOG
   #define P7_LOG(log_level, module_enum, format, ...)                              \
      p7_logger_singleton::instance() ?                                                  \
         p7_logger_singleton::instance()->trace()->P7_DELIVER                            \
            (                                                                       \
               0,                                                                   \
               log_level,                                                           \
               p7_logger_singleton::instance()->module(static_cast<size_t>(module_enum)),\
               format,                                                              \
               ##__VA_ARGS__                                                        \
            ) : false
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
