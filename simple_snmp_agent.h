#ifndef __SIMPLE_SNMP_AGENT_H__
#define __SIMPLE_SNMP_AGENT_H__

/* transport information to my_mib.c */
extern const struct snmp_mib ubirch_mib;

char *snmp_data_buffer;

void snmp_start(void);

#endif