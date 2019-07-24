set(COMPONENT_SRCS
        storage.c
        )
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_PRIV_REQUIRES nvs_flash)
register_component()