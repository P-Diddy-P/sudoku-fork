index=0

for input in ./tests/test_inputs/*; do
	index=`expr $index + 1`
	echo "test $index"
	fn=$(basename $input)
	tcsh ft.tcshrc $input $fn
	cat $input >>"./tests/test_results/$fn"
done