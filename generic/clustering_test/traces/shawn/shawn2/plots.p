set autoscale xy                      # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Topology Density"
set xlabel "nodes in topology"
set ylabel "avg node degree"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "density.png"
plot "stat1" using 1:2 title "set1" with linespoints, \
"stat2" using 1:2 title "set2" with linespoints, \
"stat3" using 1:2 title "set3" with linespoints, \
"stat6" using 1:2 title "set4" with linespoints, \
"stat11" using 1:2 title "set5" with linespoints

# K=1

set autoscale xy                      # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "clustering events"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "clevents_k1.png"
plot "1/per_k.dat" using 1:2 title "set1" with linespoints, \
"2/per_k.dat" using 1:2 title "set2" with linespoints, \
"3/per_k.dat" using 1:2 title "set3" with linespoints, \
"6/per_k.dat" using 1:2 title "set4" with linespoints, \
"11/per_k.dat" using 1:2 title "set5" with linespoints


set autoscale x                      # scale axes automatically
set yrange [0:120000]
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "# of messages"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "msgJOIN_k1.png"
plot "1/per_k.dat" using 1:3 title "set1" with linespoints, \
"2/per_k.dat" using 1:3 title "set2" with linespoints, \
"3/per_k.dat" using 1:3 title "set3" with linespoints, \
"6/per_k.dat" using 1:3 title "set4" with linespoints, \
"11/per_k.dat" using 1:3 title "set5" with linespoints

set autoscale x                      # scale axes automatically
set yrange [0:400000]
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "# of messages"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "msgRESUME_k1.png"
plot "1/per_k.dat" using 1:4 title "set1" with linespoints, \
"2/per_k.dat" using 1:4 title "set2" with linespoints, \
"3/per_k.dat" using 1:4 title "set3" with linespoints, \
"6/per_k.dat" using 1:4 title "set4" with linespoints, \
"11/per_k.dat" using 1:4 title "set5" with linespoints

set autoscale x                      # scale axes automatically
set yrange [0:900000]
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "# of messages"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "msgATTR_k1.png"
plot "1/per_k.dat" using 1:5 title "set1" with linespoints, \
"2/per_k.dat" using 1:5 title "set2" with linespoints, \
"3/per_k.dat" using 1:5 title "set3" with linespoints, \
"6/per_k.dat" using 1:5 title "set4" with linespoints, \
"11/per_k.dat" using 1:5 title "set5" with linespoints


# K=2


set autoscale xy                      # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "clustering events"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "clevents_k2.png"
plot "1/per_k.dat" using 1:6 title "set1" with linespoints, \
"2/per_k.dat" using 1:6 title "set2" with linespoints, \
"3/per_k.dat" using 1:6 title "set3" with linespoints, \
"6/per_k.dat" using 1:6 title "set4" with linespoints, \
"11/per_k.dat" using 1:6 title "set5" with linespoints


set autoscale x                      # scale axes automatically
set yrange [0:120000]
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "# of messages"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "msgJOIN_k2.png"
plot "1/per_k.dat" using 1:7 title "set1" with linespoints, \
"2/per_k.dat" using 1:7 title "set2" with linespoints, \
"3/per_k.dat" using 1:7 title "set3" with linespoints, \
"6/per_k.dat" using 1:7 title "set4" with linespoints, \
"11/per_k.dat" using 1:7 title "set5" with linespoints

set autoscale x                      # scale axes automatically
set yrange [0:400000]
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "# of messages"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "msgRESUME_k2.png"
plot "1/per_k.dat" using 1:4 title "set1" with linespoints, \
"2/per_k.dat" using 1:8 title "set2" with linespoints, \
"3/per_k.dat" using 1:8 title "set3" with linespoints, \
"6/per_k.dat" using 1:8 title "set4" with linespoints, \
"11/per_k.dat" using 1:8 title "set5" with linespoints

set autoscale x                      # scale axes automatically
set yrange [0:900000]
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "# of messages"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "msgATTR_k2.png"
plot "1/per_k.dat" using 1:9 title "set1" with linespoints, \
"2/per_k.dat" using 1:9 title "set2" with linespoints, \
"3/per_k.dat" using 1:9 title "set3" with linespoints, \
"6/per_k.dat" using 1:9 title "set4" with linespoints, \
"11/per_k.dat" using 1:9 title "set5" with linespoints


# K=4

set autoscale xy                      # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "clustering events"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "clevents_k4.png"
plot "1/per_k.dat" using 1:10 title "set1" with linespoints, \
"2/per_k.dat" using 1:10 title "set2" with linespoints, \
"3/per_k.dat" using 1:10 title "set3" with linespoints, \
"6/per_k.dat" using 1:10 title "set4" with linespoints, \
"11/per_k.dat" using 1:10 title "set5" with linespoints

set autoscale x                      # scale axes automatically
set yrange [0:120000]
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "# of messages"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "msgJOIN_k4.png"
plot "1/per_k.dat" using 1:11 title "set1" with linespoints, \
"2/per_k.dat" using 1:11 title "set2" with linespoints, \
"3/per_k.dat" using 1:11 title "set3" with linespoints, \
"6/per_k.dat" using 1:11 title "set4" with linespoints, \
"11/per_k.dat" using 1:11 title "set5" with linespoints

