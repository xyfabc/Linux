/*************************************************
　__DATE__
　　进行预处理的日期（“Mmm dd yyyy”形式的字符串文字）
　　__FILE__
　　代表当前源代码文件名的字符串文字
　　__LINE__
　　代表当前源代码中的行号的整数常量
　　__TIME__
　　源文件编译时间，格式微“hh：mm：ss”
　　__func__
　　当前所在函数名

***************************************************/


/* Turn-on extra printk debug */
#define HANVON_DEBUG_PRINT

#ifdef HANVON_DEBUG_PRINT
#define HANVON_DBG(fmt, args...)  {printk("%d:%s:",__LINE__,__func__); printk(fmt, ## args);printk("\n");}
#else
#define HANVON_DBG(fmt, args...)  do { } while (0)
#endif
