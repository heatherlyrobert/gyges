#===(source start)======================================================================================================================================================#



#===[[ VARABLES ]]======================================================================================================================================================#

#===(current variables)=================================================================================================================================================#
BASE    = gyges
DEBUG   = ${BASE}_debug
UNIT    = ${BASE}_unit
HDIR    = /home/system/gyges.technical_spreadsheet
IDIR    = /usr/local/bin
MDIR    = /usr/share/man/man1

#===(compilier variables)===============================================================================================================================================#
# must have "-x c" on gcc line so stripped files work with alternate extensions
COMP    = gcc -c -std=gnu89 -x c -g -pg -Wall -Wextra
INCS    = -I/usr/local/include 

#===(linker options)========================================================================================================================================================================#
#------   (0)-------------- (1)-------------- (2)-------------- (3)-------------- (4)-------------- (5)-------------- (6)-------------- (7)-------------- (8)-------------- (9)--------------
LINK    = gcc
LIBDIR  = -L/usr/local/lib
LIBS    = ${LIBDIR}         -lncurses         -lySTR            -lyRPN            -lm
LIBD    = ${LIBDIR}         -lncurses         -lySTR_debug      -lyRPN            -lm               -lyLOG
LIBU    = ${LIBDIR}         -lncurses         -lySTR            -lyRPN            -lm               -lyLOG            -lyUNIT           -lyVAR         

#===(file lists)============================================================================================================================================================================#
#------   (0)-------------- (1)-------------- (2)-------------- (3)-------------- (4)-------------- (5)-------------- (6)-------------- (7)-------------- (8)-------------- (9)-------------- (A)-------------- (B)-------------- (C)-------------- (D)-------------- (5)--------------
HEADS   = ${BASE}.h
OBJS    = ${BASE}_main.os   ${BASE}_prog.os   ${BASE}_move.os   ${BASE}_keys.os   ${BASE}_sel.os    ${BASE}_reg.os    ${BASE}_calc.os   ${BASE}_rpn.os    ${BASE}_dep.os    ${BASE}_cell.os   ${BASE}_curs.os   ${BASE}_file.os   ${BASE}_loc.os
OBJD    = ${BASE}_main.o    ${BASE}_prog.o    ${BASE}_move.o    ${BASE}_keys.o    ${BASE}_sel.o     ${BASE}_reg.o     ${BASE}_calc.o    ${BASE}_rpn.o     ${BASE}_dep.o     ${BASE}_cell.o    ${BASE}_curs.o    ${BASE}_file.o    ${BASE}_loc.o 
OBJU    = ${BASE}_unit.o    ${BASE}_prog.o    ${BASE}_move.o    ${BASE}_keys.o    ${BASE}_sel.o     ${BASE}_reg.o     ${BASE}_calc.o    ${BASE}_rpn.o     ${BASE}_dep.o     ${BASE}_cell.o    ${BASE}_curs.o    ${BASE}_file.o    ${BASE}_loc.o

#===(make variables)====================================================================================================================================================#
COPY    = cp -f
CLEAN   = rm -f
PRINT   = @printf
STRIP   = @grep -v -e " DEBUG_" -e " yLOG_" 







#*============================----beg-of-source---============================*#

#*---(base name)----------------------*#
#BASE    = s
#FORMAL  = gyges



#*---(standard variables)-------------*#
#COMP_L  = gcc -c -std=gnu89 -g -pg -D yLOG
#COMP    = gcc -c -std=gnu89 -g -pg
#COMP_L  = gcc -c -std=gnu89 -g -pg -Wall -Wextra -D yLOG
#COMP    = gcc -c -std=gnu89 -g -pg -Wall -Wextra
#INCS    = -I/usr/local/include 
#LINK    = gcc 
#LIBS    = -L/usr/local/libs -lncurses -lm -lyLOG -lyRPN
#COPY    = cp -f
#CLEAN   = rm -f
#ECHO    = @echo

