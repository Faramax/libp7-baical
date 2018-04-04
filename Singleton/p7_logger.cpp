#include "p7_logger.h"

p7_logger::p7_logger(char const* opts, size_t max_module_index)
   : m_modules(max_module_index + 1)
{
   P7_Set_Crash_Handler();

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

void p7_logger::register_module(const char *name, size_t module_index)
{
   m_trace->Register_Module(name, &module(module_index));
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

void p7_logger::init(char const* opts, size_t max_module_index)
{
   if(m_instance)
      p7_logger::deinit();
   try{
      m_instance = new p7_logger(opts, max_module_index);
   }
   catch(...)
   {
      p7_logger::deinit();
      throw;
   }
}

void p7_logger::deinit()
{
   if(m_instance);
      delete m_instance;
   m_instance = nullptr;
   P7_Clr_Crash_Handler();
}

p7_logger& p7_logger::instance()
{
   assert(m_instance);
   return *m_instance;
}

void p7_logger::register_thread(char const* name)
{
   m_trace->Register_Thread(name, 0);
}

p7_beam p7_logger::create_beam(const tXCHAR  *i_pName,  tINT64    i_llMin,
                            tINT64         i_llMax,  tINT64    i_llAlarm)
{
   tUINT8 tid;
   if(FALSE == telemetry().Create(i_pName, i_llMin, i_llMax, i_llAlarm, true, &tid))
      throw p7_error{};
   return {tid};
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

p7_beam::p7_beam(tUINT8 tid)
   : m_tid(tid)
{}

bool p7_beam::add(tINT64 i_llValue)
{
   return p7_logger::instance().telemetry().Add(m_tid, i_llValue);
}

p7_logger* p7_logger::m_instance = nullptr;
