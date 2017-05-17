cd `dirname $0`

PROJECT=$1
OUTDIR="_out/$1"

if [ ! -d $OUTDIR ] 
    then 
        mkdir -p $OUTDIR
    fi

cd $OUTDIR
cmake -DUSE_wchar_t=off ../../$PROJECT/ && make && ./${PROJECT}Tests
