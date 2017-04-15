# script that put everything in a single file, in order to be able to have a better inlining function
#used only for production build

rm -rf build_all_tmp/ 
mkdir build_all_tmp
cat OCS_V1.2.ino > build_all_tmp/build_all_tmp.ino
cat a_tables.ino >> build_all_tmp/build_all_tmp.ino
cat in_*.ino >> build_all_tmp/build_all_tmp.ino
cat m_*.ino >> build_all_tmp/build_all_tmp.ino
cat out_*.ino >> build_all_tmp/build_all_tmp.ino
cat v_*.ino >> build_all_tmp/build_all_tmp.ino
cat z_dueFlashStorage.ino >> build_all_tmp/build_all_tmp.ino
cat z_efc.ino >> build_all_tmp/build_all_tmp.ino
cat z_flash_efc.ino >> build_all_tmp/build_all_tmp.ino
cat b_test.ino >> build_all_tmp/build_all_tmp.ino

cp *.h build_all_tmp/
~/arduino-1.8.2/arduino build_all_tmp/build_all_tmp.ino
