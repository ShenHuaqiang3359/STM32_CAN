#define TSMP_IMPL
#include "TSMaster.h"
#include "MPLibrary.h"
#include "Database.h"
#include "TSMasterBaseInclude.h"
#include "Configuration.h"

// Timers defintions
TMPTimerMS SendTimer;

// ======== Function Prorotypes START ========
// ========= Function Prorotypes END =========

// CODE BLOCK BEGIN Step_Function  NQ__
// 主step函数，执行周期 5 ms
void step(void) { try { // 周期 = 5 ms
// C INTERFACE BEGIN Step_Function
app.run_python_function(app.FObj, app.get_current_mp_name(), __FUNCTION__, "");
// C INTERFACE END Step_Function

} catch (...) { log_nok("CRASH detected"); app.terminate_application(); }}
// CODE BLOCK END Step_Function 


// CODE BLOCK BEGIN On_Timer Send U2VuZFRpbWVy
// 定时器触发事件 "Send" for Timer SendTimer
void on_timer_Send(void) { try { // 定时器 = SendTimer
// C INTERFACE BEGIN On_Timer Send
app.run_python_function(app.FObj, app.get_current_mp_name(), __FUNCTION__, "");
// C INTERFACE END On_Timer Send

} catch (...) { log_nok("CRASH detected"); app.terminate_application(); }}
// CODE BLOCK END On_Timer Send


// CODE BLOCK BEGIN On_Start Init
// 启动事件 "Init"
void on_start_Init(void) { try { // 程序启动事件
// C INTERFACE BEGIN On_Start Init
app.run_python_function(app.FObj, app.get_current_mp_name(), __FUNCTION__, "");
// C INTERFACE END On_Start Init

} catch (...) { log_nok("CRASH detected"); app.terminate_application(); }}
// CODE BLOCK END On_Start Init


// CODE BLOCK BEGIN On_Stop Stop
// 停止事件 "Stop"
void on_stop_Stop(void) { try { // 程序停止事件
// C INTERFACE BEGIN On_Stop Stop
app.run_python_function(app.FObj, app.get_current_mp_name(), __FUNCTION__, "");
// C INTERFACE END On_Stop Stop

} catch (...) { log_nok("CRASH detected"); app.terminate_application(); }}
// CODE BLOCK END On_Stop Stop


