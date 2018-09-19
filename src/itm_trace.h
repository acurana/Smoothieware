/*
 * itm_trace.h
 *
 *  Created on: 27.12.2016
 *      Author: chris
 */

#ifndef SRC_ITM_TRACE_H_
#define SRC_ITM_TRACE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * configure trace to output IRQ timing on SWO
 */
void configure_tracing(void);


#ifdef __cplusplus
} // extern "C" {
#endif


#endif /* SRC_ITM_TRACE_H_ */

