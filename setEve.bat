adb root;
adb remount;
adb push ./public.libraries.txt /etc/;

adb push ./app/build/intermediates/cmake/debug/obj/armeabi-v7a/libexcast.so /system/lib