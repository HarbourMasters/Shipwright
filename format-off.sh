read -n1 bom < soh/src/overlays/gamestates/ovl_file_choose/z_file_choose.c
for file in $(find soh \( -path soh/assets -prune -or -path soh/soh/Enhancements/randomizer/3drando -prune \) -name "*.cpp" -or -name "*.hpp" -or -name "*.c" -or -name "*.h"); do
  read -n1 firstchar < $file
  if [ "$firstchar" = "$bom" ];
  then echo bom $file;
  sed -i '2s/^/\/\/ clang-format off\n\n/' $file
  else
  sed -i '1s/^/\/\/ clang-format off\n\n/' $file
  fi
done
