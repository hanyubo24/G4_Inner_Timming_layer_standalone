#./build/Silicon -p e- -pmin 80 -pmax 80 -o output/Silicon_e_15cm.root -n 20000 -z -15.0

#for z in -20.0; do
###for z in -2.0; do
####for z in -14.0 -16.0 -18.0 -20.0 -22.0 -24.0 -26.0 -28.0 -30.0 -32.0 -34.0 -36.0; do
####for z in -14.0 -20.0 -28.0 -36.0; do
####for z in -14.0 -20.0 -28.0 -36.0; do
###  #for p in kaon-; do
###  for p in e- mu- pi- kaon- proton; do
###    fname="output/Silicon_200um_100ps_test_80mev_stepsize0_${p//-}_${z/cm}.root"
###    #./build/Silicon_thick -p $p -pmin 10 -pmax 1000 -o $fname -n 40000 -z $z
###    ./build/Silicon_noB -p $p -pmin 80 -pmax 80.1 -o $fname -n 40000 -z $z
###  done
###done

#./build/Silicon_cylinder -csv table/belle3_noSL0.csv  -o output/belle3_noSL0_30ps_2layers_24cm_36cm.root -z 0 -n 10
#./build/Silicon_cylinder -csv table/belle3_noSL0SL1.csv  -o output/belle3_noSL0SL1_30ps_2layers_24cm_36cm.root -z 0 
#./build/Silicon_cylinder_noB  -p pi- -pmin 10 -pmax 1000 -o output/ITT_100ps_2layers_24cm_36cm_noB_2Tracks.root -n 4000 -z 0
#./build/Silicon_cylinder -p pi- -pmin 1 -pmax 800 -o output/ITT_100ps_2layers_24cm_36cm_testacceptance_pi.root -n 80000 -z 0
#./build/Silicon_cylinder -p e- -pmin 1 -pmax 800 -o output/ITT_100ps_2layers_24cm_36cm_testacceptance_e.root -n 80000 -z 0
#./build/Silicon_cylinder -p mu- -pmin 1 -pmax 800 -o output/ITT_100ps_2layers_24cm_36cm_testacceptance_mu.root -n 80000 -z 0
./build/Silicon_cylinder -p kaon- -pmin 1 -pmax 800 -o output/ITT_100ps_2layers_24cm_36cm_testacceptance_kaon.root -n 80000 -z 0
./build/Silicon_cylinder -p proton -pmin 1 -pmax 800 -o output/ITT_100ps_2layers_24cm_36cm_testacceptance_proton.root -n 80000 -z 0
