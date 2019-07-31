
#include <string.h>
#include <snmp_scalar.h>
#include <esp_log.h>
#include <snmp_asn1.h>
//#include <sensor.h>
//#include <esp32-hal.h>
//#include <util.h>
#include <freertos/event_groups.h>
#include <sht31.h>

#include "networking.h"
#include "my_mib.h"
#include "../../main/util.h"
#include "simple_snmp_agent.h"


extern unsigned char UUID[16];
/*
 * SNMP general function prototype
 * Add here any function to be usable in specific private MIB node.
 */
static s16_t get_temperature_value(struct snmp_node_instance *instance, void *value);
static s16_t get_humidity_value(struct snmp_node_instance *instance, void *value);
static s16_t get_timestamp_value(struct snmp_node_instance *instance, void *value);
static s16_t get_uuid_value(struct snmp_node_instance* instance, void* value);
static s16_t get_complete_package(struct snmp_node_instance *instance, void *value);

/* temperature      .1.3.6.1.4.1.54021.1.1 */
static const struct snmp_scalar_node temperature_node = SNMP_SCALAR_CREATE_NODE_READONLY(1, SNMP_ASN1_TYPE_INTEGER, get_temperature_value); // ready-only example

/* humidity         .1.3.6.1.4.1.54021.1.2 */
static const struct snmp_scalar_node humidity_node = SNMP_SCALAR_CREATE_NODE_READONLY(2, SNMP_ASN1_TYPE_INTEGER, get_humidity_value); // ready-only example

/* timestamp        .1.3.6.1.4.1.54021.1.3 */
static const struct snmp_scalar_node timestamp_node = SNMP_SCALAR_CREATE_NODE_READONLY(3,  SNMP_ASN1_TYPE_INTEGER, get_timestamp_value);

/* uuid             .1.3.6.1.4.1.54021.1.4 */
static const struct snmp_scalar_node uuid_node = SNMP_SCALAR_CREATE_NODE_READONLY(4, SNMP_ASN1_TYPE_OCTET_STRING, get_uuid_value);

/* complete package .1.3.6.1.4.1.54021.1.5 */
static const struct snmp_scalar_node package_node = SNMP_SCALAR_CREATE_NODE_READONLY(5, SNMP_ASN1_TYPE_OCTET_STRING, get_complete_package);

/* all private nodes  .1.3.6.1.4.1.54021.1 .. (.1,.2,.3,.4,.5) .0 */
static const struct snmp_node* const all_nodes[] = {
		&temperature_node.node.node,
		&humidity_node.node.node,
		&timestamp_node.node.node,
		&uuid_node.node.node,
		&package_node.node.node
};
static const struct snmp_tree_node ubirch_tree_node = SNMP_CREATE_TREE_NODE(1, all_nodes);

/* define private mib */
static const u32_t my_base_oid[] = { 1, 3, 6, 1, 4, 1, 54021, 1 };
const struct snmp_mib ubirch_mib = SNMP_MIB_CREATE(my_base_oid, &ubirch_tree_node.node);


/* temperature      .1.3.6.1.4.1.54021.1.1.0 */
static s16_t get_temperature_value(struct snmp_node_instance *instance, void *value) {
	float *float_ptr = (float *) value;
	sht31_readTempHum();
	*float_ptr = sht31_readTemperature();
	return sizeof(*float_ptr);
}

/* humidity         .1.3.6.1.4.1.54021.1.2.0 */
static s16_t get_humidity_value(struct snmp_node_instance *instance, void *value){
	float *float_ptr = (float *) value;
	sht31_readTempHum();
	*float_ptr = sht31_readHumidity();
	return sizeof(*float_ptr);
}

/* timestamp        .1.3.6.1.4.1.54021.1.3.0 */
static s16_t get_timestamp_value(struct snmp_node_instance *instance, void *value){
	u32_t *uint_ptr = (u32_t*) value;
	time_t now;
	time(&now);
	*uint_ptr = (u32_t)now;
	return sizeof(*uint_ptr);
}

/* uuid             .1.3.6.1.4.1.54021.1.4.0 */
static s16_t get_uuid_value(struct snmp_node_instance* instance, void* value) {
	u8_t *char_ptr = (u8_t *) value;
	memcpy(char_ptr, (u8_t *)UUID, UUID_SIZE);
	return UUID_SIZE * sizeof(*char_ptr);
}


union
{
	float f;
	time_t t;
	u8_t c[4];
}snmp_value;

/* complete package .1.3.6.1.4.1.54021.1.5.0 */
static s16_t get_complete_package(struct snmp_node_instance *instance, void *value) {
/*!
 * |----UUID----|--TIMESTAMP--|--TEMPERATURE--|--HUMIDITY--|
 * | (16 Byte)  |  (4 Byte)   |  (4 Byte)     |  (4 Byte)  |
 */

	u8_t buffer[SNMP_BUFFER_SIZE];

	memcpy(buffer, UUID, UUID_SIZE);

	time_t now;
	time(&snmp_value.t);
	memcpy((buffer + 16), snmp_value.c, 4);

	//TODO
	sht31_readTempHum();

	snmp_value.f = sht31_readTemperature();
	memcpy((buffer + 20), snmp_value.c, 4);

	snmp_value.f = sht31_readHumidity();
	memcpy((buffer + 24), snmp_value.c, 4);

	u8_t *uint_ptr = (u8_t*) value;
	memcpy(uint_ptr, buffer, SNMP_BUFFER_SIZE);
	memcpy(snmp_data_buffer, buffer, SNMP_BUFFER_SIZE);
	xEventGroupSetBits(network_event_group, SNMP_READY);

	return (SNMP_BUFFER_SIZE * sizeof(*uint_ptr));
}
