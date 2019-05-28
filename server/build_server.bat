gcc -Wall ./server/server.c -lws2_32 -L./third_party -llibsndfile-1 -o ./output/server.exe
copy third_party\libsndfile-1.dll output