set autoscale xy                      # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Events per k"
set xlabel "clustering parameter k"
set ylabel "# of CL events"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "enentsVSk.png"
plot "newdata.dat" using 1:2 title "set1" with linespoints, \
"newdata.dat" using 1:6 title "set2" with linespoints, \
"newdata.dat" using 1:10 title "set3" with linespoints, \
"newdata.dat" using 1:14 title "set4" with linespoints, \
"newdata.dat" using 1:18 title "set5" with linespoints


set autoscale xy                      # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "ATTR per k"
set xlabel "clustering parameter k"
set ylabel "# of ATTR messages"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "attrVSk.png"
plot "newdata.dat" using 1:5 title "set1" with linespoints, \
"newdata.dat" using 1:9 title "set2" with linespoints, \
"newdata.dat" using 1:13 title "set3" with linespoints, \
"newdata.dat" using 1:17 title "set4" with linespoints, \
"newdata.dat" using 1:21 title "set5" with linespoints


set autoscale xy                      # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "JOIN per k"
set xlabel "clustering parameter k"
set ylabel "# of JOIN messages"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "joinVSk.png"
plot "newdata.dat" using 1:3 title "set1" with linespoints, \
"newdata.dat" using 1:7 title "set2" with linespoints, \
"newdata.dat" using 1:11 title "set3" with linespoints, \
"newdata.dat" using 1:15 title "set4" with linespoints, \
"newdata.dat" using 1:19 title "set5" with linespoints
