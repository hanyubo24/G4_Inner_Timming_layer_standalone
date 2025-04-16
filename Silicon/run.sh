#./build/Silicon -p e- -pmin 80 -pmax 80 -o output/Silicon_e_15cm.root -n 20000 -z -15.0

#for z in -20.0; do
#for z in -14.0 -16.0 -18.0 -20.0 -22.0 -24.0 -26.0 -28.0 -30.0 -32.0 -34.0 -36.0; do
#for z in -14.0 -20.0 -28.0 -36.0; do
for z in -14.0 -20.0 -28.0 -36.0; do
  #for p in kaon-; do
  for p in e- mu- pi- kaon- proton; do
    fname="output/Silicon_noB_100ps_test_${p//-}_${z/cm}.root"
    ./build/Silicon_noB -p $p -pmin 10 -pmax 1000 -o $fname -n 40000 -z $z
  done
done
