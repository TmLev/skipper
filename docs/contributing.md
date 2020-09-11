# Contributing

## Pull Requests

New features should be accompanied by tests:
* For an existing test file, add new `TEST_CASE`(-s).
* For a new test file, place it in `tests` 
  and register with `add_skipper_test` in [`tests/CMakeLists.txt`](../tests/CMakeLists.txt).

Checklist:
- [ ] Tests for the changes have been added / updated.
- [ ] Documentation comments have been added / updated.
- [ ] Format code with the `clang-format-10` (or above).
- [ ] Format CMake code with the `cmake-format` (if applicable).
