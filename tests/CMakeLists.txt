cmake_minimum_required(VERSION 3.20.0)

# Always built targets
add_executable(test_my_player test_my_player.cpp)
target_link_libraries(test_my_player tttplayer)

add_executable(test_stats test_stats.cpp)
target_link_libraries(test_stats tttplayer)

# Тест человек против бота
add_executable(test_vs_human test_vs_human.cpp)
target_link_libraries(test_vs_human tttplayer)

# Targets that require full or prebuilt tttcore
if((BUILD_TTTCORE STREQUAL "FULL") OR (BUILD_TTTCORE STREQUAL "PREBUILT"))
  add_executable(test_with_baseline test_baseline.cpp)
  target_link_libraries(test_with_baseline tttplayer)

  add_executable(test_stats_vs_baseline test_stats_vs_baseline.cpp)
  target_link_libraries(test_stats_vs_baseline tttplayer)
endif()
