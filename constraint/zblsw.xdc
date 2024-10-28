# ZedBoard xdc
# zLEDs [7:0]
############################
# On-board led             #
############################
set_property PACKAGE_PIN T22 [get_ports zLED[0]]
set_property PACKAGE_PIN T21 [get_ports zLED[1]]
set_property PACKAGE_PIN U22 [get_ports zLED[2]]
set_property PACKAGE_PIN U21 [get_ports zLED[3]]
set_property PACKAGE_PIN V22 [get_ports zLED[4]]
set_property PACKAGE_PIN W22 [get_ports zLED[5]]
set_property PACKAGE_PIN U19 [get_ports zLED[6]]
set_property PACKAGE_PIN U14 [get_ports zLED[7]]
set_property IOSTANDARD LVCMOS33 [get_ports zLED]

# zSwitch [7:0]
############################
# On-board switches        #
############################
set_property PACKAGE_PIN F22 [get_ports zSwitch[0]]
set_property PACKAGE_PIN G22 [get_ports zSwitch[1]]
set_property PACKAGE_PIN H22 [get_ports zSwitch[2]]
set_property PACKAGE_PIN F21 [get_ports zSwitch[3]]
set_property PACKAGE_PIN H19 [get_ports zSwitch[4]]
set_property PACKAGE_PIN H18 [get_ports zSwitch[5]]
set_property PACKAGE_PIN H17 [get_ports zSwitch[6]]
set_property PACKAGE_PIN M15 [get_ports zSwitch[7]]
set_property IOSTANDARD LVCMOS18 [get_ports zSwitch]

# zPushB [4:0]
############################
# On-board pushbuttons     #
############################
set_property PACKAGE_PIN P16 [get_ports zPushB[0]]
set_property PACKAGE_PIN R16 [get_ports zPushB[1]]
set_property PACKAGE_PIN N15 [get_ports zPushB[2]]
set_property PACKAGE_PIN R18 [get_ports zPushB[3]]
set_property PACKAGE_PIN T18 [get_ports zPushB[4]]
set_property IOSTANDARD LVCMOS18 [get_ports zPushB]
