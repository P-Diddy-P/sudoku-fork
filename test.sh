for input in ./tests/test_inputs/*; do
	fn=$(basename $input)
	tcsh ft.tcshrc $input $fn
	cat $input >>"./tests/test_results/$fn"
done