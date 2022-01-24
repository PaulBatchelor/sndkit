gnuplot <<EOF
set terminal png
unset key
set output "sine.png"
plot "sine.dat" with lines
EOF
