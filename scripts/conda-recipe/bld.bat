
mkdir conda-build
cd conda-build
cmake -DBUILD_FRONTEND=OFF -DPYTHON_BINDING=ON -DCMAKE_INSTALL_PREFIX=%PREFIX% ..
cmake --build . --target install --config release
