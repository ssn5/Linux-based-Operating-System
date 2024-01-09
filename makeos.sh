cd fish
make all
make fish.exe
cd ..

cd syscalls
make ALL 2> all.log
make ls.exe shell.exe testprint.exe syserr.exe pingpong.exe counter.exe 2> exelog.log


cd ..


cp syscalls/to_fsdir/counter fsdir/
./createfs -i fsdir -o student-distrib/filesys_img


cd student-distrib
make clean
make dep
sudo make

cd ..

