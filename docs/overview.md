# 项目概览

<!-- - [基本类型](\ref docs/basic_type.md) -->
# basic type
- [ByteBuffer](\ref nx::ByteBuffer)
- [String](\ref nx::String)
- [Optional<T>](\ref nx::Optional)
- [Vector<T>](\ref nx::Vector)
- [SharedPtr<T>](\ref nx::SharedPtr)
- [UniquePtr<T>](\ref nx::UniquePtr)
- [Variant<T ...>](\ref nx::Variant)

<!-- # basic helper class
- [Uncopyable](\ref nx::Uncopyable) -->

# basic interface
- [Read](\ref nx::Read)
- [Write](\ref nx::Write)

# Time Utils
- [TimePoint](\ref nx::TimePoint)
- [TimeDuration](\ref nx::TimeDuration)
- [time_now](\ref nx::time_now)
- [time_diff](\ref nx::time_diff)

### Examples

```
TimePoint before = time_now();
// do something
TimePoint after = time_now();
TimeDuration duration = time_diff(before, after);
```

# Math Utils
- [ceil_pow2](\ref nx::ceil_pow2)

# File System 
- [is_directory](\ref nx::file_system::is_directory)
- [is_file](\ref nx::file_system::is_file)
- [exists](\ref nx::file_system::exists)
- [make_dirs](\ref nx::file_system::make_dirs)
- [relative_path](\ref nx::file_system::relative_path)
- [list_dir](\ref nx::file_system::list_dir)