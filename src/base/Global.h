#pragma once

#include <tchar.h>
#include <assert.h>
#include <Classes.hpp>
#include <Sysutils.hpp>

#define FORMAT(S, ...) ::Format(S, ##__VA_ARGS__)
#define FMTLOAD(Id, ...) ::FmtLoadStr(Id, ##__VA_ARGS__)
#define FLAGSET(SET, FLAG) (((SET) & (FLAG)) == (FLAG))
#define FLAGCLEAR(SET, FLAG) (((SET) & (FLAG)) == 0)
#define FLAGMASK(ENABLE, FLAG) ((ENABLE) ? (FLAG) : 0)

void ShowExtendedException(Exception * E);
bool AppendExceptionStackTraceAndForget(TStrings *& MoreMessages);

class TGuard : public TObject
{
NB_DISABLE_COPY(TGuard)
public:
  explicit TGuard(const TCriticalSection & ACriticalSection);
  ~TGuard();

private:
  const TCriticalSection & FCriticalSection;
};

class TUnguard : public TObject
{
NB_DISABLE_COPY(TUnguard)
public:
  explicit TUnguard(TCriticalSection & ACriticalSection);
  ~TUnguard();

private:
  TCriticalSection & FCriticalSection;
};

#if !defined(_DEBUG)

#define DebugAssert(p) (void)(p)
#define DebugCheck(p) (p)
#define DebugFail() (void)0

#define DebugAlwaysTrue(p) (p)
#define DebugAlwaysFalse(p) (p)
#define DebugNotNull(p) (p)

#else // _DEBUG

void SetTraceFile(HANDLE TraceFile);
void CleanupTracing();
#define TRACEENV "WINSCPTRACE"
extern BOOL IsTracing;
const unsigned int CallstackTlsOff = (unsigned int)-1;
extern unsigned int CallstackTls;
extern "C" void Trace(const wchar_t * SourceFile, const wchar_t * Func,
  int Line, const wchar_t * Message);
void TraceFmt(const wchar_t * SourceFile, const wchar_t * Func,
  int Line, const wchar_t * Format, va_list Args);

#ifdef TRACE_IN_MEMORY

void TraceDumpToFile();
void TraceInMemoryCallback(const wchar_t * Msg);

#endif // TRACE_IN_MEMORY

#define ACCESS_VIOLATION_TEST { (*((int*)NULL)) = 0; }

void DoAssert(wchar_t * Message, wchar_t * Filename, int LineNumber);

#define DebugAssert(p) ((p) ? (void)0 : DoAssert(TEXT(#p), TEXT(__FILE__), __LINE__))
#define DebugCheck(p) { bool __CHECK_RESULT__ = (p); DebugAssert(__CHECK_RESULT__); }
#define DebugFail() DebugAssert(false)

inline bool DoAlwaysTrue(bool Value, wchar_t * Message, wchar_t * Filename, int LineNumber)
{
  if (!Value)
  {
    DoAssert(Message, Filename, LineNumber);
  }
  return Value;
}

inline bool DoAlwaysFalse(bool Value, wchar_t * Message, wchar_t * Filename, int LineNumber)
{
  if (Value)
  {
    DoAssert(Message, Filename, LineNumber);
  }
  return Value;
}

template<typename T>
inline typename T * DoCheckNotNull(T* p, wchar_t * Message, wchar_t * Filename, int LineNumber)
{
  if (p == NULL)
  {
    DoAssert(Message, Filename, LineNumber);
  }
  return p;
}

#define DebugAlwaysTrue(p) DoAlwaysTrue((p), TEXT(#p), TEXT(__FILE__), __LINE__)
#define DebugAlwaysFalse(p) DoAlwaysFalse((p), TEXT(#p), TEXT(__FILE__), __LINE__)
#define DebugNotNull(p) DoCheckNotNull((p), TEXT(#p), TEXT(__FILE__), __LINE__)

#endif // _DEBUG

#define DebugUsedParam(p) (void)(p)

#define MB_TEXT(x) const_cast<wchar_t *>(::MB2W(x).c_str())

#if defined(__MINGW32__) && (__MINGW_GCC_VERSION < 50100)
typedef struct _TIME_DYNAMIC_ZONE_INFORMATION
{
  LONG       Bias;
  WCHAR      StandardName[32];
  SYSTEMTIME StandardDate;
  LONG       StandardBias;
  WCHAR      DaylightName[32];
  SYSTEMTIME DaylightDate;
  LONG       DaylightBias;
  WCHAR      TimeZoneKeyName[128];
  BOOLEAN    DynamicDaylightTimeDisabled;
} DYNAMIC_TIME_ZONE_INFORMATION, *PDYNAMIC_TIME_ZONE_INFORMATION;
#endif
