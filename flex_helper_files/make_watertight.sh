
meshlab.meshlabserver -i $1 -o temp.obj
/home/arsalan/codes/Manifold/build/manifold temp.obj temp.watertight.obj -s
/home/arsalan/codes/Manifold/build/simplify -i temp.watertight.obj -o $2 -m -r 0.02
