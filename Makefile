#================================[[ beg-code ]]================================#



#===(user defined)=========================================#
# basename of executable, header, and all code files
NAME_BASE  = gyges
# additional standard and outside libraries
LIB_STD    = -lm -lncurses
# all heatherly libraries used, debug versions whenever available
LIB_MINE   = -lySTR_debug -lyVIKEYS_debug -lyRPN_debug
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
include /usr/local/sbin/make_program



#===(post-processing)======================================#
# create a rule for...
#install_post       :
#remove_post        :



#================================[[ end-code ]]================================#


#===(source start)======================================================================================================================================================#



#===[[ VARABLES ]]======================================================================================================================================================#

#===(current variables)=================================================================================================================================================#
#BASE    = gyges
#DEBUG   = ${BASE}_debug
#UNIT    = ${BASE}_unit
#HDIR    = /home/system/gyges.technical_spreadsheet
#IDIR    = /usr/local/bin
#MDIR    = /usr/share/man/man1

#===(compilier variables)===============================================================================================================================================#
# must have "-x c" on gcc line so stripped files work with alternate extensions
#COMP    = gcc -c -std=gnu89 -x c -g -pg -Wall -Wextra
#INCS    = -I/usr/local/include 

#===(linker options)========================================================================================================================================================================#
#------   (0)-------------- (1)-------------- (2)-------------- (3)-------------- (4)-------------- (5)-------------- (6)-------------- (7)-------------- (8)-------------- (9)--------------
#LINK    = gcc
#LIBDIR  = -L/usr/local/lib
#LIBS    = ${LIBDIR}         -lncurses         -lyVIKEYS         -lySTR            -lyURG            -lyRPN            -lm
#LIBD    = ${LIBDIR}         -lncurses         -lyVIKEYS         -lySTR_debug      -lyURG_debug      -lyRPN            -lm               -lyLOG
#LIBU    = ${LIBD}           -lyUNIT           -lyVAR         

#===(file lists)============================================================================================================================================================================#
#------   (0)-------------- (1)-------------- (2)-------------- (3)-------------- (4)-------------- (5)-------------- (6)-------------- (7)-------------- (8)-------------- (9)-------------- (A)-------------- (B)-------------- (C)-------------- (D)-------------- (5)--------------
#HEADS   = ${BASE}.h
#OBJS    = ${BASE}_main.os   ${BASE}_prog.os   ${BASE}_move.os   ${BASE}_keys.os   ${BASE}_visu.os   ${BASE}_reg.os    ${BASE}_calc.os   ${BASE}_rpn.os    ${BASE}_dep.os    ${BASE}_cell.os   ${BASE}_curs.os   ${BASE}_file.os   ${BASE}_loc.os
#OBJD    = ${BASE}_main.o    ${BASE}_prog.o    ${BASE}_move.o    ${BASE}_keys.o    ${BASE}_visu.o    ${BASE}_reg.o     ${BASE}_calc.o    ${BASE}_rpn.o     ${BASE}_dep.o     ${BASE}_cell.o    ${BASE}_curs.o    ${BASE}_file.o    ${BASE}_loc.o 
#OBJU    = ${BASE}_unit.o    ${BASE}_prog.o    ${BASE}_move.o    ${BASE}_keys.o    ${BASE}_visu.o    ${BASE}_reg.o     ${BASE}_calc.o    ${BASE}_rpn.o     ${BASE}_dep.o     ${BASE}_cell.o    ${BASE}_curs.o    ${BASE}_file.o    ${BASE}_loc.o

#===(make variables)====================================================================================================================================================#
#COPY    = cp -f
#CLEAN   = rm -f
#PRINT   = @printf
#STRIP   = @grep -v -e " DEBUG_" -e " yLOG_" 



#===[[ EXECUTABLES ]]===================================================================================================================================================#

#all                : ${DEBUG} ${BASE} ${UNIT}

#${BASE}            : ${OBJD}
#	${LINK}  -o ${BASE}        ${OBJS}   ${LIBS}

#${DEBUG}           : ${OBJD}
#	${LINK}  -o ${DEBUG}       ${OBJD}   ${LIBD}

#${UNIT}            : ${OBJU}
#	${LINK}  -o ${UNIT}        ${OBJU}   ${LIBU}



#===[[ OBJECT FILES ]]==================================================================================================================================================#

