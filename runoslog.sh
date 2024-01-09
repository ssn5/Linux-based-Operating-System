cd fish
make fish.exe
cd ..

cd syscalls
make ls.exe shell.exe testprint.exe syserr.exe pingpong.exe


cd ..

cd student-distrib
make clean
make dep
make 2> out.log

cd ..
