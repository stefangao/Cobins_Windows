#ifndef __WTERMIN_H__
#define	__WTERMIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#define WT_ENABLE    1

/** @defgroup groupwtermin wtermin
 *  This is a module providing trace and packet print.
\n\n<b>Header:</b> Declared in wtermin.h.\n
    <b>Import Library:</b> Use wtermin.lib.\n
 *  @{
 */

typedef enum
{
    WT_TEXTTYPE_SEND,    //show in view(0,0)
    WT_BYTETYPE_SEND,    //show in view(0,0)
    WT_TEXTTYPE_RECV,    //show in view(0,0)
    WT_BYTETYPE_RECV,    //show in view(0,0)

    WT_TEXTTYPE_TRACE,   //show in view(1,0)
    WT_BYTETYPE_TRACE,   //show in view(1,0)
    WT_TEXTTYPE_PRINT,   //show in view(1,0)
    WT_BYTETYPE_PRINT,   //show in view(1,0) 

} WT_DATATYPE;

#if WT_ENABLE >= 1
#pragma comment(lib,"wtermin.lib") 

void  WT_Trace(const char* fmt, ...);
void  WT_Printf(const char* fmt, ...);
void  WT_Error(const char* fmt, ...);
void  WT_PutData( WT_DATATYPE type, void* data, int len );
void  WT_SetTerminWnd( const void* hWnd );
void* WT_GetTerminWnd( );
#else
#define WT_Trace
#define WT_Printf
#define WT_Error
//void    WT_Error(const char* fmt, ...);
#define WT_PutData
#define WT_SetTerminWnd
#define WT_GetTerminWnd
#endif

/** @} */ // end of groupwtermin

#ifdef __cplusplus
}
#endif

#endif
