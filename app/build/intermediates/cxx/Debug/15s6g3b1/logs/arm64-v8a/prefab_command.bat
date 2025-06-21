@echo off
"C:\\Program Files\\Java\\jdk-24\\bin\\java" ^
  --class-path ^
  "C:\\Users\\moz04\\.gradle\\caches\\modules-2\\files-2.1\\com.google.prefab\\cli\\2.1.0\\aa32fec809c44fa531f01dcfb739b5b3304d3050\\cli-2.1.0-all.jar" ^
  com.google.prefab.cli.AppKt ^
  --build-system ^
  cmake ^
  --platform ^
  android ^
  --abi ^
  arm64-v8a ^
  --os-version ^
  21 ^
  --stl ^
  c++_shared ^
  --ndk-version ^
  27 ^
  --output ^
  "C:\\Users\\moz04\\AppData\\Local\\Temp\\agp-prefab-staging1913999676686651648\\staged-cli-output" ^
  "C:\\Users\\moz04\\.gradle\\caches\\8.14.2\\transforms\\ed78dc27aab8c547774bdfd60775ae4d\\transformed\\SDL3-3.2.16\\prefab"
