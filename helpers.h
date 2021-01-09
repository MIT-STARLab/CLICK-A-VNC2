/*
** Filename: helpers.h
** Author: Ondrej
** 
** Helper VOS definitions
*/

#ifndef _helpers_H_
#define _helpers_H_

/* Editor IntelliSense doesn't understand some VNC2 compiler flags */
#ifndef __INTELLISENSE__
    #define ROM rom
    #define ENTER_CRITICAL_SECTION VOS_ENTER_CRITICAL_SECTION
    #define EXIT_CRITICAL_SECTION VOS_EXIT_CRITICAL_SECTION
#else /* __INTELLISENSE__ */
    #define ROM
    #define ENTER_CRITICAL_SECTION
    #define EXIT_CRITICAL_SECTION
#endif /* __INTELLISENSE__ */

#endif /* _helpers_H_ */
