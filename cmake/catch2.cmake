
Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

enable_testing()

function(putCatch2InFolder)
    set_target_properties(Catch2 Catch2WithMain Continuous Experimental Nightly NightlyMemoryCheck PROPERTIES FOLDER Catch2)
endfunction()
