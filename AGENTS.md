# Rules for coding agents

These apply to any AI agent working in this repository (Claude Code reads them through `CLAUDE.md`).

## Merging upstream RPCS3

This repository builds the libretro core and nothing else. The standalone's parts - the Qt application and its resources, the executable, packaging, MSVC projects, unit tests, upstream's CI for the application, Discord RPC and GameMode - were deleted on purpose, and must not come back with a merge. The paths are listed in `.upstream-excluded`.

- Right after `git merge` of upstream, before resolving anything else, run:
  `git rm -r -q --ignore-unmatch --pathspec-from-file=.upstream-excluded`
  This settles the modify/delete conflicts in favour of the deletion, and it also removes files upstream newly added under those paths, which git would otherwise bring in without any conflict.
- Never resolve a conflict on one of those paths by restoring the file.
- When upstream changes a CMake file around something that was removed, keep it removed and take the rest of the change.
- When something new is deleted for the same reason, add its path to `.upstream-excluded` in the same commit.

## These files

- Only the developer writes to or deletes `AGENTS.md` and `CLAUDE.md`. An agent does not change them on its own; it proposes the change to the developer instead.
