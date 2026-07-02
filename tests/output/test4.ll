declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %result = call i32 (ptr, ...) @printf(ptr null)
  ret i32 0
}
