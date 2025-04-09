#./build/Silicon -p e- -pmin 80 -pmax 80 -o output/Silicon_e_15cm.root -n 20000 -z -15.0

#for z in -15.0 -14.0; do
for z in -16.0 -17.0 -18.0 -19.0 -20.0 -21.0 -22.0 -23.0 -24.0; do
  for p in e- mu- pi- kaon-; do
    fname="output/Silicon_${p//-}_${z/cm}.root"
    ./build/Silicon -p $p -pmin 80 -pmax 80 -o $fname -n 20000 -z $z
  done
done