#OBJS    = ${BASE}_prog.o   ${BASE}_move.o   ${BASE}_keys.o   \
#			 ${BASE}_sel.o    ${BASE}_calc.o   ${BASE}_rpn.o    \
#			 ${BASE}_dep.o    ${BASE}_cell.o   ${BASE}_curs.o   \
#			 ${BASE}_file.o   ${BASE}_loc.o    ${BASE}_reg.o

#OBJS_L  = ${BASE}_Lprog.o  ${BASE}_Lmove.o  ${BASE}_Lkeys.o  \
#			 ${BASE}_Lsel.o   ${BASE}_Lcalc.o  ${BASE}_Lrpn.o   \
#			 ${BASE}_Ldep.o   ${BASE}_Lcell.o  ${BASE}_Lcurs.o  \
#			 ${BASE}_Lfile.o  ${BASE}_Lloc.o   ${BASE}_Lreg.o



#===[[ EXECUTABLES ]]===================================================================================================================================================#

all                : ${DEBUG} ${BASE} ${UNIT}

${BASE}            : ${OBJD}
	${LINK}  -o ${BASE}        ${OBJS}   ${LIBS}

${DEBUG}           : ${OBJD}
	${LINK}  -o ${DEBUG}       ${OBJD}   ${LIBD}

${UNIT}            : ${OBJU}
	${LINK}  -o ${UNIT}        ${OBJU}   ${LIBU}



#===[[ OBJECT FILES ]]==================================================================================================================================================#

${BASE}_main.o     : ${HEADS}       ${BASE}_main.c
	${COMP}    ${BASE}_main.c                           ${INC}
	${STRIP}   ${BASE}_main.c      > ${BASE}_main.cs
	${COMP}    ${BASE}_main.cs    -o ${BASE}_main.os    ${INC}

${BASE}_prog.o     : ${HEADS}       ${BASE}_prog.c
	${COMP}    ${BASE}_prog.c                           ${INC}
	${STRIP}   ${BASE}_prog.c      > ${BASE}_prog.cs
	${COMP}    ${BASE}_prog.cs    -o ${BASE}_prog.os    ${INC}

${BASE}_move.o     : ${HEADS}       ${BASE}_move.c
	${COMP}    ${BASE}_move.c                           ${INC}
	${STRIP}   ${BASE}_move.c      > ${BASE}_move.cs
	${COMP}    ${BASE}_move.cs    -o ${BASE}_move.os    ${INC}

${BASE}_keys.o     : ${HEADS}       ${BASE}_keys.c
	${COMP}    ${BASE}_keys.c                           ${INC}
	${STRIP}   ${BASE}_keys.c      > ${BASE}_keys.cs
	${COMP}    ${BASE}_keys.cs    -o ${BASE}_keys.os    ${INC}

${BASE}_sel.o      : ${HEADS}       ${BASE}_sel.c
	${COMP}    ${BASE}_sel.c                            ${INC}
	${STRIP}   ${BASE}_sel.c       > ${BASE}_sel.cs
	${COMP}    ${BASE}_sel.cs     -o ${BASE}_sel.os     ${INC}

${BASE}_reg.o      : ${HEADS}       ${BASE}_reg.c
	${COMP}    ${BASE}_reg.c                            ${INC}
	${STRIP}   ${BASE}_reg.c       > ${BASE}_reg.cs
	${COMP}    ${BASE}_reg.cs     -o ${BASE}_reg.os     ${INC}

${BASE}_calc.o     : ${HEADS}       ${BASE}_calc.c
	${COMP}    ${BASE}_calc.c                           ${INC}
	${STRIP}   ${BASE}_calc.c      > ${BASE}_calc.cs
	${COMP}    ${BASE}_calc.cs    -o ${BASE}_calc.os    ${INC}

${BASE}_rpn.o      : ${HEADS}       ${BASE}_rpn.c
	${COMP}    ${BASE}_rpn.c                            ${INC}
	${STRIP}   ${BASE}_rpn.c       > ${BASE}_rpn.cs
	${COMP}    ${BASE}_rpn.cs     -o ${BASE}_rpn.os     ${INC}

