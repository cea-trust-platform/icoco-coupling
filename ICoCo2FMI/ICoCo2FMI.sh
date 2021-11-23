#!/bin/bash

DIR=$(cd $(dirname $0);pwd)

datafile=""
varsfile=""
MODEL_NAME=""
ICOCOLIB=""
usage()
{
echo
echo Usage:
echo $0 -d datafile -v varsfile -n MODEL_NAME -l ICOCOLIB
echo or
echo $0 --datafile datafile --varsfile varsfile --name MODEL_NAME --library ICOCOLIB
}
while [ $# -ne 0  ]
  do
  if [ "$1"  = "-d" ] || [ "$1" = "--datafile" ]
      then 
      datafile=$2
  elif [ "$1"  = "-v" ] || [ "$1" = "--varsfile" ]
      then 
      varsfile=$2
  elif [ "$1"  = "-n" ] || [ "$1" = "--name" ]
      then 
      MODEL_NAME=$2 
  elif [ "$1" = "-l" ] || [ "$1" = "--library" ]
      then
      ICOCOLIB=$2
  else
      echo $1 unkown option
      usage 
      exit 1
  fi
  shift;shift
done

# check
[ "$datafile" = "" ] && echo datafile not set && usage && exit 1
[ "$varsfile" = "" ] && echo varsfile not set && usage && exit 1
[ "$MODEL_NAME" = "" ] && echo MODEL_NAME not set && usage && exit 1
[ "$ICOCOLIB" = "" ] && echo ICOCOLIB not set && usage && exit 1


#echo $datafile $varsfile $MODEL_NAME $ICOCOLIB

ICOCOINC=$DIR/../include

MODEL_GUID="{"$(echo $MODEL_NAME | md5sum | awk '{print $1}')"}"

echo " #define MODEL_IDENTIFIERi ${MODEL_NAME}
#define MODEL_GUID \"${MODEL_GUID}\"" > define_model.h

python $DIR/src/generate_files_from_json.py $varsfile $MODEL_NAME $MODEL_GUID || exit 1


echo "#include <fstream>
void    writeDataFile(std::string& datafile)
{
datafile=\"$(basename $datafile)\";
std::ofstream f(datafile.c_str());" > writeDataFile.h

while read line 
  do
  echo "f <<\"$line\"<<std::endl;"
done < $datafile  >> writeDataFile.h

echo "f.close();
}"  >>   writeDataFile.h



mkdir -p build
for f in vars_def.h modelDescription_cs.xml  writeDataFile.h define_model.h
  do
  mv -f $f build
done
(cd build; cmake $DIR/src -DCMAKE_BUILD_TYPE=Debug -DICOCODIR=$ICOCOINC -DICOCOLIB=$ICOCOLIB -DMODEL_NAME=$MODEL_NAME; make VERBOSE=1 ) || exit 2
mv build/$MODEL_NAME.fmu .

echo
echo
echo $PWD/$MODEL_NAME.fmu generated.
echo
echo
