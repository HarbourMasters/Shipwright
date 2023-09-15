thanks for making a PR

we have `clang-format-14` as part of the CI process, to make sure your code passes `clang-format` validation you can run

```sh
find soh -name "*.cpp" -or -name "*.hpp" -or -name "*.c" -or -name "*.h" | sed 's| |\\ |g' | xargs clang-format-14 -i
```

todo: windows

edit
