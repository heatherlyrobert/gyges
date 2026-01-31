#================================[[ beg-code ]]================================#
include /usr/local/sbin/make_prepare



#===(user defined)=========================================#
# basename of executable, header, and all code files
NAME_BASE  = gyges
# additional standard and outside libraries
LIB_STD    = $(LIB_CURSTD)
# all heatherly libraries used, debug versions whenever available
#            123456789-1234 123456789-1234 123456789-1234 123456789-1234 123456789-1234 123456789-1234 123456789-1234 123456789-1234 123456789-1234
LIB_CORE   = -lyLOG_debug   -lyURG_debug   -lySTR_debug
LIB_VIKEYS = -lyMODE_debug  -lyKEYS_debug  -lyMACRO_debug -lySRC_debug   -lyFILE_debug  -lyVIEW_debug  -lyMAP_debug   -lyCMD_debug   -lyMARK_debug
LIB_OTHER  = -lyRPN_debug   -lyREGEX_debug -lyCALC_debug  -lyPARSE_debug -lySORT_debug  -lyEXEC_debug
LIB_GRAPH  = -lncurses      -ltinfo        -lyVICURSES_debug   -lyX11_debug
LIB_MINE   = $(LIB_CORE)   $(LIB_VIKEYS)   $(LIB_OTHER)   $(LIB_GRAPH) 
# directory for production code, no trailing slash
INST_DIR   = /usr/local/bin



#===(optional)=============================================#
# extra include directories required
INC_MINE   = 
# utilities generated, separate from main program
NAME_UTIL  = 
# libraries only for the utilities
LIB_UTIL   = 



#===(post-processing)======================================#
# create a rule for...
#install_prep       :
#remove_prep        :



#===(master template)======================================#
include /usr/local/sbin/zeno_make



#===(post-processing)======================================#
# create a rule for...
#install_post       :
#remove_post        :



#================================[[ end-code ]]================================#
