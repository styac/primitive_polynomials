#/bin/bash

export SRCDIR=`pwd`
export BINDIR=${SRCDIR}_build
export GENERATOR=" "

if [ "${1,,}" == "ninja" ]
then 
	GENERATOR="-G Ninja "
	echo " ** generate Ninja files"
else
	echo " ** generate make files"
fi

for PRJ in Primpoly PrimpolyC
do	
	export PRJDIR=${SRCDIR}/${PRJ}
	export BINDIR=${PRJDIR}_build
	if [ ! -d "${BINDIR}" ]; then
		mkdir ${BINDIR}
	fi

	if [ -d "${BINDIR}" ]; then
        	cd ${BINDIR}
	        echo "***  cd ${BINDIR} and run make **** "
		cmake ${GENERATOR} ${PRJDIR}
	else 
        	echo "***  can't make ${BINDIR} check access rights ***"
	fi
done

