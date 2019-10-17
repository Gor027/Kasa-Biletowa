#!/bin/bash
set -e

prog=$1
dir=$2

if [ ! -f "$prog" ]; then
	if [ ! -f "$dir" ]; then 
		echo "Executable file not found!"
    	exit 1
    else
    	if [ ! -d "$prog" ]; then
    			echo "Could not find test directory!"
  				exit 1
    		else
    			prog=$2
    			dir=$1
    	fi
    fi
else
	if [ ! -d "$dir" ]; then
		echo " Could not find test directory!"
		exit 1
	fi
fi

cd ${dir}

for f in ./*.in; do
  test=${f%.*}
  expect_stderr="${test}.err"
  expect_stdout="${test}.out"
  printf "Running Test ${test} ... "
  cat ${f} | ../${prog} 2> ../temperr 1> ../tempout
  if diff "../tempout" ${expect_stdout} && diff "../temperr" ${expect_stderr}; then
  	  echo "PASSED"
  else
  	  echo "FAILED"
  fi
done

#If there are no errors remove temporary files
rm -rf "../temperr"
rm -rf "../tempout"