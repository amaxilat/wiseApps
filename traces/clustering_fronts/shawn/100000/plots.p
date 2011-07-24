set autoscale xy                      # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Clusters In Topology"
set xlabel "hop Clusters in 100k nodes topology"
set ylabel "Clustering Events"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "cl_events.png"
plot "resul.file" using 1:2 title "Clustering Events" with linespoints

set autoscale xy                      # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Clusters In Topology"
set xlabel "hop Clusters in 100k nodes topology"
set ylabel "# of Messages"
set size 1.0, 0.6
set terminal png  size 800, 600 crop
set tmargin 0
set output "cl_messages.png"
plot "resul.file" using 1:3 title "Join" with linespoints, \
"resul.file" using 1:4 title "Resume" with linespoints, \
"resul.file" using 1:5 title "Attribute" with linespoints


