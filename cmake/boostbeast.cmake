include(FetchContent)

FetchContent_Declare(
  BoostBeast
  GIT_REPOSITORY https://github.com/boostorg/beast.git
  GIT_TAG boost-1.81.0)

FetchContent_MakeAvailable(BoostBeast)
