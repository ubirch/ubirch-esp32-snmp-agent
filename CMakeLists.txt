set(COMPONENT_SRCS
        my_mib.c
        simple_snmp_agent.c
        )
set(COMPONENT_ADD_INCLUDEDIRS
        "."
        ../../../esp-idf/components/lwip/lwip/src/apps/snmp
        ../../../esp-idf/components/lwip/lwip/src/include/lwip/apps
        )

set(COMPONENT_PRIV_REQUIRES
        lwip
        ubirch-esp32-networking
        sht31
        )

register_component()