${BASE}_dep.o      : ${HEADS}       ${BASE}_dep.c
	${COMP}    ${BASE}_dep.c                            ${INC}
	${STRIP}   ${BASE}_dep.c       > ${BASE}_dep.cs
	${COMP}    ${BASE}_dep.cs     -o ${BASE}_dep.os     ${INC}

${BASE}_cell.o     : ${HEADS}       ${BASE}_cell.c
	${COMP}    ${BASE}_cell.c                           ${INC}
	${STRIP}   ${BASE}_cell.c      > ${BASE}_cell.cs
	${COMP}    ${BASE}_cell.cs    -o ${BASE}_cell.os    ${INC}

${BASE}_curs.o     : ${HEADS}       ${BASE}_curs.c
	${COMP}    ${BASE}_curs.c                           ${INC}
	${STRIP}   ${BASE}_curs.c      > ${BASE}_curs.cs
	${COMP}    ${BASE}_curs.cs    -o ${BASE}_curs.os    ${INC}

${BASE}_file.o     : ${HEADS}       ${BASE}_file.c
	${COMP}    ${BASE}_file.c                           ${INC}
	${STRIP}   ${BASE}_file.c      > ${BASE}_file.cs
	${COMP}    ${BASE}_file.cs    -o ${BASE}_file.os    ${INC}

${BASE}_loc.o      : ${HEADS}       ${BASE}_loc.c
	${COMP}    ${BASE}_loc.c                            ${INC}
	${STRIP}   ${BASE}_loc.c       > ${BASE}_loc.cs
	${COMP}    ${BASE}_loc.cs     -o ${BASE}_loc.os     ${INC}

${BASE}_unit.o     : ${BASE}.unit
	koios    ${BASE}
	${COMP}  ${BASE}_unit.c




#all                : ${FORMAL} ${BASE}_unit

#*---(executables)--------------------*#
#${FORMAL}          : ${OBJS} ${BASE}_main.o
#	${LINK}  -o ${FORMAL}      ${OBJS}    ${BASE}_main.o   ${LIBS}
#	${LINK}  -o ${FORMAL}_log  ${OBJS_L}  ${BASE}_Lmain.o  ${LIBS}


#${BASE}_unit       : ${OBJS} ${BASE}_main.o ${BASE}_test.o ${BASE}_unit.o 
#	${LINK}  -o ${BASE}_unit   ${OBJS_L}  ${BASE}_test.o  ${BASE}_unit.o ${LIBS} -lyUNIT -lyVAR



#*---(normal objects)-----------------*#

#${BASE}_main.o     : ${BASE}.h ${BASE}_main.c
#	${COMP}    ${BASE}_main.c
#	${COMP_L}  ${BASE}_main.c  -o ${BASE}_Lmain.o

#${BASE}_prog.o     : ${BASE}.h ${BASE}_prog.c
#	${COMP}    ${BASE}_prog.c
#	${COMP_L}  ${BASE}_prog.c  -o ${BASE}_Lprog.o

#${BASE}_move.o     : ${BASE}.h ${BASE}_move.c
#	${COMP}    ${BASE}_move.c
#	${COMP_L}  ${BASE}_move.c  -o ${BASE}_Lmove.o

#${BASE}_keys.o     : ${BASE}.h ${BASE}_keys.c
#	${COMP}    ${BASE}_keys.c
#	${COMP_L}  ${BASE}_keys.c  -o ${BASE}_Lkeys.o

#${BASE}_sel.o      : ${BASE}.h ${BASE}_sel.c
#	${COMP}    ${BASE}_sel.c
#	${COMP_L}  ${BASE}_sel.c   -o ${BASE}_Lsel.o

#${BASE}_reg.o      : ${BASE}.h ${BASE}_reg.c
#	${COMP}    ${BASE}_reg.c
#	${COMP_L}  ${BASE}_reg.c   -o ${BASE}_Lreg.o

