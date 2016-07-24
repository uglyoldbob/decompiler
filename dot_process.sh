#/bin/sh
cd $1
for file in *.gv
do
 echo "$file"
 dot -Tpng "$file" > "$file".png
done