set autoscale x                      # scale axes automatically
set yrange [0:400000]
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "# of messages"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "msgRESUME_k4.png"
plot "1/per_k.dat" using 1:12 title "set1" with linespoints, \
"2/per_k.dat" using 1:12 title "set2" with linespoints, \
"3/per_k.dat" using 1:12 title "set3" with linespoints, \
"6/per_k.dat" using 1:12 title "set4" with linespoints, \
"11/per_k.dat" using 1:12 title "set5" with linespoints

set autoscale x                      # scale axes automatically
set yrange [0:900000]
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "# of messages"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "msgATTR_k4.png"
plot "1/per_k.dat" using 1:13 title "set1" with linespoints, \
"2/per_k.dat" using 1:13 title "set2" with linespoints, \
"3/per_k.dat" using 1:13 title "set3" with linespoints, \
"6/per_k.dat" using 1:13 title "set4" with linespoints, \
"11/per_k.dat" using 1:13 title "set5" with linespoints

# K=6

set autoscale xy                      # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "clustering events"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "clevents_k6.png"
plot "1/per_k.dat" using 1:14 title "set1" with linespoints, \
"2/per_k.dat" using 1:14 title "set2" with linespoints, \
"3/per_k.dat" using 1:14 title "set3" with linespoints, \
"6/per_k.dat" using 1:14 title "set4" with linespoints, \
"11/per_k.dat" using 1:14 title "set5" with linespoints

set autoscale x                      # scale axes automatically
set yrange [0:120000]
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "# of messages"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "msgJOIN_k6.png"
plot "1/per_k.dat" using 1:15 title "set1" with linespoints, \
"2/per_k.dat" using 1:15 title "set2" with linespoints, \
"3/per_k.dat" using 1:15 title "set3" with linespoints, \
"6/per_k.dat" using 1:15 title "set4" with linespoints, \
"11/per_k.dat" using 1:15 title "set5" with linespoints

set autoscale x                      # scale axes automatically
set yrange [0:400000]
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "# of messages"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "msgRESUME_k6.png"
plot "1/per_k.dat" using 1:16 title "set1" with linespoints, \
"2/per_k.dat" using 1:16 title "set2" with linespoints, \
"3/per_k.dat" using 1:16 title "set3" with linespoints, \
"6/per_k.dat" using 1:16 title "set4" with linespoints, \
"11/per_k.dat" using 1:16 title "set5" with linespoints

set autoscale x                      # scale axes automatically
set yrange [0:900000]
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "# of messages"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "msgATTR_k6.png"
plot "1/per_k.dat" using 1:17 title "set1" with linespoints, \
"2/per_k.dat" using 1:17 title "set2" with linespoints, \
"3/per_k.dat" using 1:17 title "set3" with linespoints, \
"6/per_k.dat" using 1:17 title "set4" with linespoints, \
"11/per_k.dat" using 1:17 title "set5" with linespoints

# K=8

set autoscale xy                      # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "clustering events"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "clevents_k8.png"
plot "1/per_k.dat" using 1:18 title "set1" with linespoints, \
"2/per_k.dat" using 1:18 title "set2" with linespoints, \
"3/per_k.dat" using 1:18 title "set3" with linespoints, \
"6/per_k.dat" using 1:18 title "set4" with linespoints, \
"11/per_k.dat" using 1:18 title "set5" with linespoints

set autoscale x                      # scale axes automatically
set yrange [0:120000]
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "# of messages"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "msgJOIN_k8.png"
plot "1/per_k.dat" using 1:19 title "set1" with linespoints, \
"2/per_k.dat" using 1:19 title "set2" with linespoints, \
"3/per_k.dat" using 1:19 title "set3" with linespoints, \
"6/per_k.dat" using 1:19 title "set4" with linespoints, \
"11/per_k.dat" using 1:19 title "set5" with linespoints

#error in shawn data ?crashed simulation?
#set autoscale xy                      # scale axes automatically
#unset log                              # remove any log-scaling
#unset label                            # remove any previous labels
#set xtic auto                          # set xtics automatically
#set ytic auto                          # set ytics automatically
##set title "Events per nodes"
#set xlabel "nodes in topology"
#set ylabel "# of messages"
#set size 1.0, 0.6
#set terminal png  size 800, 600 crop
#set tmargin 0
#set output "msgRESUME_k8.png"
#plot "1/per_k.dat" using 1:20 title "set1" with linespoints, \
#"2/per_k.dat" using 1:20 title "set2" with linespoints, \
#"3/per_k.dat" using 1:20 title "set3" with linespoints, \
#"6/per_k.dat" using 1:20 title "set4" with linespoints, \
#"11/per_k.dat" using 1:20 title "set5" with linespoints

set autoscale x                      # scale axes automatically
set yrange [0:900000]
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per nodes"
set xlabel "nodes in topology"
set ylabel "# of messages"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "msgATTR_k8.png"
plot "1/per_k.dat" using 1:21 title "set1" with linespoints, \
"2/per_k.dat" using 1:21 title "set2" with linespoints, \
"3/per_k.dat" using 1:21 title "set3" with linespoints, \
"6/per_k.dat" using 1:21 title "set4" with linespoints, \
"11/per_k.dat" using 1:21 title "set5" with linespoints

