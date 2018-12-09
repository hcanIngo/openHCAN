# UI-conf 2 graph
#################

awk -f cf2dot.awk $1.cf > $1.dot
dot -Tpng $1.dot -o $1.png
# rm $1.dot
