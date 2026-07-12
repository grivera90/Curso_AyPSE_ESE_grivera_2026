file (GLOB srcs_files "../ads1115_drv/*.c"
						"../bsp/*.c")

idf_component_register(
    SRCS 			"main.c" ${srcs_files}
    INCLUDE_DIRS 	"../ads1115_drv"
    				"../bsp"
    PRIV_INCLUDE_DIRS   					
    REQUIRES 		esp_driver_i2c
    
    PRIV_REQUIRES       					# optional, list the private requirements
)

#idf_component_register(SRCS "x_template.c"
#                    INCLUDE_DIRS ""
#                    REQUIRES middleware board_support)
