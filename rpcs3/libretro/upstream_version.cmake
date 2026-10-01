# Report the upstream RPCS3 build instead of this fork's commit count.
#
# git-version.cmake counts the commits of HEAD, which on this fork is neither
# upstream's build number nor anything a user can look up. upstream.version
# names the upstream build of the last merge, and that is what the core reports,
# so a report can be checked against the same standalone build. The branch keeps
# naming this fork's branch.
file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/upstream.version" _rpcs3_upstream REGEX "^RPCS3_[A-Z]+=")
foreach(_line IN LISTS _rpcs3_upstream)
	if(_line MATCHES "^(RPCS3_[A-Z]+)=(.+)$")
		set(_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}")
	endif()
endforeach()

if(NOT _RPCS3_VERSION MATCHES "^v[0-9]+\\.[0-9]+\\.[0-9]+-([0-9]+)$" OR NOT _RPCS3_COMMIT)
	message(FATAL_ERROR "rpcs3/libretro/upstream.version must set RPCS3_VERSION (vX.Y.Z-build) and RPCS3_COMMIT")
endif()

# The library version, and git-version.h's "build-commit" the way upstream's
# own builds write it into the log.
set(RPCS3_LIBRETRO_VERSION "${_RPCS3_VERSION}")
string(SUBSTRING "${_RPCS3_COMMIT}" 0 8 _rpcs3_upstream_short)
set(RPCS3_GIT_VERSION "${CMAKE_MATCH_1}-${_rpcs3_upstream_short}")

# A merge that forgot to bump the file leaves the recorded commit behind. It is
# only checkable with the history at hand, so a shallow clone says nothing.
if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git/")
	execute_process(COMMAND ${GIT_EXECUTABLE} cat-file -e "${_RPCS3_COMMIT}^{commit}"
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} RESULT_VARIABLE _have_commit ERROR_QUIET)
	if(_have_commit EQUAL 0)
		execute_process(COMMAND ${GIT_EXECUTABLE} merge-base --is-ancestor "${_RPCS3_COMMIT}" HEAD
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} RESULT_VARIABLE _is_ancestor)
		if(NOT _is_ancestor EQUAL 0)
			message(WARNING "upstream.version names ${_RPCS3_COMMIT}, which HEAD does not contain")
		endif()
	endif()
endif()