#${BASE}_main.o     : ${HEADS}       ${BASE}_main.c
#	${COMP}    ${BASE}_main.c                           ${INC}
#	${STRIP}   ${BASE}_main.c      > ${BASE}_main.cs
#	${COMP}    ${BASE}_main.cs    -o ${BASE}_main.os    ${INC}
#
#${BASE}_prog.o     : ${HEADS}       ${BASE}_prog.c
#	${COMP}    ${BASE}_prog.c                           ${INC}
#	${STRIP}   ${BASE}_prog.c      > ${BASE}_prog.cs
#	${COMP}    ${BASE}_prog.cs    -o ${BASE}_prog.os    ${INC}
#
#${BASE}_move.o     : ${HEADS}       ${BASE}_move.c
#	${COMP}    ${BASE}_move.c                           ${INC}
#	${STRIP}   ${BASE}_move.c      > ${BASE}_move.cs
#	${COMP}    ${BASE}_move.cs    -o ${BASE}_move.os    ${INC}
#
#${BASE}_keys.o     : ${HEADS}       ${BASE}_keys.c
#	${COMP}    ${BASE}_keys.c                           ${INC}
#	${STRIP}   ${BASE}_keys.c      > ${BASE}_keys.cs
#	${COMP}    ${BASE}_keys.cs    -o ${BASE}_keys.os    ${INC}
#
#${BASE}_visu.o     : ${HEADS}       ${BASE}_visu.c
#	${COMP}    ${BASE}_visu.c                           ${INC}
#	${STRIP}   ${BASE}_visu.c      > ${BASE}_visu.cs
#	${COMP}    ${BASE}_visu.cs    -o ${BASE}_visu.os    ${INC}
#
#${BASE}_reg.o      : ${HEADS}       ${BASE}_reg.c
#	${COMP}    ${BASE}_reg.c                            ${INC}
#	${STRIP}   ${BASE}_reg.c       > ${BASE}_reg.cs
#	${COMP}    ${BASE}_reg.cs     -o ${BASE}_reg.os     ${INC}
#
#${BASE}_calc.o     : ${HEADS}       ${BASE}_calc.c
#	${COMP}    ${BASE}_calc.c                           ${INC}
#	${STRIP}   ${BASE}_calc.c      > ${BASE}_calc.cs
#	${COMP}    ${BASE}_calc.cs    -o ${BASE}_calc.os    ${INC}
#
#${BASE}_rpn.o      : ${HEADS}       ${BASE}_rpn.c
#	${COMP}    ${BASE}_rpn.c                            ${INC}
#	${STRIP}   ${BASE}_rpn.c       > ${BASE}_rpn.cs
#	${COMP}    ${BASE}_rpn.cs     -o ${BASE}_rpn.os     ${INC}
#
#${BASE}_dep.o      : ${HEADS}       ${BASE}_dep.c
#	${COMP}    ${BASE}_dep.c                            ${INC}
#	${STRIP}   ${BASE}_dep.c       > ${BASE}_dep.cs
#	${COMP}    ${BASE}_dep.cs     -o ${BASE}_dep.os     ${INC}
#
#${BASE}_cell.o     : ${HEADS}       ${BASE}_cell.c
#	${COMP}    ${BASE}_cell.c                           ${INC}
#	${STRIP}   ${BASE}_cell.c      > ${BASE}_cell.cs
#	${COMP}    ${BASE}_cell.cs    -o ${BASE}_cell.os    ${INC}
#
#${BASE}_curs.o     : ${HEADS}       ${BASE}_curs.c
#	${COMP}    ${BASE}_curs.c                           ${INC}
#	${STRIP}   ${BASE}_curs.c      > ${BASE}_curs.cs
#	${COMP}    ${BASE}_curs.cs    -o ${BASE}_curs.os    ${INC}
#
#${BASE}_file.o     : ${HEADS}       ${BASE}_file.c
#	${COMP}    ${BASE}_file.c                           ${INC}
#	${STRIP}   ${BASE}_file.c      > ${BASE}_file.cs
#	${COMP}    ${BASE}_file.cs    -o ${BASE}_file.os    ${INC}
#
#${BASE}_loc.o      : ${HEADS}       ${BASE}_loc.c
#	${COMP}    ${BASE}_loc.c                            ${INC}
#	${STRIP}   ${BASE}_loc.c       > ${BASE}_loc.cs
#	${COMP}    ${BASE}_loc.cs     -o ${BASE}_loc.os     ${INC}
#
#${BASE}_unit.o     : ${BASE}.unit
#	koios    ${BASE}
#	${COMP}  ${BASE}_unit.c



#*---(housekeeping)-------------------*#

#clean              :
#	#---(all versions)--------------------#
#	${CLEAN} ${BASE}
#	${CLEAN} ${DEBUG}
#	${CLEAN} ${UNIT}
#	#---(object and stripped files)-------#
#	${CLEAN} ${BASE}*.o
#	${CLEAN} ${BASE}*.cs
#	${CLEAN} ${BASE}*.os
#	${CLEAN} ${BASE}*.Sc
#	${CLEAN} ${BASE}*.So
#	#---(created unit code)---------------#
#	${CLEAN} ${UNIT}.code
#	${CLEAN} ${UNIT}.c
#	#---(junk files)----------------------#
#	${CLEAN} *~
#	${CLEAN} temp*

#bigclean           :
#	${CLEAN} .*.swp

#install            : ${BASE}
#	#---(production version)--------------#
#	${COPY}   ${BASE}    ${IDIR}/
#	chown     root:root  ${IDIR}/${BASE}
#	chmod     0755       ${IDIR}/${BASE}
#	@sha1sum  ${BASE}
#	#---(debug version)-------------------#
#	${COPY}  ${DEBUG}    ${IDIR}/
#	chown     root:root  ${IDIR}/${DEBUG}
#	chmod     0755       ${IDIR}/${DEBUG}
#	@sha1sum  ${DEBUG}
#	#---(man page)------------------------#
#	rm -f       ${MDIR}/${BASE}.1.bz2
#	cp -f       ${BASE}.1  ${MDIR}/
#	bzip2       ${MDIR}/${BASE}.1
#	chmod 0644  ${MDIR}/${BASE}.1.bz2

#remove             :
#	#---(all versions)--------------------#
#	${CLEAN}  ${IDIR}/${BASE}
#	${CLEAN}  ${IDIR}/${DEBUG}
#	#---(documentation)-------------------#
#	${CLEAN}  ${MDIR}/${BASE}.1.bz2



#*============================----end-of-source---============================*#
