# assigment_1


// nella cartella 'build'

cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR /home/user/Downloads/Laboratori_Compilatori/assigment1/

make



// nella cartella test

// clang -S -emit-llvm -O0 prova.c -o prova.ll

clang -Xclang -disable-O0-optnone -S -emit-llvm -O0 prova.c -o prova.ll
opt -p memreg prova.ll -o prova.m2r.ll

// esecuzione opt con local-opts (tutte 3 funzioni attive)

opt -load-pass-plugin ../build/libLocalOpt.so -passes=local-opts prova.m2r.ll -o prova.optimized.ll
llvm-dis prova.optimized.ll -o prova.optimized.ll

// esecuzione opt con algebraic-opts

opt -load-pass-plugin ../build/libLocalOpt.so -passes=algebraic-opts prova.m2r.ll -o prova.optimized.ll
llvm-dis prova.optimized.ll -o prova.optimized.ll

// esecuzione opt con strength-opts

opt -load-pass-plugin ../build/libLocalOpt.so -passes=strength-opts prova.m2r.ll -o prova.optimized.ll
llvm-dis prova.optimized.ll -o prova.optimized.ll

// esecuzione opt con multi-opts

opt -load-pass-plugin ../build/libLocalOpt.so -passes=multi-opts prova.m2r.ll -o prova.optimized.ll
llvm-dis prova.optimized.ll -o prova.optimized.ll


// visualizzazione risultati

llvm-dis prova.optimized.ll -o prova.m2r_opt.ll
