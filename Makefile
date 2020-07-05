mandelbrot:
	g++ Mandelbrot.cpp -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchGccVec -O2 -mavx512f -mavx512cd -mavx512dq -mavx512bw -mavx512vl -ffast-math -fno-finite-math-only -Wall -ftree-vectorize -fopt-info-vec -fopt-info-vec-missed -Wpsabi
	g++ Mandelbrot.cpp -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchGcc -O2 -mavx512f -mavx512cd -mavx512dq -mavx512bw -mavx512vl -ffast-math -fno-finite-math-only -Wall -Wpsabi -fno-tree-vectorize
	clang++ -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchClangVec -O2 -mavx512f -mavx512cd -mavx512dq -mavx512bw -mavx512vl -ffast-math -fno-finite-math-only -Wall -fvectorize Mandelbrot.cpp
	clang++ -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchClang -O2 -mavx512f -mavx512cd -mavx512dq -mavx512bw -mavx512vl -ffast-math -fno-finite-math-only -Wall -fno-vectorize Mandelbrot.cpp

linear_regression:
	g++ LinearRegression.cpp -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchGccAVX512 -O3 -mavx2 -mavx512f -mavx512cd -mavx512dq -mavx512bw -mavx512vl -ffast-math -fno-finite-math-only -Wall -ftree-vectorize -fopt-info-vec -fopt-info-vec-all -Wpsabi
	g++ LinearRegression.cpp -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchGccAVX2 -O3 -mavx2 -ffast-math -fno-finite-math-only -Wall -ftree-vectorize -fopt-info-vec -fopt-info-vec-all -Wpsabi
	g++ LinearRegression.cpp -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchGccSSE -O3 -msse4 -ffast-math -fno-finite-math-only -Wall -ftree-vectorize -fopt-info-vec -fopt-info-vec-all -Wpsabi
	g++ LinearRegression.cpp -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchGcc -O3 -ffast-math -fno-finite-math-only -Wall -Wpsabi -fno-tree-vectorize
	clang++ -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchClangAVX512 -O3 -mavx512f -mavx512cd -mavx512dq -mavx512bw -mavx512vl -ffast-math -fno-finite-math-only -Wall -fvectorize LinearRegression.cpp
	clang++ -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchClangAVX2 -O3 -mavx2 -ffast-math -fno-finite-math-only -Wall -fvectorize LinearRegression.cpp
	clang++ -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchClangSSE -O3 -msse4 -ffast-math -fno-finite-math-only -Wall -fvectorize LinearRegression.cpp
	clang++ -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchClang -O3 -ffast-math -fno-finite-math-only -Wall -fno-vectorize -fno-slp-vectorize LinearRegression.cpp

central_tendencies:
	g++ CentralTendencies.cpp -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchGccAVX512 -O3 -mavx2 -mavx512f -mavx512cd -mavx512dq -mavx512bw -mavx512vl -ffast-math -fno-finite-math-only -Wall -ftree-vectorize -fopt-info-vec -fopt-info-vec-all -Wpsabi
	g++ CentralTendencies.cpp -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchGccAVX2 -O3 -mavx2 -ffast-math -fno-finite-math-only -Wall -ftree-vectorize -fopt-info-vec -fopt-info-vec-all -Wpsabi
	g++ CentralTendencies.cpp -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchGccSSE -O3 -msse4 -ffast-math -fno-finite-math-only -Wall -ftree-vectorize -fopt-info-vec -fopt-info-vec-all -Wpsabi
	g++ CentralTendencies.cpp -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchGcc -O3 -ffast-math -fno-finite-math-only -Wall -Wpsabi -fno-tree-vectorize
	clang++ -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchClangAVX512 -O3 -mavx512f -mavx512cd -mavx512dq -mavx512bw -mavx512vl -ffast-math -fno-finite-math-only -Wall -fvectorize CentralTendencies.cpp
	clang++ -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchClangAVX2 -O3 -mavx2 -ffast-math -fno-finite-math-only -Wall -fvectorize CentralTendencies.cpp
	clang++ -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchClangSSE -O3 -msse4 -ffast-math -fno-finite-math-only -Wall -fvectorize CentralTendencies.cpp
	clang++ -std=c++2a -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o BenchClang -O3 -ffast-math -fno-finite-math-only -Wall -fno-vectorize -fno-slp-vectorize CentralTendencies.cpp
