idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ".")

# Unique ULP binary name 
set(ulp_app_name ulp_${COMPONENT_NAME})

# ULP source file
set(ulp_sources "ulp/ulp.c")
set(ulp_exp_dep_srcs "main.c")

# ULP binary into component
ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}")