kalc:kalc.c
	gcc kalc.c -Os -o kalc
	echo "(7+9*2)*(8-1)/(1+3-2)" | ./kalc
