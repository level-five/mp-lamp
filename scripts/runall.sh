#!/bin/bash

# TODO: Run parallel jobs.

simple () {
    samples=200
    features=50
    r0=0.5
    np=2
    instance="synth_${samples}_${features}_${r0}"
    method="jinnai-0.95"
    RESULT=../results/$instance.${method}.np$np.a5.stat
    echo "qsub -pe orte $np -v inst=\"$instance\",method=\"${method}\",np=\"$np\" -e \"../results/$instance.${method}.np$np.a5.e\" -o \"$RESULT\"  run.sh"
    if [ -f $RESULT ]
       rm $RESULT
    fi
    qsub -pe orte $np -v instance="$instance",method="${method}",np="$np" -e "../results/$instance.${method}.np$np.a5.e" -o "$RESULT"  run.sh
}
simple
exit

RUN="run-raiden.sh"

testsummary () {
    vsamples=200
    vfeatures="50 100"
    rs="0.5"
    jids=""
    results=""
    method="jinnai-0.95"
    summary=../results/summary/${method}.a5
#    nps="4 8 16"
    nps="4"
    for np in $nps
    do
	for samples in $vsamples
	do
	    for features in $vfeatures
	    do
		for r0 in $rs
		do
		    instance="synth_${samples}_${features}_${r0}"
		    RESULT=../results/$instance.${method}.np$np.a5.stat
		    id=`qsub -pe impi $np -v instanced=$instance,method=${method},np=$np -e "../results.$instance.${method}.np$np.a5.e" -o "$RESULT"  $RUN | awk '{print $3}'`
		    jids="$jids,$id"
		    results="${results} ${RESULT}"
		done
	    done
	done
    done
    jids=`echo $jids | sed -r 's/^.//'`
    #    results=`echo $results | sed -r 's/^.{2}//'`
    echo "jids=$jids"
    echo "results=$results"
    qsub -v results="$results",summary="$summary" \
	-hold_jid $jids summary.sh
}
#testsummary
#exit

