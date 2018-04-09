#include "p7_logger.h"

p7_logger::p7_logger(char const* opts)
{
   m_client = P7_Create_Client(opts);
   if(m_client == nullptr)
      throw p7_error{};
   m_trace = P7_Create_Trace(m_client, "MainLog");
   if (m_trace == nullptr)
      throw p7_error{};
   m_telemetry = P7_Create_Telemetry(m_client, "MainTelemetry", &m_telemetry_conf);
   if (m_telemetry == nullptr)
      throw p7_error{};
}

void p7_logger::register_module(std::vector<const char*> const& module_names)
{
   const size_t last_size = m_modules.size();
   const size_t new_size = last_size + module_names.size();
   m_modules.resize(new_size);
   for(size_t i = last_size, j = 0; i < new_size; ++i, ++j)
   {
      assert(j < module_names.size());
      char const* name = module_names[j];
      m_trace->Register_Module(name, &module(i));
   }
}

p7_logger::~p7_logger()
{
   if(m_trace)
      m_trace->Release();
   m_trace = nullptr;
   if(m_telemetry)
      m_telemetry->Release();
   m_telemetry = nullptr;
   if(m_client)
      m_client->Release();
   m_client = nullptr;
}

void p7_logger::register_thread(char const* name)
{
   m_trace->Register_Thread(name, 0);
}

IP7_Trace::hModule &p7_logger::module(size_t u)
{
   assert(u < m_modules.size());
   return m_modules[static_cast<size_t>(u)];
}

IP7_Trace &p7_logger::trace()
{
   return *m_trace;
}

IP7_Telemetry &p7_logger::telemetry()
{
   return *m_telemetry;
}

p7_beam p7_logger::create_beam(const tXCHAR  *i_pName,  tINT64    i_llMin,
                            tINT64         i_llMax,  tINT64    i_llAlarm)
{
   tUINT8 tid;
   if(FALSE == telemetry().Create(i_pName, i_llMin, i_llMax, i_llAlarm, true, &tid))
      throw p7_error{};
   return {tid};
}

void p7_logger::set_verbosity(size_t module_idx, eP7Trace_Level const& level)
{
   assert(P7_TRACE_LEVEL_TRACE == EP7TRACE_LEVEL_TRACE);
   assert(P7_TRACE_LEVEL_CRITICAL == EP7TRACE_LEVEL_CRITICAL);
   P7_Trace_Set_Verbosity(m_trace, module(module_idx), static_cast<tUINT32>(level));
}

void p7_logger::set_verbosity(eP7Trace_Level const& level)
{
   assert(P7_TRACE_LEVEL_TRACE == EP7TRACE_LEVEL_TRACE);
   assert(P7_TRACE_LEVEL_CRITICAL == EP7TRACE_LEVEL_CRITICAL);
   m_trace->Set_Verbosity(NULL, level);
   for(IP7_Trace::hModule& m : m_modules)
   {
      if(m != IP7_Trace::hModule{})
         m_trace->Set_Verbosity(m, level);
   }
}

p7_logger_raii::p7_logger_raii(char const* opts)
{
   deinit();
   m_instance = new p7_logger(opts);
   P7_Set_Crash_Handler();
}

p7_logger_raii::~p7_logger_raii()
{
   deinit();
   P7_Clr_Crash_Handler();
}

p7_logger& p7_logger_raii::instance()
{
   assert(m_instance);
   return *m_instance;
}

void p7_logger_raii::deinit()
{
   if(m_instance)
      delete m_instance;
   m_instance = nullptr;
}

p7_beam::p7_beam(tUINT8 tid)
   : m_tid(tid)
{}

bool p7_beam::add(tINT64 i_llValue)
{
   return p7_logger_raii::instance().telemetry().Add(m_tid, i_llValue);
}

p7_logger* p7_logger_raii::m_instance = nullptr;