#${BASE}_calc.o     : ${BASE}.h ${BASE}_calc.c
#	${COMP}    ${BASE}_calc.c
#	${COMP_L}  ${BASE}_calc.c  -o ${BASE}_Lcalc.o

#${BASE}_rpn.o      : ${BASE}.h ${BASE}_rpn.c
#	${COMP}    ${BASE}_rpn.c
#	${COMP_L}  ${BASE}_rpn.c   -o ${BASE}_Lrpn.o

#${BASE}_cell.o     : ${BASE}.h ${BASE}_cell.c
#	${COMP}    ${BASE}_cell.c
#	${COMP_L}  ${BASE}_cell.c  -o ${BASE}_Lcell.o

#${BASE}_dep.o      : ${BASE}.h ${BASE}_dep.c
#	${COMP}    ${BASE}_dep.c
#	${COMP_L}  ${BASE}_dep.c   -o ${BASE}_Ldep.o

#${BASE}_curs.o     : ${BASE}.h ${BASE}_curs.c
#	${COMP}    ${BASE}_curs.c
#	${COMP_L}  ${BASE}_curs.c  -o ${BASE}_Lcurs.o

#${BASE}_file.o     : ${BASE}.h ${BASE}_file.c
#	${COMP}    ${BASE}_file.c
#	${COMP_L}  ${BASE}_file.c  -o ${BASE}_Lfile.o

#${BASE}_loc.o      : ${BASE}.h ${BASE}_loc.c
#	${COMP}    ${BASE}_loc.c
#	${COMP_L}  ${BASE}_loc.c   -o ${BASE}_Lloc.o


#*---(test objects)-------------------*#

#${BASE}_test.o     : ${BASE}.h ${BASE}_test.c
#	${COMP}    ${BASE}_test.c

#${BASE}_unit.o     : ${BASE}.h ${BASE}.unit
#	uUNIT      ${BASE}
#	${COMP}  -x c ${BASE}_unit.code
#	mv         ${BASE}_unit.code ${BASE}_unit.c
#	${COMP}    ${BASE}_unit.c


#*---(housekeeping)-------------------*#

clean              :
	#---(all versions)--------------------#
	${CLEAN} ${BASE}
	${CLEAN} ${DEBUG}
	${CLEAN} ${UNIT}
	#---(object and stripped files)-------#
	${CLEAN} ${BASE}*.o
	${CLEAN} ${BASE}*.cs
	${CLEAN} ${BASE}*.os
	${CLEAN} ${BASE}*.Sc
	${CLEAN} ${BASE}*.So
	#---(created unit code)---------------#
	${CLEAN} ${UNIT}.code
	${CLEAN} ${UNIT}.c
	#---(junk files)----------------------#
	${CLEAN} *~
	${CLEAN} temp*

bigclean           :
	${CLEAN} .*.swp

install            : ${BASE}
	#---(production version)--------------#
	${COPY}   ${BASE}    ${IDIR}/
	chown     root:root  ${IDIR}/${BASE}
	chmod     0755       ${IDIR}/${BASE}
	@sha1sum  ${BASE}
	#---(debug version)-------------------#
	${COPY}  ${DEBUG}    ${IDIR}/
	chown     root:root  ${IDIR}/${DEBUG}
	chmod     0755       ${IDIR}/${DEBUG}
	@sha1sum  ${DEBUG}
	#---(man page)------------------------#
	rm -f       ${MDIR}/${BASE}.1.bz2
	cp -f       ${BASE}.1  ${MDIR}/
	bzip2       ${MDIR}/${BASE}.1
	chmod 0644  ${MDIR}/${BASE}.1.bz2

remove             :
	#---(all versions)--------------------#
	${CLEAN}  ${IDIR}/${BASE}
	${CLEAN}  ${IDIR}/${DEBUG}
	#---(documentation)-------------------#
	${CLEAN}  ${MDIR}/${BASE}.1.bz2



#*============================----end-of-source---============================*#
